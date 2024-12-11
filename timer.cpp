#include "timer.h"
#include "therapysession.h"

/**
 * Constructor for the Timer class. Initializes the qtimer and sets it up as a precise,
 * singleshot timer. Connects the slot for timeout of the timer as well.
 */
Timer::Timer(TherapySession* creator)
{
    timer = new QTimer(this);
    timer->setTimerType(Qt::PreciseTimer);
    timer->setInterval(1000); //1sec
    parent = creator;
    connect(timer, &QTimer::timeout, this, &Timer::timerTimeout);
}


/**
 * Deconstructor for the timer class
 */
Timer::~Timer()
{
    delete timer;
}


/**
 * Starts or restarts the timer with 1 second
 */
void Timer::startTimer()
{
    timer->start(1000);
}


/**
 * Stops the timer
 */
void Timer::stopTimer()
{
    timer->stop();

}


/**
 * Slot method for when the timer times out
 */
void Timer::timerTimeout()
{
    parent->timerTimeout();
}

//Getters
QTimer* Timer::getTimer() { return this->timer; }


