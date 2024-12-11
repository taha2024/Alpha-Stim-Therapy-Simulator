#ifndef BATTERY_H
#define BATTERY_H

/*
Class: Battery

Purpose: This class represents the battery for the ces device

Usage: - Keeps track of the battery percent for the device
       - Can increase/decrease the rate at which battery is used up
       - Depletes battery percentage
       - Keeps track of when to warn user about 5% and 2% battery levels
       - Has getters/setters for percentage and fiveWarning
*/


class Battery
{

public:
    Battery();
    ~Battery();

    void increaseBurnRate();        //Increases the rate at which the battery is depleted
    void decreaseBurnRate();        //Decreases the rate at which the battery is depleted
    void defaultBurnRate();         //Set the battery burn rate back to the default setting
    void intialTherapyBurnRate();   //Set the battery burn rate when therapy starts
    void depleteBattery();          //Decreases the battery percent left using the burn rate

    //Getter/setters
    int getBatteryPercentage();             //Returns the percentage
    void setBatteryPercentage(int choice);  //Sets the battery perecentage to the specified choice. Only used for admin area
    void setFiveWarning(bool choice);       //Set to true when battery reaches 5% left
    bool getFiveWarning();                  //Get whether the five warning has already been displayed

private:
    int burnCount;                  //Counts the number of seconds since the last battery percentage depletion
    int burnRate;                   //The number of seconds it takes to burn a percentage off the battery
    int percentage;                 //The percentage of the battery
    bool fiveWarning;               //True when the battery reaches 5% otherwise false
};

#endif // BATTERY_H
