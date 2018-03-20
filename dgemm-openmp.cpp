#include<omp.h>
#include<stdlib.h>
#include<stdio.h>

#define ADDPAPI
#ifdef ADDPAPI
#include <papi.h>
#define NUM_EVENTS 1
#endif

const char *dgemm_desc = "openMP, three-loop dgemm.";
extern int thread_block;

void square_dgemm( int n, double *A, double *B, double *C)
{
  //omp_set_num_threads(thread_count);
  //printf("enter matrix multiplication using openmp\n");

    /*dgemm_ijk*/
/*  #pragma omp parallel default(shared)*/
  //{
  ////printf("Hello! I am thread_num=%i\n",omp_get_thread_num());
  //int i,j,k;
  //#pragma omp for private(i,j,k) schedule(static)
  //for( i = 0; i < n; i++ )
       //for( j = 0; j < n; j++ )
       //{
            //double cij = C[i+j*n];
            //for( k = 0; k < n; k++ )
                 //cij += A[i+k*n] * B[k+j*n];
            //C[i+j*n] = cij;
       //}
  /*}*/
#ifdef ADDPAPI
    int retval = PAPI_library_init (PAPI_VER_CURRENT);
    if (retval != PAPI_VER_CURRENT) {
        printf("PAPI_library_init error!\n");
        exit(1);
    }
    if (PAPI_thread_init((unsigned long (*)(void))(omp_get_thread_num)) != PAPI_OK){
        printf("PAPI_thread_init error!\n");
        exit(1);
    }
#endif

#ifdef ADDPAPI
        long long value_CM[NUM_EVENTS];
        retval;
        int EventSet[]={PAPI_TOT_CYC};
        retval = PAPI_start_counters(EventSet, NUM_EVENTS);
#endif

    /*dgemm_jki*/
  #pragma omp parallel default(shared)
  {
/*  printf("start computation, num_threads=%i\n",omp_get_num_threads());*/
  /*printf("Hello! Thread rank:%i\n",omp_get_thread_num());*/

  double bkj;
  int i, j, k;
  #pragma omp for private(i,j,k, bkj) schedule(static, thread_block)
    for (j = 0; j < n; ++j)
        for (k = 0; k < n; ++k){
            bkj = B[k+j*n];
            for(i = 0; i < n; i++ ){
                C[i+j*n] += A[i+k*n] * bkj; /* cij += A[i][k]*B[k][j] */
            }
        }

#ifdef ADDPAPI
        retval=PAPI_stop_counters(value_CM, NUM_EVENTS);
        //printf("Total_Cycle:%lld\tclock_rate/core:%e\n", value_CM[0], ((double)value_CM[0])/(threads*seconds));
        //printf("clock_rate/core:%e\n", ((double)value_CM[0])/(threads*seconds));
        printf("T%02d: Total_Cycle:%lld\n", omp_get_thread_num(), value_CM[0]);
#endif

  }


}

/*void dgemm_jki(int n, double* A, double* B, double* C){*/
  //#pragma omp parallel default(shared)
  //{
  ////printf("Hello! I am thread_num=%i\n",omp_get_thread_num());

  //double bkj;
  //#pragma omp for private(i,j,k, bkj) schedule(static)
    //for (int j = 0; j < n; ++j)
        //for (int k = 0; k < n; ++k){
            //bkj = B[k+j*n];
            //for(int i = 0; i < n; i++ ){
                //C[i+j*n] += A[i+k*n] * bkj; [> cij += A[i][k]*B[k][j] <]
            //}
        //}
  //}
/*}*/
