// NOTE TO SELF: Set the semaphore init to be the numbers per thread +/-, so that the values count, print, count, print, etc., until all each thread prints and counts on its own

#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <string>
using namespace std;

// GLOBAL variables
const int NUM_THREADS = 8;		// the number of threads to use
const int UPPER_BOUND = 5000;	// the upper bound of the counting
const int NUM_PER_THREAD = UPPER_BOUND / NUM_THREADS;	// numbers to check per thread
int NUM_DIV = 0;		// number of numbers divisible by 3, 5, or both
sem_t sem[NUM_THREADS];	// semaphores

// FUNCTION DECLARATIONS
void* initCount(void* tID);
void countNum(int begin, int end, int threadID);
void printNum(int val);

int main() {
	cout << "Running counting program." << endl;
	cout << "Finding the amount of numbers between 1 and " << UPPER_BOUND 
		<< "\ndivisible by 3 and/or 5...." << endl;

	pthread_t tID[NUM_THREADS];		// threads

	// initialize semaphores
	sem_init(&sem[0], 0, 1);
	for (int i = 1; i < NUM_THREADS; i++) {
		sem_init(&sem[i], 0, 0);
	}

	// spawn the threads
	for (long i = 0; i < NUM_THREADS; i++) {
		int rc = pthread_create(&tID[i], NULL, initCount, (void *)i);

		// display an error if a thread cannot be created
		if (rc) {
			cout << "ERROR: return code from pthread_create() is " << rc << endl;
		}
	}

	// join the threads
	for (long i = 0; i < NUM_THREADS; i++) {
		pthread_join(tID[i], NULL);
	}

	cout << "Numbers divisible by 3 and/or 5: " << NUM_DIV << endl;
}	// end main

/**
 * @brief Initialize ranges for threads to count in
 * 
 * @param tID		Thread's ID passed from pthread_create 
 * @return void* 
 */
void* initCount(void* tID) {
	int threadID = (long) tID;	// get the thread's ID
	int beginVal = threadID * NUM_PER_THREAD + 1;	// starting val (i.e. 0->1)
	int endVal = (threadID + 1) * NUM_PER_THREAD;	// ending val

	// set the endVal for the highest thread to the upper bound (5000)
	if (threadID == (NUM_THREADS - 1)) {
		endVal = UPPER_BOUND;
	}

	countNum(beginVal, endVal, threadID);
	return NULL;
}

/**
 * @brief Print the value passed
 * 
 * @param val 		Value to print
 */
void printNum(int val) {
	string msg = to_string(val) + "\n";
	cout << msg;
}

/**
 * @brief Count the divisible numbers for the thread
 * 
 * @param begin 	Beginning value to count from
 * @param end 		Ending value to count to
 */
void countNum(int begin, int end, int threadID) {
	int semTest;	// value holding semaphore information

	// loop through counting numbers by NUM_THREADS-ths
	for (int i = begin; i <= end; i += 1) {
		semTest = 1;
		if ((i % 3) == 0) {
			NUM_DIV++;
			semTest = sem_wait(&sem[threadID]);
			printNum(i);
		} 
		else if ((i % 5) == 0) {
			NUM_DIV++;
			semTest = sem_wait(&sem[threadID]);
			printNum(i);
		}

		if (!semTest) {
			sem_post(&sem[threadID]);
		}
	}
	sem_post(&sem[(threadID+1) % NUM_THREADS]);
}