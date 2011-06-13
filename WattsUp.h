/*
 * WattsUp.h
 *
 *  Created on: 13 Jun 2011
 *      Author: jack
 */

#ifndef WATTSUP_H_
#define WATTSUP_H_

#include <fstream>

using namespace std;

class WattsUp {
public:
    WattsUp();
    ~WattsUp();
    int connect();
    float getWatts();
private:

    /**
     * for /dev/ttyUSB0
     */
    fstream fd;

    /**
     * Port from wattsup.c
     * TODO add lots of error-checking
     */
    int open_device();

    /**
     * Literal port from wattsup.c
     */
    int setup_serial_device();
};

#endif /* WATTSUP_H_ */
