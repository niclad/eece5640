// Nicolas Tedori
// EECE 5640
// Prof. Kaeli
// January 24, 2020
// Homework 1, Question 2

// pthreads implementation from Malith Jayaweera
// Link to article: https://malithjayaweera.com/2019/02/parallel-merge-sort/
// written on Febuary 5, 2019

// note: g++ -pthread -o test -std=c++11 tedori_hw1q2.cpp
// must include -pthread flag if using pthreads

#include <iostream>
#include <iomanip>
#include <pthread.h>
#include <ctime>
#include <chrono>
#include <random>

using namespace std;

// number settings
const int MAX_NUM = 10000;      // amount of numbers wanted
const int L_NUM = 100;          // lower limit for the numbers
const int U_NUM = 100000;       // upper limit for the numbers
int unsortedNumbers[MAX_NUM];   // initialize empty array of MAX_NUM integers
/* 
 * the array is made as a global variable so any threaded functions may access it
 * threaded programs can only take on input (arg) from pthread_create().
 * A possible solution to this would be to make an object where everything is a method
 * and the relevant array is a variable within.
 */

// thread settings
const int NUM_THREADS = 8; // this works -- manually change this value
const int NUM_PER_THREAD = MAX_NUM / NUM_THREADS;   // the amount of number each thread should sort
const int OFFSET = MAX_NUM % NUM_THREADS;           // the amount of numbers left over

int RandNum(); // works (i think)
void PrintArray(int *numArr, int arrLen);   // works -- tested twice
void MergeSort(int *arr, int left, int right);
void Merge(int *arr, int left, int mid, int right);
void* ThreadMerge(void *arg);
void MergeSections(int *arr, int num, int agg);

int main() {
     // fill the array
    for(int i = 0; i < MAX_NUM; i++) {
        unsortedNumbers[i] = RandNum();
    }
    // display the unsorted list
    cout << "Unsorted array:" << endl;
    PrintArray(unsortedNumbers, MAX_NUM);

    // start program timer
    //clock_t tStart = clock();
    auto tStart = chrono::high_resolution_clock::now();
    
    pthread_t threads[NUM_THREADS];

    // create the threads - fork
    // side note: long is >= 32 bit
    for(long int i = 0; i < NUM_THREADS; i++) {
        int rc = pthread_create(&threads[i], NULL, ThreadMerge, (void *)i);
        if(rc) {
            cout << "ERROR; return code from pthread_create() is" << rc << endl;
            exit(-1);
        }

        /*
        Other sidenote: this might not work since nothing is passed to 
        ThreadMerge from pthread_create(). Must be why og code assigned
        "arr" as a global variable. Though let's test and see...
        */
    }
    // join
    for (long int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    MergeSections(unsortedNumbers, NUM_THREADS, 1);

    // calculate end time
    //clock_t tEnd = clock();
    auto tEnd = chrono::high_resolution_clock::now();

    // display the sorted array
    cout << "Sorted array:" << endl;
    PrintArray(unsortedNumbers, MAX_NUM);

    // display the running time
    cout << fixed << setprecision(4) << "Time to run for " << NUM_THREADS 
         << " threads is " 
         << chrono::duration<double, milli>(tEnd - tStart).count() 
         << " seconds." << endl;

    // test for print function -- works
    /*
    int arr[12] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    int arrLen = *(&arr + 1) - arr;a
    PrintArray(arr, arrLen);
    */
}

// run merge sort
void MergeSort(int *arr, int left, int right) {
    if(left < right) {
        int mid = left + (right - left) / 2;
        MergeSort(arr, left, mid);
        MergeSort(arr, (mid + 1), right);
        Merge(arr, left, mid, right);
    }
}

// merge the given array
void Merge(int *arr, int left, int mid, int right) {
    int i = 0;
    int j = 0;
    int k = 0;
    int leftLen = mid - left + 1;
    int rightLen = right - mid;
    int leftArr[leftLen];
    int rightArr[rightLen];

    // copy elements to left array
    for(int i = 0; i < leftLen; i++) {
        leftArr[i] = arr[left + i];
    }

    // copy elements to right array
    for(int j = 0; j < rightLen; j++) {
        rightArr[j] = arr[(mid + 1) + j];
    }

    i = 0;
    j = 0;

    // choose from right or left arrays and copy
    while(i < leftLen && j < rightLen) {
        if(leftArr[i] <= rightArr[j]) {
            arr[left + k] = leftArr[i];
            i++;
        }
        else {
            arr[left + k] = rightArr[j];
            j++;
        }
        k++;
    }

    // copy any remaining elements to array
    while(i < leftLen) {
        arr[left + k] = leftArr[i];
        i++;
        k++;
    }

    while(j < rightLen) {
        arr[left + k] = rightArr[j];
        j++;
        k++;
    }
}

// assign work to threads to perform merge sort
void* ThreadMerge(void *arg) {
    // not quite sure what the logic is behind this...?

    int threadID = (long)arg;
    int left = threadID * NUM_PER_THREAD;
    int right = (threadID + 1) * NUM_PER_THREAD - 1;

    if(threadID == NUM_THREADS - 1) {
        right += OFFSET;
    }

    int mid = left + (right - left) / 2;

    if(left < right) {
        MergeSort(unsortedNumbers, left, right);
        MergeSort(unsortedNumbers, (left + 1), right);
        Merge(unsortedNumbers, left, mid, right);
    }
}

// merge sections of the array for this sub-array
void MergeSections(int *arr, int num, int agg) {
    for(int i = 0; i < num; i = i + 2) {
        int left = i * (NUM_PER_THREAD * agg);  // get left index
        int right = ((i + 2) * NUM_PER_THREAD * agg) - 1;   // get right index
        int mid = left + (NUM_PER_THREAD * agg) - 1;        // get middle index

        // if the right index is larger than the most amount of numbers
        // set to end index
        if (right >= MAX_NUM) {
            right = MAX_NUM - 1;
        }
        Merge(arr, left, mid, right);   // merge this section
    }

    // if theres still elements to be merged, merge 'em
    if((num / 2) >= 1) {
        MergeSections(arr, (num / 2), (agg * 2));
    }
}

// OrderTest

// generate a random number using mersenne twister engine
int RandNum() {
    random_device rd;      // get random number from hardware
    mt19937 eng(rd());     // seed generator - mersenne twister engine
    uniform_int_distribution<> distr(L_NUM, U_NUM);    // define the range [100, 100,000]

    return distr(eng);  // return a random number
}

// print the first and last bits of an array
void PrintArray(int *numArr, int arrLen) {
    // (if the array is less than a certain size, then just print the whole thing)

    int maxLinePrint = 11;  // print upto 11 lines

    cout << "*** BEGIN PRINT ***" << endl;

    // if the array can be displayed at once, print the whole thing
    if (arrLen <= maxLinePrint) {
        for(int i = 0; i < maxLinePrint; i++) {
            cout << numArr[i] << endl;
        }
    }
    else {  // o.w. print the first section and last section of the array
        int j;
        int arrIdx;
        for(int i = 0; i < maxLinePrint; i++) {
            // print first section of the array
            if(i < (maxLinePrint / 2)) {
                cout << numArr[i] << endl;
            }
            // print the last section of the array
            else if(i > (maxLinePrint / 2)) {
                j = maxLinePrint - i;
                arrIdx = arrLen - j;    // get the index for the last few items

                cout << numArr[arrIdx] << endl;
            }
            // print an elipses
            else {
                cout << "..." << endl;
            } 
        }
    }

    cout << "**** END PRINT ****" << endl;
}