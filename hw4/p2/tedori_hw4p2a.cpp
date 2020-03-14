/**
 * @file binning.cpp
 * @author Nicolas Tedori (tedori.n@northeastern.edu)
 * @brief 
 * @version 0.1
 * @date 2020-03-13
 * 
 * 
 */

#include <iomanip>
#include <iostream>
#include "mpi.h"
#include <random>
#include <time.h>

using namespace std;

int UPPER_BOUND = 1000; // number range upperbound
int LOWER_BOUND = 1;    // number range lower bound

// function declarations
void getInput(int argc, char *argv[], int &N, int &nodes, int &bins);
int assignRest(int size, int N, int numPerNode);
void fillArray(int N, int arr[]);
int randomInt();
int* binning(int localNums[], int numPerNode, int bins);
void printBins(int bins, int N, int totalBinCounts[]);

int main(int argc, char *argv[])
{
    int rank;           // process rank
    int size;           // number of processes
    int N;              // amount of numbers to bin
    int nodes;          // number of nodes to run on
    int numPerNode;     // amount of ints each node is tasked with classifiying
    double start;       // the start time 
    double finish;      // the end time
    double locElapsed;  // elapsed time for a node
    double elapsed;     // the elapsed time
    int bins;           // the number of bins -- change this to be alterable
    int *tempNumbers;   // temp numbers for nodes

    getInput(argc, argv, N, nodes, bins);

    // initialize arrays
    int *binCounts = new int[bins-1];       // bin counts for a node
    int *totalBinCounts = new int[bins-1];  // total bin counts for all nodes

    int *numbers = new int[N];  // initialize array

    numPerNode = N / nodes;     // calculate the amount of N each node is tasked with
    // assume N is evenly divisible by nodes

    tempNumbers = new int[numPerNode];

    // set-up MPI
    MPI_Init(&argc, &argv);                 // initialize MPI
    MPI_Comm_size(MPI_COMM_WORLD, &size);   // get number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);   // get process number (ie ID)

    // display the number of processes 
    if (rank == 0)
    {
        cout << "Num processes: " << size << endl;
        cout << "N: " << N << endl;
        cout << "# nodes: " << nodes << endl;
        cout << "Bins: " << bins << endl;
    }
    
    fillArray(numPerNode, tempNumbers);     // fill array with random ints

    if (rank == 0)
    {
        cout << "working" << endl;
    }
    //MPI_Gather(tempNumbers, numPerNode, MPI_INT, numbers, numPerNode, MPI_INT, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
        cout << "working" << endl;
    }
    int *localNum = new int[numPerNode];

    /*
    if ((numPerNode % 2 != 0) && rank == size-1)
    {
        assignRest(size, N, numPerNode);
    }
    */

    if (rank == 0)
    {
        cout << "Numbers per node: " << numPerNode << endl;
    }
    

    // send list chunks to different nodes (including root)
    //MPI_Scatter(numbers, numPerNode, MPI_INT, localNum, numPerNode, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);    // make sure timing starts for all nodes simultaneously
    start = MPI_Wtime();
    binCounts = binning(tempNumbers, numPerNode, bins);
    finish = MPI_Wtime();
    locElapsed = finish - start;
    cout << rank << ": " << locElapsed << " sec" << endl;
    MPI_Reduce(&locElapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);   // get run time
    
    //MPI_Gather(binCounts, numPerNode, MPI_INT, totalBinCounts, numPerNode, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Reduce(binCounts, totalBinCounts, bins, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {   
        /*
        cout << "Bin counts:" << endl;
        for (int i = 0; i < bins; i++)
        {
            cout << "Bin " << i + 1 << ": " << totalBinCounts[i] << endl;
        }
        */

        printBins(bins, N, totalBinCounts);
        int count = 0;
        for (int i = 0; i < bins; i++)
        {
            count += totalBinCounts[i];
        }
        cout << "Total numbers binned: " << count << endl;

        cout << "Running time: " << elapsed << " sec" << endl;
        // free mem
        cout << "FREEING MEMORY" << endl;
        delete [] localNum;
        cout << setw(14) << "localNum" << setw(10) << "FREED" << endl;
        delete [] binCounts;
        cout << setw(14) << "binCounts" << setw(10) << "FREED" << endl;
        delete [] totalBinCounts;
        cout << setw(14) << "totalBinCounts" << setw(10) << "FREED" << endl;
        delete [] numbers;
        cout << setw(14) << "numbers" << setw(10) << "FREED" << endl;
        delete [] tempNumbers;
        cout << setw(14) << "tempNumbers" << setw(10) << "FREED" << endl;
    }
    
    MPI_Finalize();

}

