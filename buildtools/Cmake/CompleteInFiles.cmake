set(CMAKE_MODULE_PATH 
${CMAKE_MODULE_PATH}
${CMAKE_HOME_DIRECTORY}/buildtools/Cmake/Modules
)

message(STATUS "Cmake version ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}")

include(CheckFunctionExists)
include(CheckTypeSize)
include(CheckIncludeFile)
include(CheckIncludeFiles)
include(CheckLibraryExists)
include(TestBigEndian)
TEST_BIG_ENDIAN(BIGENDIAN)

include(FindGraphviz)
if(enable_pcre)
include(FindPCRE)
endif(enable_pcre)
if(enable_gtnets)	
	include(FindGTnets)
endif(enable_gtnets)
if(enable_smpi)
	include(FindF2c)
endif(enable_smpi)
if(enable_lua)
	include(FindLua51Simgrid)
endif(enable_lua)

# Checks for header libraries functions.
CHECK_LIBRARY_EXISTS(pthread 	pthread_create 			"" pthread)
CHECK_LIBRARY_EXISTS(pthread 	sem_init 				"" HAVE_SEM_INIT_LIB)
CHECK_LIBRARY_EXISTS(pthread 	sem_timedwait 			"" HAVE_SEM_TIMEDWAIT_LIB)
CHECK_LIBRARY_EXISTS(pthread 	pthread_mutex_timedlock "" HAVE_MUTEX_TIMEDLOCK_LIB)
CHECK_LIBRARY_EXISTS(rt 		clock_gettime 			"" HAVE_POSIX_GETTIME)

CHECK_INCLUDE_FILES("time.h;sys/time.h" TIME_WITH_SYS_TIME)
CHECK_INCLUDE_FILES("stdlib.h;stdarg.h;string.h;float.h" STDC_HEADERS)
CHECK_INCLUDE_FILE("pthread.h" HAVE_PTHREAD_H)
CHECK_INCLUDE_FILE("valgrind/valgrind.h" HAVE_VALGRIND_VALGRIND_H)
CHECK_INCLUDE_FILE("socket.h" HAVE_SOCKET_H)
CHECK_INCLUDE_FILE("sys/socket.h" HAVE_SYS_SOCKET_H)
CHECK_INCLUDE_FILE("stat.h" HAVE_STAT_H)
CHECK_INCLUDE_FILE("sys/stat.h" HAVE_SYS_STAT_H)
CHECK_INCLUDE_FILE("windows.h" HAVE_WINDOWS_H)
CHECK_INCLUDE_FILE("winsock.h" HAVE_WINSOCK_H)
CHECK_INCLUDE_FILE("winsock2.h" HAVE_WINSOCK2_H)
CHECK_INCLUDE_FILE("errno.h" HAVE_ERRNO_H)
CHECK_INCLUDE_FILE("unistd.h" HAVE_UNISTD_H)
CHECK_INCLUDE_FILE("execinfo.h" HAVE_EXECINFO_H)
CHECK_INCLUDE_FILE("signal.h" HAVE_SIGNAL_H)
CHECK_INCLUDE_FILE("sys/time.h" HAVE_SYS_TIME_H)
CHECK_INCLUDE_FILE("time.h" HAVE_TIME_H)
CHECK_INCLUDE_FILE("dlfcn.h" HAVE_DLFCN_H)
CHECK_INCLUDE_FILE("inttypes.h" HAVE_INTTYPES_H)
CHECK_INCLUDE_FILE("memory.h" HAVE_MEMORY_H)
CHECK_INCLUDE_FILE("stdlib.h" HAVE_STDLIB_H)
CHECK_INCLUDE_FILE("strings.h" HAVE_STRINGS_H)
CHECK_INCLUDE_FILE("string.h" HAVE_STRING_H)
CHECK_INCLUDE_FILE("ucontext.h" HAVE_UCONTEXT_H)
CHECK_INCLUDE_FILE("stdio.h" HAVE_STDIO_H)
CHECK_INCLUDE_FILE("linux/futex.h" HAVE_FUTEX_H)


CHECK_FUNCTION_EXISTS(gettimeofday HAVE_GETTIMEOFDAY)
CHECK_FUNCTION_EXISTS(usleep HAVE_USLEEP)
CHECK_FUNCTION_EXISTS(getdtablesize HAVE_GETDTABLESIZE)
CHECK_FUNCTION_EXISTS(sysconf HAVE_SYSCONF)
CHECK_FUNCTION_EXISTS(readv HAVE_READV)
CHECK_FUNCTION_EXISTS(popen HAVE_POPEN)
CHECK_FUNCTION_EXISTS(signal HAVE_SIGNAL)
CHECK_FUNCTION_EXISTS(snprintf HAVE_SNPRINTF)
CHECK_FUNCTION_EXISTS(vsnprintf HAVE_VSNPRINTF)
CHECK_FUNCTION_EXISTS(asprintf HAVE_ASPRINTF)
CHECK_FUNCTION_EXISTS(vasprintf HAVE_VASPRINTF)
CHECK_FUNCTION_EXISTS(makecontext HAVE_MAKECONTEXT)
CHECK_FUNCTION_EXISTS(mmap HAVE_MMAP)

