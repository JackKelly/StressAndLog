/*
 * Diskstats.cpp
 *
 *  Created on: 23 Jun 2011
 *      Author: jack
 */

#include "Diskstats.h"
#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>

using namespace std;

void get_diskstats(const int disks, int diskstats[])
{
    fstream dstatsfile;
    dstatsfile.open( "/proc/diskstats", fstream::in );
    if (!dstatsfile.good()) {
        cerr << "Failed to open /proc/diskstats" << endl;
        exit(1);
    }


    // Seek to the row which starts "sda"
    const int INDENT = 13; // distance from  beginning of line to the disk ID
    string disk_id, // the name of the disk for this line of diskstats file
           next_sdisk, next_hdisk, suffix;

    for (int disk=0; disk<disks; disk++) {
        suffix      = ('a' + disk);
        next_sdisk  = "sd" + suffix; // next_sdisk now reads "sda" or "sdb" or "sdc" etc...
        next_hdisk  = "hd" + suffix; // next_hdisk now reads "hda" or "hdb" or "hdc" etc...

        do {
            if (dstatsfile.eof()) {
                cerr << "Reached the end of the /proc/diskstats file before finding all the disks.  Perhaps the configuration for the number of disks is too high?" << endl;
                exit(1);
            }
            dstatsfile.ignore(256, '\n'); // skip to start of next line
            dstatsfile.ignore(INDENT);
            dstatsfile >> disk_id;
        } while (disk_id != next_sdisk); // && disk_id != next_hdisk);

        // Read through each column of figures in our current row
        // Skip the first 9 columns.
        for (int col=0; col<10; col++) {
            dstatsfile.ignore(256,' '); // skip 10 columns
        }
        dstatsfile >> diskstats[disk];
    }

    dstatsfile.close();

}

/*
int main()
{
    const int NUM_DISKS = 1;
    int diskstats[NUM_DISKS];

    get_diskstats(NUM_DISKS, diskstats);

    cout << diskstats[0] << endl;

    return 0;
}
*/
