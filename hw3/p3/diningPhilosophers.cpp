/**
 * @file diningPhilosophers.cpp
 * @author Nicolas Tedori (you@domain.com)
 * @brief Dining philosophers problem for EECE 5640
 * @version 0.1
 * @date 2020-02-07
 */

// in/out libraries
#include <iostream>
#include <iomanip>
#include <string>
// timing libraries
#include <ctime>
#include <chrono>
// numbers
#include <cmath>
#include <random>
// parallel
#include <pthread.h>
#include <unistd.h>

using namespace std;

int RUNNING = 1;
int NUM_PHILO = 0;
int MEALS = 10;
pthread_mutex_t* forks;

/**
 * @brief a data object to represent a philosopher
 * 
 */
struct Philosopher {
    pthread_mutex_t *forkLeft;  // mutex for left fork
    pthread_mutex_t *forkRight; // mutex for right fork

    int id;             // philosopher's id from 0 - (nPhilosophers - 1)
    pthread_t thread;   // thread for the philosopher
    int fail;           // 
};

chrono::high_resolution_clock::time_point SetLap() {
    return chrono::high_resolution_clock::now();
}

/**
 * @brief Let a philosopher think or eat, based on fork availability
 * 
 * @param aPhilo 
 * @return void* 
 */
void *philosophize(void *aPhilo) {
    Philosopher *philo = (Philosopher*)aPhilo;  // cast input to a Philosopher

    int id = (long) aPhilo;
    int leftFork;
    int rightFork;
    int tempFork;
    int lockStatus;
    /*
    int failed;
    int triesRemaining;
    pthread_mutex_t *forkLeft;
    pthread_mutex_t *forkRight;
    pthread_mutex_t *forkTemp;
    */

    string msg;
    int tries;
    int failed;

    if (id < NUM_PHILO-1) {
        rightFork = id;
        leftFork = id + 1;
    } else {
        rightFork = 0;
        leftFork = id;
    }

    for (int i = 0; i < MEALS; i++) {
        // think
        msg = "Philosopher " + to_string(id + 1) + " of " 
            + to_string(NUM_PHILO) + " is thinking.\n";
        cout << msg;

        /*
        forkLeft = philo->forkLeft;
        forkRight = philo->forkRight;
        */
        tries = 2; // arbitrary number of tries to try eating

        do {
            failed = pthread_mutex_lock(&forks[leftFork]);
            if (tries > 0) {
                // help philosophers by deciding to pickup other forks
                failed = pthread_mutex_trylock(&forks[rightFork]);
            } else {
                // if the philosopher is having trouble finding a fork, just block
                // until the philospher can eat
                failed = pthread_mutex_lock(&forks[rightFork]);
            }
            

            // if a philosopher can't find both forks,
            // try picking up the other fork first
            // and letting an adjacent philosopher eat if they can
            if (failed) {
                pthread_mutex_unlock(&forks[leftFork]);
                tempFork = leftFork;
                leftFork = rightFork;
                rightFork = tempFork;
                --tries;
            }
        } while (failed && (i < MEALS));

        // eat
        if (!failed) {
            msg = "Philosopher " + to_string(id + 1) + " of " + to_string(NUM_PHILO) 
                + " is EATING with forks " + to_string(id + 1) + " and " 
                + to_string(((id + 1) % NUM_PHILO) + 1) + ".\n";
            cout << msg;
            usleep(100);
            msg = "Philosopher " + to_string(id + 1) + " of " + to_string(NUM_PHILO) 
                + " is done EATING. Freeing forks " + to_string(id + 1) + " and " 
                + to_string(((id + 1) % NUM_PHILO) + 1) + ".\n";
            cout << msg;
            // let go of the forks
            pthread_mutex_unlock(&forks[rightFork]);
            pthread_mutex_unlock(&forks[leftFork]);
        }
    }
    return NULL;
}

void initialize() {
    // get the number of philosophers
    int numPhilosophers;
    cout << "DINING PHILOSOPHERS" << endl;
    cout << "Number of philosophers: ";
    cin >> numPhilosophers;

/*
    if ((numPhilosophers % 2) == 0) {
        cout << "Adding 1 to the number of philosophers." << endl;
        numPhilosophers++;
    }
*/

    NUM_PHILO = numPhilosophers;
    forks = new pthread_mutex_t[NUM_PHILO];
    Philosopher philosophers[numPhilosophers];
    Philosopher *philo;
    pthread_t threads[numPhilosophers];

    chrono::high_resolution_clock::time_point start = SetLap();

    // initialize mutexes
    for (int i = 0; i < numPhilosophers; i++) {
        int failed = pthread_mutex_init(&forks[i], NULL);

        if (failed) {
            cout << "Mutex init failed!" << endl;
            exit(1);
        }
    }

    for (long i = 0; i < numPhilosophers; i++) {
        pthread_create(&threads[i], NULL, philosophize, (void*) i);
    }

    /*
    for (int i = 0; i < numPhilosophers; i++) {
        philo = &philosophers[i];
        philo->id = i;
        philo->forkRight = &forks[i];
        philo->forkLeft = &forks[(i + 1) % numPhilosophers];
        philo->fail = pthread_create(&philo->thread, NULL, 
            philosophize, philo);
    }
    */

    for (int i = 0; i < numPhilosophers; i++) {
        //philo = &philosophers[i];
        pthread_join(threads[i], NULL);

/*
        if (!philo->fail && pthread_join(philo->thread, NULL)) {
            cout << "ERROR: joining thread " << philo->id << endl;
            exit(1);
        }
        */
    }

    chrono::high_resolution_clock::time_point end = SetLap();
    delete[] forks;
    //sleep(40);
    RUNNING = 0;
    cout << "PHILOSOPHERS ARE DONE." << endl;

    cout << fixed << setprecision(8) << "Time for philosophers to eat: "
         << chrono::duration<double>(end - start).count()
         << " seconds." << endl;
}

// main func
int main() {
    initialize();
}