if(WIN32) #THOSE FILES ARE FUNCTIONS ARE NOT DETECTED BUT THEY SHOULD...
    set(HAVE_UCONTEXT_H 1)
    set(HAVE_MAKECONTEXT 1)
    set(HAVE_SNPRINTF 1)
    set(HAVE_VSNPRINTF 1)
endif(WIN32)

set(CONTEXT_UCONTEXT 0)
SET(CONTEXT_THREADS 0)
SET(HAVE_TRACING 0)

if(enable_tracing)
	SET(HAVE_TRACING 1)
endif(enable_tracing)

if(enable_jedule)
	SET(HAVE_JEDULE 1)
endif(enable_jedule)

if(enable_latency_bound_tracking)
	SET(HAVE_LATENCY_BOUND_TRACKING 1)
else(enable_latency_bound_tracking)
  if(enable_gtnets)
    message(STATUS "Warning : Turning latency_bound_tracking to ON because GTNeTs is ON")
    SET(enable_latency_bound_tracking ON)
    SET(HAVE_LATENCY_BOUND_TRACKING 1)
  else(enable_gtnets)
    SET(HAVE_LATENCY_BOUND_TRACKING 0)
  endif(enable_gtnets)
endif(enable_latency_bound_tracking)

if(enable_model-checking AND HAVE_MMAP)
	SET(HAVE_MC 1)
	SET(MMALLOC_WANT_OVERIDE_LEGACY 1)
else(enable_model-checking AND HAVE_MMAP)
	SET(HAVE_MC 0)
	SET(MMALLOC_WANT_OVERIDE_LEGACY 0)
endif(enable_model-checking AND HAVE_MMAP)

#--------------------------------------------------------------------------------------------------
### Check for some architecture dependent values
CHECK_TYPE_SIZE(int SIZEOF_INT)
CHECK_TYPE_SIZE(void* SIZEOF_VOIDP)


#--------------------------------------------------------------------------------------------------
### Initialize of CONTEXT THREADS

if(pthread)
	set(pthread 1)
elseif(pthread)
	set(pthread 0)
endif(pthread)

if(pthread)
	### HAVE_SEM_INIT
  	
  	if(HAVE_SEM_INIT_LIB)
		exec_program("${CMAKE_C_COMPILER} -lpthread ${CMAKE_HOME_DIRECTORY}/buildtools/Cmake/test_prog/prog_sem_init.c" OUTPUT_VARIABLE HAVE_SEM_INIT_run)
	    	if(HAVE_SEM_INIT_run)
			set(HAVE_SEM_INIT 0)
	    	else(HAVE_SEM_INIT_run)
			set(HAVE_SEM_INIT 1)
		endif(HAVE_SEM_INIT_run)
  	endif(HAVE_SEM_INIT_LIB)

	### HAVE_SEM_TIMEDWAIT

	if(HAVE_SEM_TIMEDWAIT_LIB)
		exec_program("${CMAKE_C_COMPILER} -lpthread ${CMAKE_HOME_DIRECTORY}/buildtools/Cmake/test_prog/prog_sem_timedwait.c" OUTPUT_VARIABLE HAVE_SEM_TIMEDWAIT_run)
		if(HAVE_SEM_TIMEDWAIT_run)
			set(HAVE_SEM_TIMEDWAIT 0)
		else(HAVE_SEM_TIMEDWAIT_run)
			set(HAVE_SEM_TIMEDWAIT 1)
		endif(HAVE_SEM_TIMEDWAIT_run)
	endif(HAVE_SEM_TIMEDWAIT_LIB)

	### HAVE_MUTEX_TIMEDLOCK

	if(HAVE_MUTEX_TIMEDLOCK_LIB)
		exec_program("${CMAKE_C_COMPILER} -lpthread ${CMAKE_HOME_DIRECTORY}/buildtools/Cmake/test_prog/prog_mutex_timedlock.c" OUTPUT_VARIABLE HAVE_SEM_TIMEDWAIT_run)
		if(HAVE_MUTEX_TIMEDLOCK_run)
			set(HAVE_MUTEX_TIMEDLOCK 0)
		else(HAVE_MUTEX_TIMEDLOCK_run)
			set(HAVE_MUTEX_TIMEDLOCK 1)
		endif(HAVE_MUTEX_TIMEDLOCK_run)
	endif(HAVE_MUTEX_TIMEDLOCK_LIB)
endif(pthread)

# AC_CHECK_MCSC(mcsc=yes, mcsc=no) 
set(mcsc_flags "")
if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
	set(mcsc_flags "-D_XOPEN_SOURCE")
endif(CMAKE_SYSTEM_NAME MATCHES "Darwin")

