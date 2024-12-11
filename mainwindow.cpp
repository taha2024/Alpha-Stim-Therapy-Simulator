#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>



/**
 * Constructor for MainWindow class.
 * Initializes device/admin settings, timers, connects buttons and slots to tabs
 *
 * @param parent is the QWidget parent of the MainWindow class
 */
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Initialize the device
    device = new CESDevice(ui->therapyTimer, ui->recordsList);

    //Connect device's timer timeout to resetting the window
    connect(device->getCurrSession()->getInternalClock()->getTimer(),
            &QTimer::timeout,
            this,
            &MainWindow::resetUI);

    //Create timer used for checking battery life and connect it to slot function
    batteryTimer = new QTimer(this);
    batteryTimer->setTimerType(Qt::PreciseTimer);
    connect(batteryTimer, SIGNAL(timeout()), this, SLOT(batteryUpdate()));

    //Create timer used for checking inactivity on the device and connect it to slot function
    inactivityTimer = new QTimer(this);
    inactivityTimer->setTimerType(Qt::PreciseTimer);
    connect(inactivityTimer, SIGNAL(timeout()), this, SLOT(inactivityUpdate()));

    //Create timer used for checking for loss of skin contact and connect it to slot function
    skinOffTimer = new QTimer(this);
    skinOffTimer->setTimerType(Qt::PreciseTimer);
    skinOffTimer->setSingleShot(true);
    connect(skinOffTimer, SIGNAL(timeout()), this, SLOT(skinContactUpdate()));

    //Set intial time for large timer to "00:00"
    ui->therapyTimer->display(QTime(0,0,0).toString("mm:ss"));

    //Set inactivity timer to zero and display it
    resetInactivity();

    //Start battery and inactivity timers
    batteryTimer->start(1000);
    inactivityTimer->start(1000);

    //Connect buttons on the device
    connect(ui->powerButton, SIGNAL(clicked(bool)), this, SLOT(powerClick()));
    connect(ui->recordButton, SIGNAL(clicked(bool)), this, SLOT(recordClick()));
    connect(ui->downButton, SIGNAL(clicked(bool)), this, SLOT(downClick()));
    connect(ui->upButton, SIGNAL(clicked(bool)), this, SLOT(upClick()));
    connect(ui->selectButton, SIGNAL(clicked(bool)), this, SLOT(selectClick()));
    connect(ui->returnButton, SIGNAL(clicked(bool)), this, SLOT(returnClick()));

    //Connect tabs on the device
    connect(ui->screenTabs, SIGNAL(currentChanged(int)), this, SLOT(resetInactivity()));

    //Connections for admin area
    connect(ui->powerLevelAdminValue, SIGNAL(valueChanged(int)), this, SLOT(powerLevelAdminChange(int)));
    connect(ui->contactSkinValue, SIGNAL(currentIndexChanged(int)), this, SLOT(skinContactAdminChange(int)));
    connect(ui->deviceEnabledValue, SIGNAL(currentIndexChanged(int)), this, SLOT(deviceEnabledChange(int)));
    connect(ui->batteryPercentageValue, SIGNAL(valueChanged(int)), this, SLOT(adminBatteryUpdate(int)));
    connect(ui->increaseInactiveTime, SIGNAL(clicked(bool)), this, SLOT(inactivityUpdate()));



}

/**
 * Deconstructor for MainWindow
 */
MainWindow::~MainWindow()
{
    delete ui;
    delete device;
}



/**
 * Triggered when user presses the power button on the device
 */
