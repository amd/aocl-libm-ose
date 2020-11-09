# How to build

## Installing virtualenv
  There are various ways to install virtualenv on Linux environment.
  Ubuntu
  ``` sh
     $ sudo apt install virtualenv-python3
  ```

  Use Virtual Environment to install python3 and scons inside.
  ```
   $ virtualenv -p python3 .venv3

   $ source .venv3/bin/activate
   $ pip install scons
  ```


  Now use the path 
   ```
     $ export PATH=`pwd`/.venv3/bin:$PATH
   ```

  Start buildling 
   ```
    $ scons  -h
   ```

  Building parallely
   ``` shell
    $ scons -j32
   ```
   
  Building with verbosity
   ``` shell
    $ scons <other options> --verbose=1
   ```
   
  Building with debug
   ```sh
    $ scons <other options> --debug=1
   ```
  
  Developer mode - Developers get a new define for their experimentation.
   ```sh
    $ scons <other options> --developer=1
   ```

## Building Tests

### To Build Tests

scons tests -j32                    //release mode
scons tests --debug_mode=all -j32   //debug mode

### To Build gtests

scons gtests -j32                    //release mode
scons gtests --debug_mode=all -j32   //debug mode


## Running Tests
###Set environment variable LD_LIBRARY_PATH for tests
 ```sh
    $ export BDIR=${pwd}/aocl-release
    $ export BDIR_DBG=${pwd}/aocl-debug
    
    $ LD_LIBRARY_PATH=${BDIR}/src/:${BDIR}/tests/libs/mparith/32/:${BDIR}/tests/libs/mparith/64/       # release mode
    
    LD_LIBRARY_PATH=${BDIR_DBG}/src/:${BDIR_DBG}/tests/libs/mparith/32/:${BDIR_DBG}/tests/libs/mparith/64/             # debug mode
 ```

### Set environment variable LD_LIBRARY_PATH for gtests

```sh
     $ LD_LIBRARY_PATH=${BDIR}/src/:${BDIR}/gtests/gapi/gtest/:${BDIR}/gtests/gapi/gbench/:${BDIR}/gtests/libs/mparith/32/:${BDIR}/gtests/libs/mparith/64/        #release mode
     
     $ LD_LIBRARY_PATH=${BDIR_DBG}/src/:${BDIR_DBG}/gtests/gapi/gtest/:${BDIR_DBG}/gtests/gapi/gbench/:${BDIR_DBG}/gtests/libs/mparith/32/:${BDIR_DBG}/gtests/libs/mparith/64/                  #debug mode


    $ export LD_LIBRARY_PATH
    $ echo $LD_LIBRARY_PATH
``` 

### To run test cases for gtests
#### command line arguments 
```sh
    ${BDIR}/funcname/func_exe --type= --count= --input= --vector= \
        --range= --verbose --Iterations=
                                                (or)
    ${BDIR}/funcname/func_exe -t -c -i -e -r -v -n
```
    where
        --type=accu or conf or spec or perf
        --count=Number of input_size
        --input=float or double
        --vector=1 or 2 or 4 or 8
        --range="min,max,algorithm(simple or random or linear)"
        --verbose=1-5(1-prints information only for failed cases, 4/5-prints information for all the cases)
        --Iterations=loop count to measure performance

Ex: 
```sh
    $ ${BDIR}/funcname/func_exe --type=accu --count=1000 --input=float --vector=1 --range=-120,80,simple --verbose=1
    (or) 
    $ ${BDIR}/funcname/func_exe -t accu -c 1000 -i f -e 1 -r -120,80,simple -v 1
``` 
For Help :     $ ${BDIR}/funcname/func_exe --help

### To run accuracy_test cases:
```sh
    $ ${BDIR}/funcname/func_exe --type=accu --count=200 --range=-1,1,simple

    $ ${BDIR}/funcname/func_exe --type=accu --count=1000 --input=float --range=-1,1,simple

    $ ${BDIR}/funcname/func_exe --type=accu --count=10000 --input=double --vector=4 --range=-1,1,simple --verbose=5
```

### With two input range values
```sh
    $ ${BDIR}/funcname/func_exe --type=accu --count=1000 --input=float --vector=1 --range=-120,80,simple  --range=-120,80,simple --verbose=1
```

### To run conformance_test cases
```sh
    $ ${BDIR}/funcname/func_exe --type=conf --input=float

    $ ${BDIR}/funcname/func_exe --type=conf --input=double
```
### To run special_test cases
```sh
    $ ${BDIR}/funcname/func_exe --type=spec --input=float

    $ ${BDIR}/funcname/func_exe --type=spec --input=double
```
### To measure performance
```sh
    $ ${BDIR}/funcname/func_exe --type=perf --Iterations=10000 --count=1000 --range=-120,80,simple

    $ ${BDIR}/funcname/func_exe -t perf -n 10000 -c 10000 -i f -e 1 -r -120,80,simple
```

## To Clean:
```sh
scons -c
```

## Possible future building
  An glibc compatible library, which has the table of functions compatible with
  GLIBC. or compile with AOCC to generate proper API calls to amdlibm.
  ``` sh
        $ $ LD_PRELOAD=/abs/path/amdlibm_wrapper.so
  ```
  
  A fast library, with upto 8 ULP (when available).
  ``` sh
        $ $ LD_PRELOAD=/abs/path/amdlibm_fast.so
  ```
  
### Building with glibc compatibility for tests

  Following is to build tests with glibc compatibility api, otherwise the tests
  will call, amd_fma3_* functions.

``` sh
      $ $ scons --libabi=glibc
```

  With this the tests will be compiled to call _libm_ functions, without the
  amd specific prefix.

  
