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

using namespace std;

int Workload::counter = 0;
int Workload::permutations;
bool Workload::fin = false;
struct Workload_config * Workload::workload_config  = 0;
struct Workload_config * Workload::current_workload = 0;

void Workload::set_workload_config(struct Workload_config * _workload_config) {
    workload_config = _workload_config;

    // Calculate how many different permutations we're going to run
    permutations  = (workload_config->cpu ? (workload_config->cpu + 1) : 1);
    permutations *= (workload_config->io  ? (workload_config->io  + 1) : 1);
    permutations *= (workload_config->vm  ? (workload_config->vm  + 1) : 1);
    permutations *= (workload_config->hdd ? (workload_config->hdd + 1) : 1);
    permutations--; // because we can't run 'stress' with all zeros

    cout << "Initialised workload_config.  Total permutations = " << permutations << endl;

    int runtime = permutations*workload_config->timeout;
    cout << "Estimated runtime = " << runtime/60   << "mins" << endl;

    // Now initialise 'current_workload' to all zeros
    current_workload = new Workload_config(0,0,0,0,0,0);

    // Set the constant values in current_config
    current_workload->vm_bytes = workload_config->vm_bytes;
    current_workload->timeout  = workload_config->timeout;
}

char * Workload::i_to_c(const int i, const bool s)
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

void Workload::run_workload()
{
    cout << "Running workload #" << counter << "/" << permutations << ": "
         << *current_workload << endl
         << "Estimated time remaining="
         << ((permutations-counter)*workload_config->timeout) << "s" << endl;

    pid_t childpid = fork();
    if (childpid >= 0) { // fork succeeded
        if (childpid == 0) { // child

            /**
             * Figure out how many arguments we're passing to 'stress'
             * It turns out 'stress' doesn't like being given '0' as an argument
             */
            int argc = 2;
            argc += (current_workload->cpu ? 2 : 0);
            argc += (current_workload->io  ? 2 : 0);
            argc += (current_workload->vm  ? 2 : 0);
            argc += (current_workload->vm_bytes ? 2 : 0);
            argc += (current_workload->hdd ? 2 : 0);
            argc += (current_workload->timeout ? 2 : 0);

            char * argv[argc];
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

            // Print out the argv[] string
            argv_index=0;
            while(argv[argv_index]) {
                cout << argv[argv_index++] << " ";
            }
            cout << endl;

            /**
             * We're a child process so replace our image with 'stress'
             * execvp searches for the program in our path and passes our arguments
             */
            execvp("stress", argv);

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
        exit(1);
    }
}

void Workload::next()
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

    if (!fin) {
        counter++;
        run_workload();
    }

}

bool Workload::finished()
{
    return fin;
}
