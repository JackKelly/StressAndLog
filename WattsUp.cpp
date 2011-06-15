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
    initialiseResponse();
}

WattsUp::~WattsUp()
{
    wattsUpSerialPort.Close();
}

void WattsUp::initialiseResponse()
{
    response[0].name = "params";
    response[1].name = "watts*10";  // Tenths of Watts
    response[2].name = "volts*10";  // Tenths of Volts
    response[3].name = "miliamps";  // miliamps
    response[4].name = "Watt hours";     // Tenths of Watt hours
    response[5].name = "cost";   // Tenths of cents or other currency
    response[6].name = "mo kWh";
    response[7].name = "mo Cost";
    response[8].name = "max Watts*10";
    response[9].name = "max Volts*10";
    response[10].name = "max miliAmps";
    response[11].name = "min Watts*10";
    response[12].name = "min Volts*10";
    response[13].name = "min miliamps";
    response[14].name = "powerFactor";
    response[15].name = "dutyCycle";
    response[16].name = "powerCycle";
    response[17].name = "lineFrequency*10"; // Tenths of Hz
    response[18].name = "va*10";            // Tenths of volt-amps

    for (int i=0; i<MAX_PARAMS+1; i++) {
        response[i].value = -1;
    }
}

void WattsUp::openDevice()
{
    wattsUpSerialPort.Open("/dev/ttyUSB0", std::ios_base::in | std::ios_base::out);
    if ( ! wattsUpSerialPort.good() ) {
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

    cout << "Successfully established a serial connection." << endl;

    // send command to tell the meter to send data every second
    cout << "Sending command \"#C,W,18,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1;\" to wattsup to tell it to send us every parameter" << endl;
    wattsUpSerialPort << "#C,W,18,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1;" << endl;
    wattsUpSerialPort.ignore(100,'\n');     // Skip empty line returned after command given
    cout << "Sending command \"#L,W,3,E,1,1;\" to wattsup to set external logging interval" << endl;
    wattsUpSerialPort << "#L,W,3,E,1,1;" << endl; // (external logging, interval 1 - the last number)
    wattsUpSerialPort.ignore(100,'\n');     // Skip empty line returned after command given

}

int WattsUp::getWatts()
{
    getResponse();
    // set 2-second timer
    // receive

    return response[1].value;
}

void WattsUp::getResponse()
{
    //TODO 2 second timer

    // Step through a line of text from the meter, separating off each number
    char c;
    int count=0;

    // The first reading is the number of parameters.  We'll
    // set this to 18 to start with
    int & numParams = response[0].value = MAX_PARAMS;

    c = wattsUpSerialPort.peek();
    while (c != ';' && c != '\n' && c != '\0' && count<numParams) {
        if (isdigit(c)) {
            wattsUpSerialPort >> response[count++].value;
            if (count==1 && numParams>MAX_PARAMS) {
                cerr << "WattsUp meter has replied with " << numParams << " parameters, which is too many for this code to deal with." << endl;
                exit(1);
            }
        } else if (c=='_') { // No reading available
            response[count++].value = -1;
            wattsUpSerialPort.ignore(1); // jump over the underscore
        }

#ifdef DEBUG
//        if (count > 0) {
//            cout << "Reading " << count << "/" << numParams << " = " << response[count-1].name << "=" << response[count-1].value << endl;
//        }
#endif
        wattsUpSerialPort.ignore(1); // move 1 char along
        c = wattsUpSerialPort.peek();
    }

    // If necessary, fill the remaining params with -1
    if (numParams != MAX_PARAMS) {
        for (int i = numParams; i<MAX_PARAMS; i++) {
            response[i].value = -1;
        }
    }

    wattsUpSerialPort.ignore(100,'\n');     // Skip empty line
}

/*
int main()
{

    WattsUp wu;

    cout << "Watts x 10 = " << wu.getWatts() << endl;;

    return 0;
}
*/
