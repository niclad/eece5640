// OpenMP matrix multiplication
// Nicolas Tedori

#include <omp.h>
#include <iostream>
#include <string>

using namespace std;

const int M = 20;      // the m-dimension for matrices
const int RANGE = 10;   // maximum value for the random stuff

// FUNCTION DECLARATIONS
int* initVect(bool results) {
    int* tempArr = new int[M];

    for (int i = 0; i < M; i++) {
        if(results) {
            tempArr[i] = 0;
            continue;
        }
        tempArr[i] = i;//rand() % RANGE;
    }

    return tempArr;
}

int* matMult(int mat[][M], int vec[]) {
    int* result = new int[M];
    result = initVect(true);
    string msg;
    int element;
 
    int threadID = omp_get_thread_num();    // get the thread running this section
    msg = "Thread: " + to_string(threadID) + "\n";
    cout << msg;

    
    //#pragma omp parallel for shared(result, mat, vec) reduction(+: element)
    for (int i = 0; i < M; i++) {
        element = 0;
        //#pragma omp parallel for shared(mat, vec) reduction(+: element)
        for (int j = 0; j < M; j++) {
            msg = "thread" + to_string(omp_get_thread_num()) + "\n";
            cout << msg;
            element += mat[j][i] * vec[j];
        }
        result[i] = element;
    }

    return result;
}

int main() {
    int matrix[M][M];
    int* vect = new int[M];
    int maxThreads = omp_get_max_threads();
    int* result;

    cout << "Maximum threads available for omp: " << maxThreads << endl;

    // fill matrix
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < M; j++) {
            //cout << i*j << endl;
            matrix[i][j] = i*j;//rand() % RANGE;
        }
    }

    // fill vector
    vect = initVect(false);

    #pragma omp parallel
    result = matMult(matrix, vect);

    cout << "Resulting vector" << endl;
    for (int i = 0; i < 10; i++) {
        cout << result[i] << endl;
    }
    cout << "..." << endl;
    for (int i = M-11; i < M; i++) {
        cout << result[i] << endl;
    }
}