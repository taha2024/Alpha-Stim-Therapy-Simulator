#ifndef CESDEVICE_H
#define CESDEVICE_H

#include <ctime>
#include <QTimer>
#include <QLCDNumber>
#include <QListWidget>
#include <iomanip>

#include "therapysession.h"
#include "battery.h"

/*
Class: CESDevice

Purpose: This class represents the main control of the CES Device

Usage: Controls the device, therapy sessions, the battery, and the therapy timer
       Provides the follosing functionality:
        - Can increase of decrease the power of the device
        - Sets the frequency, duration and waveform for the therapy session
        - Starts and stop sessions
        - Records therapy sessions
        - displays the therapy timer to the device
        - Provides getters/setters for battery, therapysession, skin contact, disabled status, treating status, recording status, and power status
*/


class CESDevice
{
public:
    CESDevice(QLCDNumber* display, QListWidget* recordList);
    ~CESDevice();

    void increasePower();                           //increase the power level by 50mu
    void decreasePower();                           //descrease the power level by 100mu
    void selectFrequency(int choice);               //Call currentSession and set the frequency
    void selectWaveform(int choice);                //call currentSession and set the waveform
    void selectTherapyTime(int choice);             //call currentSession and set the length of the therapy
    void startRecording();                          //Set the system to record the current session
    QString saveRecording(int endTime);             //Save the session to the list of sessions
    void updateDisplay();                           //Update the display whenever the timer times out
    void stopSession(int endTime);                  //End the current session immediatly

    //Getter/Setters
    TherapySession* getCurrSession();                   //Return the current session object
    Battery* getBattery();                              //Return the battery object
    bool getContact();                                  //Get whether device earclips are in contact with the skin
    void setContact(bool state);                        //Connect the earclips to the skin
    bool getIsOn();                                     //Get power status of the device
    void setIsOn(bool choice);                          //Set the power to on or off for the device
    bool getIsTreating();                               //Get whether the device is currently treating or not
    void setIsTreating(bool choice);                    //Sets whether the device is treating or not
    void setRecording(bool choice);                     //Set whether the device will record a therapy or not
    bool getRecording();                                //Get whether the device will record a therapy or not
    void setIsDisabled(bool choice);                    //Set whether the device is disbaled or not
    bool getIsDisabled();                               //Get whether the device is disabled or not

private:
    TherapySession* currentSession;                 //The current session of the machine
    int recordedSessionsIDs;                        //Count of the recorded session IDs
    Battery* battery;                               //Simulate the battery
    QLCDNumber* timerDisplay;                       //Allow the device to update the timer itself
    QListWidget* recordList;                        //Allow device to update the list of records
    bool isContactingSkin;                          //Are the earclips connected to the skin
    bool isOn;                                      //Is the power on or not
    bool isDisabled;                                //Has the device has been "permanently" disabled or not
    bool isRecording;                               //Should the system save the session when it ends
    bool isTreating;                                //Is the device treating or not
};

#endif // CESDEVICE_H
