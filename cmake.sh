#!/bin/ksh
#/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8
# Name        :
# Author      : Avi
# Revision    : $Revision: #5 $ 
#
# Copyright 2009- ECMWF.
# This software is licensed under the terms of the Apache Licence version 2.0 
# which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
# In applying this licence, ECMWF does not waive the privileges and immunities 
# granted to it by virtue of its status as an intergovernmental organisation 
# nor does it submit to any jurisdiction. 
#/////////1/////////2/////////3/////////4/////////5/////////6/////////7/////////8

# ==================================================================
# Error handling
set -e # stop the shell on first error
set -u # fail when using an undefined variable

# ensure correct permission for installation
umask 0022

# docker build?
[[ -d opt/boost_1_53_0/ ]] && export BOOST_ROOT=/opt/boost_1_53_0/ ARCH=linux PYTHONPATH=/usr/local:${PYTHONPATH:=} LD_LIBRARY_PATH=/usr/local/lib:${LD_LIBRARY_PATH:=}

# ====================================================================
show_error_and_exit() {
   echo "cmake.sh expects at least one argument"
   echo " cmake.sh [ options] "
   echo ""
   echo "   debug          - make a DEBUG build"
   echo "   make           - run make after cmake"
   echo "   ecbuild        - Use git cloned ecbuild over the module loaded ecbuild(default)"
   echo "   sys_install    - install to /usr/local/apps/ecflow/<version>  defaults is /var/tmp/$USER/install/cmake/ecflow/<version>"
   echo "   test           - run all the tests"
   echo "   test_safe      - only run deterministic tests"
   echo "   ctest          - all ctest -R <test> -V"
   echo "   clang          - build with clang compiler"
   echo "   shared         - build with shared boost libs"
   echo "   intel          - build with intel compiler"
   echo "   clang_tidy     - create compilation database for clang_tdiy and then call run-clang-tidy.py"
   echo "   iwyu           - INCLUDE what you use"
   echo "   tsan           - is short for clang thread sanitiser"
   echo "   asan           - is short for address sanitiser"
   echo "   msan           - is short for memory sanitiser"
   echo "   no_gui         - Don't build the gui"
   echo "   no_ssl         - build without using openssl"
   echo "   log            - enable debug output"
   echo "   boost          - use environment variable BOOST_ROOT=$BOOST_ROOT"
   echo "   package_source - produces ecFlow-<version>-Source.tar.gz file, for users"
   echo "                    copies the tar file to $SCRATCH"
   echo "   copy_tarball   - copies ecFlow-<version>-Source.tar.gz to /tmp/$USER/tmp/. and untars file"
   echo ""
   echo "For a system install"
   echo "./cmake.sh sys_install make -j8 install"
   exit 1
}

sys_install=
ecbuild_arg=
copy_tarball_arg=
package_source_arg=
make_arg=
shared_arg=
make_only_arg=
test_arg=
test_safe_arg=
clang_arg=
clang_tidy_arg=
clang_tidy_args=
intel_arg=
tsan_arg=
mode_arg=release
verbose_arg=
ctest_arg=
clean_arg=
no_gui_arg=
no_ssl_arg=
log_arg=
asan_arg=
msan_arg=
ubsan_arg=
iwyu_arg=
boost_arg=
while [[ "$#" != 0 ]] ; do   
   if [[ "$1" = debug || "$1" = release ]] ; then
      mode_arg=$1
   elif  [[ "$1" = make_only ]] ; then
      make_only_arg=make
      shift
      while [[ "$#" != 0 ]] ; do
         make_only_arg="$make_only_arg $1"
         shift
      done
      break
   elif  [[ "$1" = make ]] ; then
      make_arg=$1
      shift
      while [[ "$#" != 0 ]] ; do
         make_arg="$make_arg $1"
         shift
      done
      break
   elif [[ "$1" = clang_tidy ]] ; then 
      clang_tidy_arg=$1 ;
      shift
      while [[ "$#" != 0 ]] ; do
         clang_tidy_args="$clang_tidy_args $1"
         shift
      done
      break      
   elif [[ "$1" = iwyu ]] ;    then iwyu_arg=$1 ;
   elif [[ "$1" = shared ]] ;  then shared_arg=$1 ;
   elif [[ "$1" = boost ]] ;   then boost_arg=$1 ;
   elif [[ "$1" = no_gui ]] ;  then no_gui_arg=$1 ;
   elif [[ "$1" = no_ssl ]] ;  then no_ssl_arg=$1 ;
   elif [[ "$1" = sys_install ]] ; then sys_install=$1 ;
   elif [[ "$1" = ecbuild ]] ; then ecbuild_arg=$1 ;
   elif [[ "$1" = log ]]   ; then log_arg=$1 ;
   elif [[ "$1" = clang ]] ; then clang_arg=$1 ;
   elif [[ "$1" = intel ]] ; then intel_arg=$1 ;
   elif [[ "$1" = clean ]] ; then clean_arg=$1 ;
   elif [[ "$1" = tsan ]]   ; then tsan_arg=$1 ;
   elif [[ "$1" = asan ]]  ; then asan_arg=$1 ;
   elif [[ "$1" = msan ]]  ; then msan_arg=$1 ;
   elif [[ "$1" = ubsan ]]  ; then ubsan_arg=$1 ;
   elif [[ "$1" = package_source ]] ; then package_source_arg=$1 ;
   elif [[ "$1" = copy_tarball ]] ; then copy_tarball_arg=$1 ;
   elif [[ "$1" = test ]] ;  then test_arg=$1 ;
   elif [[ "$1" = test_safe ]] ; then test_safe_arg=$1 ;
   elif [[ "$1" = ctest ]] ; then  
      ctest_arg=$1 ; 
      shift
      while [[ "$#" != 0 ]] ; do
         ctest_arg="$ctest_arg $1"
         shift
      done
      break
   else
   	 show_error_and_exit
   fi

   # shift remove last argument
   shift
