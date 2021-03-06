if(enable_maintainer_mode AND NOT WIN32)
find_program(FLEX_EXE NAMES flex)
find_program(FLEXML_EXE NAMES flexml)
find_program(SED_EXE NAMES sed)

IF(FLEX_EXE)
	set(HAVE_FLEX 1)
	exec_program("${FLEX_EXE} --version" OUTPUT_VARIABLE FLEX_VERSION)
	string(REGEX MATCH "[0-9]+[.]+[0-9]+[.]+[0-9]+" FLEX_VERSION "${FLEX_VERSION}")
	string(REGEX MATCH "^[0-9]+" FLEX_MAJOR_VERSION "${FLEX_VERSION}")
	string(REGEX MATCH "[0-9]+[.]+[0-9]+$" FLEX_VERSION "${FLEX_VERSION}")
	string(REGEX MATCH "^[0-9]+" FLEX_MINOR_VERSION "${FLEX_VERSION}")
	string(REGEX MATCH "[0-9]+$" FLEX_PATCH_VERSION "${FLEX_VERSION}")
ENDIF(FLEX_EXE)

IF(FLEXML_EXE)
	set(HAVE_FLEXML 1)
	exec_program("${FLEXML_EXE} --version" OUTPUT_VARIABLE FLEXML_VERSION)
	string(REGEX MATCH "[0-9]+[.]+[0-9]+" FLEXML_VERSION "${FLEXML_VERSION}")
	string(REGEX MATCH "^[0-9]*" FLEXML_MAJOR_VERSION "${FLEXML_VERSION}")
	string(REGEX MATCH "[0-9]*$" FLEXML_MINOR_VERSION "${FLEXML_VERSION}")
ENDIF(FLEXML_EXE)

message(STATUS "Found flex: ${FLEX_EXE}")
message(STATUS "Found flexml: ${FLEXML_EXE}")
message(STATUS "Found sed: ${SED_EXE}")

if(HAVE_FLEXML AND HAVE_FLEX AND SED_EXE)

message(STATUS "Flex version: ${FLEX_MAJOR_VERSION}.${FLEX_MINOR_VERSION}.${FLEX_PATCH_VERSION}")
message(STATUS "Flexml version: ${FLEXML_MAJOR_VERSION}.${FLEXML_MINOR_VERSION}")

set(string1  "'s/extern  *\\([^ ]*[ \\*]*\\)/XBT_PUBLIC_DATA(\\1) /'")
set(string2  "'s/XBT_PUBLIC_DATA(\\([^)]*\\)) *\\([^(]*\\)(/XBT_PUBLIC(\\1) \\2(/'")
set(string3  "'s/extern  *\\([^ ]*[ \\*]*\\)/XBT_PUBLIC_DATA(\\1) /'")
set(string4  "'s/XBT_PUBLIC_DATA(\\([^)]*\\)) *\\([^(]*\\)(/XBT_PUBLIC(\\1) \\2(/'")
set(string5  "'s/SET(DOCTYPE)/SET(ROOT_dax__adag)/'")
set(string6  "'s/extern  *\\([^ ]*[ \\*]*\\)/XBT_PUBLIC_DATA(\\1) /'")
set(string7  "'s/XBT_PUBLIC_DATA(\\([^)]*\\)) *\\([^(]*\\)(/XBT_PUBLIC(\\1) \\2(/'")
set(string8  "'s/#if defined(_WIN32)/#if defined(_XBT_WIN32)/g'")
set(string9  "'s/#include <unistd.h>/#if defined(_XBT_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__TOS_WIN__)\\n#  ifndef __STRICT_ANSI__\\n#    include <io.h>\\n#    include <process.h>\\n#  endif\\n#else\\n#  include <unistd.h>\\n#endif/g'")
set(string10 "'s/#if defined(_WIN32)/#if defined(_XBT_WIN32)/g'")
set(string11 "'s/#include <unistd.h>/#if defined(_XBT_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__TOS_WIN__)\\n#  ifndef __STRICT_ANSI__\\n#    include <io.h>\\n#    include <process.h>\\n#  endif\\n#else\\n#  include <unistd.h>\\n#endif/g'")
set(string12 "'s/#if defined(_WIN32)/#if defined(_XBT_WIN32)/g'")
set(string13 "'s/#include <unistd.h>/#if defined(_XBT_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__TOS_WIN__)\\n#  ifndef __STRICT_ANSI__\\n#    include <io.h>\\n#    include <process.h>\\n#  endif\\n#else\\n#  include <unistd.h>\\n#endif/g'")

