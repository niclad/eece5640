/* Nicolas Tedori
 * EECE 5640
 * Prof. Kaeli
 * January 24, 2020
 *  Homework 1, Question 2
 */

// See README for compilation information
// max threads from `cat /proc/sys/kernel/threads-max
// 384764

// using segmented sieve of eratosthenes: https://en.wikipedia.org/wiki/Sieve_of_Eratosthenes
// where number of segments is nSeg <= sqrt(N), where N is the largest number and nSeg is >= nThreads.

#include <iostream>
#include <iomanip>
#include <ctime>
#include <chrono>
#include <random>
#include <cmath>
#include <pthread.h>
#include <climits>
#include <sstream>
#include <string>

using namespace std;

// global variables
int NUM_THREADS;
int NUM_PER_SEG;
int SEGMENT_SIZE;
int OFFSET;
int NUM_OFFSET;

// pthreads class
class PrimeCalc {
private:
    int nThreads;       // number of threads to run on
    pthread_t *threads; // the thread IDs
    char *boolPrime;    // list of booleans, with sortSize bools
    int upperLimit;     // the upper limit of where to find primes
    int segments;       // number of segments to break list into
    int valsPerSeg;     // number of values per a segment
    int offset;         // remaining numbers left over
    int *primeResults;  // the prime number results
    int nPrimes;        // the number of prime numbers found
    int sortSize;       // the actual amount of numbers needed to sort through (only odd)
    int blockSize;      // the size of each segment
public:
    // contstructor
    PrimeCalc(int numberOfThreads, int upperLim, bool isAtkin): 
        nThreads(numberOfThreads), primeResults(NULL), nPrimes(0)  {
        upperLimit = TestUpperLim(upperLim);    // set the upper limit
        SetMethod(isAtkin);
        GenerateSegments();
        threads = new pthread_t[nThreads];
    }

    PrimeCalc(int numberOfThreads, int upperLim): 
        nThreads(numberOfThreads), primeResults(NULL), nPrimes(0) {
        upperLimit = TestUpperLim(upperLim);    // set the upper limit
        SetMethod(false);
        GenerateSegments();
        threads = new pthread_t[nThreads];
    }

    PrimeCalc() {
        nThreads = 0;
        primeResults = NULL;
        nPrimes = 0;
        upperLimit = 0;
    }

    ~PrimeCalc() {
        delete[] boolPrime;  // free the memory
        delete[] primeResults;
        delete[] threads;
    }

    void SetMethod(bool isAtkin) {
        if(isAtkin) {
            sortSize = upperLimit;
            boolPrime = new char[upperLimit];
            FillArray(upperLimit);
        }
        else {
            sortSize = FillSize();      // determine the size to sift through
            boolPrime = new char[sortSize + 1]; // init array on heap
            FillArray(sortSize);        // fill boolPrime with initial vals
        }
    }

    // get upper limit
    int TestUpperLim(int ul) {
        // test the upper limit value
        // keep input w/in limits
        if (ul < 0) {
            ul = ul * -1;   // make postive
            cout << "Limit is negative. Setting limit to " << ul << "." << endl;
            
        }
        else if (ul > INT_MAX) {
            ul = INT_MAX;
            cout << "Limit too large. Truncating value to maximum int: " 
                    << ul << "." << endl;
        }

        return ul;
    }

    // get the actual size needed for bool array
    // The value returned assumes that half of the values needed to test
    // aren't prime. Which is true b/c ~50% of values up to a number are even,
    // which are by definition not prime.
    int FillSize() {
        return (upperLimit - 1) / 2;
    }

    // fill the array with bool vals
    void FillArray(int fillSize) {
        for(int i = 0; i <= fillSize; i++) {
            boolPrime[i] = 1;
        }
    }