done

echo "copy_tarball_arg=$copy_tarball_arg"
echo "package_source_arg=$package_source_arg"
echo "make_arg=$make_arg"
echo "test_arg=$test_arg"
echo "test_safe_arg=$test_safe_arg"
echo "clang_arg=$clang_arg"
echo "clang_tidy_arg=$clang_tidy_arg"
echo "tsan_arg=$tsan_arg"
echo "mode_arg=$mode_arg"
echo "verbose_arg=$verbose_arg"
echo "no_gui_arg=$no_gui_arg"
echo "ecbuild_arg=$ecbuild_arg"
set -x # echo script lines as they are executed
set -o pipefail # fail if last(rightmost) command exits with a non-zero status

source_dir=$(pwd)

# ==================== compiler flags ========================================
# 
# GNU 4.8+ -Wno-unused-local-typedefs   -> get round warning in boost headers
# GNU 5.3  -Wno-unused-variable         -> get round warning in boost headers
# GNU 6.1  -Wno-deprecated-declarations -> auto_ptr deprecated warning, mostly in boost headers  
# GNU 7.3  -Wno-maybe-uninitialized     -> boost python warnings, specific to Pyext only
# CLANG    -ftemplate-depth=512
#
CXX_FLAGS="-Wno-unused-local-typedefs -Wno-unused-variable -Wno-deprecated-declarations -Wno-uninitialized -Wno-maybe-uninitialized"
CXX_LINK_FLAGS=""

# ==================== modules ================================================
# To load module automatically requires Korn shell, system start scripts

module swap gnu/7.3.0     

cmake_extra_options=""
if [[ "$clang_arg" = clang || "$clang_tidy_arg" = clang_tidy || "$iwyu_arg" = iwyu ]] ; then
	module unload gnu
	module load clang/9.0.1

    # [-Wdeprecated-register] /usr/local/apps/python/2.7.12-01/include/python2.7/unicodeobject.h:534:5: warning: 'register' storage class specifier is deprecated and incompatible with C++17 [-Wdeprecated-register]
    # [-Wmacro-redefined]     /usr/local/apps/python/2.7.12-01/include/python2.7/pyconfig.h:1215:9: warning: '_XOPEN_SOURCE' macro redefined
    CXX_FLAGS=""
    CXX_FLAGS="$CXX_FLAGS -Wno-deprecated-declarations -Wno-deprecated-register -Wno-expansion-to-defined -Wno-exceptions"

    if [[ "$iwyu_arg" = iwyu ]] ; then
        cmake_extra_options="-DCMAKE_CXX_INCLUDE_WHAT_YOU_USE=/usr/local/apps/iwyu/0.11/bin/include-what-you-use;-Xiwyu;--mapping_file=$source_dir/build_scripts/iwyu/boost.imp"
    fi
