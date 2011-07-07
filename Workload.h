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

    Workload();
    ~Workload();

    /**
     * Call next workload
     */
    struct Workload_config;
    void next();
    int * set_workload_config(struct Workload::Workload_config *
                              ,const bool _run_every_permutation);
    bool finished();

private:

    /***************************
     *   Member functions      *
     ***************************/
    void run_workload();

    char const * i_to_c(const int i, const bool s=false);

    void inc_current_workload_every();

    void inc_current_workload_perpendicular();

    int calc_num_permutations_full();

    int calc_num_permutations_orthogonal();

    void open_logfile(const std::string base);

    void calc_num_permutations();

    bool inc_element(int * current, int max);

    /***************************
     *   Member variables      *
     ***************************/

    int counter;
    int permutations;
    bool fin;
    std::fstream workload_log;

    /**
     * This structure stores the maximums for each config option
     */
    struct Workload_config * workload_config;

    /**
     * Stores last known config
     */
    struct Workload_config * current_workload;

    /**
     * Should we run every single permutation?
     * If set to TRUE then we will run every permutation
     * else we'll run each parameter on its own, up to the
     * maximum defined by the workload_config
     */
    bool run_every_permutation;

    enum {HDD, VM, IO, CPU} turn;
};

/**
 * Structure for storing configuration information
 */
struct Workload::Workload_config
{
    int cpu, io, vm, vm_bytes, hdd, timeout;
    std::string filename_base;
    time_t start_time;

    Workload_config() {}
    explicit Workload_config(const int _cpu, const int _io, const int _vm,
                    const int _vm_bytes, const int _hdd, const int _timeout,
                    const std::string _filename_base, const time_t _start_time) :
        cpu(_cpu), io(_io), vm(_vm), vm_bytes(_vm_bytes), hdd(_hdd),
        timeout(_timeout), filename_base(_filename_base), start_time(_start_time) {}

    friend std::ostream& operator<<(std::ostream& o, const Workload_config& wc);
};

typedef Singleton<Workload> WorkloadSingleton;

#endif /* WORKLOAD_H_ */
