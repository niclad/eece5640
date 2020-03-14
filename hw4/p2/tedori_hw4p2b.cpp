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

const int UPPER_BOUND = 1000;   // number range upperbound
const int LOWER_BOUND = 1;      // number range lower bound

// function declarations
void getInput(int argc, char *argv[], int &N, int &nodes, int &bins);
void fillArray(int N, int arr[]);
int randomInt();
int* binning(int binRanges[], int rangesPerNode, int N, int rank, int bins, int numbers[]);
void printBins(int bins, int N, int totalBinCounts[]);
void genRanges(int nodeBins[], int nodeRanges[], int nodes, int bins);

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
    int *rangeBuf;      // range send buffer
    int *tempRanges;    // temp bins for nodes
    int recv;

    getInput(argc, argv, N, nodes, bins);

    // initialize arrays
    int *binCounts;// = new int[bins-1];    // bin counts for a node
    int *totalBinCounts = new int[bins-1];  // total bin counts for all nodes
    int nodeLens[nodes];
    int binRanges[bins];

    int numbers[N];     // initialize array

    numPerNode = N / nodes;     // calculate the amount of N each node is tasked with
    // assume N is evenly divisible by nodes

    tempNumbers = new int[numPerNode];

    // assign volume of bins to nodes
    int nodeBins[nodes];            // number of bins assigned to a node: index == rank
    int unbufNodebins[nodes];
    for (int i = 0; i < nodes; i++) // fill nodeBins with 0
    {
        nodeBins[i] = 1; // old: = 0;
        unbufNodebins[i] = 0;
    }


    int nodeNum = 0;                // number for the node
    for (int i = 0; i < bins; i++)  // assign bin counts to each node
    {
        nodeBins[nodeNum]++;        // add 1 to the number of bins
        unbufNodebins[nodeNum]++;

        nodeNum = (nodeNum >= nodes-1) ? 0 : nodeNum+1; // increment nodeNum or reset it
    }
    

    int nodeRanges[(bins+1) + (nodes-1)];   // range bounds, including overlaps
    genRanges(nodeBins, nodeRanges, nodes, bins);
    
    int sendDispls[nodes];
    int recvDispls[nodes];

    // determine displacement for MPI_Scatterv
    sendDispls[0] = 0;
    recvDispls[0] = 0;
    for (int i = 1; i < nodes; i++)
    {
        sendDispls[i] = sendDispls[i-1] + (nodeBins[i-1]);
        recvDispls[i] = recvDispls[i-1] + (nodeBins[i-1]-1);
    }

    // set-up MPI
    MPI_Init(&argc, &argv);                 // initialize MPI
    MPI_Comm_size(MPI_COMM_WORLD, &size);   // get number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);   // get process number (ie ID)

    // display the number of processes 
    if (rank == 0)
    {
        cout << "HW4 P2-b" << endl;
        cout << "N: " << N << endl;
        cout << "No. nodes: " << size << endl;
        cout << "Bins: " << bins << endl;
    }
    
    /* DEBUGGING
    if (rank == 0)
    {
        cout << "Node bins:" << endl;
        for (int i = 0; i < nodes; i++)
        {
            cout << nodeBins[i] << endl;
        }
    }
    */
    
    /* DEBUGGING
    if (rank == 0)
    {
        cout << "Node ranges:"<< endl;
        for (int i = 0; i < ((bins+1) + (nodes-1)); i++)
        {
            cout << nodeRanges[i] << endl;
        }
    }
    */
    
    
    
    fillArray(numPerNode, tempNumbers);  // fill array with random ints

    /* DEBUGGING
    if (rank == 0)
    {
        cout << "fillArray() WORKING" << endl;
    }
    */
    
    for (int i = 0; i < nodes; i++)
    {
        MPI_Gather(tempNumbers, numPerNode, MPI_INT, numbers, numPerNode, MPI_INT, i, MPI_COMM_WORLD);
    }

    /* DEBUGGING
    if (rank == 0)
    {
        cout << "MPI_Gather() WORKING" << endl;
    }
    */

    //cout << rank << ": 1000=" << numbers[999] << endl;    // DEBUGGING

    
    /*
    if (rank == 0)
    {
        for (int i = 1; i < nodes; i++)
        {
            MPI_Send(numbers, N, MPI_INT, i, 0, MPI_COMM_WORLD);
        } 
    }
    
    if (rank != 0)
    {
        for (int i = 1; i < nodes; i++)
        {
            MPI_Recv(numbers, N, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }
    */
    

    //int *localNum = new int[numPerNode];    

    // set temp bins for each node
    for (int i = 0; i < nodes; i++)
    {
        if (rank == i) 
        {
            tempRanges = new int[nodeBins[i]];
            binCounts = new int[nodeBins[i]-1];
            recv = nodeBins[i];
        }
    }

    // send list chunks to different nodes (including root)
    MPI_Scatterv(nodeRanges, nodeBins, sendDispls, MPI_INT, tempRanges, recv, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);    // make sure timing starts for all nodes simultaneously
    start = MPI_Wtime();

    /* DEBUGGING
    cout << "testy testy" << endl;
    for (int i = 0; i < nodes; i++)
    {
        if (rank == i)
        {
            for (int i = 0; i < nodeBins[i]; i++)
            {
                cout << "node " << i << ": " << tempRanges[i] << endl;
            }
        }
    }
    */
    

    binCounts = binning(tempRanges, nodeBins[rank], N, rank, nodeBins[rank]-1, numbers);
    finish = MPI_Wtime();
    locElapsed = finish - start;
    cout << rank << ": " << locElapsed << " sec" << endl;
    MPI_Reduce(&locElapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);   // get run time
    
    MPI_Gatherv(binCounts, recv-1, MPI_INT, totalBinCounts, unbufNodebins, recvDispls, MPI_INT, 0, MPI_COMM_WORLD);
    //MPI_Reduce(binCounts, totalBinCounts, bins, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
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
        //delete [] localNum;
        //cout << setw(14) << "localNum" << setw(10) << "FREED" << endl;
        delete [] binCounts;
        cout << setw(14) << "binCounts" << setw(10) << "FREED" << endl;
        delete [] totalBinCounts;
        cout << setw(14) << "totalBinCounts" << setw(10) << "FREED" << endl;
        delete [] tempNumbers;
        cout << setw(14) << "tempNumbers" << setw(10) << "FREED" << endl;
        delete [] tempRanges;
        //delete [] binCounts;
    }
    /*
    for (int i = 0; i < nodes; i++)
    {
        if (rank == i) 
        {
            delete [] tempRanges;
            delete [] binCounts;
            delete [] binCounts;
        }
    }
    */

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

