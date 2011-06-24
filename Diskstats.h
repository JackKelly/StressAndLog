/*
 * Diskstats.h
 *
 *  Created on: 23 Jun 2011
 *      Author: jack
 */

#ifndef DISKSTATS_H_
#define DISKSTATS_H_

#include <fstream>
#include <sys/time.h>
#include "Singleton.h"

using namespace std;

class Diskstats {
public:
    Diskstats();
    ~Diskstats();
    int * get_utilisation();
    int get_num_disks();
private:
    void read_diskstats(int diskstats[]);

    fstream* open_diskstats();

    int discover_num_disks();

    timeval * get_time();

    int get_msecs_elapsed();

    int num_disks;

    int * prev_diskstats;

    const int INDENT; // distance from  beginning of line to the disk ID

    /**
     * The time that we last read the disk stats
     */
    timeval * prev_time;
};

typedef Singleton<Diskstats> DiskstatsSingleton;

#endif /* DISKSTATS_H_ */