fi
if [[ "$intel_arg" = intel ]] ; then
    # fails because:
    # /tmp/ma0/workspace/ecflow/ACore/src/cereal_optional_nvp.hpp(52): error: namespace "std" has no member "enable_if_t"
    # > icc -v
    # > icc version 19.0.4.243 (gcc version 4.8.0 compatibility)
    # Require setting LD_RUN_PATH an CXX, to correctly pick the right std library
    module unload eccodes
    module unload gnu
    module load intel/19.0.4
    export LD_RUN_PATH="/usr/local/apps/gcc/8.3.0/lib64"
    export CXX="icpc -cxxlib=/usr/local/apps/gcc/8.3.0"
    export PATH="/usr/local/apps/gcc/8.3.0/bin:$PATH"
    $CXX -v
    #exit 1
    CXX_FLAGS="-std=c++14"
    #CXX_FLAGS="$CXX_FLAGS -Wno-deprecated-declarations -Wno-deprecated-register -Wno-expansion-to-defined -Wno-exceptions"
fi

if [[ $ecbuild_arg != ecbuild ]] ; then
    module load ecbuild/new   
fi
module load python
module load python3/3.6.10-01
#module load python3/3.8.8-01
module load cmake/new   # need cmake 3.12.0 to build python3. Allow boost python 2 and 3 libs to be found  


# ==============================================================================================
# sanitisers
if [[ "$tsan_arg" = tsan && "$asan_arg" = asan ]] ; then
    echo "Cant use address and thread sanitiser at the same time"
fi
if [[ "$tsan_arg" = tsan ]] ; then
   CXX_FLAGS="$CXX_FLAGS -fsanitize=thread -fno-omit-frame-pointer"
   CXX_LINK_FLAGS="$CXX_LINK_FLAGS -fsanitize=thread"
fi
if [[ "$asan_arg" = asan ]] ; then
   CXX_FLAGS="$CXX_FLAGS -fsanitize=address -fno-omit-frame-pointer"
   CXX_LINK_FLAGS="$CXX_LINK_FLAGS -fsanitize=address"
   export ECF_TEST_SANITIZER_AS=1  # enable address sanitizer tests
fi
if [[ "$ubsan_arg" = ubsan ]] ; then
   #CXX_FLAGS="$CXX_FLAGS -fsanitize=undefined -fsanitize-minimal-runtime -fno-omit-frame-pointer -fno-sanitize-recover=all" # exit after all errors minimal info
   #CXX_FLAGS="$CXX_FLAGS -fsanitize=undefined -fno-omit-frame-pointer -fno-sanitize-recover=all"                           # exit on first error, max info
   CXX_FLAGS="$CXX_FLAGS -fsanitize=undefined -fno-omit-frame-pointer"                                                     # report error, max info, no exit on error
   CXX_LINK_FLAGS="$CXX_LINK_FLAGS -fsanitize=undefined"
   export ECF_TEST_SANITIZER_UB=1  # enable undefined behaviour tests
fi
if [[ "$msan_arg" = msan ]] ; then
   CXX_FLAGS="$CXX_FLAGS -fsanitize=memory -fPIE -fno-omit-frame-pointer -fsanitize-memory-track-origins"
   CXX_LINK_FLAGS="$CXX_LINK_FLAGS -fsanitize=memory" # -fPIE -pie"
fi


if [[ "$shared_arg" = shared ]] ; then
   cmake_extra_options="$cmake_extra_options -DENABLE_STATIC_BOOST_LIBS=OFF"
fi


if [[ "$ARCH" = cray ]] ; then

    # disable new UI, no QT on cray
    # Use the cray wrappers, these will add the correct flags.
    # Assumes we have CRAY_ADD_RPATH=yes
    cmake_extra_options="$cmake_extra_options -DENABLE_UI=OFF -DCMAKE_C_COMPILER=cc -DCMAKE_CXX_COMPILER=CC"
    
    if [[ $intel_arg = intel ]] ; then
        module swap PrgEnv-cray PrgEnv-intel
    else
    	module swap PrgEnv-cray PrgEnv-gnu
    	module swap gcc/7.3.0     
    fi
    module unload eccodes                 # Otherwise ecflow.so pulls in libeccodes.so
    module unload atp                     # must use for NON MPI code (ATP abnormal termination processing only works with cray MPI for ESM modes)
    module load craype-target-local_host  # must use for NON MPI code
    export CRAY_ADD_RPATH=yes
    export ECFLOW_CRAY_BATCH=1
fi

#####################################################################################
# Boost:
#  By default it looks for environment variable BOOST_ROOT, if not it can specified on the command line. i.e
#  -DBOOST_ROOT=/var/tmp/$USER/boost/boost_1_53_0
#
# This must be done after change of compiler/environment
# versions of boost >= 1.67 now tag the python libs, i.e. libboost_python27-mt.a, libboost_python36-mt.so
if [[ $boost_arg = boost ]] ; then
    echo "Using environment variable BOOST_ROOT=$BOOST_ROOT"
else
    module load boost/1.71.0      
fi


