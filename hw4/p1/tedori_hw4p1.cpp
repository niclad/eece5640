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
double CIRCLE = 1.0;    // circle radius

double randNum(rng generator);
void getInput(int argc, char *argv[], int rank, long *totalDarts);
double errorCalc(double piEst);
long toss(long procDarts, int rank);
bool circleTest(double x, double y);
double CLOCK();

int main(int argc, char *argv[])
{
    int rank;   // calling process ID
    int size;   // number of processes

    long totalDarts;   // total tosses
    long procDarts;    // total tosses for the process
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

    getInput(argc, argv, rank, &totalDarts);   // get the input values

    procDarts = totalDarts / size;    // tosses per process
    if (rank == 0) {
        cout << "Darts per process: " << procDarts << endl;
    }

    MPI_Barrier(MPI_COMM_WORLD);    // sync processes for the sake of timing
    start = MPI_Wtime();    // get start time
    //start = CLOCK();    // end timing
    procCircle = toss(procDarts, rank);
    end = MPI_Wtime();      // get end time
    locElapsed = end - start;
    cout << rank << " time: " << locElapsed << endl;
    // get largest process time
    MPI_Reduce(&locElapsed, &elapsed, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

    // get the total number of tosses that landed in the circle for all processes
    MPI_Reduce(&procCircle, &totalCircle, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    //end = CLOCK();  // end timing

    if (rank == 0)
    {
        //elapsed = (end - start) / 1000;     // calculate elapsed time in seconds
        cout << "totalCircle: " << totalCircle << endl;
        cout << "totalDarts: " << totalDarts << endl;
        piEst = (((double) totalCircle) / ((double) totalDarts)) * 4;   // get pi
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
 * @param totalDarts 
 */
void getInput(int argc, char *argv[], int rank, long *totalDarts)
{
    if (rank == 0)
    {
        if (argc != 2)
        {
            cout << "Error, not enough inputs" << endl;
            *totalDarts = 0;
        } else {
            *totalDarts = atoi(argv[1]);
        }
    }

    // send all processes the total number of tosses
    MPI_Bcast(totalDarts, 1, MPI_LONG, 0, MPI_COMM_WORLD);

    // end program because user wanted estimation from no tosses
    if (*totalDarts == 0)
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
 * @param procDarts 
 * @param rank 
 * @return long 
 */
long toss(long procDarts, int rank)
{
    long numCirc = 0;

    double x, y;

    srand48(time(NULL) + rank);
    for (long toss = 0; toss < procDarts; toss++)
    {
        /*
        x = randNum(generate);
        y = randNum(generate);
        */

        x = (double)random()/RAND_MAX;;
        y = (double)random()/RAND_MAX;;
        if (circleTest(x, y))
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
    return sqrt((x * x) + (y * y)) <= 1;
}

/**
 * @brief 
 * 
 * @return double 
 */
double CLOCK() {
        struct timespec t;
        clock_gettime(CLOCK_MONOTONIC,  &t);
        return (t.tv_sec * 1000)+(t.tv_nsec*1e-6);
}