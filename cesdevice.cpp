#include "cesdevice.h"



/**
 * The basic constructor for the CESDevice
 * Sets up the battery and Session objects.
 * Sets recording, treating, skin contact status to false by default
 *
 * @param display is used for displaying therapy time
 * @param recordList is used for showing recorded therapies
 */
CESDevice::CESDevice(QLCDNumber* display, QListWidget* recordList)
{
    this->battery = new Battery();
    this->currentSession = new TherapySession(this);

    this->isOn = true;

    this->isTreating = false;
    this->isRecording = false;
    this->isContactingSkin = false;

    this->timerDisplay =  display;
    this->recordList = recordList;

    this->recordedSessionsIDs = 0;
}

/**
 * The basic deconstructor for the CESDevice
 * Delete the battery and therapy session
 */
CESDevice::~CESDevice()
{
    delete this->battery;
    delete this->currentSession;
}

/**
 * Increase the power of the current Therapy Session by 50mu
 * Increases the burn rate of the battery
 *
 */
void CESDevice::increasePower()
{
    int currentSessionPowerLevel = currentSession->getLastPowerLevel();

    this->currentSession->setLastPowerLevel(currentSessionPowerLevel == 10 ? 10 : currentSessionPowerLevel + 1);

    this->battery->increaseBurnRate();


}

/**
 * Decrease the power of the current Therapy Session by 100mu
 * Decreases the burn rate of the battery
 */
void CESDevice::decreasePower()
{

    int currentSessionPowerLevel = currentSession->getLastPowerLevel();

    this->currentSession->setLastPowerLevel(currentSessionPowerLevel - 2 < 1 ? 1 : currentSessionPowerLevel - 2);

    this->battery->decreaseBurnRate();
}

/**
 * Sets the selected frequency chosen by the user to the choice
 *
 * INPUT: int choice -> the chosen frequency
 * 0 - 0.5
 * 1 - 77
 * 2 - 100
 *
 * @param choice is the int representing the frequency selection
 */
void CESDevice::selectFrequency(int choice)
{
    this->currentSession->setFrequency(choice);

}


/**
 * Sets the selected waveform chosen by the user to the choice
 *
 * INPUT: int choice -> the chosen waveform
 * 0 - Alpha
 * 1 - Beta
 * 2 - Gamma
 *
 * @param choice is the int representing the waveform selection
 */
void CESDevice::selectWaveform(int choice)
{
    this->currentSession->setWaveform(choice);
}

/**
 * Sets the selected therapy time chosen by the user to the choice
 *
 * INPUT: int choice -> the chosen therapy time
 * 0 - 20 minutes
 * 1 - 40 minutes
 * 2 - 60 minutes
 *
 * @param choice is the int representing the time selection
 */
void CESDevice::selectTherapyTime(int choice)
{
    switch(choice)
    {
    case 0:
    {
        //The actual duration for the session
        this->currentSession->setDuration(20);

        //In case the user wants to restart the session
        this->currentSession->setLastDuration(20);

        //Update the display
        QTime time = QTime(0,0,0).addSecs(20 * 60);
        QString minuteSeconds = time.toString("mm:ss");
        this->timerDisplay->display(minuteSeconds);
        break;
    }
    case 1:
    {
        this->currentSession->setDuration(40);
        this->currentSession->setLastDuration(40);

        QTime time = QTime(0,0,0).addSecs(40 * 60);
        QString minuteSeconds = time.toString("mm:ss");
        this->timerDisplay->display(minuteSeconds);
        break;
    }
    case 2:
    {
        this->currentSession->setDuration(60);
        this->currentSession->setLastDuration(60);

        //Special case where Qtime converts 60 minutes to hh:mm:ss
        this->timerDisplay->display(QString::number(60)+":00");
        break;
    }
    default:
        //Shouldn't happen
        return;
    }
}


/**
 * Set the status of the system to recording.
 * This informs the system to store the data when the session ends or powers off.
 */
