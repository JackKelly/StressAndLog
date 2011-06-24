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
#include "Diskstats.h"

using namespace std;

/**
 * Handler for the Posix SIGCHLD signal, which is fired whenever one of our children terminates.
 *
 * In our case, our only child is an instance of "stress".  So the only responsibility of this
 * function is to fire off the next workload
 */
void sigchld_handler(int signum);

/**
 * Generates the base of the filename of the form
 * MM-DD-HH-MM-SS
 * @returns base filename like 06-23-14-38-13
 */
string generate_filename();

/**
 * Output a line of the log file.
 */
void log_line(
               WattsUp& wu    /**< watts up object */
              ,int * workload_number  /**< a pointer to a variable storing the current workload number */
              ,time_t start_time   /**<  */
              ,fstream& log_file  /**< opened log file */
              );

#endif /* SAL_H_ */
