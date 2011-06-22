/*
 * sal.h
 *
 *  Created on: 5 Jun 2011
 *      Author: jack
 */

#ifndef SAL_H_
#define SAL_H_

#include <string>

using namespace std;

void sigchld_handler(int signum);

/**
 * Generates the base of the filename of the form
 * MM-DD-HH-MM-SS
 */
string generate_filename();

#endif /* SAL_H_ */
