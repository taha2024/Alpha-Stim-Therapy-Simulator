#include "therapysession.h"
#include "cesdevice.h"

/**
 * Constructor for the TherapySession class.
 * Sets the default settings for waveform, frequency,
 * duration, lastDuration, assigns timer for therapy
 *
 * @param creator is the CESDevice that created the therapy
 */
TherapySession::TherapySession(CESDevice* creator)
{
    waveform = 0;
    frequency = 0;
    duration = 20;
    lastDuration = 20;
    parent = creator;
    internalClock = new Timer(this);
}


/**
 * Deconstructor for TherapySession.
 */
TherapySession::~TherapySession()
{
    delete internalClock;
}


/**
 * Starts a new therapy session.
 * Sets the start time for the therapy session to
 * the current time, intial powerlevel, duration
 * Starts timer for the therapy
 */
void TherapySession::startSession()
{
    startTime = time(0);
    internalClock->startTimer();
    lastPowerLevel = 2;
    isRunning = true;
    duration = lastDuration;
    parent->setIsTreating(true);

}



/**
 * Get the waveform for the therapy session
 * @return 0 - Alpha, 1 - Betta, 2 - Gamma
 */
int TherapySession::getWaveform(){ return waveform; }


/**
 * Get the frequency of the therapy session
 * @return 0 - 0.5Hz, 1 - 77Hz, 2 - 100Hz
 */
int TherapySession::getFrequency(){ return frequency; }


/**
 * Get the last selected power level of the therapy
 * Will be one of the following:
 * 0 = 0uA, 1 = 50uA, 2 = 100uA, 3 = 150uA, 4 = 200uA, 5 = 250uA,
 * 6 = 300uA, 7 = 350uA, 8 = 400uA, 9 = 450uA, 10 = 500uA
 *
 * @return power level from 0 - 10
 */
int TherapySession::getLastPowerLevel(){ return lastPowerLevel; }


/**
 * Get the duration of the therapy in seconds
 * @return an int representing the number of seconds the therapy lasted
 */
int TherapySession::getDuration(){ return duration; }

/**
 * @brief Get the previously selected duration in seconds
 * @return an int representing the number of seconds to set the duration to
 */
int TherapySession::getLastDuration(){ return lastDuration; }


/**
 * @brief TherapySession::getIsRunning
 * @return True if the session is active, False otherwise
 */
bool TherapySession::getIsRunning() { return isRunning; }

/**
 * Gets the start time for the therapy
 * @return the start time as a time_t object
 */
time_t TherapySession::getStartTime(){ return startTime; }

/**
 * Gets the timer for the therapy
 * @return the timer object for the therapy
 */
Timer* TherapySession::getInternalClock() { return this->internalClock; }

/**
 * Set the waveform for the therapy session
 * @param choice is the waveform selection (0 - Alpha, 1 - Betta, 2 - Gamma)
 */
void TherapySession::setWaveform(int choice){ waveform = choice;}


/**
 * Set the frequency for the therpy
 * @param choice is the frequency (0 - 0.5Hz, 1 - 77Hz, 2 - 100Hz)
 */
void TherapySession::setFrequency(int choice){ frequency = choice; }


/**
 * Sets the power level for the therapy session.
 * Will be one of the following:
 * 0 = 0uA, 1 = 50uA, 2 = 100uA, 3 = 150uA, 4 = 200uA, 5 = 250uA,
 * 6 = 300uA, 7 = 350uA, 8 = 400uA, 9 = 450uA, 10 = 500uA
 *
 * @param level is the latest selected power level for the therapy
 */
void TherapySession::setLastPowerLevel(int level){ lastPowerLevel = level; }


/**
 * Sets the duration for the therapy session
 *
 * @param seconds is the new duration of the therapy
 */
void TherapySession::setDuration(int seconds){ duration = seconds; }

/**
 * Sets the previously selected duration of the Therapy
 * Allows for immediatly begining the session
 *
 * @param seconds the duration selected by the user
 */
void TherapySession::setLastDuration(int seconds){ lastDuration = seconds; }


/**
 * Function for when the timer times out
 * Call the parent CESDevice to update the display
 * Ends the therapy if duration time is at zero
 */
void TherapySession::timerTimeout()
{
    //Decrement by 1 second
    this->duration--;
    parent->updateDisplay();


    //Stop at 0 seconds
    if (this->duration <= 0)
    {
        //Stop the timer
        this->internalClock->stopTimer();

        //End the session when timer end
        int endofSession = lastDuration - duration;
        this->parent->stopSession(endofSession);

        this->isRunning = false;
    }
}

/**
 * Pauses the a therapy session
 */
void TherapySession::pauseSession()
{
    this->internalClock->stopTimer();
    this->isRunning = false;

}

/**
 * Resumes a paused therapy session
 */
void TherapySession::resumeSession()
{
    this->isRunning = true;
    this->internalClock->startTimer();
}
