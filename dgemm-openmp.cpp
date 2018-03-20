#include<omp.h>
#include<stdlib.h>
#include<stdio.h>

#ifdef ADDPAPI
#include <papi.h>
#include "global.h"
int EventSet[]={PAPI_TOT_CYC, PAPI_L1_TCM};
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
    

/*#ifdef ADDPAPI*/
    //int retval = PAPI_library_init (PAPI_VER_CURRENT);
    //if (retval != PAPI_VER_CURRENT) {
        //printf("PAPI_library_init error!\n");
        //exit(1);
    //}
    //if (PAPI_thread_init((unsigned long (*)(void))(omp_get_thread_num)) != PAPI_OK){
        //printf("PAPI_thread_init error!\n");
        //exit(1);
    //}
//#endif

    /*dgemm_jki*/
/*#ifdef ADDPAPI*/
    //long long global_CM[NUM_EVENTS];
    //for(int m=0; m < NUM_EVENTS; m++)
        //global_CM[m] = 0;
/*#endif*/
    int num_threads=0;
  #pragma omp parallel default(shared) reduction(+: global_CM)
  {
      num_threads = omp_get_num_threads();
      printf("start computation, num_threads=%i, thread_block=%d\n", num_threads, thread_block);
  /*printf("Hello! Thread rank:%i\n",omp_get_thread_num());*/

#ifdef ADDPAPI
        long long value_CM[NUM_EVENTS];
        int retval;
        //int EventSet[]={PAPI_TOT_CYC, PAPI_L1_TCM};
/*        for(int m=0; m<NUM_EVENTS; m++)*/
            /*value_CM[m]=0;*/

        retval = PAPI_start_counters(EventSet, NUM_EVENTS);
#endif

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
        for(int m=0; m<NUM_EVENTS; m++){
            printf("T%02d: EVENT[%d]:%lld\n", omp_get_thread_num(), m, value_CM[m]);
        } 
        
        for(int m=0; m<NUM_EVENTS; m++)
            global_CM[m] += value_CM[m];
#endif

  }

#ifdef ADDPAPI
    for(int m=0; m<NUM_EVENTS; m++){
      printf("Global_Total_EVENT[%d]:%lld\n", m, global_CM[m]);
      //printf("AVG_Global_Total_EVENT[%d]:%.3f\n", m, ((double)global_CM[m])/num_threads);
    }  
#endif

  printf("Finish computation\n");

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