if(WIN32)
    if(__VISUALC__)
	set(mcsc_flags "/D_XBT_WIN32 /I${CMAKE_HOME_DIRECTORY}/include/xbt /I${CMAKE_HOME_DIRECTORY}/src/xbt")
	endif(__VISUALC__)
	if(__GNUC__)
		set(mcsc_flags "-D_XBT_WIN32 -I${CMAKE_HOME_DIRECTORY}/include/xbt -I${CMAKE_HOME_DIRECTORY}/src/xbt")
	endif(__GNUC__)
endif(WIN32)

IF(CMAKE_CROSSCOMPILING)
	IF(WIN32)
		set(windows_context "yes")
		set(IS_WINDOWS 1)	
	ENDIF(WIN32)
ELSE(CMAKE_CROSSCOMPILING)
	try_run(RUN_mcsc_VAR COMPILE_mcsc_VAR
		${simgrid_BINARY_DIR}
		${CMAKE_HOME_DIRECTORY}/buildtools/Cmake/test_prog/prog_AC_CHECK_MCSC.c
		COMPILE_DEFINITIONS "${mcsc_flags}"
		OUTPUT_VARIABLE var_compil
		)
		
		if(EXISTS "${simgrid_BINARY_DIR}/conftestval" AND COMPILE_mcsc_VAR)
			file(READ "${simgrid_BINARY_DIR}/conftestval" mcsc)
			STRING(REPLACE "\n" "" mcsc "${mcsc}")
			if(mcsc)
				set(mcsc "yes")
			elseif(mcsc)
				set(mcsc "no")
			endif(mcsc)
	        else(EXISTS "${simgrid_BINARY_DIR}/conftestval" AND COMPILE_mcsc_VAR)
			set(mcsc "no")
		endif(EXISTS "${simgrid_BINARY_DIR}/conftestval" AND COMPILE_mcsc_VAR)
ENDIF(CMAKE_CROSSCOMPILING)

if(mcsc MATCHES "no" AND pthread)
	if(HAVE_WINDOWS_H)
		set(windows_context "yes")
		set(IS_WINDOWS 1)
	elseif(HAVE_WINDOWS_H)
		message(FATAL_ERROR "no appropriate backend found")
	endif(HAVE_WINDOWS_H)
endif(mcsc MATCHES "no" AND pthread)

#Only windows

if(WIN32)
	if(NOT HAVE_WINDOWS_H)
		message(FATAL_ERROR "no appropriate backend found windows")
	endif(NOT HAVE_WINDOWS_H)
endif(WIN32)

if(windows_context MATCHES "yes")
	message(STATUS "Context change to windows")
endif(windows_context MATCHES "yes")

#If can have both context

if(mcsc)
	set(CONTEXT_UCONTEXT 1)
endif(mcsc)

if(pthread)
	set(CONTEXT_THREADS 1)
endif(pthread)


###############
## SVN version check
##
if(IS_DIRECTORY ${CMAKE_HOME_DIRECTORY}/.svn)
	find_file(SVN ".svn" ${CMAKE_HOME_DIRECTORY})
	exec_program("svnversion ${CMAKE_HOME_DIRECTORY}" OUTPUT_VARIABLE "SVN_VERSION")
	message(STATUS "svn version ${SVN_VERSION}")
else(IS_DIRECTORY ${CMAKE_HOME_DIRECTORY}/.svn)
	exec_program("git config --get svn-remote.svn.url"
		OUTPUT_VARIABLE url
		RETURN_VALUE ret)
endif(IS_DIRECTORY ${CMAKE_HOME_DIRECTORY}/.svn)

if(url)
	exec_program("git --git-dir=${CMAKE_HOME_DIRECTORY}/.git log --oneline -1" OUTPUT_VARIABLE "GIT_VERSION")
	exec_program("git --git-dir=${CMAKE_HOME_DIRECTORY}/.git log -n 1 --format=%ai ." OUTPUT_VARIABLE "GIT_DATE")
	
	string(REGEX REPLACE " .*" "" GIT_VERSION "${GIT_VERSION}")
	STRING(REPLACE " +0000" "" GIT_DATE ${GIT_DATE})
	STRING(REPLACE " " "~" GIT_DATE ${GIT_DATE})
	STRING(REPLACE ":" "-" GIT_DATE ${GIT_DATE})
	
	exec_program("git svn info" ${CMAKE_HOME_DIRECTORY}
		OUTPUT_VARIABLE "GIT_SVN_VERSION")
	string(REPLACE "\n" ";" GIT_SVN_VERSION ${GIT_SVN_VERSION})
	foreach(line ${GIT_SVN_VERSION})
		string(REGEX MATCH "^Revision:.*" line_good ${line})
		if(line_good)
			string(REPLACE "Revision: " ""
				line_good ${line_good})
			set(SVN_VERSION ${line_good})
		endif(line_good)
	endforeach(line ${GIT_SVN_VERSION})
endif(url)


###################################
## SimGrid and GRAS specific checks
##

