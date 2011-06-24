/*
 * CPUstats.cpp
 *
 *  Created on: 24 Jun 2011
 *      Author: jack
 */

#include "CPUstats.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstring>

using namespace std;

CPUstats * CPUstats::instance = NULL;

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
    delete instance;
}

CPUstats * CPUstats::get_instance()
{
    if (instance == NULL) {
        instance = new CPUstats();
    }
    return instance;
}

/**
 * Find the current work jiffies and current total jiffies for each CPU
 *
 * num_cpu_lines MUST be set before this function is called
 * current_work_jiffied and current_total_jiffies must be int arrays allocated
 *     num_cpu_lines of space
 *
 * @param current_work_jiffies  = a return parameter
 * @param current_total_jiffies = a return parameter
 */
void CPUstats::get_jiffies(int current_work_jiffies[], int current_total_jiffies[])
{
    open_stat();

    /* Read through each cpu line */
    int  column;
    for (int cpu=0; cpu<num_cpu_lines; cpu++) {
        stat.ignore(4); // skip over the "cpu0" column
        current_total_jiffies[cpu] = current_work_jiffies[cpu] = 0; // reset

        // get each column
        for (int col=0; col<7; col++) {
            stat >> column;
            current_total_jiffies[cpu] += column;
            if (col < 3) {
                current_work_jiffies[cpu] += column;
            }
        }
        stat.ignore(256, '\n'); // skip to start of next line
    }

    stat.close();
}

void CPUstats::get_cpu_utilisation(int utilisation[])
{
    int * current_work_jiffies  = new int[num_cpu_lines];
    int * current_total_jiffies = new int[num_cpu_lines];
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
}

void CPUstats::open_stat()
{
    const char * STAT = "/proc/stat";

    stat.open(STAT, fstream::in);
    if ( ! stat.good() ) {
        cerr << "Failed to open " << STAT << endl;
        exit(1);
    }
}

int CPUstats::discover_num_cpu_lines()
{
    char first_column[4];
    first_column[3] = '\0'; // because istream::read does not add a sentinel char

    open_stat();

    int _num_cpu_lines = 0;

    while(true) {
        stat.read(first_column,3); // read the first 3 chars of the current line
        stat.ignore(256,'\n'); // go to next line
        if ( strcmp(first_column, "cpu")==0 ) {
            _num_cpu_lines++;
        } else {
            break;
        }
    }

    stat.close();

    return _num_cpu_lines;
}

int CPUstats::get_num_cpu_lines()
{
    return num_cpu_lines;
}
