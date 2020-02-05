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


/**
 * @brief Let a philosopher think or eat, based on fork availability
 * 
 * @param aPhilo 
 * @return void* 
 */
void *philosophize(void *aPhilo) {
    Philosopher *philo = (Philosopher*)aPhilo;  // cast input to a Philosopher

    int failed;
    int triesRemaining;
    pthread_mutex_t *forkLeft;
    pthread_mutex_t *forkRight;
    pthread_mutex_t *forkTemp;
    string msg;

    while (RUNNING) {
        // think
        msg = "Philosopher " + to_string(philo->id + 1) + " of " 
            + to_string(NUM_PHILO) + " is thinking.\n";
        cout << msg;
        sleep( 1 + rand() % 10);

        forkLeft = philo->forkLeft;
        forkRight = philo->forkRight;

        triesRemaining = 4; // arbitrary number of tries to try eating

        do {
            failed = pthread_mutex_lock(forkRight);
            //if (triesRemaining > 0) {
                // help philosophers by deciding to pickup other forks
                failed = pthread_mutex_trylock(forkLeft);
            /*    
            } else {
                // if the philosopher is having trouble finding a fork, just block
                // until the philospher can eat
                failed = pthread_mutex_lock(forkLeft);
            }
            */

            // if a philosopher can't find both forks,
            // try picking up the other fork first
            // and letting an adjacent philosopher eat if they can
            if (failed) {
                pthread_mutex_unlock(forkRight);
                forkTemp = forkRight;
                forkRight = forkLeft;
                forkLeft = forkTemp;
                --triesRemaining;
            }
        } while (failed && RUNNING);

        // eat
        if (!failed) {
            msg = "Philosopher " + to_string(philo->id + 1) + " of " + to_string(NUM_PHILO) 
                + " is EATING with forks " + to_string(philo->id + 1) + " and " 
                + to_string(((philo->id + 1) % NUM_PHILO) + 1) + ".\n";
            cout << msg;
            sleep(1 + rand() % 10);
            msg = "Philosopher " + to_string(philo->id + 1) + " of " + to_string(NUM_PHILO) 
                + " is done EATING. Freeing forks " + to_string(philo->id + 1) + " and " 
                + to_string(((philo->id + 1) % NUM_PHILO) + 1) + ".\n";
            cout << msg;
            // let go of the forks
            pthread_mutex_unlock(forkLeft);
            pthread_mutex_unlock(forkRight);
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
    pthread_mutex_t forks[numPhilosophers];
    Philosopher philosophers[numPhilosophers];
    Philosopher *philo;

    // initialize mutexes
    for (int i = 0; i < numPhilosophers; i++) {
        int failed = pthread_mutex_init(&forks[i], NULL);

        if (failed) {
            cout << "Mutex init failed!" << endl;
            exit(1);
        }
    }

    for (int i = 0; i < numPhilosophers; i++) {
        philo = &philosophers[i];
        philo->id = i;
        philo->forkRight = &forks[i];
        philo->forkLeft = &forks[(i + 1) % numPhilosophers];
        philo->fail = pthread_create(&philo->thread, NULL, 
            philosophize, philo);
    }

    sleep(40);
    RUNNING = 0;
    cout << "PHILOSOPHERS ARE FINISHING." << endl;
    for (int i = 0; i < numPhilosophers; i++) {
        philo = &philosophers[i];

        if (!philo->fail && pthread_join(philo->thread, NULL)) {
            cout << "ERROR: joining thread " << philo->id << endl;
            exit(1);
        }
    }
}

// main func
int main() {
    /* DO STUFF */
    initialize();
}