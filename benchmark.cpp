#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>
#include <float.h>
#include <math.h>
#include <mkl.h>
#include <sys/time.h>
//#include <acml.h> //assumes AMD platform
#include <mkl.h>

#ifdef ADDPAPI
#include <papi.h>
#include "global.h"
long long global_CM[NUM_EVENTS];
#endif

/* Your function must have the following signature: */

extern const char* dgemm_desc;
extern void square_dgemm( int M, double *A, double *B, double *C);
//extern void dgemm_jki( int M, double *A, double *B, double *C);
int thread_block;
/* Helper functions */

double read_timer( )
{
    static bool initialized = false;
    static struct timeval start;
    struct timeval end;
    if( !initialized )
    {
        gettimeofday( &start, NULL );
        initialized = true;
    }

    gettimeofday( &end, NULL );

    return (end.tv_sec - start.tv_sec) + 1.0e-6 * (end.tv_usec - start.tv_usec);
}

void fill( double *p, int n )
{
    for( int i = 0; i < n; i++ )
        p[i] = 2 * drand48( ) - 1;
}

void absolute_value( double *p, int n )
{
    for( int i = 0; i < n; i++ )
        p[i] = fabs( p[i] );
}


/* The benchmarking program */

int main( int argc, char **argv )
{
    printf ("Description:\t%s\n\n", dgemm_desc);

    /* These sizes should highlight performance dips at multiples of certain powers-of-two */
    //int test_sizes[] = {49152};
    int test_sizes[1];
    test_sizes[0] = atoi(argv[1]);
    int loop = atoi(argv[2]);
    int threads = atoi(getenv("OMP_NUM_THREADS"));
    thread_block = atoi(argv[3]);

    printf("test_size[0]=%d, loop=%d, threads=%d, thread_block=%d\n", 
            test_sizes[0], loop, threads, thread_block);
    fflush(stdout);

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

    /*For each test size*/
    for( int isize = 0; isize < sizeof(test_sizes)/sizeof(test_sizes[0]); isize++ )
    {
		/*Craete and fill 3 random matrices A,B,C*/
        int n = test_sizes[isize];

        double *A = (double*) malloc( n * n * sizeof(double) );
        double *B = (double*) malloc( n * n * sizeof(double) );
        double *C = (double*) malloc( n * n * sizeof(double) );

        fill( A, n * n );
        fill( B, n * n );
        fill( C, n * n );
        
        printf("start calculation\n"); 
        fflush(stdout);

        /*  measure Mflop/s rate; time a sufficiently long sequence of calls to eliminate noise*/
        double Mflop_s, seconds = -1.0;
        //int iter = 0;
        int n_iterations;

/*#ifdef ADDPAPI*/
        //long long value_CM[NUM_EVENTS];
        //int retval;
        //retval = PAPI_start_counters(EventSet, NUM_EVENTS);
/*#endif*/

#ifdef ADDPAPI
    for(int m=0; m < NUM_EVENTS; m++)
        global_CM[m] = 0;
#endif

        seconds = read_timer( );
        //for( int  n_iterations = 1; seconds < 0.1; n_iterations *= 2 ) 
        for( n_iterations = 0; n_iterations < loop; n_iterations++  ) 
        {
            /* warm-up */
            //square_dgemm( n, A, B, C);
            
            /*  measure time */
            //seconds = read_timer( );
            //for( int i = 0; i < n_iterations; i++ )
                square_dgemm( n, A, B, C);
                //dgemm_jki( n, A, B, C);
            //seconds = read_timer( ) - seconds;
           
            /*  compute Mflop/s rate */
            //Mflop_s = 2e-6 * n_iterations * n * n * n / seconds;
            //iter = n_iterations;
        }	
        seconds = read_timer( ) - seconds;

#ifdef ADDPAPI
        //retval=PAPI_stop_counters(value_CM, NUM_EVENTS);
        ////printf("Total_Cycle:%lld\tclock_rate/core:%e\n", value_CM[0], ((double)value_CM[0])/(threads*seconds));
        printf("clock_rate/core:%e\n", ((double)global_CM[0])/(threads*seconds));
        //printf("Total_Cycle:%lld\n", value_CM[0]);
#endif
        Mflop_s = 2e-9 * n_iterations * n * n * n / seconds;
        double Gflops_per_core = Mflop_s / threads;
        
        printf ("Size: %d\tWhole_Gflop/s: %.3f\tn_iterations: %d\tthreads=%d\n", n, Mflop_s, n_iterations, threads);
        printf("Time: %.3f\n", seconds);
        printf("Gflops_per_core: %.3f\n", Gflops_per_core);
        /*  Ensure that error does not exceed the theoretical error bound */
		
	/* Set initial C to 0 and do matrix multiply of A*B */
        memset( C, 0, sizeof( double ) * n * n );
        square_dgemm( n, A, B, C);
	/*Subtract A*B from C using standard dgemm (note that this should be 0 to within machine roundoff)*/
        const double BETA = 1.0;
	const double negOne = -1.0;
	char *ntran = "N";
	//dgemm( 'N','N', n,n,n, -1, A,n, B,n, 1, C,n );
	dgemm(ntran, ntran, &n, &n, &n, &negOne, A, &n, B, &n, &BETA, C, &n);
		/*Subtract the maximum allowed roundoff from each element of C*/
        absolute_value( A, n * n );
        absolute_value( B, n * n );
        absolute_value( C, n * n );
	double temp_val = -3.0*DBL_EPSILON*n;
        //dgemm( 'N','N', n,n,n, -3.0*DBL_EPSILON*n, A,n, B,n, 1, C,n );
	dgemm(ntran, ntran, &n, &n, &n, &temp_val, A, &n, B, &n, &BETA, C, &n);
	/*After this test if any element in C is still positive something went wrong in square_dgemm*/
        for( int i = 0; i < n * n; i++ ){
            if( C[i] > 0 )
            {
                printf( "FAILURE: error in matrix multiply exceeds an acceptable margin\n" );
                exit(-1);
            }
        }
    printf("Pass verification!\n");
	/*Deallocate memory*/
        free( C );
        free( B );
        free( A );
    }
    
    return 0;
}
