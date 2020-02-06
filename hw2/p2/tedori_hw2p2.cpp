// prime threads test
#include <algorithm>
#include <chrono>
#include <climits>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <pthread.h>
#include <random>
#include <sstream>
#include <string>

using namespace std;

// global variables
int MAX_PRIMES;         // maximum number of primes that can be found
int UPPER_LIMIT;        // the upper limit of where to find primes
int NUM_THREADS;        // number of threads to operate on
int NUM_PER_SEG;        // numbers per a segment
int SEGMENT_SIZE;       // size of a segment
int OFFSET;             // segment offset -- left over numbers for a segment
int NUM_OFFSET;         // offset for the number range
char* PRIME_TEST;       // list to test for PRIMALITY
int NUM_PRIMES = 0;     // the number of primes found so far (so far, 0 primes found)
pthread_mutex_t lock;   // lock, really only used for testing (so far)
int IDX_PER_THREAD;     // INDICES to sort through PER a thread
int NUM_PER_THREAD;     // NUMBERS to check for primality PER a thread
ofstream PRIME_FILE;    // file to save prime numbers to

/**
 * @brief Fill an array with chars of val "1"
 *        1 represent TRUE for prime and 0 o.w.
 * 
 * @param fillSize 
 * @return char* 
 */
char* FillArray(int fillSize) {
    char* primeList = new char[fillSize];
    for(int i = 0; i <= fillSize; i++) {
        primeList[i] = 1;
    }

    return primeList;
}

/**
 * @brief Calculate prime numbers from a list of booleans.
 *        Could be sped up with parallelization, like segmented sieve.
 * 
 * @param primeResults 
 */
void FindPrimes(int*& primeResults, bool saveFile) {
    primeResults[0] = 2;
    int primeIndex = 1;

    if(saveFile) {
        PRIME_FILE << NUM_PRIMES << " found up to " << UPPER_LIMIT 
            << " on " << NUM_THREADS << " threads.\n";
        for(int i = 1; i <= MAX_PRIMES; i++) {
            if(PRIME_TEST[i]) {
                primeResults[primeIndex] = ((i - 1) * 2) + 3;
                PRIME_FILE << primeResults[primeIndex] << endl;
                primeIndex++;
            }
        }  
    } else {
        for(int i = 1; i <= MAX_PRIMES; i++) {
            if(PRIME_TEST[i]) {
                primeResults[primeIndex] = ((i - 1) * 2) + 3;
             primeIndex++;
            }
        }  
    }
       
}

/**
 * @brief Check for a multiple of a prime.
 *        Basically, check to see if a small group of primes has already been tested.
 *        Prevents checking the same number twice, like 15 for primes 3 and 5.
 * 
 * @param num 
 * @return true 
 * @return false 
 */
bool NumIsPrime(int num) {
    // could go on for every prime found but then we get Zeno's Arrow :(
    return ((num >= 3*3) && (num % 3 == 0)) || ((num >= 5*5) && (num % 5 == 0))
        || ((num >= 7*7) && (num % 7 == 0)) || ((num >= 11*11) && (num % 11 == 0))
        || ((num >= 13*13) && (num % 13 == 0)); 
}

/**
 * @brief Sieve through segments of primes, checking for multiples of that prime.
 *        Only works on prime lists that exclude ALL even numbers (except 2).
 *        One must be *very* careful when passing values. If vals don't align,
 *        catastrophic results may occur! Output is number of primes found for
 *        this section. A little weird, but see Sieve of Eratosthenes on Wikipedia.
 * 
 * @param start 
 * @param end 
 * @param beginIdx 
 * @param endIdx 
 * @param threadID 
 * @return int 
 */