IF(NOT CMAKE_CROSSCOMPILING)
# Check architecture signature begin
try_run(RUN_GRAS_VAR COMPILE_GRAS_VAR
	${simgrid_BINARY_DIR}
	${CMAKE_HOME_DIRECTORY}/buildtools/Cmake/test_prog/prog_GRAS_ARCH.c
	RUN_OUTPUT_VARIABLE var1
	)
if(BIGENDIAN)
  set(val_big "B${var1}")
  set(GRAS_BIGENDIAN 1)
else(BIGENDIAN)
  set(val_big "l${var1}")
  set(GRAS_BIGENDIAN 0)
endif(BIGENDIAN)

if(val_big MATCHES "l_C:1/1:_I:2/1:4/1:4/1:8/1:_P:4/1:4/1:_D:4/1:8/1:")
	#gras_arch=0; gras_size=32; gras_arch_name=little32_1;
	SET(GRAS_ARCH_32_BITS 1)
	SET(GRAS_THISARCH 0)
endif(val_big MATCHES "l_C:1/1:_I:2/1:4/1:4/1:8/1:_P:4/1:4/1:_D:4/1:8/1:")
if(val_big MATCHES "l_C:1/1:_I:2/2:4/2:4/2:8/2:_P:4/2:4/2:_D:4/2:8/2:")
	#gras_arch=1; gras_size=32; gras_arch_name=little32_2;
	SET(GRAS_ARCH_32_BITS 1)
	SET(GRAS_THISARCH 1)
endif(val_big MATCHES "l_C:1/1:_I:2/2:4/2:4/2:8/2:_P:4/2:4/2:_D:4/2:8/2:")
if(val_big MATCHES "l_C:1/1:_I:2/2:4/4:4/4:8/4:_P:4/4:4/4:_D:4/4:8/4:") 
	#gras_arch=2; gras_size=32; gras_arch_name=little32_4;
	SET(GRAS_ARCH_32_BITS 1)
	SET(GRAS_THISARCH 2)
endif(val_big MATCHES "l_C:1/1:_I:2/2:4/4:4/4:8/4:_P:4/4:4/4:_D:4/4:8/4:")
if(val_big MATCHES "l_C:1/1:_I:2/2:4/4:4/4:8/8:_P:4/4:4/4:_D:4/4:8/8:") 
	#gras_arch=3; gras_size=32; gras_arch_name=little32_8;
	SET(GRAS_ARCH_32_BITS 1)
	SET(GRAS_THISARCH 3)
endif(val_big MATCHES "l_C:1/1:_I:2/2:4/4:4/4:8/8:_P:4/4:4/4:_D:4/4:8/8:") 
if(val_big MATCHES "l_C:1/1:_I:2/2:4/4:8/8:8/8:_P:8/8:8/8:_D:4/4:8/8:") 
	#gras_arch=4; gras_size=64; gras_arch_name=little64;
	SET(GRAS_ARCH_32_BITS 0)
	SET(GRAS_THISARCH 4)
endif(val_big MATCHES "l_C:1/1:_I:2/2:4/4:8/8:8/8:_P:8/8:8/8:_D:4/4:8/8:")

if(val_big MATCHES "B_C:1/1:_I:2/2:4/4:4/4:8/8:_P:4/4:4/4:_D:4/4:8/8:") 
	#gras_arch=5; gras_size=32; gras_arch_name=big32;
	SET(GRAS_ARCH_32_BITS 1)
	SET(GRAS_THISARCH 5)
endif(val_big MATCHES "B_C:1/1:_I:2/2:4/4:4/4:8/8:_P:4/4:4/4:_D:4/4:8/8:")
if(val_big MATCHES "B_C:1/1:_I:2/2:4/4:4/4:8/8:_P:4/4:4/4:_D:4/4:8/4:") 
	#gras_arch=6; gras_size=32; gras_arch_name=big32_8_4;
	SET(GRAS_ARCH_32_BITS 1)
	SET(GRAS_THISARCH 6)
endif(val_big MATCHES "B_C:1/1:_I:2/2:4/4:4/4:8/8:_P:4/4:4/4:_D:4/4:8/4:")
if(val_big MATCHES "B_C:1/1:_I:2/2:4/4:4/4:8/4:_P:4/4:4/4:_D:4/4:8/4:") 
	#gras_arch=7; gras_size=32; gras_arch_name=big32_4;
	SET(GRAS_ARCH_32_BITS 1)
	SET(GRAS_THISARCH 7)
endif(val_big MATCHES "B_C:1/1:_I:2/2:4/4:4/4:8/4:_P:4/4:4/4:_D:4/4:8/4:")
if(val_big MATCHES "B_C:1/1:_I:2/2:4/2:4/2:8/2:_P:4/2:4/2:_D:4/2:8/2:") 
	#gras_arch=8; gras_size=32; gras_arch_name=big32_2;
	SET(GRAS_ARCH_32_BITS 1)
	SET(GRAS_THISARCH 8)
