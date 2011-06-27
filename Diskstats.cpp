/*
 * Diskstats.cpp
 *
 *  Created on: 23 Jun 2011
 *      Author: jack
 */

#include "Diskstats.h"
#include "Log.h"
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <cstring>

using namespace std;

Diskstats::Diskstats() :
        INDENT(13)
{
    num_disks = discover_num_disks();
    prev_diskstats = read_diskstats();
    prev_time = get_time();
}

Diskstats::~Diskstats()
{
    delete [] prev_diskstats;
    prev_diskstats = 0;

    delete prev_time;
    prev_time = 0;
}

/**
 * Get calendar time
 *
 * @return current calendar time
 */
timeval * Diskstats::get_time()
{
    timeval * tv = new timeval;
    if ( gettimeofday(tv, NULL) != 0 ) {  // get timestamp
        cerr << "Failed to get time stamp." << endl;
        exit(1);
    }
    return tv;
}

/**
 * Finds the number of milliseconds spent doing IO for each disk.
 *
 * Returns the 10th column of the /proc/diskstats file for each disk
 * For details of what each column in /proc/diskstats means, check
 * http://www.mjmwired.net/kernel/Documentation/iostats.txt
 *
 * It's the caller's responsibility to delete the returned array
 *
 * @return array of diskstats, 1 entry for each disk
 *
 */
int * Diskstats::read_diskstats()
{
    int * diskstats = new int[num_disks];

    fstream * dstatsfile = open_diskstats();

    // Seek to the row which starts "sda"
    string disk_id, // the name of the disk for this line of diskstats file
           next_sdisk, next_hdisk, suffix;

    for (int disk=0; disk<num_disks; disk++) {
        suffix      = ('a' + disk);
        next_sdisk  = "sd" + suffix; // next_sdisk now reads "sda" or "sdb" or "sdc" etc...
        next_hdisk  = "hd" + suffix; // next_hdisk now reads "hda" or "hdb" or "hdc" etc...

        do {
            if (dstatsfile->eof()) {
                cerr << "Reached the end of the /proc/diskstats file before finding all the disks.  Perhaps the configuration for the number of disks is too high?" << endl;
                exit(1);
            }
            dstatsfile->ignore(256, '\n'); // skip to start of next line
            dstatsfile->ignore(INDENT);
            *dstatsfile >> disk_id;
        } while (disk_id != next_sdisk); // && disk_id != next_hdisk);

        // Read through each column of figures in our current row
        // Skip the first 9 columns.
        for (int col=0; col<10; col++) {
            dstatsfile->ignore(256,' '); // skip 10 columns
        }
        *dstatsfile >> diskstats[disk];
    }
    dstatsfile->close();
    delete dstatsfile;

    return diskstats;
}

/**
 * Get the number of miliseconds since this function was last called.
 *
 * @return number of miliseconds since this function was last called.
 */
int Diskstats::get_msecs_elapsed()
{
    timeval * current_time = get_time();

    int msecs_elapsed = 0;

    // First get the number of seconds elapsed
    msecs_elapsed = (current_time->tv_sec - prev_time->tv_sec) * 1000;

    // Now add in microseconds
    msecs_elapsed += (current_time->tv_usec - prev_time->tv_usec) / 1000;

    delete prev_time;
    prev_time = current_time;

    return msecs_elapsed;
}

/**
 * Get the utilisation for each disk.
 *
 * @param utilisation[] is the returned disk utilisation, in percentage.  1 entry per disk.
 */
int * Diskstats::get_utilisation()
{
    int * current_diskstats = read_diskstats();
    int milliseconds_elapsed = get_msecs_elapsed();

    int * utilisation = new int[num_disks];

    LogSingleton::get_instance()->log("Elapsed", milliseconds_elapsed, "ms");

    for (int disk=0; disk<num_disks; disk++) {
        utilisation[disk] = 100 *
                ((double)(current_diskstats[disk] - prev_diskstats[disk]) /
                        milliseconds_elapsed);
    }

    delete [] prev_diskstats;
    prev_diskstats = current_diskstats;

    return utilisation;
}

/**
 * Open /proc/diskstats
 *
 * @return a pointer to an fstream object
 *         It's the caller's responsibilty to delete this
 */
fstream* Diskstats::open_diskstats()
{
    const char * DISKSTATS = "/proc/diskstats";
    fstream * dstatsfile = new fstream;
    dstatsfile->open( DISKSTATS, fstream::in );
    if ( ! dstatsfile->good() ) {
        cerr << "Failed to open " << DISKSTATS << endl;
        exit(1);
    }
    return dstatsfile;
}

/**
 * Read /proc/stat and figure out how many physical disks are installed
 *
 * @return number of physical disks installed
 */
int Diskstats::discover_num_disks()
{
    fstream * dstatsfile = open_diskstats();
    char device_id[3];
    device_id[2] = '\0';
    char ch[1];
    int num_disks = 0;

    while ( ! dstatsfile->eof() ) {
        dstatsfile->ignore(INDENT);
        dstatsfile->read(device_id, 2); // read "hd" or "sd" of "hda" or "sda" etc

        if ( strcmp(device_id, "hd")==0 || strcmp(device_id, "sd")==0 ) {
            dstatsfile->ignore(1);
            dstatsfile->read(ch,1);
            if (*ch == ' ') num_disks++;
        }

        dstatsfile->ignore(256, '\n'); // skip to start of next line
    }

    dstatsfile->close();
    delete dstatsfile;

    cout << "INFO: Detected " << num_disks
         << " physical disk" << (num_disks > 1 ? "s" : "") << " installed." << endl;

    return num_disks;
}

/**
 * Return the number of disks installed
 */
int Diskstats::get_num_disks()
{
    return num_disks;
}

/**
 * Send disk stats to the logs
 */
void Diskstats::log()
{
    int * disk_utilisation = get_utilisation();

    for (int disk=0; disk<num_disks; disk++) {
        LogSingleton::get_instance()->log("DISK", disk, disk_utilisation[disk], "%");
    }

    delete [] disk_utilisation;
}