# ====================================================================================
# default to Release  
cmake_build_type=
if [[ $mode_arg = debug ]] ; then
    cmake_build_type=Debug
else
    cmake_build_type=Release
fi


# ====================================================================================
# clean up source before packaging, do this after deleting ecbuild
#
if [[ $package_source_arg = package_source ]] ; then
	source build_scripts/clean.sh
fi

# ====================================================================================
# Use for local install
#
cd $WK
release=$(cat ACore/src/ecflow_version.h | grep 'ECFLOW_RELEASE' | awk '{print $3}'| sed 's/["]//g')
major=$(cat ACore/src/ecflow_version.h   | grep 'ECFLOW_MAJOR'   | awk '{print $3}'| sed 's/["]//g')
minor=$(cat ACore/src/ecflow_version.h   | grep 'ECFLOW_MINOR'   | awk '{print $3}'| sed 's/["]//g')

install_prefix=/var/tmp/$USER/install/cmake/ecflow/$release.$major.$minor
if [[ $sys_install = sys_install ]] ; then
   install_prefix=/usr/local/apps/ecflow/$release.$major.$minor
fi

# =======================================================================================
# Change directory to build directory
#
workspace=$(pwd)/..

build_dir_root=../bdir

# separate build dir for clang tidy, so we can check fixes
if [[ "$clang_tidy_arg" = clang_tidy ]] ; then
    build_dir_root=../bdir_clang
fi

if [[ $clean_arg = clean ]] ; then
	rm -rf $build_dir_root/$mode_arg/ecflow
fi

mkdir -p $build_dir_root/$mode_arg/ecflow
cd $build_dir_root/$mode_arg/ecflow

# =============================================================================================
# ctest
#
if [[ $test_safe_arg = test_safe ]] ; then
	ctest -R ^u_
	ctest -R c_
	ctest -R py_u
	ctest -R s_client
	if [[  $test_safe_arg = test_safe ]] ; then
	   exit 0
	fi
fi


if [[ "$ctest_arg" != "" ]] ; then
    if [[ "$asan_arg" = asan ]] ; then
    
        # LD_PRELOAD is only required when creating a shared lib, than needs asan
        # export LD_PRELOAD=/usr/local/apps/clang/7.0.1/lib64/libasan.so  
        #if [[ $clang_arg != "clang" ]] ; then
        #    # for python module we need to preload asan as it needs to be the very first library
        #    # ==2971==ASan runtime does not come first in initial library list; 
        #    #              you should either link runtime to your application or manually preload it with LD_PRELOAD.
	    #    export LD_PRELOAD=/usr/local/apps/gcc/7.3.0/lib64/gcc/x86_64-suse-linux/7.3.0/libasan.so 
	    #fi
	    export ASAN_OPTIONS=suppressions=$WK/build_scripts/ecflow_asan.supp  
	    export LSAN_OPTIONS=suppressions=$WK/build_scripts/ecflow_lsan.supp
	    $ctest_arg  
    elif [[ "$tsan_arg" = tsan ]] ; then
        export LD_PRELOAD=/usr/local/apps/clang/7.0.1/lib64/libtsan.so  

        if [[ $clang_arg != "clang" ]] ; then
            # LD_PRELOAD needed otherwise we get: .... cannot allocate memory in static TLS block
            export LD_PRELOAD=/usr/local/apps/gcc/7.3.0/lib64/gcc/x86_64-suse-linux/7.3.0/libtsan.so
        fi
        export ASAN_OPTIONS=suppressions=$WK/build_scripts/ecflow_asan.supp  
        export LSAN_OPTIONS=suppressions=$WK/build_scripts/ecflow_lsan.supp
        $ctest_arg 
    else
        $ctest_arg 
	fi
	exit 0
fi

if [[ "$make_only_arg" != "" ]] ; then
	$make_only_arg
	exit 0
fi

# ====================================================================================
#
# -DCMAKE_PYTHON_INSTALL_TYPE = [ local | setup ]
# -DCMAKE_PYTHON_INSTALL_PREFIX should *only* used when using python setup.py (CMAKE_PYTHON_INSTALL_TYPE=setup)
#   *AND* for testing python install to local directory
#

ssl_options=
if [[ $no_ssl_arg = no_ssl ]] ; then
    ssl_options="-DENABLE_SSL=OFF"
fi

log_options=
if [[ $log_arg = log ]] ; then
    log_options="-DECBUILD_LOG_LEVEL=DEBUG"
fi

gui_options=
if [[ $no_gui_arg = no_gui ]] ; then
    gui_options="-DENABLE_UI=OFF"
fi

