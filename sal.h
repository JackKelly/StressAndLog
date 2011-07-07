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

void sigchld_handler(int signum);

void set_sigchld_handler();

std::string generate_filename();

int * configure_workload(const std::string filename_base, const time_t start_time);

void log_line(
               WattsUp& wu    /**< watts up object */
              ,int * workload_number  /**< a pointer to a variable storing the current workload number */
              ,time_t start_time   /**<  */
              );

void log_and_run_workload(const time_t start_time, int * workload_number);

void register_sensors_with_logger();

#endif /* SAL_H_ */
