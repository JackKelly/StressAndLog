/*
 * Log.cpp
 *
 *  Created on: 25 Jun 2011
 *      Author: jack
 */

#include "Log.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>

using namespace std;

Log::Log()
{
}

Log::~Log()
{
    log_file.close();
}

/**
 * Generate a valid filename
 *
 * @param base - the base to use for the filename
 * @return the filename
 */
string Log::generate_filename(const string base)
{
    std::stringstream ss (stringstream::in | stringstream::out);
    ss << "stress-log-" << base << ".csv";
    return ss.str();
}

/**
 * Open the file to be used for logging
 *
 * @param base - the base filename
 */
void Log::open_log(const string base)
{
    string filename = generate_filename(base);
    log_file.open( filename.c_str(), fstream::out | fstream::app );
    if ( ! log_file.good() ) {
        cerr << "Cannot open " << filename << std::endl;
        exit(1);
    }
    first_column = true;
    header = true;
}

/**
 * Send information to the log file and to the screen
 *
 * @param name  - the name of the thing being logged (e.g. "Time elapsed")
 * @param value - the value (e.g. 1000)
 * @param unit  - the unit  (e.g. "ms")
 */
void Log::log(const string name, const int value, const string unit)
{
    // Log to screen
    cout << (first_column ? "" : ",") << name << "=" << value << unit;

    // Log to file
    // Should we log the column headers or the values?
    log_file << (first_column ? "" : ",");
    if (header) {
        log_file << name;
    } else {
        log_file << value;
    }

    log_file.flush();

    first_column = false;
}

/**
 * Send information to the log file and to the screen
 *
 * @param name  - the name of the thing being logged (e.g. "Time elapsed")
 * @param id    - the id (e.g. for CPU02 the id=2);
 * @param value - the value (e.g. 1000)
 * @param unit  - the unit  (e.g. "ms")
 */
void Log::log(const string name, const int id, const int value, const string unit)
{
    stringstream ss;
    ss << name << id;
    log( ss.str(), value, unit );
}

/**
 * Add a new sensor for logging
 *
 * @param sensor
 */
void Log::add_sensor(Sensor* sensor)
{
    sensors.push_back( sensor );
}

/**
 * Log all the sensors registered by add_sensor
 */
void Log::log_sensors() const
{
    for (list<Sensor *>::const_iterator it = sensors.begin(); it != sensors.end(); it++ ) {
        (*it)->log();
    }
}

/**
 * Send a new line character to log and to screen
 */
void Log::endl()
{
    log_file << std::endl;
    cout     << std::endl;
    first_column = true;
    header = false;
}