endif(val_big MATCHES "B_C:1/1:_I:2/2:4/2:4/2:8/2:_P:4/2:4/2:_D:4/2:8/2:") 
if(val_big MATCHES "B_C:1/1:_I:2/2:4/4:8/8:8/8:_P:8/8:8/8:_D:4/4:8/8:") 
	#gras_arch=9; gras_size=64; gras_arch_name=big64;
	SET(GRAS_ARCH_32_BITS 0)
	SET(GRAS_THISARCH 9)
endif(val_big MATCHES "B_C:1/1:_I:2/2:4/4:8/8:8/8:_P:8/8:8/8:_D:4/4:8/8:")
if(val_big MATCHES "B_C:1/1:_I:2/2:4/4:8/8:8/8:_P:8/8:8/8:_D:4/4:8/4:") 
	#gras_arch=10;gras_size=64; gras_arch_name=big64_8_4;
	SET(GRAS_ARCH_32_BITS 0)
	SET(GRAS_THISARCH 10)
endif(val_big MATCHES "B_C:1/1:_I:2/2:4/4:8/8:8/8:_P:8/8:8/8:_D:4/4:8/4:") 


# Check architecture signature end
try_run(RUN_GRAS_VAR COMPILE_GRAS_VAR
	${simgrid_BINARY_DIR}
	${CMAKE_HOME_DIRECTORY}/buildtools/Cmake/test_prog/prog_GRAS_CHECK_STRUCT_COMPACTION.c
	RUN_OUTPUT_VARIABLE var2
	)
separate_arguments(var2)
foreach(var_tmp ${var2})
	set(${var_tmp} 1)
endforeach(var_tmp ${var2})

# Check for [SIZEOF_MAX]
try_run(RUN_SM_VAR COMPILE_SM_VAR
	${simgrid_BINARY_DIR}
	${CMAKE_HOME_DIRECTORY}/buildtools/Cmake/test_prog/prog_max_size.c
	RUN_OUTPUT_VARIABLE var3
	)
SET(SIZEOF_MAX ${var3})
ENDIF(NOT CMAKE_CROSSCOMPILING)

#--------------------------------------------------------------------------------------------------

set(makecontext_CPPFLAGS_2 "")
if(HAVE_MAKECONTEXT OR WIN32)
	set(makecontext_CPPFLAGS "-DTEST_makecontext")
	if(CMAKE_SYSTEM_NAME MATCHES "Darwin")
		set(makecontext_CPPFLAGS_2 "-DOSX")
	endif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
	
    if(WIN32 AND __VISUALC__)
        set(makecontext_CPPFLAGS "/DTEST_makecontext")
	    set(makecontext_CPPFLAGS_2 "/D_XBT_WIN32 /I${CMAKE_HOME_DIRECTORY}/include/xbt /I${CMAKE_HOME_DIRECTORY}/src/xbt")
	endif(WIN32 AND __VISUALC__)
	if(WIN32 AND __GNUC__)
	    set(makecontext_CPPFLAGS "-DTEST_makecontext")
	    set(makecontext_CPPFLAGS_2 "-D_XBT_WIN32 -I${CMAKE_HOME_DIRECTORY}/include/xbt -I${CMAKE_HOME_DIRECTORY}/src/xbt")
	endif(WIN32 AND __GNUC__)
	
	try_run(RUN_makecontext_VAR COMPILE_makecontext_VAR
		${simgrid_BINARY_DIR}
		${CMAKE_HOME_DIRECTORY}/buildtools/Cmake/test_prog/prog_stacksetup.c
		COMPILE_DEFINITIONS "${makecontext_CPPFLAGS} ${makecontext_CPPFLAGS_2}"
		)
	file(READ ${simgrid_BINARY_DIR}/conftestval MAKECONTEXT_ADDR_SIZE)
	string(REPLACE "\n" "" MAKECONTEXT_ADDR_SIZE "${MAKECONTEXT_ADDR_SIZE}")
	string(REGEX MATCH ;^.*,;MAKECONTEXT_ADDR "${MAKECONTEXT_ADDR_SIZE}")
	string(REGEX MATCH ;,.*$; MAKECONTEXT_SIZE "${MAKECONTEXT_ADDR_SIZE}")
	string(REPLACE "," "" makecontext_addr "${MAKECONTEXT_ADDR}")
	string(REPLACE "," "" makecontext_size "${MAKECONTEXT_SIZE}")	
	set(pth_skaddr_makecontext "#define pth_skaddr_makecontext(skaddr,sksize) (${makecontext_addr})")
	set(pth_sksize_makecontext "#define pth_sksize_makecontext(skaddr,sksize) (${makecontext_size})")
	
endif(HAVE_MAKECONTEXT OR WIN32)

#--------------------------------------------------------------------------------------------------

### check for stackgrowth
if (NOT CMAKE_CROSSCOMPILING)
	try_run(RUN_makecontext_VAR COMPILE_makecontext_VAR
		${simgrid_BINARY_DIR}
		${CMAKE_HOME_DIRECTORY}/buildtools/Cmake/test_prog/prog_stackgrowth.c
		)
file(READ "${simgrid_BINARY_DIR}/conftestval" stack)
if(stack MATCHES "down")
	set(PTH_STACKGROWTH "-1")
