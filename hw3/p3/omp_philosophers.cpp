#include <omp.h>
#include <iostream>
#include <unistd.h>
#include <string>
#include <random>
#include <ctime>
#include <chrono>
#include <iomanip>
using namespace std;

// global vars
omp_lock_t* forks;      // omp locks
int NUM_PHILOSOPHERS;   // number of philosophers
int MEALS;              // number of meals philosophers eat

// function declarations
void philosophize();
chrono::high_resolution_clock::time_point SetLap();


int main() {
    // do mainy stuff
    cout << "OpenMP Dining Philosophers" << endl;
    cout << "How many philosophers are dining: ";
    cin >> NUM_PHILOSOPHERS;

    // set number of meals
    int min = 10;
    int max = 100;
    //MEALS = min + (rand() % (max - min + 1));
    MEALS = 10;

    // make forks array
    forks = new omp_lock_t[NUM_PHILOSOPHERS];

    chrono::high_resolution_clock::time_point tstart = SetLap();

    // initialize the omp locks
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        omp_init_lock(&forks[i]);
    }

    // start philosopher threads
    #pragma omp parallel num_threads(NUM_PHILOSOPHERS)
    {
        philosophize();
    }

    // destroy the locks when done
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        omp_destroy_lock(&forks[i]);
    }

    chrono::high_resolution_clock::time_point tend = SetLap();

    cout << fixed << setprecision(8) << "Time for omp philosophers to eat: "
         << chrono::duration<double>(tend - tstart).count()
         << " seconds." << endl;

    // free the memory
    delete[] forks;
}

chrono::high_resolution_clock::time_point SetLap() {
    return chrono::high_resolution_clock::now();
}

void philosophize() {
    // wait for threads to start
    #pragma omp barrier

    int id = omp_get_thread_num();  // philosopher's number
    int leftFork;           // left fork index
    int rightFork;          // right fork index
    int tempFork;           // temp fork index
    bool lockStatus = true; // true for successful lock
    string msg;             // philosophers status message
    int tries = 2;          // tries before spinning

    // grab left fork first
    if (id < NUM_PHILOSOPHERS-1) {
        rightFork = id;
        leftFork = id + 1;
    } else {
        rightFork = 0;
        leftFork = id;
    }

    for (int mealNum = 0; mealNum < MEALS; mealNum++) {
        msg = "Philosopher " + to_string(id + 1) + " of " 
            + to_string(NUM_PHILOSOPHERS) + " is thinking.\n";
        cout << msg;

        tries = 2;

        do {
            omp_set_lock(&forks[leftFork]);
            if (tries > 0 ) {
                lockStatus = omp_test_lock(&forks[rightFork]);
            } else {
                omp_set_lock(&forks[rightFork]);
            }

            if(!lockStatus) {
                omp_unset_lock(&forks[leftFork]);
                tempFork = leftFork;
                leftFork = rightFork;
                rightFork = tempFork;
            } 
        } while (!lockStatus && mealNum < MEALS);

        if (lockStatus) {
            msg = "Philosopher " + to_string(id + 1) + " of " + to_string(NUM_PHILOSOPHERS) 
                + " is EATING with forks " + to_string(id + 1) + " and " 
                + to_string(((id + 1) % NUM_PHILOSOPHERS) + 1) + ".\n";
            cout << msg;

            usleep(100);

            msg = "Philosopher " + to_string(id + 1) + " of " + to_string(NUM_PHILOSOPHERS) 
                + " is done EATING. Freeing forks " + to_string(id + 1) + " and " 
                + to_string(((id + 1) % NUM_PHILOSOPHERS) + 1) + ".\n";
            cout << msg;

            omp_unset_lock(&forks[leftFork]);
            omp_unset_lock(&forks[rightFork]);
        }
    }
}
