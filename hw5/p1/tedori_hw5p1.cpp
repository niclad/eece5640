// mpi authors

#include <algorithm> // swap
#include <fstream>   // file io
#include <iomanip>
#include <iostream> // io
#include "mpi.h"    // mpi
#include <sstream>  // string stream
#include <string>   // strings

using namespace std;

const int VERT = 317080;   // from http://snap.stanford.edu/data/com-DBLP.html
const int EDGES = 1049866; // number of edges. from above
const int NODES = 2;       // mpi nodes
const int TASKS = 2;       // tasks-per-node
const int top = 10;        // top number of authors to determine
int ROW_IDX[VERT + 1];     // row counts (CSR format)

// FUNCTION DECLARATIONS
void readFileCSR(string fileName, int *rowIdx);
void countAuthors(int numPerProc, int *authCount, int rank);
void determineTop(int *authors, int numPerProc, int *topAuth, int *topCounts, int top, int rank);
void sort(int *id, int *val, int len);
void initArray(int *arr, int len);
void coauthorVolume(int *dist, int *authors, int distSize, int authPerProc, int rank);
void generateData(int *dist, int distSize);
void dp(int rank, int pass)
{
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == pass)
        cout << rank << ": working" << endl;
}

int main(int argc, char *argv[])
{
    string fileName = "dblp-co-authors.txt";
    int rank, size;                // rank and size for mpi
    int numPerProc;                // the number of authors per process
    double start, finish;          // timing markers
    double locElapsed;             // time delta
    double elapsed;                // overall time delta
    double total = 0.0;            // total elapsed time
    int processes = NODES * TASKS; // the number of processes for the mpirun

    // author counts
    int topAuth[top] = {0};                // top ten authors from each node
    int topCounts[top] = {0};              // top counts
    int topAll[top * NODES * TASKS];       // total list. (should be 40 for top==10, nodes==tasks==2)
    int topAllCounts[top * NODES * TASKS]; // all top counts.

    numPerProc = VERT / processes; // determine numPerProc. Every author will be assigned (for 4 processes)
    int tempAuth[numPerProc];      // list for each process' authors
    int authCount[numPerProc];     // the count of each authors coauths
    initArray(authCount, numPerProc);

    // initialize MPI
    MPI_Init(&argc, &argv);               // initialize MPI
    MPI_Comm_size(MPI_COMM_WORLD, &size); // get number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // get process ID

    // display the number of processes
    if (rank == 0)
    {
        cout << "NUM. PROCESSES: " << size << endl;
        cout << "UNIQUE AUTHORS PER PROC: " << numPerProc << endl;

        // get authors
        readFileCSR(fileName, ROW_IDX);
    }

    MPI_Bcast(ROW_IDX, VERT + 1, MPI_INT, 0, MPI_COMM_WORLD); // Boradcast row counts to all nodes

    // COUNT AUTHORS' CO-AUTHORS
    MPI_Barrier(MPI_COMM_WORLD); // ensure timing is fair
    start = MPI_Wtime();         // start timing

    countAuthors(numPerProc, authCount, rank); // count authors

    finish = MPI_Wtime();                                                         // finish timing
    locElapsed = finish - start;                                                  // get time delta
    MPI_Reduce(&locElapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD); // determine slowest process
    total += elapsed;                                                             // get the total execution time (for MPI calls) so far

    if (rank == 0) // Display the running time for this operation
        cout << "MPI count time: " << elapsed << " sec" << endl;

    // get total counts
    //MPI_Reduce(authCount, totalAuthCount, VERT, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // FIND AUTHORS WITH MOST CO-AUTHORS
    MPI_Barrier(MPI_COMM_WORLD); // ensure process timing is fair
    start = MPI_Wtime();         // start timing

    determineTop(authCount, numPerProc, topAuth, topCounts, top, rank); // find the authors with most co-authors

    finish = MPI_Wtime();                                                         // finish timing
    locElapsed = finish - start;                                                  // get the time
    MPI_Reduce(&locElapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD); // determine slowest processes
    total += elapsed;                                                             // update total time

    if (rank == 0) // Display the running time for this operation
        cout << "MPI topAuth time: " << elapsed << " sec" << endl;

    // gather counting data
    MPI_Gather(topAuth, top, MPI_INT, topAll, top, MPI_INT, 0, MPI_COMM_WORLD);         // gather top author id's
    MPI_Gather(topCounts, top, MPI_INT, topAllCounts, top, MPI_INT, 0, MPI_COMM_WORLD); // gather top author co-auth counts

    // GENERATE AUTHOR DATA
    sort(topAll, topAllCounts, top * NODES * TASKS);

    int maxAuth;
    if (rank == 0)
        maxAuth = topAllCounts[0];  // get the maximum co authors    

    MPI_Bcast(&maxAuth, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int authDist[maxAuth];
    int totalDist[maxAuth];
    int procCount[numPerProc];
    
    //MPI_Scatter(totalAuthCount, numPerProc, MPI_INT, procCount, numPerProc, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();

    coauthorVolume(authDist, authCount, maxAuth, numPerProc, rank);

    finish = MPI_Wtime();
    
    locElapsed = finish - start;
    MPI_Reduce(&locElapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    total += elapsed;

    if (rank == 0) // Display the running time for this section
        cout << "MPI volume time: " << elapsed << " sec" << endl;
    
    MPI_Reduce(authDist, totalDist, maxAuth, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // display running statistics
    if (rank == 0)
    {
        cout << "***************************" << endl; 
        cout << "FINAL RESULTS:" << endl;
        sort(topAll, topAllCounts, top * NODES * TASKS); // sort top list
        cout << "Exe. time for all MPI calls: " << total << " sec" << endl;
        cout << "Top " << top << " authors with most co-authors:" << endl;

        for (int i = 0; i < top; i++) // print the top authors
        {
            cout << "AuthID:" << setw(6) << topAll[i] + 1 << ", "
                 << "co-authors: " << topAllCounts[i] << endl;
        }

        cout << "first 10 dists" << endl;
        for (int i = 0; i < 10; i++)
            cout << totalDist[i] << endl;

        generateData(totalDist, maxAuth);
    }

    MPI_Finalize();
}

/**
 * @brief Read a file, generating a CSR row offset
 * 
 * @param fileName      File name to read from
 * @param rowIdx        Row index (offset)
 */
void readFileCSR(string fileName, int *rowIdx)
{
    ifstream fin(fileName.c_str()); // open the input file
    size_t found;                   // check for the first character
    string line, tempAuth;
    char delim = ' '; // delimiter for the data
    int author;       // author value
    int adjIdx[2];    // adjaceny matrix value indices

    int count = 0;
    while (fin)
    {
        // check for line being a comment
        getline(fin, line);
        found = line.find_first_not_of(" \t");

        // check for comments in the file. comments appear to start with '%'
        if (found != string::npos)
        {
            // if the line is a comment, move onto the next one.
            if (line[found] == '%')
                continue;
        }

        stringstream s(line); // make this line a string stream
        int i = 0;
        while (getline(s, tempAuth, delim)) // parse the data from the sstream
        {
            author = stoi(tempAuth); // convert the parsed data to an int
            adjIdx[i] = author; // add the author to the adjacency index list
            i++;
        }
        
        int idx0 = adjIdx[0]; // make authors easier to access
        int idx1 = adjIdx[1];

        rowIdx[idx0]++; // increment the elements in this row
        rowIdx[idx1]++;

        /*
        NOTE: Thinking about it now, the code up to here just counts
        the elements for each author. This would make the rest of the 
        processes (making this CSR row offset format and counting the
        co-authors from the row offset) unnecessary. Uh-oh.
        */

        if (fin.eof())
            break;

        count++;
    }
    fin.close();

    // cumulatively sum the elements -- see above note
    for (int i = 2; i < VERT + 2; i++)
    {
        rowIdx[i] = rowIdx[i] + rowIdx[i - 1];
    }
    cout << "In-file lines processed: " << count << endl;
}

/**
 * @brief Count the number of co-authors for each author from CSR row offset
 * 
 * @param numPerProc    Number of authors to process per process
 * @param authCount     The counts stored for each process
 * @param rank          Process' rank
 */
void countAuthors(int numPerProc, int *authCount, int rank)
{
    int countVal;                         // current count calculated
    int startIdx = rank * numPerProc;     // Index to start counting at
    int endIdx = (rank + 1) * numPerProc; // Indext to end counting at
    // loop through the list counting authors.
    for (int i = startIdx; i < endIdx; i++)
    {
        countVal = ROW_IDX[i + 1] - ROW_IDX[i]; // this just undoes what readFileCSR does
        authCount[i - startIdx] = countVal;
        if (countVal < 0)
        {
            cout << rank << ", " << i << "\n" << ROW_IDX[i + 1] << " - " << ROW_IDX[i] << " = "  << countVal << endl;
        }
    }
}

/**
 * @brief Detertime the top number of authors
 * 
 * @param authors       list of authors in order of id (index)
 * @param authPerProc   size of authors
 * @param topAuth       list of top authors
 * @param topCounts     counts associated with author at index
 * @param top           size of top list
 */
void determineTop(int *authors, int authPerProc, int *topAuth, int *topCounts, int top, int rank)
{
    int currAuth;                        // current author to process
    int currCount;                       // current author's co-author count
    int rankOffset = rank * authPerProc; // offset for the author rank

    for (int i = 0; i < authPerProc; i++)
    {
        currAuth = i + rankOffset;
        currCount = authors[i];

        for (int j = 0; j < top; j++) // compare each author to the current list of top authors
        {
            if (currCount > topCounts[j])
            {
                // replace smallest author at the end.
                topAuth[top - 1] = currAuth;
                topCounts[top - 1] = currCount;

                sort(topAuth, topCounts, top); // maintain list order
                break;
            }
        }
    }
}

/**
 * @brief Bubble sort - List is short, so this should be efficient
 * 
 * @param id    ids assoc with vals
 * @param val   values to sort
 * @param len   list length
 */
void sort(int *id, int *val, int len)
{
    bool swapped;

    for (int i = 0; i < len - 1; i++)
    {
        swapped = false;
        for (int j = 0; j < len - i - 1; j++)
        {
            if (val[j] < val[j + 1])
            {
                swap(val[j], val[j + 1]);
                swap(id[j], id[j + 1]);
                swapped = true;
            }
        }

        if (!swapped)
            break;
    }
}

/**
 * @brief initialize an array of 0
 * 
 * @param arr   Array to initialize
 * @param len   Length of the array
 */
void initArray(int *arr, int len)
{
    for (int i = 0; i < len; i++)
    {
        arr[i] = 0;
    }
}

/**
 * @brief Count the number of authors who have published with d authors
 * 
 * @param dist          The distribution of authors
 * @param authors       The authors
 * @param distSize      The number of coauthor-counts
 */
void coauthorVolume(int *dist, int *authors, int distSize, int authPerProc, int rank)
{
    initArray(dist, distSize);
    //cout <<rank << " start end data: "<< dist[342] << endl; // debugging
    int currDist;
    for (int i = 0; i < authPerProc; i++)
    {
        currDist = authors[i];
        if (currDist-1 < 0 || currDist > distSize)
            cout << rank << ": " << currDist << endl;
        dist[currDist-1]++;
    }
/*
    if (rank == 0)
        for (int i = 0; i < 10; i++)
            cout << dist[i] << endl;
*/
    //cout <<rank << "end data: "<< dist[342] << endl;    // debugging
}

/**
 * @brief Save the data to a text file
 * 
 * @param dist 
 * @param distSize 
 */
void generateData(int *dist, int distSize)
{
    ofstream data("author_data.txt");

    for (int i = 0; i < distSize-1; i++)
    {
        data << i+1 << " " << dist[i] << "\n";
    }
    data << distSize << " " << dist[distSize-1];

    cout << "Data saved as \"author_data.txt\"" << endl;
}