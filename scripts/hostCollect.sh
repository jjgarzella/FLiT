#!/bin/bash -x

#$1 is # of procs; $2 is cuda only; $3 is (PIN) collect opcode stats

mkdir -p results

#do the full test suite
cd src

# if [ $2 == True ]; then
# then
#     export CUDA_ONLY=True
# fi

make -j ${CORES}

cd ..

#do PIN
if [ ${DO_PIN} == True ]; then
    
    #setup PIN tool
    if [ -e pin ]; then
	rm -fr pin
    fi
    mkdir pin
    cd pin
    wget http://software.intel.com/sites/landingpage/pintool/downloads/\
	 pin-3.0-76991-gcc-linux.tar.gz
    tar xf pin*
    rm *.gz
    mv pin* pin

    pushd .
    cd pin/source/tools/SimpleExamples
    make obj-intel64/opcodemix.so
    popd

    export PINPATH=$(pwd)/pin

    #run pin tests
    cd ../results
    make -j ${CORES} -f ../scripts/MakeCollectPin
    cd ..
fi

cd results

#zip up all outputs
tar zcf $(hostname)_$(shell date +%m%d%y)_flit.tgz *

exit $?