void CESDevice::startRecording() { this->isRecording = true; }


/**
 * Saves the current recording to the list of recordings.
 * Should only ever be called when the session ends or the system powers off.
 *
 * @param endTime -> the total time the session took (time(0) - startTime)
 */
QString CESDevice::saveRecording(int endTime)
{
    //Convert the epoch time startTime into a readable format
    const QDateTime dt = QDateTime::fromTime_t(currentSession->getStartTime());
    const QString textdate = dt.toString( Qt::DefaultLocaleShortDate );
    QString tempString = QString("[");
    tempString += textdate;
    tempString += QString("]");

    //Insert the ID into the string
    tempString += QString::fromStdString("\nID: " + std::to_string(recordedSessionsIDs));

    //Set the duration of the time into mm:ss format
    tempString += ", Duration: " + QString::number(endTime) + ":00";

    //Set the waveform of the string
    switch(currentSession->getWaveform())
    {
    case 0:
        tempString += QString(", Waveform: Alpha");
        break;
    case 1:
        tempString += QString(", Waveform: Betta");
        break;
    case 2:
        tempString += QString(", Waveform: Gamma");
        break;
    default:
        break;
    }

    //Set the Frequency of the string, shortened to Freq
    switch (currentSession->getFrequency())
    {
    case 0:
        tempString += QString(", Freq: 0.5Hz");
        break;
    case 1:
        tempString += QString(", Freq: 77Hz");
        break;
    case 2:
        tempString += QString(", Freq: 100Hz");
        break;
    default:
        break;
    }

    //Set the powerlevel
    tempString += QString::fromStdString(", Powerlevel: " + std::to_string(currentSession->getLastPowerLevel()));
    this->recordedSessionsIDs++; // increment the ID counter for future recordings
    return tempString;
}

/**
 * End the currently running session.
 * Save the session if record was selected
 *
 * @param endTime -> the total time the session took (time(0) - startTime)
 */
void CESDevice::stopSession(int endTime)
{

    //No active sessions
    if(!isTreating)
    {
        return;
    }


    //Otherwise stop the session
    //Record if it was selected
    if(isRecording)
    {
        this->recordList->insertItem(0, this->saveRecording(endTime));
        isRecording = false;
    }

    //Stop the device
    if(currentSession->getInternalClock()->getTimer()->isActive())
    {
        //Stop the internal clock if it is running
        currentSession->getInternalClock()->stopTimer();
    }

    this->isTreating = false;


}

/**
 * Update the display to decrement by 1 when ever a second passes
 */
void CESDevice::updateDisplay()
{
    QString minuteSeconds = "";
    int sessionDuration = this->currentSession->getDuration();

    if(sessionDuration < 60){
        QTime time = QTime(0,0,0).addSecs(this->currentSession->getDuration() * 60);
        minuteSeconds = time.toString("mm:ss");
    }else{
        minuteSeconds = QString::number(sessionDuration)+"00";
    }

    this->timerDisplay->display(minuteSeconds);
}

//Getters/Setters
Battery* CESDevice::getBattery(){ return this->battery; }
TherapySession* CESDevice::getCurrSession() { return this->currentSession; }

void CESDevice::setContact(bool state) { this->isContactingSkin = state; }
bool CESDevice::getContact(){ return this->isContactingSkin; }

bool CESDevice::getIsOn(){ return this->isOn; }
void CESDevice::setIsOn(bool choice){ this->isOn = choice; }

bool CESDevice::getIsTreating(){ return this->isTreating; }
void CESDevice::setIsTreating(bool choice){ this->isTreating = choice;  }

bool CESDevice::getRecording(){ return this->isRecording; }
void CESDevice::setRecording(bool choice){ this->isRecording = choice; }

void CESDevice::setIsDisabled(bool choice){ this->isDisabled = choice; }
bool CESDevice::getIsDisabled(){ return this->isDisabled; }
