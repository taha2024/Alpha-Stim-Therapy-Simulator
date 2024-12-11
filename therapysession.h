#ifndef THERAPYSESSION_H
#define THERAPYSESSION_H

#include <ctime>
#include <QTime>
#include <timer.h>

//Forward declare parent
class CESDevice; //Prevents circular dependency

/*
Class: TherapySession

Purpose: This class serves as a representation of a therapy session for the CES device.

Usage: Keeps track of the current settings for a therapy session including:
        - selected waveform
        - selected frequency
        - last selected power level
        - duration and last duration of the therapy
        - the start time of the therapy
        - whether or not the therapy was recorded

       Provides getters and setters for these statuses.
*/

class TherapySession
{
public:
    TherapySession(CESDevice *creator);
    ~TherapySession();

    void startSession();        //Sets the start time for the session
    void pauseSession();        //Pause current therapy session
    void resumeSession();       //Resume a paused therapy session
    void timerTimeout();        //Method called when the timer runs down

    //Getters
    int getWaveform();          //Return the selected waveform of the therapy
    int getFrequency();         //Return the selected frequency of the therapy
    int getLastPowerLevel();    //Return the last selected power level for the therapy
    int getDuration();          //Return the duration of the therapy (in seconds)
    int getLastDuration();      //Return the previously selected duration of the therapy (in seconds)
    bool getIsRunning();        //Return whether or not the session is active
    time_t getStartTime();      //Return the start time of the therapy
    Timer* getInternalClock();  //Return the timer object

    //Setters
    void setWaveform(int choice);       //Set the waveform for the therapy
    void setFrequency(int choice);      //Set the frequency for the therapy
    void setLastPowerLevel(int level);  //Set the last power level of the therapy
    void setDuration(int seconds);      //Set the duration of the therapy (in seconds)
    void setLastDuration(int seconds);  //Set the last duration selected to allow therapy to restart properly

private:
    int waveform;           //The selected waveform for the therapy
    int frequency;          //The selected frequency for the therapy
    int lastPowerLevel;     //The last selected power level for the therapy
    int duration;           //The duration of the therapy (in seconds)
    int lastDuration;       //The previously selected duration (in seconds)
    bool isRunning;         //Whether or not the session is active
    time_t startTime;       //The start time for the therapy
    CESDevice* parent;      //The CESDevice that made the therapy session, only used for call backs (maybe wipe when recording is done NOT delete)
    Timer* internalClock;   //A clock that keeps track of the seconds passing
};

#endif // THERAPYSESSION_H