    // generate segments
    void GenerateSegments() {
        /*
        segments = sqrt(upperLimit);    // find the number of segments
        cout << "Calculated segments: " << segments << endl;
        
        // make sure there are enough segments to evenly divide into N threads
        if(segments < nThreads) {
            segments = nThreads;    // make segments equal to threads - could actually lead to slower times
        }
        else if ((segments > nThreads) && ((segments % nThreads != 0)))
        {
            int unbalance = segments % nThreads;    // determine how much the threads are unbalanced
            int addSeg = nThreads - unbalance;      // determine how many more segments to add to balance
            segments = segments + addSeg;           // fix number of segments
        }
        cout << "Optimal segments: " << segments << endl;
        */

        // find the numbers per thread
        segments = upperLimit / nThreads;     // the numbers to sieve per thread
        valsPerSeg = sortSize / nThreads;   // the numbers to sieve per segment per thread
        offset = sortSize % nThreads;       // the remaining numbers to sieve after all the numbers
        
        cout << "Numbers to sieve per thread: " << segments << endl;
        cout << "Bools per thread = " << valsPerSeg << endl;
    }

    // find all the odd non-prime numbers (and 2)
    void FilterPrimes() {
        // serial sieve
        for(int i = 3; i*i <= upperLimit; i += 2) { // basically, skip all the even numbers bc we know they're not prime (half as many operations!)
            if(boolPrime[i / 2]) {
                for(int j = i*i; j <= upperLimit; j += (2 * i)) {
                    boolPrime[j / 2] = 0;
                    //cout << j << " is not prime!" << endl;
                }
            }
        }

        nPrimes = 0; // might need to make a list of size segment, where each index is nPrimes for that segment
        for (int i = 0; i <= sortSize; i++) {
            nPrimes += boolPrime[i];
        }
        cout << "nPrimes = " << nPrimes << endl;
    }

    // get the prime numbers
    void FindPrimes() {
        if (nPrimes > 0) {
            primeResults = new int[nPrimes];
            primeResults[0] = 2;
            int primeIndex = 1;

            for(int i = 1; i <= sortSize; i++) {
                if(boolPrime[i]) {
                    primeResults[primeIndex] = ((i - 1) * 2) + 3;
                    primeIndex++;
                }
            }   
        }   
    }

    // find the primes from a start number to an end number
    void FilterPrimeSeg(const int start, const int end, 
                        const int beginIdx, const int endIdx) {

        /*
        // generate a list of primes for the segment
        char *tempPrimes = new char[arraySize];
        for (int i = 0; i < arraySize; i++) {
            tempPrimes[i] = 1;
        }
        */

        for (int i = 3; (i * i) <= end; i += 2) {
            if (NumIsPrime(i)) {
                continue;
            }

            int minStart = ((start + i - 1)/ i) * i;
            if (minStart < (i * i)) {
                minStart = i * i;
            }

            // force min to be odd
            if ((minStart % 2) == 0) {
                minStart += i;
            }

            // find all odd non-primes
            for (int j = minStart; j <= end; j += 2 * i) {
                boolPrime[j / 2] = 0;
            }
        }

        for (int i = beginIdx; i <= endIdx; i++) {
            nPrimes += boolPrime[i];
        }
    }

    // control FindPrimeSeg
    void PrimeSeg(const int startNum, const int endNum,
                  const int beginIdx, const int endIdx) {
        cout << "Array size = " << sortSize << endl;
        for (int start = 2; start <= upperLimit; start += segments) {
            int end = start + segments;
            if (end > upperLimit) {
                end = upperLimit;
            }

            FilterPrimeSeg(start, end, beginIdx, endIdx);  // filter the segment
        }
    }