int FilterPrimeSeg(const int start, const int end, 
    const int beginIdx, const int endIdx, long threadID) {

    /*
    // create a temp array for this segment
    const int segSize = (end - start + 1) / 2;
    char* segPrimes = FillArray(segSize);
    //cout << segSize << endl;
    */

    //cout << beginIdx << endl;
    //cout << endIdx << endl;
    //cout << endIdx - beginIdx + 1 << endl;
    
    for (int i = 3; (i * i) <= end; i += 2) {
        
        if (NumIsPrime(i)) {
            continue;
        }
        

        int minMultiple = ((start + i - 1)/ i) * i;
        if (minMultiple < (i * i)) {
            minMultiple = i * i;
        }

        // force min to be odd
        if ((minMultiple % 2) == 0) {
            minMultiple += i;
        }

        // find all odd non-primes
        for (int j = minMultiple; j <= end; j += 2 * i) {
            //int idx = (j - start);
            PRIME_TEST[j / 2] = 0;
        }
    }

    // i = 0, i < segSize for true segmentation
    // i = beginIdx, i <= endIdx for inplace segmentation
    int numPrimesFound = 0;
    for (int i = beginIdx; i <= endIdx; i++) {
        if(PRIME_TEST[i]) {
            numPrimesFound++;
        }
        //numPrimesFound += PRIME_TEST[i];
    }
    string msg = "threadID: " + to_string(threadID) + "; primes found: " + to_string(numPrimesFound) + "\n"
    + "Idx range: [" +to_string(beginIdx) +","+to_string(endIdx)+"]\n"
    + "Num range: [" +to_string(start) +","+to_string(end)+"]\n";
    cout << msg << endl;

    //delete[] segPrimes;
    return numPrimesFound;
}

/**
 * @brief Runs FilterPrimeSeg. for() loop is unnecessary.
 * 
 * @param startNum 
 * @param endNum 
 * @param beginIdx 
 * @param endIdx 
 * @param threadID 
 * @param segSize 
 */
void PrimeSeg(const int startNum, const int endNum,
    const int beginIdx, const int endIdx, long threadID, int segSize) {
    for (int start = startNum; start < endNum; start += segSize) {
        //cout << threadID << endl;
        //cout << segSize << endl;
        int end = start + segSize - 1;
        if (end > UPPER_LIMIT) {
            end = UPPER_LIMIT;
        }
        NUM_PRIMES += FilterPrimeSeg(start, endNum, beginIdx, endIdx, threadID);  // filter the segment
    }
}

/**
 * @brief Print PRIME_TEST, making sure it filled correctly.
 *        Debugging is its only real use.
 * 
 */
void PrintBoolPrime() {
    cout << "Sort Size: " << MAX_PRIMES << endl;
    cout << "Printing PRIME_TEST:" << endl;
    for(int i = 0; i < MAX_PRIMES + 1; i++) {
        cout << i + 1 << ": " << (int)PRIME_TEST[i] << endl;
    }
    cout << "End PRIME_TEST" << endl;
}

/**
 * @brief Print out the prime numbers found. Restric printing to 21 total lines.
 * 
 * @param primeResults 
 */
void PrintPrimes(int *primeResults) {
    cout << NUM_PRIMES << " prime numbers found: " << endl;
    if (NUM_PRIMES < 1) {
        cout << "NO PRIME NUMBERS FOUND!" << endl;
    }
    else if(NUM_PRIMES <= 21) {
        for(int i = 0; i < NUM_PRIMES; i++) {
            
            cout << primeResults[i] << endl;
        }
    }
    else {
        for(int i = 0; i < 10; i++){
            cout << primeResults[i] << endl;
        }
        cout << "..." << endl;
        for(int i = (NUM_PRIMES - 11); i < NUM_PRIMES; i++) {
            cout << primeResults[i] << endl;
        }
    }
}


/**
 * @brief Run segmented sieve for each thread.
 *        The indices and number ranges must be calculated such that they align.
 * 
 * @param arg 
 * @return void* 
 */
