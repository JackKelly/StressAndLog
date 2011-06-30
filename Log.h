/*
 * Log.h
 *
 *  Created on: 25 Jun 2011
 *      Author: jack
 *
 */

#ifndef LOG_H_
#define LOG_H_

#include <string>
#include <fstream>
#include <list>
#include "Singleton.h"
#include "Sensor.h"

using namespace std;

/**
 * This class is responsible for logging data to the screen
 * and to a file.  It takes information as a (name, value) pair.
 */
class Log {
public:
    Log();
    ~Log();
    void open_log(const string base);
    void add_sensor(Sensor* sensor);
    void log_sensors() const;
    void log(const string name, const int value, const string unit="");
    void log(const string name, const int id, const int value, const string unit="");
    void endl();
private:
    /***************************
     *   Member functions      *
     ***************************/

    string generate_filename(string base);


    /***************************
     *   Member variables      *
     ***************************/

    fstream log_file;
    bool first_column;
    bool header;
    list<Sensor *> sensors;
};

typedef Singleton<Log> LogSingleton;

#endif /* LOG_H_ */
