// mpi authors

#include <algorithm>    // swap
#include <fstream>      // file io
#include <iomanip>
#include <iostream>     // io
#include "mpi.h"        // mpi
#include <sstream>      // string stream
#include <string>       // strings

using namespace std;

const int VERT = 317080;        // from http://snap.stanford.edu/data/com-DBLP.html
const int EDGES = 1049866;      // number of edges. from above
const int NODES = 2;            // mpi nodes
const int TASKS = 2;            // tasks-per-node  
const int top = 10;             // top number of authors to determine


struct Author
{
    int id, numCoauth;
};

void findLastLine(ifstream fin);
void initMat(int (&mat)[VERT][VERT]);
void readFile(string fileName, int (&adjmat)[VERT][VERT]);
void readFileCSR(string fileName, int *rowIdx, int *colIdx);
void readFileAuthors(string fileName, int *authors);
void printMat(int mat[VERT][VERT]);
void countAuthors(int *authors, int numPerProc, int *authCount);
void determineTop(int *authors, int numPerProc, int *topAuth, int *topCounts, int top, int rank);
void sort(int *id, int *val, int len);
void initArray(int *arr, int len);
void coauthorVolume(int *dist, int *authors, int distSize, int authPerProc, int rank);
void dispTime(double time, string section);
void generateData(int *dist, int distSize);