void* PrimeThreads(void *arg) {
    /* 
        The two number ranges are necessary because the 
        stoarge space is <0.5 the amount of numbers sieving
    */

    // boolean index range for each thread
    int threadID = (long) arg;
    int beginIdx = threadID * IDX_PER_THREAD;           // incorrect index range!!!!
    int endIdx = (threadID + 1) * IDX_PER_THREAD - 1;

    // number range for each thread
    int startNum = threadID * NUM_PER_THREAD + 1;
    int endNum = (threadID + 1) * NUM_PER_THREAD;
    int segSize = (endNum - startNum) + 1;

    if (threadID == 0) {
        startNum = 2;   // ensure the first thread starts at 2
    }

    if (threadID == (NUM_THREADS - 1)) {
        endIdx = MAX_PRIMES;    // include the remaining flags in subset
    }
    if ((NUM_OFFSET != 0) && (threadID == (NUM_THREADS - 1))) {
        endNum = UPPER_LIMIT; // include the remaining numbers in subset
        segSize = (endNum - startNum) + 1 + NUM_OFFSET;
        //cout << "ENDNUM " << endNum << endl;
    }

    if (beginIdx < endIdx) {
        //cout << "Finding primes on thread " << threadID << endl;
        PrimeSeg(startNum, endNum, beginIdx, endIdx, threadID, segSize);
    }
    return NULL;
}

/**
 * @brief Handle input without being dense... ironic.
 *        Specifically for the limit of where to find primes to.
 * 
 * @param upperLim 
 */
void LimitHandler(int& upperLim) {
    string line;
    while(getline(cin, line))   // read a line at a time for parsing.
    {
        stringstream linestream(line);
        if (!(linestream >> upperLim)) {
             cout << "Unaccaptable input. Is your number larger than " 
                  << INT_MAX << "?\n" 
                  << "Try again: ";
             continue;
        }
        if ((upperLim < 2) || (upperLim >= INT_MAX)) { // segmentation fault with max int
             if (upperLim < 2) {
                 cout << "Try a bigger number: ";
             }
             else {
                 cout << "Try a smaller number: ";
             }
             continue;
        }
        char errorTest;
        if (linestream >> errorTest) {
             continue;
        }
        break;
    }
}

/**
 * @brief Handle input without being dense... ironic.
 *        Specifically to get the number of threads.
 *        Could likely be combined with LimitHandler()
 * 
 * @param upperLim 
 * @param maxThreads 
 */
void ThreadHandler(int& upperLim, int maxThreads) {
    string line;
    while(getline(cin, line))   // read a line at a time for parsing.
    {
        stringstream linestream(line);
        if (!(linestream >> upperLim)) {
             cout << "Unaccaptable input. Is your number larger than " 
                  << maxThreads << "?\n" 
                  << "Try again: ";
             continue;
        }
        if ((upperLim < 1) || (upperLim > maxThreads)) { // maximum number of threads that can complete the job
             if (upperLim < 1) {
                 cout << "Not enough threads. Input bigger number: ";
             }
             else {
                 cout << "Too many threads. Try a number smaller than " << maxThreads+1 << ": ";
             }
             continue;
        }
        char errorTest;
        if (linestream >> errorTest) {
             continue;
        }
        break;
    }
}

/**
 * @brief Get a point in time.
 * 
 * @return chrono::high_resolution_clock::time_point 
 */
chrono::high_resolution_clock::time_point SetLap() {
    return chrono::high_resolution_clock::now();
}


