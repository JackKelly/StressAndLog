/*
 * CPUstats.cpp
 *
 *  Created on: 24 Jun 2011
 *      Author: jack
 */

#include "CPUstats.h"
#include "Sensor.h"
#include "Log.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstring>

using namespace std;

CPUstats::CPUstats()
{
    num_cpu_lines = discover_num_cpu_lines();
    previous_work_jiffies  = new int[num_cpu_lines];
    previous_total_jiffies = new int[num_cpu_lines];
    get_jiffies(previous_work_jiffies, previous_total_jiffies);
}

CPUstats::~CPUstats()
{
    delete [] previous_work_jiffies;
    delete [] previous_total_jiffies;
    previous_work_jiffies = NULL;
    previous_total_jiffies = NULL;
}

/**
 * Find the current work jiffies and current total jiffies for each CPU.
 *
 * num_cpu_lines MUST be set before this function is called
 * current_work_jiffied and current_total_jiffies must be int arrays allocated
 *     num_cpu_lines of space
 *
 * @return current_work_jiffies
 * @return current_total_jiffies
 */
void CPUstats::get_jiffies(int current_work_jiffies[], int current_total_jiffies[])
{
    fstream * stat = open_stat();

    /* Read through each cpu line */
    int  column;
    for (int cpu=0; cpu<num_cpu_lines; cpu++) {
        stat->ignore(4); // skip over the "cpu0" column
        current_total_jiffies[cpu] = current_work_jiffies[cpu] = 0; // reset

        // get each column
        for (int col=0; col<7; col++) {
            *stat >> column;
            current_total_jiffies[cpu] += column;
            if (col < 3) {
                current_work_jiffies[cpu] += column;
            }
        }
        stat->ignore(256, '\n'); // skip to start of next line
    }

    stat->close();
    delete stat;
}

/**
 * Calculate the CPU utilisation between now and the last time this function
 * or the constructor was called.
 *
 * @return utilisation array, one entry for each logical CPU.
 *         The calling function has responsibility for deleting this.
 */
int * CPUstats::get_cpu_utilisation()
{
    int * current_work_jiffies  = new int[num_cpu_lines];
    int * current_total_jiffies = new int[num_cpu_lines];
    int * utilisation           = new int[num_cpu_lines];
    get_jiffies(current_work_jiffies, current_total_jiffies);

    for (int cpu=0; cpu<num_cpu_lines; cpu++) {
        utilisation[cpu] =
                100 *
                ((double) ( current_work_jiffies[cpu]  - previous_work_jiffies[cpu]  ) /
                          ( current_total_jiffies[cpu] - previous_total_jiffies[cpu] ));
    }

    delete [] previous_work_jiffies;
    delete [] previous_total_jiffies;

    previous_work_jiffies  = current_work_jiffies;
    previous_total_jiffies = current_total_jiffies;

    return utilisation;
}

/**
 * Write CPU utilisation data to the log
 */
void CPUstats::log()
{
    int * cpu_utilisation = get_cpu_utilisation();

    Log * log = LogSingleton::get_instance();

    for (int cpu_line=0; cpu_line < num_cpu_lines; cpu_line++) {
        if (cpu_line==0) {
            log->log("CPUav", cpu_utilisation[cpu_line], "%");
        } else {
            log->log("CPU", cpu_line, cpu_utilisation[cpu_line], "%");
        }
    }

    delete [] cpu_utilisation;
}

/**
 * Open the virtual file /proc/stat.
 */
fstream * CPUstats::open_stat()
{
    fstream * stat = new fstream;

    const char * STAT = "/proc/stat";

    stat->open(STAT, fstream::in);
    if ( ! stat->good() ) {
        cerr << "Failed to open " << STAT << endl;
        exit(1);
    }

    return stat;
}

/**
 * Count the number of CPU lines in /proc/stat.
 *
 * See comments for num_cpu_lines for more details.
 *
 * @return the number of CPU lines in /proc/stat
 */
int CPUstats::discover_num_cpu_lines()
{
    char first_column[4];
    first_column[3] = '\0'; // because istream::read does not add a sentinel char

    fstream * stat = open_stat();

    int _num_cpu_lines = 0;

    while(true) {
        stat->read(first_column,3); // read the first 3 chars of the current line
        stat->ignore(256,'\n'); // go to next line
        if ( strcmp(first_column, "cpu")==0 ) {
            _num_cpu_lines++;
        } else {
            break;
        }
    }

    stat->close();
    delete stat;

    cout << "INFO: Detected " << _num_cpu_lines-1 << " logical CPU"
         << (_num_cpu_lines > 2 ? "s" : "") << "." << endl;

    return _num_cpu_lines;
}

/**
 * Get number of CPU lines in /proc/stat
 *
 * See comments for num_cpu_lines for more details.
 *
 * @return the number of CPU lines in /proc/stat
 */
int CPUstats::get_num_cpu_lines()
{
    return num_cpu_lines;
}