void MainWindow::powerClick()
{

    //If device is on, turn it off
    if(device->getIsOn()){

        device->setIsOn(false);

        //Don't burn battery or check inactivity when device is off
        batteryTimer->stop();
        inactivityTimer->stop();
        resetInactivity();

        //If session was in progress, end it. Will record it if that setting was chosen
        if(device->getIsTreating()){
            device->stopSession(device->getCurrSession()->getLastDuration() - device->getCurrSession()->getDuration());
            ui->contactSkinValue->setCurrentIndex(1);
            ui->timerOnLabel->setStyleSheet("color: rgb(211, 215, 207);");
        }

        //Turn off device ui/buttons
        turnOffDevice();


    //Device is off and user is turning it on
    }else{

        //If battery percent is above 2 and the device is not disabled it can be turned on
        if(device->getBattery()->getBatteryPercentage() > 2 && !device->getIsDisabled()){

            //Set device to turned on
            device->setIsOn(true);

            //Turn on device ui/buttons
            turnOnDevice();

            //Set the onscreen battery level and start battery timer
            ui->batteryLevelBar->setValue(device->getBattery()->getBatteryPercentage());
            batteryTimer->start(1000);

            //start timing for inactivity
            inactivityTimer->start(1000);
            resetInactivity();

            //If device is contacting skin, start therapy right away
            if(device->getContact()){

                //Set device for treatment mode
                device->setIsTreating(true);
                ui->notTreatingLabel->setText("Treating");

                //Use duration that was selected last time device was on
                device->getCurrSession()->setDuration(device->getCurrSession()->getLastDuration());

                //Update the displayed time and show timer on
                int sessionDuration = device->getCurrSession()->getDuration();
                QTime time = QTime(0,0,0).addSecs(sessionDuration * 60);
                QString minuteSeconds = time.toString("mm:ss");
                ui->therapyTimer->display(sessionDuration < 60 ? minuteSeconds : QString::number(sessionDuration) + ":00");
                ui->timerOnLabel->setStyleSheet(QString::fromUtf8("color: rgb(0,0,0);"));

                device->getCurrSession()->setLastPowerLevel(2);
                ui->powerLevelAdminValue->setValue(100);
                ui->powerLevelBar->setValue(2);

                //Start the therapy
                device->getCurrSession()->startSession();
            }

        }
    }
}


/**
 * Triggered when user presses record button
 */
void MainWindow::recordClick()
{
    //Reset inactivity count
    resetInactivity();

    //Get which screen is being viewed
    int curTab = ui->screenTabs->currentIndex();

    //If on the therapy screen
    if(curTab == 1){

        //Turn on or off recording
        device->setRecording(!device->getRecording());

        //Set on screen label to recording or not recording
        ui->recordingLabel->setText(device->getRecording() ? "Recording" : "Not Recording");
     }
}


/**
 * Triggered when user clicks the down button
 */
void MainWindow::downClick()
{
    //Reset inactivity count
    resetInactivity();

    //Get the current screen the user is on
    int curTab = ui->screenTabs->currentIndex();

    //Get the current selected indices for the time, waveform and frequenct selection lists (0,1,2,-1)
    //Will be -1 if not in use
    int waveformSelection = ui->waveformList->currentRow();
    int frequencySelection = ui->frequencyList->currentRow();
    int timeSelection = ui->timeList->currentRow();

    //Get whether the device is in treatment mode or not
    bool deviceTreating = device->getIsTreating();

    //If on therapy screen
    if(curTab == 1){

        //Device is not in treatment mode, can change time,waveform,frequency settings
        if(!deviceTreating){

            //If picking through waveform list
            if(waveformSelection > -1){
                //Decrease the selected row by 1. Wrap around if at end of the list
                ui->waveformList->setCurrentRow(ui->waveformList->currentRow() == 0 ? 2: ui->waveformList->currentRow()-1);

            //If picking through frequency list
            }else if(frequencySelection > -1){
                //Increase the selected row by 1. Wrap around if at end of the list
                ui->frequencyList->setCurrentRow(ui->frequencyList->currentRow() == 0 ? 2: ui->frequencyList->currentRow()-1);

            //If picking through the time list
            }else if(timeSelection > -1){
                //Increase the selected row by 1. Wrap around if at end of the list
                ui->timeList->setCurrentRow(ui->timeList->currentRow() == 0 ? 2: ui->timeList->currentRow()-1);
            }

        //Device is treating, can change power level
        }else{
            //Decrease power by 100 mU
            device->decreasePower();

            //Update displayed power level on device and in admin area
            int newPowerLevel = ui->powerLevelBar->value() - 2 < 0 ? 0 : ui->powerLevelBar->value()-2;

            ui->powerLevelBar->setValue(newPowerLevel);
            ui->powerLevelAdminValue->setValue(newPowerLevel * 50);
        }

    //On the records page, scrolls down to the bottom of the list of records
    }else{
        ui->recordsList->scrollToBottom();
    }

}

