#!/bin/bash

if [ -e ./pipol ] ; then
	rm -rf ./pipol/$PIPOL_HOST
	mkdir ./pipol/$PIPOL_HOST
else
	mkdir ./pipol
	rm -rf ./pipol/$PIPOL_HOST
	mkdir ./pipol/$PIPOL_HOST
fi
cd ./pipol/$PIPOL_HOST

svn checkout svn://scm.gforge.inria.fr/svn/simgrid/simgrid/trunk simgrid-trunk --quiet
cd simgrid-trunk

perl ./buildtools/pipol/cmake.pl
perl ./buildtools/pipol/ruby.pl

#mem-check
cmake \
-Denable_lua=off \
-Denable_tracing=off \
-Denable_smpi=off \
-Denable_supernovae=off \
-Denable_compile_optimizations=off \
-Denable_compile_warnings=on \
-Denable_lib_static=off \
-Denable_model-checking=off \
-Denable_latency_bound_tracking=off \
-Denable_gtnets=off \
-Denable_jedule=off \
-Denable_memcheck=on ./
ctest -D ExperimentalStart
ctest -D ExperimentalConfigure
ctest -D ExperimentalBuild
ctest -D ExperimentalMemCheck
ctest -D ExperimentalSubmit