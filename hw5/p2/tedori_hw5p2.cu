// mpi authors

#include <algorithm> // swap
#include <cstdio>
#include <fstream> // file io
#include <iomanip>
#include <iostream> // io
#include <cmath>
#include <sstream> // string stream
#include <string>  // strings
#include <time.h>

using namespace std;

const int VERT = 317080;   // from http://snap.stanford.edu/data/com-DBLP.html
const int EDGES = 1049866; // number of edges. from above
const int TOP = 10;        // top number of authors to determine

void readFileCSR(string fileName, int *rowIdx);
__global__ void countAuthors(int *d_rowIdx, int *d_counts, int n);
__global__ void topAuthors(int *d_counts, int *d_max, int top, int n);
__global__ void coauthorVolume(int *d_dist, int *d_counts, int max, int n);
void determineTop(int *authors, int *topAuth, int *topCounts);
void sort(int *id, int *val, int len);
void generateData(int *dist, int distSize);
double CLOCK();

int main(int argc, char *argv[])
{
    string fileName = "dblp-co-authors.txt";             // file name with data
    int *h_rowIdx, *h_counts, *h_max, *h_tCnts, *h_dist; // host authors
    int *d_rowIdx, *d_counts, *d_max, *d_dist;           // device authors
    size_t rowBytes, cntBytes, maxBytes;                 // size (in bytes) of each array
    double start, finish, total = 0;                     // timing variables
    int max;                                             // maximum number of co-authors

    // determine size of device arrays
    rowBytes = (VERT + 1) * sizeof(int);
    cntBytes = VERT * sizeof(int);
    maxBytes = TOP * sizeof(int);

    // allocate memory on host
    h_rowIdx = new int[VERT + 1]; // host row counts (CSR format)
    h_counts = new int[VERT];
    h_max = new int[TOP];
    h_tCnts = new int[TOP];
    cudaMalloc(&d_rowIdx, rowBytes); // allocate memory device
    cudaMalloc(&d_counts, cntBytes);
    cudaMalloc(&d_max, maxBytes);

    // initialize row offset array
    for (int i = 0; i < VERT + 1; i++)
    {
        h_rowIdx[i] = 0;
    }

    // get device properties
    cudaDeviceProp props;               // devices properties
    cudaGetDeviceProperties(&props, 0); // get the device properties
    cout << "GPU: " << props.name << ": " << props.major << "." << props.minor << endl;

    start = CLOCK();
    readFileCSR(fileName, h_rowIdx);
    finish = CLOCK() - start;
    total += finish;
    cout << "File read time: " << finish / 1000 << " sec" << endl;

    for (int i = 0; i < VERT; i++) // initalize host arrays
        h_counts[i] = 0;
    //for (int i = 0; i < TOP; i++)
        //h_max[i] = i;

    cudaMemcpy(d_rowIdx, h_rowIdx, rowBytes, cudaMemcpyHostToDevice); // copy array to device
    cudaMemcpy(d_counts, h_counts, cntBytes, cudaMemcpyHostToDevice);
    cudaMemcpy(d_max, h_max, maxBytes, cudaMemcpyHostToDevice);

    int blockSize, gridSize;
    blockSize = 1024;                                    // number of threads in each thread block
    gridSize = (int)ceil((float)(VERT + 1) / blockSize); // number of thread blocks in a grid

    // COUNT AUTHORS' CO-AUTHORS
    start = CLOCK();
    // execute the kernel
    countAuthors<<<gridSize, blockSize>>>(d_rowIdx, d_counts, VERT);
    finish = CLOCK() - start;
    total += finish;
    cout << "GPU author count time: " << finish / 1000 << " sec" << endl;

    cudaMemcpy(h_counts, d_counts, cntBytes, cudaMemcpyDeviceToHost); // copy data back to host

    // DETERMINE TOP X AUTHORS WITH MOST CO-AUTHORS
    start = CLOCK();
    determineTop(h_counts, h_max, h_tCnts);
    sort(h_max, h_tCnts, TOP);
    finish = CLOCK() - start;
    total += finish;
    cout << "Max co-auth. search time: " << finish / 1000 << " sec" << endl;

    // GENERATE GRAPHING DATA
    max = h_tCnts[0];                       // get the maximum no. of co-authors
    h_dist = new int[max];                  // allocate memory on host
    cudaMalloc(&d_dist, max * sizeof(int)); // allocate memory on device

    // initialize array
    for (int i = 0; i < max; i++)
    {
        h_dist[i] = 0;
    }

    cudaMemcpy(d_dist, h_dist, max * sizeof(int), cudaMemcpyHostToDevice); // copy array to device

    start = CLOCK();
    gridSize = (int)ceil((float)VERT / blockSize);                        // update gridSize
    coauthorVolume<<<gridSize, blockSize>>>(d_dist, d_counts, max, VERT); // execute the kernel
    finish = CLOCK() - start;
    total += finish;
    cout << "GPU data generation time: " << finish / 1000 << " sec" << endl;

    cudaMemcpy(h_dist, d_dist, max * sizeof(int), cudaMemcpyDeviceToHost); // copy array to host

    // DISPLAY RUNNING INFO
    cout << "***************************" << endl; 
    cout << "FINAL RESULTS:" << endl;
     for (int i = 0; i < TOP; i++)
    {
        cout << "auth" << setw(6) << h_max[i] + 1 << ", count " << h_tCnts[i] << endl;
    }
    cout << "First 10 distribution values:" << endl;
    for (int i = 0; i < TOP; i++)
    {
        cout << h_dist[i] << endl;
    }
    cout << "Last: " << h_dist[342] << endl; // display the last value

    generateData(h_dist, max); // generate the graphing data

    cout << "Cumulative running time: " << total / 1000 << " sec" << endl;

    cudaFree(d_rowIdx); // free device memory
    cudaFree(d_counts);
    cudaFree(d_max);
    cudaFree(d_dist);
    delete[] h_rowIdx; // free host memory
    delete[] h_counts;
    delete[] h_max;
    delete[] h_dist;
}