ADD_CUSTOM_COMMAND(
  	OUTPUT 	${CMAKE_HOME_DIRECTORY}/include/surf/simgrid_dtd.h
  			${CMAKE_HOME_DIRECTORY}/include/xbt/graphxml.h
  			${CMAKE_HOME_DIRECTORY}/src/simdag/dax_dtd.h
  			${CMAKE_HOME_DIRECTORY}/src/surf/simgrid_dtd.c
  			${CMAKE_HOME_DIRECTORY}/src/xbt/graphxml.c
  			${CMAKE_HOME_DIRECTORY}/src/simdag/dax_dtd.c
  			
  	DEPENDS	${CMAKE_HOME_DIRECTORY}/src/surf/simgrid.dtd
  			${CMAKE_HOME_DIRECTORY}/src/xbt/graphxml.dtd
  			${CMAKE_HOME_DIRECTORY}/src/simdag/dax.dtd
		  			
	#${CMAKE_HOME_DIRECTORY}/src/surf/simgrid_dtd.l: ${CMAKE_HOME_DIRECTORY}/src/surf/simgrid.dtd
	COMMAND ${FLEXML_EXE} --root-tags platform -b 1000000 -P surfxml --sysid=http://simgrid.gforge.inria.fr/simgrid.dtd -S src/surf/simgrid_dtd.l -L src/surf/simgrid.dtd
	COMMAND ${CMAKE_COMMAND} -E echo "src/surf/simgrid_dtd.l"
	#${CMAKE_HOME_DIRECTORY}/src/xbt/graphxml.l: ${CMAKE_HOME_DIRECTORY}/src/xbt/graphxml.dtd
	COMMAND ${FLEXML_EXE} -b 1000000 -P graphxml --sysid=graphxml.dtd -S src/xbt/graphxml.l -L src/xbt/graphxml.dtd
	COMMAND ${CMAKE_COMMAND} -E echo "src/xbt/graphxml.l"
	#${CMAKE_HOME_DIRECTORY}/src/simdag/dax_dtd.l: ${CMAKE_HOME_DIRECTORY}/src/simdag/dax.dtd
	COMMAND ${FLEXML_EXE} -b 1000000 --root-tags adag -P dax_ --sysid=dax.dtd -S ${CMAKE_HOME_DIRECTORY}/src/simdag/dax_dtd.l -L ${CMAKE_HOME_DIRECTORY}/src/simdag/dax.dtd
	COMMAND ${SED_EXE} -i ${string5} src/simdag/dax_dtd.l
	COMMAND ${CMAKE_COMMAND} -E echo "src/simdag/dax_dtd.l"
	
	#${CMAKE_HOME_DIRECTORY}/include/surf/simgrid_dtd.h: ${CMAKE_HOME_DIRECTORY}/src/surf/simgrid.dtd
	COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_HOME_DIRECTORY}/include/surf/simgrid.h
	COMMAND ${FLEXML_EXE} --root-tags platform -P surfxml --sysid=http://simgrid.gforge.inria.fr/simgrid.dtd -H include/surf/simgrid_dtd.h -L src/surf/simgrid.dtd
	COMMAND ${SED_EXE} -i ${string1} include/surf/simgrid_dtd.h
	COMMAND ${SED_EXE} -i ${string2} include/surf/simgrid_dtd.h	
	COMMAND ${CMAKE_COMMAND} -E echo "include/surf/simgrid_dtd.h"
	#${CMAKE_HOME_DIRECTORY}/include/xbt/graphxml.h: ${CMAKE_HOME_DIRECTORY}/src/xbt/graphxml.dtd
	COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_HOME_DIRECTORY}/include/xbt/graphxml.h
	COMMAND ${FLEXML_EXE} -P graphxml --sysid=graphxml.dtd -H include/xbt/graphxml.h -L src/xbt/graphxml.dtd
	COMMAND ${SED_EXE} -i ${string3} include/xbt/graphxml.h	
	COMMAND ${SED_EXE} -i ${string4} include/xbt/graphxml.h
	COMMAND ${CMAKE_COMMAND} -E echo "include/xbt/graphxml.h"
	#${CMAKE_HOME_DIRECTORY}/src/simdag/dax_dtd.h: ${CMAKE_HOME_DIRECTORY}/src/simdag/dax.dtd
	COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_HOME_DIRECTORY}/src/simdag/dax_dtd.h
	COMMAND ${FLEXML_EXE} --root-tags adag -P dax_ --sysid=dax.dtd -H src/simdag/dax_dtd.h -L src/simdag/dax.dtd
	COMMAND ${SED_EXE} -i ${string6} src/simdag/dax_dtd.h	
	COMMAND ${SED_EXE} -i ${string7} src/simdag/dax_dtd.h
	COMMAND ${FLEX_EXE} -o src/gras/DataDesc/ddt_parse.yy.c -Pgras_ddt_parse_ --noline src/gras/DataDesc/ddt_parse.yy.l
	COMMAND ${CMAKE_COMMAND} -E echo "src/simdag/dax_dtd.h"
	
	#surf/simgrid_dtd.c: surf/simgrid_dtd.l
	COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_HOME_DIRECTORY}/src/surf/simgrid_dtd.c
	COMMAND ${SED_EXE} -i ${string8} src/surf/simgrid_dtd.l
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_HOME_DIRECTORY}/src/surf
	COMMAND ${FLEX_EXE} -o src/surf/simgrid_dtd.c -Psurf_parse_ --noline src/surf/simgrid_dtd.l
	COMMAND ${SED_EXE} -i ${string9} src/surf/simgrid_dtd.c
	COMMAND ${CMAKE_COMMAND} -E echo "surf/simgrid_dtd.c"
	#xbt/graphxml.c: xbt/graphxml.l
	COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_HOME_DIRECTORY}/src/xbt/graphxml.c
	COMMAND ${SED_EXE} -i ${string10} src/xbt/graphxml.l	
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_HOME_DIRECTORY}/src/xbt
	COMMAND ${FLEX_EXE} -o src/xbt/graphxml.c -Pxbt_graph_parse_ --noline src/xbt/graphxml.l
	COMMAND ${SED_EXE} -i ${string11} src/xbt/graphxml.c
	COMMAND ${CMAKE_COMMAND} -E echo "xbt/graphxml.c"
	#simdag/dax_dtd.c: simdag/dax_dtd.l
	COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_HOME_DIRECTORY}/src/simdag/dax_dtd.c
	COMMAND ${SED_EXE} -i ${string12} src/simdag/dax_dtd.l
	COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_HOME_DIRECTORY}/src/simdag
	COMMAND ${FLEX_EXE} -o src/simdag/dax_dtd.c -Pdax_ --noline src/simdag/dax_dtd.l
	COMMAND ${SED_EXE} -i ${string13} src/simdag/dax_dtd.c
	COMMAND ${CMAKE_COMMAND} -E echo "simdag/dax_dtd.c"
	
	WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}
	COMMENT "Generating files in maintainer mode..."
)

	add_custom_target(maintainer_files
						DEPENDS ${CMAKE_HOME_DIRECTORY}/include/surf/simgrid_dtd.h
					  			${CMAKE_HOME_DIRECTORY}/include/xbt/graphxml.h
					  			${CMAKE_HOME_DIRECTORY}/src/simdag/dax_dtd.h
					  			${CMAKE_HOME_DIRECTORY}/src/surf/simgrid_dtd.c
					  			${CMAKE_HOME_DIRECTORY}/src/xbt/graphxml.c
					  			${CMAKE_HOME_DIRECTORY}/src/simdag/dax_dtd.c
						)

else(HAVE_FLEXML AND HAVE_FLEX  AND SED_EXE)
	if(NOT HAVE_FLEXML)
		message(STATUS "Error : Install flexml before use maintainer mode.")
	endif(NOT HAVE_FLEXML)
	if(NOT HAVE_FLEX)
		message(STATUS "Error : Install flex before use maintainer mode.")
	endif(NOT HAVE_FLEX)
	if(NOT SED_EXE)
		message(STATUS "Error : Install sed before use maintainer mode.")
	endif(NOT SED_EXE)
	
	message(FATAL_ERROR STATUS "Error : Need to install all tools for maintainer mode !!!")
endif(HAVE_FLEXML AND HAVE_FLEX  AND SED_EXE)

endif(enable_maintainer_mode AND NOT WIN32)