/**
 * Triggered when user clicks the up button
 */
void MainWindow::upClick()
{
    //Reset inactivity count
    resetInactivity();

    //Get the current screen the user is on
    int curTab = ui->screenTabs->currentIndex();

    //Get the current selected indices for the time,waveform and frequenct selection lists (0,1,2,-1)
    //Will be -1 if not in use
    int waveformSelection = ui->waveformList->currentRow();
    int frequencySelection = ui->frequencyList->currentRow();
    int timeSelection = ui->timeList->currentRow();

    //Get whether the device is in treatment mode or not
    bool deviceTreating = device->getIsTreating();

    //If on therapy screen
    if(curTab == 1){

        //Device is not in treatment mode, can change time,waveform,frequency settings
        if(!deviceTreating){

            //If picking through waveform list
            if(waveformSelection > -1){
                //Increase the selected row by 1. Wrap around if at end of the list
                ui->waveformList->setCurrentRow(ui->waveformList->currentRow() == 2 ? 0: ui->waveformList->currentRow()+1);


            //If picking through frequency list
            }else if(frequencySelection > -1){
                //Increase the selected row by 1. Wrap around if at end of the list
                ui->frequencyList->setCurrentRow(ui->frequencyList->currentRow() == 2 ? 0: ui->frequencyList->currentRow()+1);

            //If picking through the time list
            }else if(timeSelection > -1){
                //Increase the selected row by 1. Wrap around if at end of the list
                ui->timeList->setCurrentRow(ui->timeList->currentRow() == 2 ? 0: ui->timeList->currentRow()+1);
            }
        //Device is treating, can change power level
        }else{
            //Increase power by 50 mU
            device->increasePower();

            //Update displayed power level on device and in admin
            int newPowerLevel = ui->powerLevelBar->value() == 10 ? 10 : ui->powerLevelBar->value()+1;

            ui->powerLevelBar->setValue(newPowerLevel);
            ui->powerLevelAdminValue->setValue(newPowerLevel * 50);
        }

    //On the records page, scrolls up to the top of the list of records
    }else{
        ui->recordsList->scrollToTop();
    }
}


/**
 * Triggered when user clicks the select button
 */
void MainWindow::selectClick()
{
    //Reset inactivity count
    resetInactivity();

    //Get current screen the user is on
    int curTab = ui->screenTabs->currentIndex();

    //Get whether the device is in treatment mode or not
    bool deviceTreating = device->getIsTreating();

    //If on therapy screen
    if(curTab == 1){

        //Device is not treating, can change time,waveform,frequency
        if(!deviceTreating){

            //Get the current selected indices for the time, waveform and frequenct selection lists (0,1,2,-1)
            //Will be -1 if not in use
            int waveformSelection = ui->waveformList->currentRow();
            int frequencySelection = ui->frequencyList->currentRow();
            int timeSelection = ui->timeList->currentRow();

            //If none of the lists were being looked at, start looking through time list
            if(waveformSelection == -1 && frequencySelection == -1 && timeSelection == -1){
                ui->timeList->setCurrentRow(0);
            }

            //Time list is being looked at and the user is locking in selection for time
            else if(timeSelection > -1){

                //Lock in time selection
                device->selectTherapyTime(timeSelection);


                //Set the selected time choice to black, others are faded
                for(int i = 0;i<3;i++){
                    if(ui->timeList->currentRow() == i){
                        ui->timeList->currentItem()->setTextColor(Qt::black);
                    }else{
                        ui->timeList->item(i)->setTextColor(QColor(211, 215, 207));
                    }

                }


                //Remove selection highlight from the time list
                ui->timeList->setCurrentRow(-1);

                //Switch to waveform list
                ui->waveformList->setCurrentRow(0);
            }

            //Waveform list is being looked at, and user is locking in selection for waveform
            else if(waveformSelection > -1){

                //Set the selected waveform
                device->selectWaveform(waveformSelection);

                //Set the waveform value in the admin area
                ui->waveFormAdminValue->setText(ui->waveformList->currentItem()->text());


                //Set the selected waveform choice to black, others are faded
                for(int i = 0;i<3;i++){
                    if(ui->waveformList->currentRow() == i){
                        ui->waveformList->currentItem()->setTextColor(Qt::black);
                    }else{
                        ui->waveformList->item(i)->setTextColor(QColor(211, 215, 207));
                    }

                }

                //Remove selection highlight from the waveform
                ui->waveformList->setCurrentRow(-1);

                //Move highlight to frequency
                ui->frequencyList->setCurrentRow(0);

            //Frequency list is being looked at and user is locking in selection for frequency
            }else{

                //Set the selected frequency for the device
                device->selectFrequency(frequencySelection);

                //Set the frequency value in the admin area
                ui->frequencyAdminValue->setText(ui->frequencyList->currentItem()->text());

                //Set the selected frequency choice to black, others are faded
                for(int i = 0;i<3;i++){
                    if(ui->frequencyList->currentRow() == i){
                        ui->frequencyList->currentItem()->setTextColor(Qt::black);
                    }else{
                        ui->frequencyList->item(i)->setTextColor(QColor(211, 215, 207));
                    }

                }

                //Remove selection highlight from the frequency list
                ui->frequencyList->setCurrentRow(-1);
            }
        }
    }
}