/**
 * @brief Read a file, generating a CSR sparse matrix
 * 
 * @param fileName      File name to read from
 * @param rowIdx        Row index (offset)
 * @param colIdx        Column index
 * @param counts        Coauthor counts
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

        stringstream s(line);
        int i = 0;
        while (getline(s, tempAuth, delim))
        {
            author = stoi(tempAuth);
            adjIdx[i] = author;
            i++;
            //cout << author << "\t";
        }
        //cout << adjIdx[0] << " " << adjIdx[1] << endl;  // DEBUGGING
        int idx0 = adjIdx[0];
        int idx1 = adjIdx[1];

        rowIdx[idx0]++;
        rowIdx[idx1]++; // = rowIdx[idx1-1] + rowIdx[idx1] + 1;

        if (fin.eof())
            break;

        count++;
    }
    fin.close();

    // cumulatively sum the elements
    for (int i = 2; i < VERT + 2; i++)
    {
        rowIdx[i] = rowIdx[i] + rowIdx[i - 1];
    }
    cout << "Lines processed: " << count << endl;
}

/**
 * @brief CUDA kernel to get the top authors and retrieve author counts
 * 
 * @param d_rowIdx 
 * @param d_counts 
 * @param d_max 
 * @param n 
 */
__global__ void countAuthors(int *d_rowIdx, int *d_counts, int n)
{
    int id = blockIdx.x * blockDim.x + threadIdx.x; // get global thread id
    int countVal;                                   // total co-authors

    if (id < n) // Make sure the threads don't index out of range
    {
        countVal = d_rowIdx[id + 1] - d_rowIdx[id]; // calculate the number of co-authors
        d_counts[id] = countVal;
    }
}

__global__ void topAuthors(int *d_counts, int *d_max, int top, int n)
{
    int id = blockIdx.x * blockDim.x + threadIdx.x; // get global thread id
    int stride = blockDim.x * gridDim.x;            // stride length
    int smallest = top - 1;                         // index of smallest member of d_max
    int curr = 0;
    __shared__ int t_top[20];
    __shared__ int t_cnt[20];

    for (int i = 0; i < top; i++)
    {
        t_top[i] = i;
        t_cnt[i] = 0;
    }

    if (id < n)
    {
        for (int i = id; i < n; i += stride)
        {
            curr = d_counts[i];
            if (curr > d_counts[t_top[smallest]])
            {
                t_top[smallest] = i;
                t_cnt[smallest] = d_counts[i];
                for (int j = threadIdx.x; j < 20; j += 2) // find new smallest
                {
                    int comp1 = t_top[j];
                    int comp2 = t_top[smallest];
                    if (d_counts[comp1] < d_counts[comp2])
                    {
                        smallest = j;
                    }
                }
            }
        }
    }

    if (threadIdx.x == 0)
    {
        printf("Thread: %d\n", threadIdx.x);
        for (int i = 0; i < 20; i++)
            printf("auth %d, count %d\n", (t_top[i] + 1), t_cnt[i]);
    }
}

/**
 * @brief 
 * 
 * @param d_dist 
 * @param d_counts 
 * @param max 
 */
__global__ void coauthorVolume(int *d_dist, int *d_counts, int max, int n)
{
    int id = blockIdx.x * blockDim.x + threadIdx.x; // get global thread id

    if (id < n)
    {
        int memLoc = d_counts[id] - 1;   // get the index that needs to be incremented
        atomicAdd((d_dist + memLoc), 1); // add 1 to the value at d_dist[memLoc]
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
void determineTop(int *authors, int *topAuth, int *topCounts)
{
    int currCount;
    for (int i = 0; i < VERT; i++)
    {
        currCount = authors[i];

        for (int j = 0; j < TOP; j++)
        {
            if (currCount > topCounts[j])
            {
                // replace smallest author at the end.
                topAuth[TOP - 1] = i;
                topCounts[TOP - 1] = currCount;

                sort(topAuth, topCounts, TOP);
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
 * @brief Generate a data file from passed values
 * 
 * @param dist      Distribution
 * @param distSize  Size of the distribution
 */
void generateData(int *dist, int distSize)
{
    ofstream data("author_data-cuda.txt");

    for (int i = 0; i < distSize-1; i++)
    {
        data << i+1 << " " << dist[i] << "\n";
    }
    data << distSize << " " << dist[distSize-1];

    cout << "Data saved as \"author_data-cuda.txt\"" << endl;
}

/**
 * @brief Get a time point
 * 
 * @return double   The time point generated
 */
double CLOCK()
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return (t.tv_sec * 1000) + (t.tv_nsec * 1e-6);
}