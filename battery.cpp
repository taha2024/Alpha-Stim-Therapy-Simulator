#include "battery.h"

/**
 * Battery's constructor, initializes the battery at 100% charge,
 * set counter for battery burn, and the burn rate.
 * Intial burn rate is 1% every 20 seconds
 *
 */
Battery::Battery()
{
   percentage = 100;
   burnCount = 0;
   burnRate = 20;
}


/**
 * Deconstructor for the Battery class
 */
Battery::~Battery(){

}

/**
 * Returns the current battery percentage
 * @return int representing the battery percentage
 */
int Battery::getBatteryPercentage(){
    return percentage;
}


/**
 * Increases the rate at which the battery is depleted.
 * Maximum is 1% every 10 seconds. This happens at 500mU and above.
 */
void Battery::increaseBurnRate(){

    //Set the burn rate lower.
    //Don't go below the max of 1% for every ten seconds
    burnRate = burnRate == 10 ? 10 : burnRate-1;
}


/**
 * Decreases the rate at which the battery is depleted
 * Minimum is 1% every 20 seconds. This happens when power level for
 * therapy is at 0, or the when the user is not in therapy, but the device
 * is turned on
 */
void Battery::decreaseBurnRate(){

    //Set the burn rate higher.
    //Don't go above the max of 1% for every 20 seconds
    burnRate = burnRate == 20 ? 20 : burnRate+2;
}


/**
 * Set the battery burn rate back to the default setting
 * Default is 1% every 20 seconds
 */
void Battery::defaultBurnRate(){ burnRate = 20; }


/**
 * When therapy starts,set burn rate to initial 1% for 18 seconds
 */
void Battery::intialTherapyBurnRate(){ burnRate = 18; }


/**
 * Depletes the percentage of the battery.
 * When burnCount / burnRate is >= 1, deplete the battery by 1%.
 * Deplete down to 0%
 */
void Battery::depleteBattery()
{
    //Increase the burn count
    burnCount += 1;

    int thresholdReached = burnCount / burnRate;

    //If the number of seconds has been reached to deplete the battery perecentage,
    //then deplete it and reset the burn count to 0
    if(thresholdReached >= 1){
        percentage = percentage == 0 ? 0 : percentage -1;
        burnCount = 0;
    }

}

/**
 * Setter for the battery percentage
 * @param choice is the new perecentage to set it to
 */
void Battery::setBatteryPercentage(int choice){ percentage = choice; }

/**
 * Set to true when battery reaches 5%, false otherwise
 * @param choice is true when the 5% warning has been displayed, false otherwise
 */
void Battery::setFiveWarning(bool choice){ fiveWarning = choice; }

/**
 * Get value of five warning boolean
 * @return true or false
 */
bool Battery::getFiveWarning(){ return fiveWarning; }