/**
 * Triggered when user clicks return button
 */
void MainWindow::returnClick()
{
    //Reset inactivity count
    resetInactivity();

    //Get the screen the user is on
    int curTab = ui->screenTabs->currentIndex();

    //Get whether the device is in treatment mode or not
    bool deviceTreating = device->getIsTreating();

    //If on therapy screen
    if(curTab == 1){

        //Device not treating currently, can move backwards through time,waveform and frequency lists
        if(!deviceTreating){

            //Get the current selected indices for the time, waveform and frequenct selection lists (0,1,2,-1)
            //Will be -1 if not in use
            int waveformSelection = ui->waveformList->currentRow();
            int frequencySelection = ui->frequencyList->currentRow();
            int timeSelection = ui->timeList->currentRow();


            //Time list is being looked at, and user is leaving the selection process
            if(timeSelection > -1){

                //Remove selection highlight from the time list
                ui->timeList->setCurrentRow(-1);

            }

            //Waveform list is being looked at, and user is going back to time selections
            else if(waveformSelection > -1){

                //Remove selection highlight from the waveform
                ui->waveformList->setCurrentRow(-1);

                //Move highlight to frequency
                ui->timeList->setCurrentRow(0);

            //Frequency list is being looked at and user is going back to waveform selections
            }else if(frequencySelection > -1){

                //Remove selection highlight from the frequency list
                ui->frequencyList->setCurrentRow(-1);

                //Move highlight to waveform
                ui->waveformList->setCurrentRow(0);
            }
        }
    }
}

/**
 * When the Session ends, update the UI to reflect this
 */
void MainWindow::resetUI()
{
    //Ignore if the system is still running
    if(device->getCurrSession()->getDuration() != 0) { return; }

    ui->recordingLabel->setText("Not Recording");
    device->setRecording(false);

    ui->notTreatingLabel->setText("Not Treating");

    ui->powerLevelBar->setValue(2);
    ui->powerLevelAdminValue->setValue(100);

    ui->contactSkinValue->setCurrentIndex(1);
    ui->skinLabel->setText("Contact Off");

    ui->timerOnLabel->setStyleSheet(QString::fromUtf8("color: rgb(238,238,236);"));

}

/**
 * Triggered when user changes the power level uA using the selector in the admin
 * @param level is the new uA of the device
 */
void MainWindow::powerLevelAdminChange(int level)
{
    //Increase/decrease power level bar between 0-500 uA
    if(level <= 500){
        device->getCurrSession()->setLastPowerLevel(level/50);
        ui->powerLevelBar->setValue(level/50);
    }

    //When maximum uA for device is exceeded
    if(level > 700){
        ui->deviceEnabledValue->setCurrentIndex(1);
    }
}

/**
 * Triggered when user changes skin contact using dropdown in admin
 * @param value is the index of the dropdown (0=true, 1=false)
 */
