#include "sal.h"
#include "Workload.h"
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

using namespace std;

void sigchld_handler(int signum)
{
#ifdef DEBUG
    cout << "child termination detected." << endl;
#endif
    Workload::next();
}

int main(int argc, char* argv[])
{
    // read config file
    // start logging power consumption
    // start logging system workload
    // fire off a sequence of 'stress' workloads

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

    Workload::next();

    while (!Workload::finished()) {}

    cout << "parent terminating" << endl;

    return 0;
}
