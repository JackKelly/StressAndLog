#include "sal.h"
#include "Workload.h"
#include "WattsUp.h"
#include "CPUstats.h"
#include "Log.h"
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
#include <list>

using namespace std;

/**
 * Handler for the Posix SIGCHLD signal, which is fired whenever one of our children terminates.
 *
 * In our case, our only child is an instance of "stress".  So the only responsibility of this
 * function is to fire off the next workload
 */
void sigchld_handler(int signum)
{
    WorkloadSingleton::get_instance()->next();
}

/**
 * Generates the base of the filename of the form
 * MM-DD-HH-MM-SS
 * @returns base filename like 06-23-14-38-13
 */
string generate_filename()
{
    time_t rawtime = time(NULL); // get UNIX time
    struct tm * timeinfo = localtime(&rawtime); // get broken-down time

    // Now take the time and date info in timeinfo and format it
    // as a string in the way we want it
    stringstream ss (stringstream::in | stringstream::out);
    ss.fill('0'); // leading zero
    ss << setw(2) << 1+timeinfo->tm_mon << "-"
       << setw(2) << timeinfo->tm_mday  << "-"
       << setw(2) << timeinfo->tm_hour  << "-"
       << setw(2) << timeinfo->tm_min   << "-"
       << setw(2) << timeinfo->tm_sec;

    cout << "INFO: Base filename = " << ss.str() << endl;

    return ss.str();
}

/**
 *  Set a signal handler for SIGCHLD (which we receive
 *  when a child (i.e. "stress") terminates
 *  This code is adapted from
 *  gnu.org/s/hello/manual/libc/Sigaction-Function-Example.html
 */
void set_sigchld_handler()
{
    struct sigaction sigchld_action;
    sigchld_action.sa_handler = sigchld_handler;
    sigemptyset(&sigchld_action.sa_mask);
    sigchld_action.sa_flags = 0;
    sigaction(SIGCHLD, &sigchld_action, NULL);
}

/**
 *
 * @param filename_base
 * @param start_time
 * @return a pointer to the workload_number
 */
int * configure_workload(const string filename_base, const time_t start_time)
{
    /* Instantiate Workload singleton */
    Workload * workload = WorkloadSingleton::get_instance();

    /**
     * Set workload config
     */
    Workload::Workload_config * workload_config = new Workload::Workload_config();
    workload_config->cpu           =   4;
    workload_config->io            =   4;
    workload_config->vm            =   4;
    workload_config->vm_bytes      = 128;
    workload_config->hdd           =   1;
    workload_config->timeout       =  10;
    workload_config->filename_base = filename_base;
    workload_config->start_time    = start_time;
    int * workload_number = workload->set_workload_config(workload_config, false);
    return workload_number;
}

/**
 * Output a line of the log file.
 */
void log_line(int * workload_number, time_t start_time)
{
    Log * log = LogSingleton::get_instance();
    log->log("Time", (time(NULL) - start_time) );
    log->log("Workload", *workload_number);
    log->log_sensors();

    log->endl();
}

void log_and_run_workload(const time_t start_time, int * workload_number)
{
    Workload * workload = WorkloadSingleton::get_instance();

    /* Log until workload finishes */
    while ( ! workload->finished()) {

        log_line(workload_number, start_time);

        if ((time(NULL)-start_time) == 10) {
            /* Kick off first workload after 10 seconds */
            cout << "Kicking off first workload..." << endl;
            workload->next();
        }

        sleep(1); // wait 1 second
    }
}

/**
 * Register all sensors with logger
 */
void register_sensors_with_logger()
{
    Log * log = LogSingleton::get_instance();
    log->add_sensor(WattsUpSingleton::get_instance());
    log->add_sensor(CPUstatsSingleton::get_instance());
    log->add_sensor(DiskstatsSingleton::get_instance());
}

int main(int argc, char* argv[])
{
    string filename_base = generate_filename();

    LogSingleton::get_instance()->open_log(filename_base);

    time_t start_time = time(NULL);

    set_sigchld_handler();

    register_sensors_with_logger();

    int * workload_number;
    workload_number = configure_workload(filename_base, start_time);

    log_and_run_workload(start_time, workload_number);

    cout << "INFO: Parent process finished" << endl;

    return 0;
}
