/*
 * Workload.cpp
 *
 *  Created on: 5 Jun 2011
 *      Author: jack
 */

#include "Workload.h"
#include <cstdlib>
#include <iostream>
#include <cstdio>
#include <fstream>
#include <cstring>
#include <time.h>

using namespace std;

Workload::Workload() :
        counter(0),
        fin(false),
        workload_config(NULL),
        current_workload(NULL)
{}

Workload::~Workload(){
    delete workload_config;
}

int Workload::calc_num_permutations_full()
{
    int p;

    // Calculate how many different permutations we're going to run
    p  = (workload_config->cpu ? (workload_config->cpu + 1) : 1);
    p *= (workload_config->io  ? (workload_config->io  + 1) : 1);
    p *= (workload_config->vm  ? (workload_config->vm  + 1) : 1);
    p *= (workload_config->hdd ? (workload_config->hdd + 1) : 1);
    p--; // because we can't run 'stress' with all zeros

    return p;
}

int Workload::calc_num_permutations_orthogonal()
{
    int p;

    // Calculate how many different permutations we're going to run
    p  = workload_config->cpu;
    p += (workload_config->io  ? (workload_config->io  + 1) : 0);
    p += (workload_config->vm  ? (workload_config->vm  + 1) : 0);
    p += (workload_config->hdd ? (workload_config->hdd + 1) : 0);
    p--; // because we can't run 'stress' with all zeros

    return p;
}

void Workload::open_logfile(const string base)
{
    string filename = "workload-log-";
    filename.append( base );
    filename.append( ".txt" );
    workload_log.open( filename.c_str(), fstream::out | fstream::app);
    if ( ! workload_log.good() ) {
        cerr << "ERROR: Failed to open workload log." << endl;
        exit(1);
    }

    // Create column headers
    workload_log << "workload#" << ","
                 << "time"  << ","
                 << "cpu" << ","
                 << "io" << ","
                 << "vm" << ","
                 << "vm_bytes" << ","
                 << "hdd" << ","
                 << "timeout"
                 << endl;
    workload_log.flush();
}

void Workload::calc_num_permutations()
{
    if ( run_every_permutation ) {
        permutations = calc_num_permutations_full();
    } else {
        permutations = calc_num_permutations_orthogonal();
    }

    cout << "INFO: Initialised workload_config. Running "
         << (run_every_permutation ? "every permutation" : "orthogonally")
         << ". Number of permutations = " << permutations << endl;

    int runtime = permutations*workload_config->timeout;
    cout << "INFO: Estimated runtime = " << runtime/60  << "mins." << endl;
}

/**
 * Set the workload config options
 * @param _workload_config - a pointer to a Workload_config structure
 *                           specifying the set of jobs we want to run
 * @param _run_every_permutation - should we run every single permutation
 *                           or should we run orthogonally?
 * @return the address of the counter (which keeps track of which job we're currently running.
 *                           This is useful for the log
 */
int* Workload::set_workload_config(struct Workload::Workload_config * _workload_config
                                  ,const bool _run_every_permutation)
{

    run_every_permutation = _run_every_permutation;
    workload_config = _workload_config;
    turn = HDD;

    calc_num_permutations();

    // Now initialise 'current_workload' to all zeros
    current_workload = new Workload_config(0,0,0,0,0,0,"",time(NULL));

    // Set the constant values in current_config
    current_workload->vm_bytes = workload_config->vm_bytes;
    current_workload->timeout  = workload_config->timeout;

    // Create log file
    open_logfile( workload_config->filename_base );

    return &counter; // return the address of the counter so we can keep track
}

/**
 * Convert from an integer to a char * string.
 *
 * @param i = the integer to convert
 * @param s = whether or not we want an 's' at the end
 * @return = the const char * string encoding the number. The caller has responsibility for deleting string.
 */
char const * Workload::i_to_c(const int i, const bool s)
{
    char * string = new char[(s ? 5 : 4)];
    string[0] = ((i/100)%10) + '0';
    string[1] = ((i/ 10)%10) + '0';
    string[2] =  (i%10)      + '0';
    string[3] = (s ? 's' : 0);
    if (s) {
        string[4] = 0;
    }
    return string;
}

/**
 * Run a workload specified by current_workload.
 *
 * The main responsibility of this function is to prepare the arguments for
 * the command-line utility "stress" and to run "stress".
 */
