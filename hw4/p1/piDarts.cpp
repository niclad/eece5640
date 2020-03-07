/**
 * @file piDarts.cpp
 * @author Nicolas Tedori (tedori.n@northeastern.edu)
 * @brief PI Dart baord esitmation - Monte Carlo simulation
 * @version 0.1
 * @date 2020-03-13
 * 
 */

#include <iomanip>
#include <iostream>
#include <math.h>
#include "mpi.h"
#include <random>
#include <time.h>

using namespace std;

typedef mt19937 rng;    // Mersenne Twister engine (more random than rand())
double CIRCLE = 1.0;

double randNum(rng generator);
void getInput(int argc, char *argv[], int rank, long *totalTosses);
double errorCalc(double piEst);
long toss(long procTosses, int rank);
bool circleTest(double x, double y);

int main(int argc, char *argv[])
{
    int rank;   // calling process ID
    int size;   // number of processes

    long totalTosses;   // total tosses
    long procTosses;    // total tosses for the process
    long procCircle;    // number in circle for process
    long totalCircle;   // total in circle

    double start, end;          // timing start and end
    double locElapsed, elapsed; // local process time
    double piEst;   // estimation of pi
    double piErr;   // error in pi estimation

    // initialize MPI
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);   // get number of processes
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);   // get process ID

    if (rank == 0)
    {
        cout << "Num processes: " << size << endl;
    }

    getInput(argc, argv, rank, &totalTosses);   // get the input values

    procTosses = totalTosses / size;    // tosses per process

    MPI_Barrier(MPI_COMM_WORLD);    // sync processes for the sake of timing
    start = MPI_Wtime();    // get start time
    procCircle = toss(procTosses, rank);
    end = MPI_Wtime();      // get end time
    locElapsed = end - start;
    cout << rank << " time: " << locElapsed << endl;
    // get largest process time
    MPI_Reduce(&locElapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    // get the total number of tosses that landed in the circle for all processes
    MPI_Reduce(&procCircle, &totalCircle, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        cout << "totalCircle: " << totalCircle << endl;
        cout << "totalTosses: " << totalTosses << endl;
        piEst = (4 * totalCircle) / ((double) totalTosses); // get pi
        piErr = errorCalc(piEst);   // calculate the percentage error in estimation
        cout << "Running time: " << elapsed << " seconds" << endl;
        cout << fixed << setprecision(16) << "Pi estimation: " << piEst << "..." << endl;
        cout << setprecision(5) << "Error (using M_PI): " << piErr << "%" << endl;
    }

    MPI_Finalize();
}

/**
 * @brief Generate a random fp number in [-1, 1]
 * 
 * @return double   The random number generated
 */
double randNum(rng generator)
{
    uniform_real_distribution<double> dis(-CIRCLE, CIRCLE); // define 

    return dis(generator);
}

/**
 * @brief Get command line input
 * 
 * @param argc 
 * @param argv 
 * @param rank 
 * @param totalTosses 
 */
void getInput(int argc, char *argv[], int rank, long *totalTosses)
{
    if (rank == 0)
    {
        if (argc != 2)
        {
            cout << "Error, not enough inputs" << endl;
            *totalTosses = 0;
        } else {
            *totalTosses = atoi(argv[1]);
        }
    }

    // send all processes the total number of tosses
    MPI_Bcast(totalTosses, 1, MPI_LONG, 0, MPI_COMM_WORLD);

    // end program because user wanted estimation from no tosses
    if (*totalTosses == 0)
    {
        MPI_Finalize();
        cout << "No tosses defined." << endl;
        exit(-1);
    }
}

/**
 * @brief calculate the error in pi estimation
 * 
 * @param piEst 
 * @return double 
 */
double errorCalc(double piEst)
{
    double error = fabs(piEst - M_PI);  // difference in "actual" and estimated pi
    double errorPercent = (error / M_PI) * 100; // error percentage

    return errorPercent;
}

/**
 * @brief Simulate tosses for each process
 * 
 * @param procTosses 
 * @param rank 
 * @return long 
 */
long toss(long procTosses, int rank)
{
    long numCirc = 0;

    double x, y;

    unsigned int seed = (unsigned) time(NULL);  // generate seed from time
    rng generate();  // create rng
    srand(seed + rank);
    for (long toss = 0; toss < procTosses; toss++)
    {
        /*
        x = randNum(generate);
        y = randNum(generate);
        */

        x = rand_r(&seed) / (double)RAND_MAX;
        y = rand_r(&seed) / (double)RAND_MAX;
        if ((x*x+y*y) <= 1.0)
        {
            numCirc++;
        }
    }

    return numCirc;
}

/**
 * @brief Test if the point is in the circle
 * 
 * @param x 
 * @param y 
 * @return bool 
 */
bool circleTest(double x, double y)
{
    return ((x * x) + (y * y)) <= 1.0;
}