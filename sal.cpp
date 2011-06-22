#include "sal.h"
#include "Workload.h"
#include "WattsUp.h"
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <fstream>
#include <cstring>
#include <unistd.h> // sleep()
#include <time.h>   // clock()
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;

void sigchld_handler(int signum)
{
#ifdef DEBUG
    cout << "child termination detected." << endl;
#endif
    Workload::next();
}

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
void get_jiffies(const int cpus, fstream& stat, int work_jiffies[], int total_jiffies[])
{
    stat.seekg(0, ios::beg); // go back to start of file
    stat.ignore(256, '\n'); // skip first line

    // Read through each cpu line
    int  column;
    for (int cpu=0; cpu<cpus; cpu++) {
        stat.ignore(4, ' '); // skip over the "cpu0" column
        total_jiffies[cpu] = work_jiffies[cpu] = 0; // reset

        // get each column
        for (int col=0; col<7; col++) {
            stat >> column;
            total_jiffies[cpu] += column;
            if (col < 4) {
                work_jiffies[cpu] += column;
            }
       }

#ifdef DEBUG
//        cout << "CPU" << cpu << " total_jiffies=" << total_jiffies[cpu] << ", work_jiffies=" << work_jiffies[cpu]  << endl;
#endif
        stat.ignore(256, '\n'); // skip to start of next line
    }

}

string generate_filename()
{
    time_t rawtime = time(NULL); // get UNIX time
    struct tm * timeinfo = localtime(&rawtime); // get broken-down time
    stringstream ss (stringstream::in | stringstream::out);
    ss.fill('0'); // leading zero

    ss << setw(2) << 1+timeinfo->tm_mon << "-"
       << setw(2) << timeinfo->tm_mday  << "-"
       << setw(2) << timeinfo->tm_hour  << "-"
       << setw(2) << timeinfo->tm_min   << "-"
       << setw(2) << timeinfo->tm_sec;

    return ss.str();
}

void log(const int cpus, fstream& stat, WattsUp & wu)
{
    int * work_jiffies1  = new int[cpus];
    int * total_jiffies1 = new int[cpus];
    int * work_jiffies2  = new int[cpus];
    int * total_jiffies2 = new int[cpus];

    get_jiffies(cpus, stat, work_jiffies1, total_jiffies1);
    sleep(1);  // commented out while using wattsup because it'll make us wait a second
    int watts = wu.getWatts();
    get_jiffies(cpus, stat, work_jiffies2, total_jiffies2);

    cout << "Watts*10 = " << watts << endl;
    for (int cpu=0; cpu<cpus; cpu++) {
        cout << "cpu" << cpu << "=" << (double)(work_jiffies2[cpu]-work_jiffies1[cpu])/(total_jiffies2[cpu]-total_jiffies1[cpu]) << endl;
    }

}

int main(int argc, char* argv[])
{
    // read config file
    // start logging power consumption
    // start logging system workload
    // fire off a sequence of 'stress' workloads

    // Generate base filename for log files
    string filename_base = generate_filename();
    cout << "Base filename = " << filename_base << endl;

    /* Set a signal handler for SIGCHLD (which we receive
     *  when a child (i.e. "stress") terminates
     *  This code is adapted from
     *  gnu.org/s/hello/manual/libc/Sigaction-Function-Example.html
     */
    struct sigaction sigchld_action;
    sigchld_action.sa_handler = sigchld_handler;
    sigemptyset(&sigchld_action.sa_mask);
    sigchld_action.sa_flags = 0;
    sigaction(SIGCHLD, &sigchld_action, NULL);

    /**
     * Set workload config
     */
    struct Workload_config workload_config;
    workload_config.cpu=2;
    workload_config.io=1;
    workload_config.vm=1;
    workload_config.vm_bytes=128;
    workload_config.hdd=0;
    workload_config.timeout=10;
    Workload::set_workload_config(&workload_config);

    // TODO find the number of physical CPUs http://software.intel.com/en-us/articles/intel-64-architecture-processor-topology-enumeration/
    // TODO figure out where laptop gets power consumption

    /**
     * Open /proc/stat (which gives us the CPU load)
     */
    fstream stat;
    stat.open("/proc/stat", fstream::in);
    if (stat.fail()) {
        cerr << "Error: failed to open /proc/stat" << endl;
        exit(1);
    }

    /**
     * Kick off first workload
     */
    cout << "Kicking off first workload..." << endl;
    Workload::next();

    cout << "Instantiating WattsUp..." << endl;
    WattsUp wu;

    /**
     * Log until workload finishes
     */
    while (!Workload::finished()) {
        log(4, stat, wu);
    }

    cout << "parent terminating" << endl;

    stat.close(); // close /proc/stat

    return 0;
}