void getInput(int argc, char *argv[], int &N, int &nodes, int &bins)
{
    if (argc != 4)
    {
        cout << "Error: incorrect number of input arugments: ./codefile <N> <nodes> <bins>" << endl;
        exit(-1);
    } else {
        N = atoi(argv[1]);      // set N
        nodes = atoi(argv[2]);  // set nodes
        bins = atoi(argv[3]);   // set the number of bins
    }
}

int assignRest(int size, int N, int numPerNode)
{
    int totalN = numPerNode * size-1;   // how much numbers are being binned by the rest of the nodes
    numPerNode = N - totalN;        // make sure no numbers are left out by assigning last node the rest

    return numPerNode;
}

void fillArray(int N, int arr[])
{
    cout << "fill working" << endl;
    for (int i = 0; i < N; i++)
    {
        arr[i] = randomInt();
    }
}

int randomInt()
{
    random_device rd;   // initialize seed
    mt19937 rng(rd());  // random-number engine
    uniform_int_distribution<int> uni(LOWER_BOUND, UPPER_BOUND);

    int randomNumber = uni(rng);

    return randomNumber;
}

int* binning(int localNums[], int numPerNode, int bins)
{
    // get ranges for bins
    int binSize = UPPER_BOUND / bins;          // the size of each bin
    int *binRanges = new int[bins+1];     // the ranges of each bin
    int *binCounts = new int[bins-1];   // the count of the numbers that fall into that bin

    // fill binRanges and counts
    binRanges[bins] = UPPER_BOUND;
    for (int binNum = 0; binNum < bins; binNum++)
    {
        binRanges[binNum] = binNum * binSize;    // the lower bound of the bin. binRanges[i+1] is upper bound of bin
        if (binNum >= bins-1)
        {
            continue;
        }
        binCounts[binNum] = 0;                   // all bins have 0... so far
    }

    // check a number for its bin -- probably not the best way to do this
    for (int num = 0; num < numPerNode; num++)          // the current number to test
    {
        //cout << localNums[num] << endl;
        int currNum = localNums[num];
        for (int range = 0; range < bins; range++)     // the current bin to test
        {
            // if the number is within the bin range...
            if (currNum > binRanges[range] && currNum <= binRanges[range+1])
            {
                binCounts[range]++;     // increment the count of the located bin
            }
        }
    }

    delete [] binRanges;
    return binCounts;
}

/**
 * @brief Print the histogram generated
 * 
 * @param bins              The number of bins
 * @param N                 The number of numbers binned
 * @param totalBinCounts    The counts of number in each bin
 */
void printBins(int bins, int N, int totalBinCounts[])
{
    int count;      // the number of bins in the 
    int binLen;     // the length of the hist bars

    cout << "Bin counts for " << bins << " bins for numbers in range [1, 1000]" << endl;
    for (int binNum = 0; binNum < bins; binNum++)
    {
        cout << "Bin " << setw(4) << binNum+1 << ":";
        count = totalBinCounts[binNum];
        binLen = count / (0.005 * N);
        while(binLen > 0)
        {
            cout << "*";
            binLen--;
        }
        cout << " " << count << endl;
    }
}