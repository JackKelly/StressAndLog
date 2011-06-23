/*
 * sal.h
 *
 *  Created on: 5 Jun 2011
 *      Author: jack
 */

#ifndef SAL_H_
#define SAL_H_

#include <string>
#include "WattsUp.h"

using namespace std;

/**
 * Handler for the Posix SIGCHLD signal, which is fired whenever one of our children terminates.
 *
 * In our case, our only child is an instance of "stress".  So the only responsibility of this
 * function is to fire off the next workload
 */
void sigchld_handler(int signum);

/**
 * get jiffie counts from /proc/stat for all CPUs
 *
 * @param cpus   number of cpu cores (including hyperthreading.
 *               so a dual-core CPU with hyperthreading enabled would
 *               have cpus=4
 *
 * @param stat   the fstream for /proc/stat
 *
 * @param work_jiffies  return array with size = cpus
 * @param total_jiffied return array with size = cpus
 */
void get_jiffies(const int cpus, fstream& stat, int work_jiffies[], int total_jiffies[]);

/**
 * Generates the base of the filename of the form
 * MM-DD-HH-MM-SS
 * @returns base filename like 06-23-14-38-13
 */
string generate_filename();

/**
 * Output a line of the log file.
 */
void log_line(const int cpus, /**< Total number of logical CPU cores (including hyperthreading
                                   so a dual-core CPU with hyperthreading enabled would have cpus=4 */
              fstream& stat,  /**< the fstream for /proc/stat */
              WattsUp& wu,    /**< watts up object */
              int * workload_number,  /**< a pointer to a variable storing the current workload number */
              time_t start_time,   /**<  */
              fstream& log_file);  /**< opened log file */


#endif /* SAL_H_ */