test_options=
if [[ $test_arg = test ]] ; then
   test_options="-DENABLE_ALL_TESTS=ON"
fi

if [[ $package_source_arg = package_source ]] ; then
    # for packaging we build GUI by default, and do not run all tests
    gui_options=  
fi

ecbuild=ecbuild
if [[ $ecbuild_arg = ecbuild ]] ; then
   ecbuild=$workspace/ecbuild/bin/ecbuild
fi

# An alternative is to run cmake directly. (i.e to use its options/flags)
# cmake -C $workspace/ecflow/bamboo/macosx1010-flags.cmake $source_dir \
#        -DCMAKE_MODULE_PATH=$workspace/ecbuild/cmake \
#  .....
#
# DCMAKE_EXPORT_COMPILE_COMMANDS create compiler database. used by clang-tidy and others

$ecbuild $source_dir \
            -DCMAKE_BUILD_TYPE=$cmake_build_type \
            -DCMAKE_INSTALL_PREFIX=$install_prefix  \
            -DENABLE_WARNINGS=ON \
            -DCMAKE_CXX_FLAGS="$CXX_FLAGS" \
            -DCMAKE_EXE_LINKER_FLAGS="$CXX_LINK_FLAGS" \
            -DSITE_SPECIFIC_SERVER_SCRIPT="/home/ma/emos/bin/ecflow_site.sh" \
            ${cmake_extra_options} \
            -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
            ${gui_options} \
            ${ssl_options} \
            ${log_options} \
            ${test_options} \
            -DCMAKE_PREFIX_PATH="/usr/local/apps/qt/5.7.0/5.7/gcc_64/" 
            
            
            #-DPYTHON_EXECUTABLE=/usr/local/apps/python3/3.6.8-01/bin/python3 \
            #-DPYTHON_EXECUTABLE=/usr/local/apps/python/2.7.12-01/bin/python \
            #-DCMAKE_EXE_LINKER_FLAGS='-fsanitize=memory -fPIE -pie' 
            #-DCMAKE_PYTHON_INSTALL_TYPE=local \
            #-DENABLE_PYTHON=OFF   \
            #-DENABLE_PYTHON_PTR_REGISTER=ON  \
            #-DCMAKE_PYTHON_INSTALL_PREFIX=/var/tmp/$USER/install/cmake/ecflow/$release.$major.$minor   \
            #-DENABLE_UI=ON        \  # ecflow_ui      
            #-DENABLE_ALL_TESTS=ON \
            #-DENABLE_SERVER=OFF   \
            #-DENABLE_PROFILING=ON \
            #-DECBUILD_GPROF_FLAGS \
# export PATH=/tmp/$USER/opt/qt5/bin:$PATH

if [[ "$clang_tidy_arg" = clang_tidy ]] ; then
    python $WK/build_scripts/run-clang-tidy.py $clang_tidy_args
    exit 0
fi

# =============================================================================================
if [[ "$make_arg" != "" ]] ; then
	$make_arg 
	# $make_arg VERBOSE=1
	
   # generate the server file locally, and install it. Otherwise list of server will not be complete set
   echo $make_arg | grep -q "install"
	if [[ $? -eq 0 ]] ; then
		if [[ -f /home/ma/emos/bin/ecflow_site_server_install.sh ]] ; then

   		/home/ma/emos/bin/ecflow_site_server_install.sh -g -5

    		if [[ -f servers ]] ; then
        		mv servers $install_prefix/share/ecflow/.
         fi
		fi
	fi
	exit 0
fi

# =============================================================================================
if [[ $package_source_arg = package_source ]] ; then
	make package_source
	
	if [[ $copy_tarball_arg = copy_tarball ]] ; then
		rm -rf /tmp/$USER/tmp
		mkdir -p /tmp/$USER/tmp
		cp ecFlow-$release.$major.$minor-Source.tar.gz  /tmp/$USER/tmp/.
		cd /tmp/$USER/tmp/
		tar -zxf ecFlow-$release.$major.$minor-Source.tar.gz
	fi
	
	cp ecFlow-$release.$major.$minor-Source.tar.gz $SCRATCH/.
fi

# ============================================================================================
# Python:
# -DCMAKE_PYTHON_INSTALL_TYPE = [ local | setup ]
#    default: local
#    local  : this will install to $INSTALL_PREFIX/$release.$major.$minor/lib/python2.7/site-packages/ecflow/
#    setup  : experimental only, python way of installing
#
#    -DCMAKE_PYTHON_INSTALL_PREFIX should *only* used when using python setup.py (CMAKE_PYTHON_INSTALL_TYPE=setup)
#    *AND* for testing python install to local directory
