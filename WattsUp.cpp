/*
 * WattsUp.cpp
 *
 *  Created on: 13 Jun 2011
 *      Author: jack
 */

#include <SerialStream.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include "WattsUp.h"

using namespace std;

WattsUp::WattsUp()
{
    openDevice();
}

WattsUp::~WattsUp()
{
    wattsUpSerialPort.Close();
}

void WattsUp::openDevice()
{
    wattsUpSerialPort.Open("/dev/ttyUSB0", std::ios_base::in | std::ios_base::out);
    if ( ! wattsUpSerialPort.good()) {
        cerr << "Failed to open /dev/ttyUSB0" << endl;
        exit(1);
    }

    /* DATA FORMAT from WattsUp API doc:
     * https://www.wattsupmeters.com/secure/downloads/CommunicationsProtocol090824.pdf
     * RS232; 115200 baud, 8 data bits, 1 stop bit, no parity
     * Data consists of standard ASCII chars
     * Numbers represented as ASCII strings
     * Packets start with "#" and end with ";"
     */

    using namespace LibSerial;

    wattsUpSerialPort.SetBaudRate( SerialStreamBuf::BAUD_115200 );
    if ( ! wattsUpSerialPort.good() ) {
        cerr << "Failed to set baud rate" << endl;
        exit(1);
    }

    wattsUpSerialPort.SetCharSize( SerialStreamBuf::CHAR_SIZE_8 );
    if ( ! wattsUpSerialPort.good() ) {
        cerr << "Failed to set Char Size" << endl;
        exit(1);
    }

    wattsUpSerialPort.SetNumOfStopBits( 1 );
    if ( ! wattsUpSerialPort.good() ) {
        cerr << "Failed to set stop bits" << endl;
        exit(1);
    }

    wattsUpSerialPort.SetParity( SerialStreamBuf::PARITY_NONE );
    if ( ! wattsUpSerialPort.good() ) {
        cerr << "Failed to set parity" << endl;
        exit(1);
    }

    wattsUpSerialPort.SetFlowControl( SerialStreamBuf::FLOW_CONTROL_DEFAULT );
    if ( ! wattsUpSerialPort.good() ) {
        cerr << "Failed to set flow control" << endl;
        exit(1);
    }

    cout << "Serial baud rate = " << wattsUpSerialPort.BaudRate() << endl;

//    wattsUpSerialPort = new LibSerial::SerialStream(
//            "/dev/ttyUSB0"
//            ,LibSerial::SerialStreamBuf::BAUD_115200
//            ,LibSerial::SerialStreamBuf::CHAR_SIZE_8
//            ,LibSerial::SerialStreamBuf::PARITY_NONE
//            ,1
//            ,LibSerial::SerialStreamBuf::FLOW_CONTROL_DEFAULT
//    );

//    wattsUpSerialPort.flush();

}

int WattsUp::getWatts()
{
    // send command
    wattsUpSerialPort << "#L,W,3,E,1,1;" << endl; // (external logging, interval 1 - the last number)

    // Skip empty line returned after command given
    wattsUpSerialPort.ignore(100,'\n');

    getResponse();
    // set 2-second timer
    // receive
    return 0; // TODO remove
}

void WattsUp::getResponse()
{
    //TODO 2 second timer
//    const int BUFFER_SIZE = 32;
//    char input_buffer[BUFFER_SIZE];
//    wattsUpSerialPort.read(input_buffer, BUFFER_SIZE);
//    cout << "response=" << input_buffer << endl;

/*    char s;
    wattsUpSerialPort >> s;
    for (int i=0; i<1000; i++) {
        cout << "=" << s;
        wattsUpSerialPort >> s;
    }
*/

      char s[100];
      for (int i=0; i<5; i++) {
          wattsUpSerialPort.getline(s, 100);
          cout << "Response= " << s << flush;
      }
}

int main()
{

    cout << "SerialStreamBuf::BAUD_DEFAULT==" << LibSerial::SerialStreamBuf::BAUD_DEFAULT << endl;
    cout << "SerialStreamBuf::BAUD_INVALID==" << LibSerial::SerialStreamBuf::BAUD_INVALID << endl;
    WattsUp wu;

    wu.getWatts();

    return 0;
}
