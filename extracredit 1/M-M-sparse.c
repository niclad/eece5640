#include <stdio.h>
#include<stdlib.h>
#include <time.h>
#define M 512 

double CLOCK() {
        struct timespec t;
        clock_gettime(CLOCK_MONOTONIC,  &t);
        return (t.tv_sec * 1000)+(t.tv_nsec*1e-6);
}

main(int argc, char **argv)
{
    int i,j,k;
    double start, finish, total, sum;
    float  a[M][M], b[M][M], c[M][M];


/* Initialize a, b and c */
    for (i=0; i<M; i++)
       for (j=0; j<M; j++)
          {
            if ((i+j)%7 == 0)
             a[i][j] = 2.;
            else a[i][j] = 0.;
	  }

    for (i=0; i<M; i++)
       for (j=0; j<M; j++)
             b[i][j] = (i/3)+(j/5); 

    for (i=0; i<M; i++)
       for (j=0; j<M; j++)
           c[i][j] = 0.;

/* Start timing */
    start = CLOCK();

/* This is the only portion of the code you should modify to improve performance. */
    float tempA, tempB;
    for (i =0; i<M; i++) {
        for (j=0; j<M; j++) {
            sum = 0;
            for (k=0; k<M; k++) {
                if ((i+k)%7 != 0)
                    continue;
                    
                if ((k/3)+(j/5) == 0)
                    continue;

                sum += a[i][k] * b[k][j];
            }
            c[i][j] += sum;
        }
    }

   finish = CLOCK();
   /* End timing */
   total = finish - start;
   printf("Time for the loop = %f\n", total);
   printf("The value of c[%d][%d] = %4.2f\n", 0, 0, c[0][0]);
   printf("The value of c[%d][%d] = %4.2f\n", 31, 32, c[31][32]);
   printf("The value of c[%d][%d] = %4.2f\n", 510, 0, c[510][0]);
   printf("The value of c[%d][%d] = %4.2f\n", 511, 511, c[511][511]);

   return 0;
}

