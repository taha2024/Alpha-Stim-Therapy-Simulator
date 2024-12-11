#ifndef TIMER_H
#define TIMER_H

#include <QTimer>

//Forward declare parent device:
class TherapySession;//Prevents circular dependency

/*
Class: Timer

Purpose: This class serves as a representation of a timer that can be used by the CES device.

Usage: Allows for extended use of the QTimer class including:
        - starting/resetting the timer
        - stopping the timer

       Also provides slot method for when the timer expires.
*/

class Timer : public QObject
{
    Q_OBJECT
public:
    Timer(TherapySession* creator);
    ~Timer();

    void startTimer();              //Starts or restarts the timer
    void stopTimer();               //Stops the timer

    //Getter
    QTimer* getTimer();             //Getter for the timer

private slots:
    void timerTimeout();            //Slot method for when the timer runs out

private:
    QTimer* timer;                  //Timer for the class (should end every 1 second)
    TherapySession* parent;         //Therapy session that created this class
};

#endif // TIMER_H