void fillArray(int N, int arr[])
{
    cout << "fill working" << endl;     // DEBUGGING
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

int* binning(int binRanges[], int rangesPerNode, int N, int rank, int bins, int numbers[])
{
    int *binCounts = new int[rangesPerNode-1];  // the count of the numbers that fall into that bin
    for (int i = 0; i < rangesPerNode-1; i++)
    {
        binCounts[i] = 0;
    }

    // check a number for its bin -- probably not the best way to do this
    for (int num = 0; num < N; num++)
    {
        //cout << localNums[num] << endl;
        int currNum = numbers[num];     // the current number to test

        for (int range = 0; range < bins; range++)     // the current bin to test
        {
            // if the number is within the bin range...
            if (currNum > binRanges[range] && currNum <= binRanges[range+1])
            {
                binCounts[range]++;     // increment the count of the located bin
            }
        }
    }
    /* DEBUGGING
    if (rank == 1)
    {
        cout << "binning():"<< endl;
        cout << ">rank: " << rank << endl;
        cout << ">rangesPerNode: " << rangesPerNode << endl;
        for (int i = 0; i < rangesPerNode-1; i++)
        {
            cout << ">binCounts[" << i << "]: " << binCounts[i] << endl;
        }
    }
    */
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

    cout << "Bin counts for " << bins << " bins for numbers in range [1, " << N << "]" << endl;
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

void genRanges(int nodeBins[], int nodeRanges[], int nodes, int bins)
{
    int binSize = UPPER_BOUND / bins;   // the range for each bin
    int count = 1;                      // count for the node bins
    int nodeIdx = 1;                    // the index for the current node bin count
    int currBins = nodeBins[0] - 1;     // current count for the node bin count 

    nodeRanges[0] = 0;
    for (int i = 1; i < ((bins+1) + (nodes-1)); i++)
    {
        if (count == 0)
        {
            nodeRanges[i] = nodeRanges[i-1];
        } else {
            nodeRanges[i] = nodeRanges[i-1] + binSize;
        }

        count++;

        if (count == currBins+1) // +1
        {
            count = 0;
            currBins = nodeBins[nodeIdx] - 1;
            nodeIdx++;
        }
    }
}