int main(int argc, char *argv[])
{
    string fileName = "dblp-co-authors.txt";
    int rank, size;         // rank and size for mpi
    int numPerProc;         // the number of authors per process    
    int authPerProc;        // the number of unique auths per process
    double locElapsed;      // time delta
    double elapsed;         // overall time delta
    int maxCoauth;
    double myTime;

    // timing markers
    double startCount, finishCount, estCount, elapsedCount;
    double startTop, finishTop, estTop, elapsedTop;
    double startDist, finishDist, estDist, elapsedDist;

    // author counts
    int authors[EDGES*2] = {0};         // essentially the number of times an author has authored. value is author id
    int authCount[VERT] = {0};          // the count of each authors (index) co-authors (value@index)
    int totalAuthCount[VERT];           // total count for each author
    int topAuth[top] = {0};             // top ten authors from each node
    int topCounts[top] = {0};           // top counts
    int topAll[top*NODES*TASKS];        // total list. (should be 40 for top==10, nodes==tasks==2)
    int topAllCounts[top*NODES*TASKS];  // all top counts.

    numPerProc = (EDGES * 2) / (NODES * TASKS);     // determine numPerProc. Every author will be assigned (for 4 processes)
    int tempAuth[numPerProc];   // list for each process' authors
    authPerProc = VERT / (NODES * TASKS);           // authors per process
    int tempCount[authPerProc];

    // initialize MPI
    MPI_Init(&argc, &argv);                 // initialize MPI
    MPI_Comm_size(MPI_COMM_WORLD, &size);   // get number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);   // get process ID
    
    // display the number of processes
    if (rank == 0)
    {
        cout << "NUM. PROCESSES: " << size << endl;
        cout << "UNIQUE AUTHORS PER PROC: " << authPerProc << endl;
        cout << "AUTHORS PER PROCESS: " <<  numPerProc << endl;

        // get authors
        readFileAuthors(fileName, authors);
        myTime = MPI_Wtime();
    }

    // send list chunks to nodes
    MPI_Scatter(authors, numPerProc, MPI_INT, tempAuth, numPerProc, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);                    // ensure timing is fair
    startCount = MPI_Wtime();                       // start timing
    countAuthors(tempAuth, numPerProc, authCount);  // count authors
    finishCount = MPI_Wtime();                      // finish timing
    estCount = finishCount - startCount;            // get time delta

    // get overall time delta
    MPI_Reduce(&estCount, &elapsedCount, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0)
        dispTime(elapsedCount, "Count");

    // get total counts
    MPI_Reduce(authCount, totalAuthCount, VERT, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // scatter totalled list
    MPI_Scatter(totalAuthCount, authPerProc, MPI_INT, tempCount, authPerProc, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    startTop = MPI_Wtime();

    determineTop(tempCount, authPerProc, topAuth, topCounts, top, rank);

    finishTop = MPI_Wtime();
    estTop = finishTop - startTop;

    MPI_Reduce(&estTop, &elapsedTop, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0)
        dispTime(elapsedTop, "Top authors");

    // gather counting data
    MPI_Gather(topAuth, top, MPI_INT, topAll, top, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Gather(topCounts, top, MPI_INT, topAllCounts, top, MPI_INT, 0, MPI_COMM_WORLD);

    sort(topAll, topAllCounts, top*NODES*TASKS);
    int maxAuth = topAllCounts[0];  // get the maximum co authors

    int authDist[maxAuth];
    int totalDist[maxAuth];
    int procCount[authPerProc];
    
    MPI_Bcast(&maxAuth, 1, MPI_INT, 0, MPI_COMM_WORLD);
    //MPI_Bcast(authDist, maxAuth, MPI_INT, 0, MPI_COMM_WORLD); // unecessart bc authDist is overwritten as soon as data is scattered. IDK why,
    MPI_Scatter(totalAuthCount, authPerProc, MPI_INT, procCount, authPerProc, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    startDist = MPI_Wtime();
    coauthorVolume(authDist, procCount, maxAuth, authPerProc, rank);
    finishDist = MPI_Wtime();
    estDist = finishDist - startDist;

    MPI_Reduce(&estDist, &elapsedDist, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    if (rank == 0)
        dispTime(elapsedDist, "Dist");

    MPI_Reduce(authDist, totalDist, maxAuth, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    /*
    // DEBUGGING
    if (rank == 0)
    {
        for (int i = 0; i < 40; i++)
        {
            cout << topAll[i] << "\t" << topAllCounts[i] << endl;
        }
    }
    

    if (rank == 0)
    {
        int test[maxAuth];

        coauthorVolume(test, totalAuthCount, maxAuth, VERT, 0);
        for (int i = 0; i < 10; i++)
            cout << test[i] << endl;
    }
*/

    // display end stats
    if (rank == 0)
    {
        myTime = MPI_Wtime() - myTime;
        cout << "Total running time: " << time << " sec" << endl;
        cout << "first 10 dists" << endl;
        for (int i = 0; i < 10; i++)
            cout << totalDist[i] << endl;

        //cout << totalDist[342] << endl;

        generateData(totalDist, maxAuth);
        /*
        // sort top list
        sort(topAll, topAllCounts, top*NODES*TASKS);

        cout << "Exe. time for MPI calls: " << elapsed << endl;
        cout << "Top " << top << " authors with most co-authors:" << endl;

        for (int i = 0; i < 10; i++)
        {
            cout << "AuthID:" << setw(6) << topAll[i]+1 << ", " << "co-authors: " << topAllCounts[i] << endl;
        }
        */
    }

    MPI_Finalize();
}

/**
 * @brief Find the last line of a file
 * 
 * @param fin   file to read
 */
void findLastLine(ifstream fin)
{
    string result;

    if(fin.is_open()) {
        fin.seekg(0,std::ios_base::end);      //Start at end of file
        char ch = ' ';                        //Init ch not equal to '\n'
        while(ch != '\n'){
            fin.seekg(-2,std::ios_base::cur); //Two steps back, this means we
                                              //will NOT check the last character
            if((int)fin.tellg() <= 0){        //If passed the start of the file,
                fin.seekg(0);                 //this is the start of the line
                break;
            }
            fin.get(ch);                      //Check the next character
        }

        std::getline(fin,result);
        fin.close();

        std::cout << "final line length: " << result.size() <<std::endl;
        std::cout << "final line character codes: ";
        for(size_t i =0; i<result.size(); i++){
            std::cout << std::hex << (int)result[i] << " ";
        }
        std::cout << std::endl;
        std::cout << "final line: " << result <<std::endl;
    }
}

/**
 * @brief Read the authors and store them in an array
 * 
 * @param fileName 
 * @param authors 
 */
void readFileAuthors(string fileName, int *authors)
{
    ifstream fin(fileName.c_str());     // open the input file
    size_t found;       // check for the first character
    string line, tempAuth;
    char delim = ' ';   // delimiter for the data
    int author;         // author value
    int adjIdx[2];      // adjaceny matrix value indices
    int idx = 0;

    int count = 0;
    while (fin)
    {
        // check for line being a comment
        getline(fin, line);
        found = line.find_first_not_of(" \t");
        
        // check for comments in the file. comments appear to start with '%'
        if(found != string::npos)
        {    
            // if the line is a comment, move onto the next one.
            if (line[found] == '%')
                continue;
        }

        stringstream s(line);   // get values from a line
        int i = 0;
        while(getline(s, tempAuth, delim))
        {
            author = stoi(tempAuth);    // convert values to ints
            adjIdx[i] = author; 
            i++;
            //cout << author << "\t";
        }
        //cout << adjIdx[0] << " " << adjIdx[1] << endl;  // DEBUGGING

        authors[idx] = adjIdx[0];
        authors[idx+1] = adjIdx[1];

        idx += 2;   // increment by 2 to account for appended vals        
        count++;    // increment line count

        if (fin.eof())
          break;
    }
    fin.close();
    //cout << "Lines processed: " << count << " of " << EDGES << endl;
}

/**
 * @brief Count the authors in this list
 * 
 * @param authCount     The counts for each author; index 0 == author 1, index 1 == author 2, ..., index N-1 == author N.
 * @param authors       The raw list of authors.
 * @param numPerProc    The size of authors[].
 */
void countAuthors(int *authors, int numPerProc, int *authCount)
{
    //int authCount[VERT] = {0};      // the count for an author
    int currAuthor;                 // current author to count

    // loop through the list counting authors.
    for (int i = 0; i < numPerProc; i++)
    {
        currAuthor = authors[i];
        authCount[currAuthor-1]++;
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
    int currAuth;
    int currCount;
    int rankOffset = rank * authPerProc; 

    for (int i = 0; i < authPerProc; i++)
    {
        currAuth = i + rankOffset;
        currCount = authors[i];

        for (int j = 0; j < top; j++)
        {
            if (currCount > topCounts[j]) {
                // replace smallest author at the end.
                topAuth[top-1] = currAuth;
                topCounts[top-1] = currCount;

                sort(topAuth, topCounts, top);
                break; 
            }
        }
    }
}

/**
 * @brief Bubble sort
 * 
 * @param id    ids assoc with vals
 * @param val   values to sort
 * @param len   list length
 */
void sort(int *id, int *val, int len)
{
  bool swapped;

  for (int i = 0; i < len-1; i++)
  {
    swapped = false;
    for (int j = 0; j < len-i-1; j++)
    {
      if (val[j] < val[j+1])
      {
        swap(val[j], val[j+1]);
        swap(id[j], id[j+1]);
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
        dist[currDist-1]++;
    }
/*
    if (rank == 0)
        for (int i = 0; i < 10; i++)
            cout << dist[i] << endl;
*/
    //cout <<rank << "end data: "<< dist[342] << endl;    // debugging
}

void dispTime(double time, string section)
{
    cout << section << " time: " << time << " sec" << endl;
}

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