    // skip any number that is a multiple of the following
    bool NumIsPrime(int num) {
        return ((num >= 3*3) && (num % 3 == 0)) || ((num >= 5*5) && (num % 5 == 0))
            || ((num >= 7*7) && (num % 7 == 0)) || ((num >= 11*11) && (num % 11 == 0))
            || ((num >= 13*13) && (num % 13 == 0)); // could go on for every prime found
                                                    // but then we get Zeno's Arrow :(
    }
    /*
    // control the generation and joining of threads in the program
    void ThreadController() {

        for(long int i = 0; i < nThreads; i++) {
            int rc = pthread_create(&threads[i], NULL, PrimeThreads, (void *)i);

            if (rc) {
                cout << "ERROR: return code from pthread_create() is " << rc << endl;
            }
        }

        // find primes

        for (long int i = 0; i < nThreads; i++) {
            pthread_join(threads[i], NULL);
        }
        cout << "nPrimes = " << nPrimes << endl;
    }

    // create the threads
    void* PrimeThreads(void *arg) {
        
        //    The two number ranges are necessary because the 
        //    stoarge space is <0.5 the amount of numbers sieving

        // boolean index range for each thread
        int threadID = (long) arg;
        int beginIdx = threadID * valsPerSeg;
        int endIdx = (threadID + 1) * valsPerSeg - 1;

        // number range for each thread
        int startNum = threadID * segments;
        int endNum = (threadID + 1) * segments - 1;
        int numOffset = upperLimit % nThreads;
        if ((offset != 0) && (numOffset != 0) && (threadID == (nThreads - 1))) {
            endIdx += offset;    // include the remaining flags in subset
            endNum += numOffset; // include the remaining numbers in subset
        }

        if (beginIdx < endIdx) {
            PrimeSeg(startNum, endNum, beginIdx, endIdx);
        }
    }
    */

    // print boolPrime
    void PrintBoolPrime() {
        cout << "Sort Size: " << sortSize << endl;
        cout << "Printing boolPrime:" << endl;
        for(int i = 0; i < sortSize + 1; i++) {
            cout << i + 1 << ": " << (int)boolPrime[i] << endl;
        }
        cout << "End boolPrime" << endl;
    }

    // print prime numbers
    void PrintPrimes() {
        cout << "nPrimes = " << nPrimes << endl;
        cout << "Prime numbers found: " << endl;
        if (nPrimes < 1) {
            cout << "NO PRIME NUMBERS FOUND!" << endl;
        }
        else if(nPrimes <= 21) {
            for(int i = 0; i < nPrimes; i++) {
                cout << primeResults[i] << endl;
            }
        }
        else {
            for(int i = 0; i < 10; i++){
                cout << primeResults[i] << endl;
            }
            cout << "..." << endl;
            for(int i = (nPrimes - 11); i < nPrimes; i++) {
                cout << primeResults[i] << endl;
            }
        }
    }

    // get the number of threads
    int GetNumThreads() {
        return nThreads;
    }

    // get the upperLimit
    int GetUpperLimit() {
        return upperLimit;
    }

    void SetLimit(int ul) {
        upperLimit = TestUpperLim(ul);    // set the upper limit
    }
    void SetThreads(int nt) {
        nThreads = nt;
        GenerateSegments();
        SetMethod(false);
    }
};

PrimeCalc my_primes = PrimeCalc();

// create the threads
void* PrimeThreads(void *arg) {
    /* 
        The two number ranges are necessary because the 
        stoarge space is <0.5 the amount of numbers sieving
    */

    // boolean index range for each thread
    int threadID = (long) arg;
    int beginIdx = threadID * NUM_PER_SEG;
    int endIdx = (threadID + 1) * NUM_PER_SEG - 1;

    // number range for each thread
    int startNum = threadID * SEGMENT_SIZE;
    int endNum = (threadID + 1) * SEGMENT_SIZE - 1;
    int numOffset = NUM_OFFSET;
    if ((OFFSET != 0) && (numOffset != 0) && (threadID == (NUM_THREADS - 1))) {
        endIdx += OFFSET;    // include the remaining flags in subset
        endNum += numOffset; // include the remaining numbers in subset
    }

    if (beginIdx < endIdx) {
        cout << "Finding primes on thread " << threadID << endl;
        my_primes.PrimeSeg(startNum, endNum, beginIdx, endIdx);
    }
}