void Workload::run_workload()
{
    cout << "INFO: Running workload #" << counter << "/" << permutations << ": "
         << *current_workload << endl
         << "Estimated time remaining="
         << ((permutations-counter)*workload_config->timeout) << "s" << endl;

    pid_t childpid = fork();
    if (childpid >= 0) { // fork succeeded
        if (childpid == 0) { // child

            /* Figure out how many arguments we're passing to 'stress'
             * It turns out 'stress' doesn't like being given '0' as an argument */
            int argc = 2;
            argc += (current_workload->cpu ? 2 : 0);
            argc += (current_workload->io  ? 2 : 0);
            argc += (current_workload->vm  ? 2 : 0);
            argc += (current_workload->vm_bytes ? 2 : 0);
            argc += (current_workload->hdd ? 2 : 0);
            argc += (current_workload->timeout ? 2 : 0);

            /* Prepare the arguments for passing to "stress"  */
            char const * argv[argc];
            int argv_index = 0;
            argv[argv_index++] =  "stress";
            if (current_workload->cpu) {
                argv[argv_index++] = "--cpu";
                argv[argv_index++] = i_to_c(current_workload->cpu);
            }
            if (current_workload->io) {
                argv[argv_index++] = "--io";
                argv[argv_index++] = i_to_c(current_workload->io);
            }
            if (current_workload->vm) {
                argv[argv_index++] = "--vm";
                argv[argv_index++] = i_to_c(current_workload->vm);
            }
            if (current_workload->vm_bytes) {
                argv[argv_index++] = "--vm-bytes";
                argv[argv_index++] = i_to_c(current_workload->vm_bytes);
            }
            if (current_workload->hdd) {
                argv[argv_index++] = "--hdd";
                argv[argv_index++] = i_to_c(current_workload->hdd);
            }
            if (current_workload->timeout) {
                argv[argv_index++] = "--timeout";
                argv[argv_index++] = i_to_c(current_workload->timeout, true);
            }
            argv[argv_index++] = (char *)0;

            // Output a line to log
            workload_log << counter << ","
                         << time(NULL)-workload_config->start_time << ","
                         << current_workload->cpu << ","
                         << current_workload->io << ","
                         << current_workload->vm << ","
                         << current_workload->vm_bytes << ","
                         << current_workload->hdd << ","
                         << current_workload->timeout
                         << endl;
            workload_log.flush();
            /* Flush after each line so we get a meaningful log
             * even if the process is terminated prematurely */

            // Print out the argv[] string
            argv_index=0;
            while(argv[argv_index]) {
                cout << argv[argv_index++] << " ";
            }
            cout << endl;

            /* We're a child process so replace our image with 'stress'
             * execvp searches for the program in our path and passes our arguments */
            execvp("stress", (char* const*)argv);

            // Delete the dynamically allocated "char *"s
/*            argv_index=0;
            while(argv[argv_index]) {
                delete argv[argv_index];
                argv_index++;
            }
*/

            // Check for errors and handle
            perror("execvp"); /* execvp() only returns on error */
            cerr << "fatal error: execvp(\"stress\"...) failed" << endl;
            exit(EXIT_FAILURE);
        }
        else { // parent
        }
    }
    else { // fork failed
        // TODO handle fork failure more gracefully
        cerr << "ERROR: Fork failed" << endl;
        exit(1);
    }
}

void Workload::inc_current_workload_every()
{
    if (current_workload->hdd < workload_config->hdd) {
        current_workload->hdd++;
    } else if (current_workload->vm < workload_config->vm) {
        current_workload->hdd = 0;
        current_workload->vm++;
    } else if (current_workload->io < workload_config->io) {
        current_workload->hdd = 0;
        current_workload->vm  = 0;
        current_workload->io++;
    } else if (current_workload->cpu < workload_config->cpu) {
        current_workload->hdd = 0;
        current_workload->vm  = 0;
        current_workload->io  = 0;
        current_workload->cpu++;
    } else {
        fin = true;
    }
}

bool Workload::inc_element(int * current, int max)
{
    bool carry;

    if ( *current < max ) {
        (*current)++;
        carry = false;
    } else {
        *current = 0;
        carry = true;
    }

    return carry;
}

void Workload::inc_current_workload_perpendicular()
{
    bool carry = false;

    switch (turn) {
    case HDD:
        carry = inc_element(&current_workload->hdd, workload_config->hdd);
        if ( ! carry )
            break;
        else
            turn = VM;
    case VM:
        carry = inc_element(&current_workload->vm, workload_config->vm);
        if ( ! carry )
            break;
        else
            turn = IO;
    case IO:
        carry = inc_element(&current_workload->io, workload_config->io);
        if ( ! carry )
            break;
        else
            turn = CPU;
    case CPU:
        carry = inc_element(&current_workload->cpu, workload_config->cpu);
        if ( ! carry )
            break;
        else
            fin = true;
    }
}

/**
 * Run the next workload
 * The main task here is to increment the current_workload structure ready
 * for passing to run_workload
 */
void Workload::next()
{

    if ( run_every_permutation) {
        inc_current_workload_every();
    } else {
        inc_current_workload_perpendicular();
    }

    if ( ! fin ) {
        counter++;
        run_workload();
    } else {
        workload_log.close();
    }
}

bool Workload::finished()
{
    return fin;
}

ostream& operator<<(ostream& o, const Workload::Workload_config& wc)
{
    o << "cpu=" << wc.cpu << " io=" << wc.io << " vm=" << wc.vm << " vm_bytes=" << wc.vm_bytes
            << " hdd=" << wc.hdd << " timeout=" << wc.timeout;
    return o;
}
