/*
 * Workload.h
 *
 *  Created on: 5 Jun 2011
 *      Author: jack
 */

#ifndef WORKLOAD_H_
#define WORKLOAD_H_

#include <iostream>
#include <string>
#include <time.h>
#include <stdio.h>
#include <fstream>
#include "Singleton.h"

using namespace std;

/**
 * Runs the workloads. Meant to be used as a singleton with the Singleton template.
 *
 * The main responsibilities of this class include:
 *     prepare arguments for the command-line utility "stress"
 *     run "stress"
 *     keep track of which workloads we've run so far and which we still need to run
 */
class Workload
{
public:

    /**
     * Call next workload
     */
    struct Workload_config;
    void next();
    int * set_workload_config(struct Workload::Workload_config * );
    bool finished();
    Workload();

private:
    int counter;
    int permutations;
    bool fin;
    fstream workload_log;

    /**
     * This structure stores the maximums for each config option
     */
    struct Workload_config * workload_config;

    /**
     * Stores last known config
     */
    struct Workload_config * current_workload;

    void run_workload();

    char const * i_to_c(const int i, const bool s=false);
};

/**
 * Structure for storing configuration information
 */
struct Workload::Workload_config
{
    int cpu, io, vm, vm_bytes, hdd, timeout;
    string filename_base;
    time_t start_time;

    Workload_config() {}
    explicit Workload_config(const int _cpu, const int _io, const int _vm,
                    const int _vm_bytes, const int _hdd, const int _timeout, const string _filename_base, const time_t _start_time) :
        cpu(_cpu), io(_io), vm(_vm), vm_bytes(_vm_bytes), hdd(_hdd), timeout(_timeout), filename_base(_filename_base), start_time(_start_time) {}

    friend ostream& operator<<(ostream& o, const Workload_config& wc);
};

typedef Singleton<Workload> WorkloadSingleton;

#endif /* WORKLOAD_H_ */
