/*
 * CPUstats.h
 *
 *  Created on: 24 Jun 2011
 *      Author: jack
 */

#ifndef CPUSTATS_H_
#define CPUSTATS_H_

#include <string>
#include <fstream>
#include "Singleton.h"
#include "Sensor.h"

/**
 * Singleton class responsible for returning CPU utilisation stats
 */
class CPUstats : public Sensor {
public:
    CPUstats();
    ~CPUstats();
    int get_num_cpu_lines();
    int * get_cpu_utilisation();
    virtual void log();

private:
    /***************************
     *   Member functions      *
     ***************************/

    void get_jiffies(int current_work_jiffies[], int current_total_jiffies[]);
    std::fstream * open_stat();
    int discover_num_cpu_lines();

    /***************************
     *   Member variables      *
     ***************************/

    /**
     * The number of lines in /proc/stat with "cpu" as the first column.
     * This number should be the number of logical cpus + 1
     */
    int num_cpu_lines;

    int * previous_work_jiffies;
    int * previous_total_jiffies;
};

typedef Singleton<CPUstats> CPUstatsSingleton;

#endif /* CPUSTATS_H_ */
