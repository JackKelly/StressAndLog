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
#include "Log.h"

using namespace std;

WattsUp::WattsUp()
{
    open_device();
    cout << "Successfully established a serial connection." << endl;
    send_init_command();
    initialise_response();
}

WattsUp::~WattsUp()
{
    wattsUpSerialPort.Close();
}

/**
 * Initialise response member variable array with the
 * names of each item
 */
void WattsUp::initialise_response()
{
    response[0].name  = "params";
    response[1].name  = "watts*10";  // Tenths of Watts
    response[2].name  = "volts*10";  // Tenths of Volts
    response[3].name  = "miliamps";  // miliamps
    response[4].name  = "Watt hours";     // Tenths of Watt hours
    response[5].name  = "cost";   // Tenths of cents or other currency
    response[6].name  = "mo kWh";
    response[7].name  = "mo Cost";
    response[8].name  = "max Watts*10";
    response[9].name  = "max Volts*10";
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

/**
 * Open serial connection
 */
void WattsUp::open_device()
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
     * Packets start with "#" and end with ";"     */

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
}

/**
 * Send the command to initialise the WattsUp
 */
void WattsUp::send_init_command()
{
    // send command to tell the meter to send data every second
    cout << "Sending command \"#C,W,18,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1;\" to wattsup to tell it to send us every parameter" << endl;
    wattsUpSerialPort << "#C,W,18,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1;" << endl;
    wattsUpSerialPort.ignore(100,'\n');     // Skip empty line returned after command given
    cout << "Sending command \"#L,W,3,E,1,1;\" to wattsup to set external logging interval" << endl;
    wattsUpSerialPort << "#L,W,3,E,1,1;" << endl; // (external logging, interval 1 - the last number)
    wattsUpSerialPort.ignore(100,'\n');     // Skip empty line returned after command given
}

/**
 * Get the current number of watts
 *
 * @returns Current number of Watts*10
 */
int WattsUp::get_deci_watts()
{
    get_response();

    return response[1].value;
}

void WattsUp::log()
{
    LogSingleton::get_instance()->log("deciWatts", get_deci_watts());
}

/**
 * Step comma-separated unit of text from the meter, separating off each number
 */
void WattsUp::get_response()
{
    //TODO 2 second time-out timer

    int count=0;

    // The first element of the response is the number of parameters to follow
    // We'll set this to MAX_PARAMS to start with, until we get the actual reading
    int & numParams = response[0].value = MAX_PARAMS;

    char c = wattsUpSerialPort.peek();
    while (    c != ';'
            && c != '\n'
            && c != '\0'
            && count < numParams) {

        // Check that the connection is still OK
        if ( ! wattsUpSerialPort.good() ) {
            // If the connection is no good then attempt to reconnect
            cout << "WARNING: Serial connection failed... reconnecting..." << endl;
            wattsUpSerialPort.Close();
            open_device();
            wattsUpSerialPort >> c;
            continue;
        }

        // If the reading is a digit then pull in the entire number
        // and store it in our 'response[]' array
        if ( isdigit(c) ) {
            wattsUpSerialPort >> response[count].value;
            count++;

            // Check that the WattsUp isn't trying to send us more
            // values than we can deal with
            if (count==1 && numParams>MAX_PARAMS) {
                cerr << "ERROR: WattsUp meter has replied with " << numParams
                     << " parameters, which is too many for this code to deal with." << endl;
                exit(1);
            }

        } else if (c=='_') { // No reading available
            response[count++].value = -1;
            wattsUpSerialPort.ignore(1); // jump over the underscore
        }

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
