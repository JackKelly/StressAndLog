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

/**
 * Singleton class for returning CPU utilisation stats
 */
class CPUstats {
public:
    static CPUstats * get_instance();
    int get_num_cpu_lines();
    void get_cpu_utilisation(int utilisation[]);

protected:
    CPUstats();
    CPUstats(const CPUstats&);
    CPUstats & operator=(const CPUstats&);
    ~CPUstats();
private:

    void get_jiffies(int current_work_jiffies[], int current_total_jiffies[]);
    void open_stat();
    int discover_num_cpu_lines();

    static CPUstats * instance;

    std::fstream stat;

    /**
     * The number of lines in /proc/stat with "cpu" as the first column.
     * This number should be the number of logical cpus + 1
     */
    int num_cpu_lines;

    int * previous_work_jiffies;
    int * previous_total_jiffies;
};

#endif /* CPUSTATS_H_ */
