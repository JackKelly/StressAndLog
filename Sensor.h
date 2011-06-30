/*
 * Sensor.h
 *
 *  Created on: 27 Jun 2011
 *      Author: jack
 */

#ifndef SENSOR_H_
#define SENSOR_H_

/*
 * Interface for all "sensors"
 */
class Sensor {
public:

    /**
     * Call this function to send relevant data to the log
     */
    virtual void log() =0;
};

#endif /* SENSOR_H_ */
