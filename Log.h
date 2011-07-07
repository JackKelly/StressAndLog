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

/**
 * This class is responsible for logging data to the screen
 * and to a file.  It takes information as a (name, value) pair.
 */
class Log {
public:
    Log();
    ~Log();
    void open_log(const std::string base);
    void add_sensor(Sensor* sensor);
    void log_sensors() const;
    void log(const std::string name, const int value, const std::string unit="");
    void log(const std::string name, const int id, const int value, const std::string unit="");
    void endl();
private:
    /***************************
     *   Member functions      *
     ***************************/

    std::string generate_filename(std::string base);


    /***************************
     *   Member variables      *
     ***************************/

    std::fstream log_file;
    bool first_column;
    bool header;
    std::list<Sensor *> sensors;
};

typedef Singleton<Log> LogSingleton;

#endif /* LOG_H_ */