endif(stack MATCHES "down")
if(stack MATCHES "up")
	set(PTH_STACKGROWTH "1")
endif(stack MATCHES "up")

endif(NOT CMAKE_CROSSCOMPILING)
###############
## System checks
##

#SG_CONFIGURE_PART([System checks...])
#AC_PROG_CC(xlC gcc cc) -auto
#AM_SANITY_CHECK -auto

#AC_PROG_MAKE_SET


#AC_PRINTF_NULL
try_run(RUN_PRINTF_NULL_VAR COMPILE_PRINTF_NULL_VAR
	${simgrid_BINARY_DIR}
	${CMAKE_HOME_DIRECTORY}/buildtools/Cmake/test_prog/prog_printf_null.c
	)

if(RUN_PRINTF_NULL_VAR MATCHES "FAILED_TO_RUN")
SET(PRINTF_NULL_WORKING "0")
else(RUN_PRINTF_NULL_VAR MATCHES "FAILED_TO_RUN")
SET(PRINTF_NULL_WORKING "1")
endif(RUN_PRINTF_NULL_VAR MATCHES "FAILED_TO_RUN")

#AC_CHECK_VA_COPY

set(diff_va "va_copy((d),(s))"
"VA_COPY((d),(s))"
"__va_copy((d),(s))"
"__builtin_va_copy((d),(s))"
"do { (d) = (s)\; } while (0)"
"do { *(d) = *(s)\; } while (0)"
"memcpy((void *)&(d), (void *)&(s), sizeof(s))"
"memcpy((void *)(d), (void *)(s), sizeof(*(s)))"
)

foreach(fct ${diff_va})
	write_file("${CMAKE_HOME_DIRECTORY}/buildtools/Cmake/test_prog/prog_va_copy.c" "#include <stdlib.h>
	#include <stdarg.h>
	#include <string.h>
	#define DO_VA_COPY(d,s) ${fct}
	void test(char *str, ...)
	{
	    va_list ap, ap2;
	    int i;
	    va_start(ap, str);
	    DO_VA_COPY(ap2, ap);
	    for (i = 1; i <= 9; i++) {
		int k = (int)va_arg(ap, int);
		if (k != i)
		    abort();
	    }
	    DO_VA_COPY(ap, ap2);
	    for (i = 1; i <= 9; i++) {
		int k = (int)va_arg(ap, int);
		if (k != i)
		    abort();
	    }
	    va_end(ap);
	}
	int main(int argc, char *argv[])
	{
	    test("test", 1, 2, 3, 4, 5, 6, 7, 8, 9);
	    exit(0);
	}"
	)
	try_compile(COMPILE_VA_NULL_VAR
	${simgrid_BINARY_DIR}
	${CMAKE_HOME_DIRECTORY}/buildtools/Cmake/test_prog/prog_va_copy.c
	)
	if(COMPILE_VA_NULL_VAR)
		string(REGEX REPLACE "\;" "" fctbis ${fct})
		if(${fctbis} STREQUAL "va_copy((d),(s))")
			set(HAVE_VA_COPY 1)
			set(ac_cv_va_copy "C99")
			set(__VA_COPY_USE_C99 "va_copy((d),(s))")	
		endif(${fctbis} STREQUAL "va_copy((d),(s))")

		if(${fctbis} STREQUAL "VA_COPY((d),(s))")
			set(ac_cv_va_copy "GCM")
			set(__VA_COPY_USE_GCM "VA_COPY((d),(s))")
		endif(${fctbis} STREQUAL "VA_COPY((d),(s))")

		if(${fctbis} STREQUAL "__va_copy((d),(s))")
			set(ac_cv_va_copy "GCH")
			set(__VA_COPY_USE_GCH "__va_copy((d),(s))")
		endif(${fctbis} STREQUAL "__va_copy((d),(s))")

		if(${fctbis} STREQUAL "__builtin_va_copy((d),(s))")
			set(ac_cv_va_copy "GCB")
			set(__VA_COPY_USE_GCB "__builtin_va_copy((d),(s))")
		endif(${fctbis} STREQUAL "__builtin_va_copy((d),(s))")

		if(${fctbis} STREQUAL "do { (d) = (s) } while (0)")
			set(ac_cv_va_copy "ASS")
			set(__VA_COPY_USE_ASS "do { (d) = (s); } while (0)")
		endif(${fctbis} STREQUAL "do { (d) = (s) } while (0)")

		if(${fctbis} STREQUAL "do { *(d) = *(s) } while (0)")
			set(ac_cv_va_copy "ASP")
			set(__VA_COPY_USE_ASP "do { *(d) = *(s); } while (0)")
		endif(${fctbis} STREQUAL "do { *(d) = *(s) } while (0)")

		if(${fctbis} STREQUAL "memcpy((void *)&(d), (void *)&(s), sizeof(s))")
			set(ac_cv_va_copy "CPS")
			set(__VA_COPY_USE_CPS "memcpy((void *)&(d), (void *)&(s), sizeof(s))")
		endif(${fctbis} STREQUAL "memcpy((void *)&(d), (void *)&(s), sizeof(s))")

		if(${fctbis} STREQUAL "memcpy((void *)(d), (void *)(s), sizeof(*(s)))")
			set(ac_cv_va_copy "CPP")
			set(__VA_COPY_USE_CPP "memcpy((void *)(d), (void *)(s), sizeof(*(s)))")
		endif(${fctbis} STREQUAL "memcpy((void *)(d), (void *)(s), sizeof(*(s)))")
				
		if(NOT STATUS_OK)
		set(__VA_COPY_USE "__VA_COPY_USE_${ac_cv_va_copy}(d, s)")
		endif(NOT STATUS_OK)
		set(STATUS_OK "1")
		
	endif(COMPILE_VA_NULL_VAR)
	