void MainWindow::skinContactAdminChange(int value)
{
    //Skin contact is true
    if(value == 0){

        //Change device skin contact to true
        device->setContact(true);
        ui->skinLabel->setText("Contact On");

        //Start treating if device is turned on
        if(device->getIsOn()){

            //If therapy was already in session and was paused by skin contact change to off,
            //resume that session
            if(device->getIsTreating()){
                device->getCurrSession()->resumeSession();
            }

            //Start a new therapy
            else{

                //Set device for the treatment
                device->setIsTreating(true);
                ui->notTreatingLabel->setText("Treating");
                ui->timerOnLabel->setStyleSheet(QString::fromUtf8("color: rgb(0,0,0);"));

                //Reset inactivity timer to zero when treating
                resetInactivity();

                //Start the session
                device->getCurrSession()->startSession();

                //Display intial therapy duration
                int sessionDuration = device->getCurrSession()->getDuration();
                QTime time = QTime(0,0,0).addSecs(sessionDuration * 60);
                QString minuteSeconds = time.toString("mm:ss");
                ui->therapyTimer->display(sessionDuration < 60 ? minuteSeconds : QString::number(sessionDuration)+":00");

                //Set onscreen/admin power level to 1
                ui->powerLevelBar->setValue(2);
                ui->powerLevelAdminValue->setValue(100);

                //Set the battery burn rate to 1% every 12 seconds
                device->getBattery()->intialTherapyBurnRate();
            }
        }

    //Skin contact is changed to false
    }else{

        //Set skin contact on device/screen to off
        device->setContact(false);
        ui->skinLabel->setText("Contact Off");

        //If device was in a therapy session, pause it and
        //start 5 second timeout
        if(device->getIsTreating()){
            device->getCurrSession()->pauseSession();
            skinOffTimer->start(5000);

        //Otherwise device is not setup for treatment
        }else{

            device->setIsTreating(false);
            ui->notTreatingLabel->setText("Not Treating");

            device->getBattery()->defaultBurnRate();
            ui->timerOnLabel->setStyleSheet(QString::fromUtf8("color: rgb(0,0,0);"));
        }
    }
}



/**
 * Triggered when user chnages device enabled using the dropdown in admin
 * @param value is the index of the dropdown (0 = true, 1 = false)
 */
void MainWindow::deviceEnabledChange(int value)
{
    //If changed to false
    if(value == 1){

        //If device was treating, end session. Record it if that was set
        if(device->getIsTreating()){
            device->stopSession(device->getCurrSession()->getLastDuration()-device->getCurrSession()->getDuration());
            ui->contactSkinValue->setCurrentIndex(1);
        }

        //Disable device, timers, and turn off screen and buttons
        device->setIsDisabled(true);
        device->setIsOn(false);
        inactivityTimer->stop();
        batteryTimer->stop();
        turnOffDevice();



    //Re-enabling device from admin area
    }else{

        //Reset the battery burn rate (not turning it on though), re-enable device
        //set uA of device back to 100
        device->getBattery()->defaultBurnRate();
        device->setIsDisabled(false);
        ui->powerLevelAdminValue->setValue(100);
    }

}

/**
 * Triggered when battery timer runs out. Updates battery percentage
 */
void MainWindow::batteryUpdate()
{
    //If the device is on, battery can be depleted
    if(device->getIsOn()){

        //Simulate battery depletion
        device->getBattery()->depleteBattery();

        int batteryPercentage = device->getBattery()->getBatteryPercentage();

        //Display the device's current battery level on the device and in admin
        ui->batteryLevelBar->setValue(batteryPercentage);
        ui->batteryPercentageValue->setValue(batteryPercentage);

        //Display warning if device is at 5% battery
        if(batteryPercentage == 5 && !device->getBattery()->getFiveWarning()){

            //Display warning message
            QMessageBox lowBattery;
            lowBattery.setText("<) Warning: Your battery is low at 5%. <)");
            lowBattery.exec();

            //Set battery warning to displayed
            device->getBattery()->setFiveWarning(true);


        //Display warning if device is at 2% battery and turn off device
        }else if(batteryPercentage == 2){

            //Display warning message
            QMessageBox lowBattery;
            lowBattery.setText("<) Warning: Your battery is low at 2%. Shutting down the device. <)");
            lowBattery.exec();

            //Turn off device if battery reaches 2%. Stop depleting battery. Stop inactivity timer
            if(device->getIsTreating()){
                device->stopSession(device->getCurrSession()->getLastDuration()-device->getCurrSession()->getDuration());
                ui->contactSkinValue->setCurrentIndex(1);
            }

            device->setIsOn(false);
            turnOffDevice();
            inactivityTimer->stop();
            batteryTimer->stop();


        //Set the 5% warning to available again
        }else if(batteryPercentage != 5){
            device->getBattery()->setFiveWarning(false);
        }
    }
}

