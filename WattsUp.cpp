/*
 * WattsUpWrapper.cpp
 *
 *  Created on: 13 Jun 2011
 *      Author: jack
 */

#include <fstream>
#include <cstdlib>
#include <termios.h>
#include "WattsUp.h"

using namespace std;

WattsUp::WattsUp()
{
    open_device();
}

WattsUp::~WattsUp()
{
    fd.close();
}

int WattsUp::open_device()
{
    //TODO add the error checking in the open_device function in wattsup.c
    fd.open ("/dev/ttyUSB0", fstream::in | fstream::out);
    if (fd.fail()) {
        cerr << "Failed to open /dev/ttyUSB0" << endl;
        exit(1);
    }
}

int WattsUp::setup_serial_device()
{
    struct termios t;
    int ret;

    ret = tcgetattr(fd. , &t);
    if (ret)
            return ret;

    cfmakeraw(&t);
    cfsetispeed(&t, B115200);
    cfsetospeed(&t, B115200);
    tcflush(fd, TCIFLUSH);

    t.c_iflag |= IGNPAR;
    t.c_cflag &= ~CSTOPB;
    ret = tcsetattr(dev_fd, TCSANOW, &t);

    if (ret) {
            perr("setting terminal attributes");
            return ret;
    }

    return 0;
}
