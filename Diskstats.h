/*
 * Diskstats.h
 *
 *  Created on: 23 Jun 2011
 *      Author: jack
 */

#ifndef DISKSTATS_H_
#define DISKSTATS_H_

#include <fstream>

using namespace std;

/**
 * Finds the number of milliseconds spent doing IO for each disk.
 *
 * Function returns the 10th column of the /proc/diskstats file for each disk
 * For details of what each column in /proc/diskstats means, check
 * http://www.mjmwired.net/kernel/Documentation/iostats.txt
 *
 * @param disks - the number of physical disks installed
 * @param dstatsfile - the opened /proc/diskstats fstream object
 * @param diskstats[] - the returned disk stats, one for each physical disk
 */
void get_diskstats(const int disks, int diskstats[]);

#endif /* DISKSTATS_H_ */