int main() {
    int upperLim;   // upper limit of prime calculation
    int numThreads; // number of threads to run on
    string line;

    cout << "Prime Listing" << endl;
    cout << "Input upper limit to search for primes " << endl;
    cout << "Upper limit: ";
    // INPUT HANDLER
    while(getline(cin, line))   // read a line at a time for parsing.
    {
        stringstream linestream(line);
        if (!(linestream >> upperLim)) {
             cout << "Unaccaptable input. Is your number larger than " 
                  << INT_MAX << "?\n" 
                  << "Try again: ";
             continue;
        }
        if ((upperLim < 2) || (upperLim > INT_MAX)) { // segmentation fault with max int
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

    cout << "Number of threads to run on: ";
    cin >> numThreads;
    cout << endl;

    my_primes.SetLimit(upperLim);
    my_primes.SetThreads(numThreads);
    //my_primes.PrintBoolPrime();

    int maxPrimes = (upperLim - 1) / 2;
    SEGMENT_SIZE = upperLim / numThreads;
    NUM_PER_SEG = maxPrimes / numThreads;
    OFFSET = maxPrimes % numThreads;
    NUM_OFFSET = upperLim % numThreads;

    auto tStart = chrono::high_resolution_clock::now();     // start timing

    cout << "Calculating primes up to " << upperLim 
         << " on " << numThreads << " threads..." << endl;

    //my_primes.PrintBoolPrime();

    // make threads
    pthread_t my_threads[NUM_THREADS];
    for(long int i = 0; i < NUM_THREADS; i++) {
        int rc = pthread_create(&my_threads[i], NULL, PrimeThreads, (void *)i);

        if (rc) {
            cout << "ERROR: return code from pthread_create() is " << rc << endl;
        }
    }

    // find primes

    for (long int i = 0; i < NUM_THREADS; i++) {
        pthread_join(my_threads[i], NULL);
    }

    my_primes.FindPrimes();
    my_primes.PrintPrimes();

    auto tEnd = chrono::high_resolution_clock::now();       // end timing
    //auto duration = chrono::duration_cast<microseconds>(tEnd - tStart);

    cout << fixed << setprecision(4) << "Time to calculate primes: "
         << chrono::duration<double>(tEnd - tStart).count()
         << " seconds." << endl;
}

/* ATKINS SIEVE IMPLEMENATION: 
// Atkin's Sieve (as an alternative to FilterPrimes which is Eratosthenes' Sieve)
// see https://en.wikipedia.org/wiki/Sieve_of_Atkin for more info
// NOTE: the way it finds primes appears to be broken. 1,000,000,000 UL
//       counts 53,326,267, which is incorrect.
//       Correct Value is: 50,847,534
void AtkinSieve() {
    int n = 0;

    boolPrime = new char[upperLimit];

    for(int i = 0; i < upperLimit; i++) {
        boolPrime[i] = 0;   // set all the values to "false"
    }

    for(int x = 1; (x * x) < upperLimit; x++) {
        for(int y = 1; (y * y) < upperLimit; y++) {

            // 3.1
            n = (4 * x * x) + (y * y);
            if ((n <= upperLimit) && ((n % 12 == 1) || (n % 12 == 5))) {
                boolPrime[n] ^= 1;
            }

            // 3.2 
            n = (3 * x * x) + (y * y);
            if((n <= upperLimit) && (n % 12 == 7)) {
                boolPrime[n] ^= 1;
            }

            // 3.3
            n = (3 * x * x) - (y * y);
            if((x > y) && (n <= upperLimit) && (n % 12 == 11)) {
                boolPrime[n] ^= 1;
            }
        }
    }
    
    // sieve the results, flagging multiples of squares as non-prime
    for(int i = 5; i*i < upperLimit; i++) {
        if(boolPrime[i]) {
            for(int j = i*i; j < upperLimit; j += i*i) {
                boolPrime[j] = 0;
            }
        }
    }

    // count the primes (there has to be a better way of doing this)
    nPrimes = 2; // include 2 and 3
    for(int i = 5; i < upperLimit; i++)
        if(boolPrime[i])
            nPrimes++;
    cout << "nPrimes = " << nPrimes << endl;

    primeResults = new int[nPrimes];
    primeResults[0] = 2;
    primeResults[1] = 3;
    int primeIndex = 2;
    for(int i = 5; i < upperLimit; i++) {
        if(boolPrime[i]) {
            primeResults[primeIndex] = i;
            primeIndex++;
        }
    }
}
*/