endforeach(fct ${diff_va})

#--------------------------------------------------------------------------------------------------
### check for getline
try_compile(COMPILE_RESULT_VAR
	${simgrid_BINARY_DIR}
	${CMAKE_HOME_DIRECTORY}/buildtools/Cmake/test_prog/prog_getline.c
	)

if(NOT COMPILE_RESULT_VAR)
SET(need_getline "#define SIMGRID_NEED_GETLINE 1")
SET(SIMGRID_NEED_GETLINE 1)
else(NOT COMPILE_RESULT_VAR)
SET(need_getline "")
SET(SIMGRID_NEED_GETLINE 0)
endif(NOT COMPILE_RESULT_VAR)

### check for a working snprintf
if(HAVE_SNPRINTF AND HAVE_VSNPRINTF OR WIN32)
    if(WIN32)
        #set(HAVE_SNPRINTF 1)
        #set(HAVE_VSNPRINTF 1)
    endif(WIN32)
    
	if(CMAKE_CROSSCOMPILING)
		set(RUN_SNPRINTF_FUNC "cross")
		#set(PREFER_PORTABLE_SNPRINTF 1)
	else(CMAKE_CROSSCOMPILING)
  	    try_run(RUN_SNPRINTF_FUNC_VAR COMPILE_SNPRINTF_FUNC_VAR
	  	${simgrid_BINARY_DIR}
		${CMAKE_HOME_DIRECTORY}/buildtools/Cmake/test_prog/prog_snprintf.c
	    )	
	endif(CMAKE_CROSSCOMPILING)

	if(CMAKE_CROSSCOMPILING)
		set(RUN_VSNPRINTF_FUNC "cross")
		set(PREFER_PORTABLE_VSNPRINTF 1)
	else(CMAKE_CROSSCOMPILING)
  	   try_run(RUN_VSNPRINTF_FUNC_VAR COMPILE_VSNPRINTF_FUNC_VAR
		${simgrid_BINARY_DIR}
		${CMAKE_HOME_DIRECTORY}/buildtools/Cmake/test_prog/prog_vsnprintf.c
	   )
	endif(CMAKE_CROSSCOMPILING)
	
	set(PREFER_PORTABLE_SNPRINTF 0)
	if(RUN_VSNPRINTF_FUNC_VAR MATCHES "FAILED_TO_RUN")
		set(PREFER_PORTABLE_SNPRINTF 1)
	endif(RUN_VSNPRINTF_FUNC_VAR MATCHES "FAILED_TO_RUN")
	if(RUN_SNPRINTF_FUNC_VAR MATCHES "FAILED_TO_RUN")
		set(PREFER_PORTABLE_SNPRINTF 1)
	endif(RUN_SNPRINTF_FUNC_VAR MATCHES "FAILED_TO_RUN")
endif(HAVE_SNPRINTF AND HAVE_VSNPRINTF OR WIN32)

### check for asprintf function familly
if(HAVE_ASPRINTF)
	SET(simgrid_need_asprintf "")
	SET(NEED_ASPRINTF 0)
else(HAVE_ASPRINTF)
	SET(simgrid_need_asprintf "#define SIMGRID_NEED_ASPRINTF 1")
	SET(NEED_ASPRINTF 1)
endif(HAVE_ASPRINTF)

if(HAVE_VASPRINTF)
	SET(simgrid_need_vasprintf "")
	SET(NEED_VASPRINTF 0)
else(HAVE_VASPRINTF)
	SET(simgrid_need_vasprintf "#define SIMGRID_NEED_VASPRINTF 1")
	SET(NEED_VASPRINTF 1)
endif(HAVE_VASPRINTF)

### check for addr2line

find_path(ADDR2LINE NAMES addr2line	PATHS NO_DEFAULT_PATHS	)
if(ADDR2LINE)
set(ADDR2LINE "${ADDR2LINE}/addr2line")
endif(ADDR2LINE)

### File to create

configure_file("${CMAKE_HOME_DIRECTORY}/src/context_sysv_config.h.in" 			"${CMAKE_BINARY_DIR}/src/context_sysv_config.h" @ONLY IMMEDIATE)