/**
 * Triggered when user adjusts batery percentage level in admin area
 * @param value is the new percentage of the battery
 */
void MainWindow::adminBatteryUpdate(int value)
{
    //Update the device's battery with the new percent
    device->getBattery()->setBatteryPercentage(value);

    //Change the displayed battery level on the device
    ui->batteryLevelBar->setValue(value);


}

/**
 * Called when inactivity timer times out or timer is increased by one second in admin area
 */
void MainWindow::inactivityUpdate()
{
    //Check device is not currently treating
    if(!device->getIsTreating()){

        //Increase number of seconds inactive and update display
        inactiveSeconds += 60;
        displayInactiveTime();

        //If 30 minutes of inactivity reached, turn off device
        if(inactiveSeconds == 1800){
            device->setIsOn(false);
            turnOffDevice();
            inactivityTimer->stop();


        }
    }
}


/**
 * Updates the display of the inactive timer
 */
void MainWindow::displayInactiveTime()
{
    //Convert seconds to "mm:ss" and display it
    QTime time = QTime(0,0,0).addSecs(inactiveSeconds);
    ui->inactivityTimer->display(time.toString("mm:ss"));

}


/**
 * Slot function for tabs being clicked to reset activity count
 */
void MainWindow::resetInactivity()
{
    inactiveSeconds = 0;
    displayInactiveTime();
}

/**
 * Turns off the screen and buttons on the device
 */
void MainWindow::turnOffDevice()
{
    //Disable on device buttons
    ui->recordButton->setEnabled(false);
    ui->selectButton->setEnabled(false);
    ui->upButton->setEnabled(false);
    ui->downButton->setEnabled(false);
    ui->returnButton->setEnabled(false);

    //Set recording to false
    ui->recordingLabel->setText("Not Recording");
    device->setRecording(false);

    //Disable the screen
    ui->screenFrame->setStyleSheet(QString::fromUtf8("background-color: rgb(85, 87, 83);"));
    ui->screenTabs->setHidden(true);
    ui->screenFrame->setEnabled(false);

}

/**
 * Turns on the screen and buttons of the device
 */
void MainWindow::turnOnDevice()
{
    //Enable on device buttons
    ui->recordButton->setEnabled(true);
    ui->selectButton->setEnabled(true);
    ui->upButton->setEnabled(true);
    ui->downButton->setEnabled(true);
    ui->returnButton->setEnabled(true);

    //Enable the screen
    ui->screenFrame->setStyleSheet(QString::fromUtf8("background-color: rgb(238, 238, 236);"));
    ui->screenTabs->setHidden(false);
    ui->screenFrame->setEnabled(true);

}

/**
 * Called when 5 seconds elapses after losing contact with skin during a therapy session
 */
void MainWindow::skinContactUpdate()
{
    //If therapy is not running again when timer ends, stop the therapy, set device to not treating and reset ui
    if(!device->getCurrSession()->getIsRunning()){

        device->stopSession(device->getCurrSession()->getLastDuration() - device->getCurrSession()->getDuration());

        device->setIsTreating(false);
        ui->notTreatingLabel->setText("Not Treating");

        ui->recordingLabel->setText("Not Recording");
        device->setRecording(false);

        ui->powerLevelBar->setValue(2);
        ui->powerLevelAdminValue->setValue(100);
        device->getBattery()->defaultBurnRate();

        ui->timerOnLabel->setStyleSheet(QString::fromUtf8("color: rgb(238,238,236);"));
    }
}

