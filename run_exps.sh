#!/bin/bash
WORK_DIR=`pwd`
RESULT_DIR=${WORK_DIR}/results/`date +"%y%m%d_%H%M"`
mkdir -pv ${RESULT_DIR}

BUILD_DIR=${WORK_DIR}/build
cd ${RESULT_DIR}

lscpu &>> ${RESULT_DIR}/log

echo "result will be saved in ${RESULT_DIR}" &>> ${RESULT_DIR}/log


#dimension
#threads=(1 2 4 8 16 24 32 48)
#lx=(1024 1440 2048 2880 4096 5040 5760 6912)
#threads=(8 16 24 32 48)
#lx=(2880 4096 5040 5760 6912)
threads=(1 2 4 8 16 24 32 48)
lx=(6912 6912 6912 6912 6912 6912 6912 )

for((k=0; k<${#lx[@]};k++)); do
    export OMP_NUM_THREADS=${threads[k]}
    export KMP_AFFINITY=verbose,granularity=core,scatter
    #thread_block=$((${lx[k]} / ${OMP_NUM_THREADS}))
    #for CASE_NAME in origin_openmp combine_openmp tight_openmp tight_block_openmp
    #do
        for i in `seq 1`
        do
            echo "No.$i exp of case lx=${lx[k]} threads=${threads[k]} "
            ${BUILD_DIR}/benchmark-openmp ${lx[k]} &>> ${RESULT_DIR}/log
            echo "No.$i exp of case lx=${lx[k]} done " &>> ${RESULT_DIR}/log
        done
    #done
done