SET( CMAKEDEFINE "#cmakedefine" )
configure_file("${CMAKE_HOME_DIRECTORY}/buildtools/Cmake/gras_config.h.in" 	"${CMAKE_BINARY_DIR}/src/gras_config.h" @ONLY IMMEDIATE)
configure_file("${CMAKE_BINARY_DIR}/src/gras_config.h" 			"${CMAKE_BINARY_DIR}/src/gras_config.h" @ONLY IMMEDIATE)
configure_file("${CMAKE_HOME_DIRECTORY}/include/simgrid_config.h.in" 		"${CMAKE_BINARY_DIR}/include/simgrid_config.h" @ONLY IMMEDIATE)

set(top_srcdir "${CMAKE_HOME_DIRECTORY}")
set(srcdir "${CMAKE_HOME_DIRECTORY}/src")

set(exec_prefix ${CMAKE_INSTALL_PREFIX})
set(includedir ${CMAKE_INSTALL_PREFIX}/include)
set(top_builddir ${CMAKE_HOME_DIRECTORY})
set(libdir ${exec_prefix}/lib)
set(CMAKE_LINKARGS "${CMAKE_BINARY_DIR}/lib")
set(CMAKE_SMPI_COMMAND "export LD_LIBRARY_PATH=${CMAKE_BINARY_DIR}/lib:${GTNETS_LIB_PATH}:$LD_LIBRARY_PATH")

configure_file(${CMAKE_HOME_DIRECTORY}/include/smpi/smpif.h.in ${CMAKE_BINARY_DIR}/include/smpi/smpif.h @ONLY)
configure_file(${CMAKE_HOME_DIRECTORY}/src/smpi/smpicc.in ${CMAKE_BINARY_DIR}/bin/smpicc @ONLY)
configure_file(${CMAKE_HOME_DIRECTORY}/src/smpi/smpif2c.in ${CMAKE_BINARY_DIR}/bin/smpif2c @ONLY)
configure_file(${CMAKE_HOME_DIRECTORY}/src/smpi/smpiff.in ${CMAKE_BINARY_DIR}/bin/smpiff @ONLY)
configure_file(${CMAKE_HOME_DIRECTORY}/src/smpi/smpirun.in ${CMAKE_BINARY_DIR}/bin/smpirun @ONLY)

exec_program("chmod a=rwx ${CMAKE_BINARY_DIR}/bin/smpicc" OUTPUT_VARIABLE OKITOKI)
exec_program("chmod a=rwx ${CMAKE_BINARY_DIR}/bin/smpif2c" OUTPUT_VARIABLE OKITOKI)
exec_program("chmod a=rwx ${CMAKE_BINARY_DIR}/bin/smpiff" OUTPUT_VARIABLE OKITOKI)
exec_program("chmod a=rwx ${CMAKE_BINARY_DIR}/bin/smpirun" OUTPUT_VARIABLE OKITOKI)

set(generate_files_to_clean
${CMAKE_BINARY_DIR}/src/context_sysv_config.h
${CMAKE_BINARY_DIR}/src/gras_config.h
${CMAKE_BINARY_DIR}/include/simgrid_config.h
${CMAKE_BINARY_DIR}/include/smpi/smpif.h
${CMAKE_BINARY_DIR}/bin/smpicc
${CMAKE_BINARY_DIR}/bin/smpif2c
${CMAKE_BINARY_DIR}/bin/smpiff
${CMAKE_BINARY_DIR}/bin/smpirun
${CMAKE_BINARY_DIR}/bin/colorize
${CMAKE_BINARY_DIR}/bin/simgrid_update_xml
${CMAKE_BINARY_DIR}/examples/smpi/smpi_traced.trace
${CMAKE_BINARY_DIR}/src/supernovae_sg.c
${CMAKE_BINARY_DIR}/src/supernovae_gras.c
${CMAKE_BINARY_DIR}/src/supernovae_smpi.c
)

if("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_HOME_DIRECTORY}")
else("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_HOME_DIRECTORY}")
	configure_file(${CMAKE_HOME_DIRECTORY}/examples/smpi/hostfile ${CMAKE_BINARY_DIR}/examples/smpi/hostfile COPYONLY)
	configure_file(${CMAKE_HOME_DIRECTORY}/examples/msg/small_platform.xml ${CMAKE_BINARY_DIR}/examples/msg/small_platform.xml COPYONLY)
	configure_file(${CMAKE_HOME_DIRECTORY}/examples/msg/small_platform_with_routers.xml ${CMAKE_BINARY_DIR}/examples/msg/small_platform_with_routers.xml COPYONLY)
	
	set(generate_files_to_clean
		${generate_files_to_clean}
		${CMAKE_BINARY_DIR}/examples/smpi/hostfile
		${CMAKE_BINARY_DIR}/examples/msg/small_platform.xml
		${CMAKE_BINARY_DIR}/examples/msg/small_platform_with_routers.xml
		)
endif("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_HOME_DIRECTORY}")

SET_DIRECTORY_PROPERTIES(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES
"${generate_files_to_clean}")
