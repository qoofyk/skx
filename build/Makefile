#Comet
#MKLPATH=/opt/intel/Compiler/11.1/072/mkl
vpath %.cpp ../
vpath %.h ../

MKLPATH=${MKLROOT}
CC=icc
MPICC=mpiicc
CFLAGS = -I. -I$(MKLPATH)/include -I${PAPI_DIR}/include -DADDPAPI
LDFLAGS = -mkl -lpthread -lm ${PAPI_DIR}/lib/libpapi.a
OMPFLAGS = -qopenmp

all: benchmark-blocked benchmark-blas benchmark-naive benchmark-openmp benchmark-mpi benchmark-pthread

benchmark-blocked: benchmark.o dgemm-blocked.o
	$(CC) -o $@ $^ $(LDFLAGS)
benchmark-blas: benchmark.o dgemm-blas.o
	$(CC) -o $@ $^ $(LDFLAGS)
benchmark-naive: benchmark.o dgemm-naive.o
	$(CC) $(OMPFLAGS) -o $@ $^ $(LDFLAGS)
benchmark-pthread: benchmark.o dgemm-pthread.o
	$(CC) $(OMPFLAGS) -o $@ $^ $(LDFLAGS)
benchmark-openmp: benchmark.o dgemm-openmp.o
	$(CC) $(OMPFLAGS) -o $@ $^ $(LDFLAGS)
benchmark-mpi: test.o dgemm-mpi.o
	$(MPICC) -o $@ $^ $(LDFLAGS)

benchmark.o: benchmark.cpp global.h
	$(CC) $(OMPFLAGS) $(CFLAGS) -c -O3 -Wall $<
dgemm-blocked.o:dgemm-blocked.cpp
	$(CC) -c -O3 -Wall $<
dgemm-blas.o:dgemm-blas.cpp
	$(CC) -c -O3 -Wall $<
dgemm-naive.o:dgemm-naive.cpp
	$(CC) -c -O3 -Wall $<
dgemm-pthread.o:dgemm-pthread.cpp
	$(CC) -c -O3 -Wall $<
dgemm-openmp.o:dgemm-openmp.cpp global.h
	$(CC) ${TACC_VEC_FLAGS} $(CFLAGS) $(OMPFLAGS) -c -O3 -Wall $<
test.o: test.cpp
	$(MPICC) -c -O3 $<
dgemm-mpi.o: dgemm-mpi.cpp
	$(MPICC) -c -O3 $<

clean:
	rm -f benchmark-naive benchmark-blocked benchmark-blas benchmark-mpi benchmark-pthread benchmark-openmp *.o