bool checkAnswer(string& respose, bool& result);
void setGlobalVars();
int main() {

    cout << "Prime Listing" << endl;
    cout << "Input upper limit to search for primes " << endl;
    cout << "Upper limit: ";
    LimitHandler(UPPER_LIMIT);   // might need to ul make a pointer

    // set global vars
    MAX_PRIMES = (UPPER_LIMIT - 1) / 2;     // maximum number of primes to check for
    SEGMENT_SIZE = sqrt(MAX_PRIMES+1);      // the ideal size for a segment //UPPER_LIMIT / NUM_THREADS;
    int maxThreads = (MAX_PRIMES + 1) / SEGMENT_SIZE;   // maximum number of threads to calc on.

    cout << "Number of threads to run on: ";
    ThreadHandler(NUM_THREADS, maxThreads);
    //cin >> NUM_THREADS;
    //cout << endl;

    setGlobalVars();

    //cout << "IDX_OFFSET = " << OFFSET << endl;      // set the global variables
    //cout << "NUM_OFFSET = " << NUM_OFFSET << endl;

    // ask user if they want the output to be saved to a file
    string saveAnswer;
    bool saveFile = false;
    
    /*
    cout << "Save output to a text file? [y/n]: ";
    while (cin >> saveAnswer && !checkAnswer(saveAnswer, saveFile)) {
        cout << "Invalid response: " << saveAnswer << ". Try again\n"
            << "Save output to a text file? [y/n]: ";
    }
    */

    // open the file
    if (saveFile) {
        PRIME_FILE.open("prime_output.txt");
    }

    chrono::high_resolution_clock::time_point tStart = SetLap();    // start timing
//==============================================================
    cout << "Calculating primes up to " << UPPER_LIMIT 
        << " on " << NUM_THREADS << " threads..." << endl;

    // make threads (and sieve primes)
    pthread_t my_threads[NUM_THREADS];
    for(long int i = 0; i < NUM_THREADS; i++) {
        int rc = pthread_create(&my_threads[i], NULL, PrimeThreads, (void *)i);

        // check for thread spawning errors
        if (rc) {
            cout << "ERROR: return code from pthread_create() is " << rc << endl;
        }
    }

    // join the threads
    for (long int i = 0; i < NUM_THREADS; i++) {
        pthread_join(my_threads[i], NULL);
    }

    int *primeResults = new int[NUM_PRIMES];    // the list to hold prime numbers as INTEGERS
    if(NUM_PRIMES > 0) {    // if any primes have been found, calculate them
        FindPrimes(primeResults, saveFile);
    }
    PrintPrimes(primeResults);  // print the results

    // free memory
    delete [] PRIME_TEST;
    delete [] primeResults;

    // destroy the lock... 
    // not sure if this is needed without actual locks in place
    pthread_mutex_destroy(&lock);
//==============================================================
    chrono::high_resolution_clock::time_point tEnd = SetLap();  // end timing;

    cout << fixed << setprecision(4) << "Time to calculate primes: "
         << chrono::duration<double>(tEnd - tStart).count()
         << " seconds." << endl;
    
    PRIME_FILE << fixed << setprecision(4) << "Time to calculate primes: "
         << chrono::duration<double>(tEnd - tStart).count()
         << " seconds." << endl;

    // close the text file
    if (PRIME_FILE.is_open()) {
        PRIME_FILE.close();
    }
}

/**
 * @brief check to see if a response is valid
 * 
 */
bool checkAnswer(string& response, bool& result) {
    // set input to all lower
    transform(response.begin(), response.end(), response.begin(), 
        [](unsigned char x){return ::tolower(x);});
    
    // determine if the response is correct
    bool validResponse = (response == "y") || (response == "n")
        || (response == "yes") || (response == "no");
    
    // determine if the result is "yes"
    result = validResponse && (response[0] == 'y');
    
    return validResponse;
}

/**
 * @brief set global variables
 * 
 */
void setGlobalVars() {
    IDX_PER_THREAD = (MAX_PRIMES+1) / NUM_THREADS;          // number of segments each thread is tasked with
    NUM_PER_SEG = UPPER_LIMIT / sqrt(UPPER_LIMIT);          // amount of numbers allocated per segment
    NUM_PER_THREAD = IDX_PER_THREAD * 2;  // the numbers allocated to each thread
    // UNUSED, OLD CODE: // NUM_PER_SEG * (sqrt(UPPER_LIMIT) / NUM_THREADS); UPPER_LIMIT / NUM_THREADS;
    OFFSET = MAX_PRIMES % NUM_THREADS;
    NUM_OFFSET = (MAX_PRIMES + 1) % NUM_THREADS;            // left over numbers that will be excluded based on sectioning algorithm
    PRIME_TEST = FillArray(MAX_PRIMES+1);           // prime number status 1 = prime, 0 = not-prime
}