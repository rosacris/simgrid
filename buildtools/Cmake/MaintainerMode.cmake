if(enable_maintainer_mode AND NOT WIN32)
message("")
message("________________________________________________________________________________")
message("________________________________________________________________________________ FLEXMl")
find_program(FLEX_EXE NAMES flex)
find_program(FLEXML_EXE NAMES flexml)
find_program(SED_EXE NAMES sed)
find_program(PERL_EXE NAMES perl)

message("  FLEX : 	${FLEX_EXE}")
message("FLEXML : 	${FLEXML_EXE}")
message("   SED : 	${SED_EXE}\n")

set(top_srcdir "${PROJECT_DIRECTORY}")
set(srcdir "${PROJECT_DIRECTORY}/src")

IF(FLEX_EXE)
	set(HAVE_FLEX 1)
ENDIF(FLEX_EXE)

IF(FLEXML_EXE)
	set(HAVE_FLEXML 1)
ENDIF(FLEXML_EXE)

if(HAVE_FLEXML AND HAVE_FLEX AND SED_EXE)

foreach(file ${script_to_install})
	exec_program("chmod a=rwx ${file}" OUTPUT_VARIABLE "OKITOKI")
endforeach(file ${script_to_install})

#surf/simgrid_dtd.l: $(srcdir)/surf/simgrid.dtd
exec_program("${CMAKE_COMMAND} -E make_directory surf")
exec_program("${FLEXML_EXE} --root-tags platform -b 1000000 -P surfxml --sysid=simgrid.dtd -S surf/simgrid_dtd.l -L ${srcdir}/surf/simgrid.dtd"  "${PROJECT_DIRECTORY}/src/")

#$(top_srcdir)/include/surf/simgrid_dtd.h: $(srcdir)/surf/simgrid.dtd
file(REMOVE "${top_srcdir}/include/surf/simgrid.h") 
exec_program("${FLEXML_EXE} --root-tags platform -P surfxml --sysid=simgrid.dtd -H ${top_srcdir}/include/surf/simgrid_dtd.h -L ${srcdir}/surf/simgrid.dtd" "${PROJECT_DIRECTORY}/src/")

if(EXISTS ${top_srcdir}/include/surf/simgrid.h)
	#mv ${top_srcdir}/include/surf/simgrid.h ${top_srcdir}/include/surf/simgrid_dtd.h 
endif(EXISTS ${top_srcdir}/include/surf/simgrid.h)
set(CHAINE "'s/extern  *\\([^ ]*[ \\*]*\\)/XBT_PUBLIC_DATA(\\1) /'")
exec_program("${SED_EXE} ${CHAINE} -i ${top_srcdir}/include/surf/simgrid_dtd.h" "${PROJECT_DIRECTORY}/src/")
set(CHAINE "'s/XBT_PUBLIC_DATA(\\([^)]*\\)) *\\([^(]*\\)(/XBT_PUBLIC(\\1) \\2(/'")	
exec_program("${SED_EXE} ${CHAINE} -i ${top_srcdir}/include/surf/simgrid_dtd.h" "${PROJECT_DIRECTORY}/src/")

#xbt/graphxml.l: $(srcdir)/xbt/graphxml.dtd
exec_program("${FLEXML_EXE} -b 1000000 -P graphxml --sysid=graphxml.dtd -S xbt/graphxml.l -L ${srcdir}/xbt/graphxml.dtd" "${PROJECT_DIRECTORY}/src/")

#$(top_srcdir)/include/xbt/graphxml.h: $(srcdir)/xbt/graphxml.dtd
exec_program("${FLEXML_EXE} -P graphxml --sysid=graphxml.dtd -H ${top_srcdir}/include/xbt/graphxml.h -L ${srcdir}/xbt/graphxml.dtd" "${PROJECT_DIRECTORY}/src/")
set(CHAINE "'s/extern  *\\([^ ]*[ \\*]*\\)/XBT_PUBLIC_DATA(\\1) /'")
exec_program("${SED_EXE} ${CHAINE} -i ${top_srcdir}/include/xbt/graphxml.h" "${PROJECT_DIRECTORY}/src/")
set(CHAINE "'s/XBT_PUBLIC_DATA(\\([^)]*\\)) *\\([^(]*\\)(/XBT_PUBLIC(\\1) \\2(/'")
exec_program("${SED_EXE} ${CHAINE} -i ${top_srcdir}/include/xbt/graphxml.h" "${PROJECT_DIRECTORY}/src/")

#simdag/dax_dtd.l: simdag/dax.dtd
exec_program("${FLEXML_EXE} -b 1000000 --root-tags adag -P dax_ --sysid=dax.dtd -S simdag/dax_dtd.l -L simdag/dax.dtd" "${PROJECT_DIRECTORY}/src/")
set(CHAINE "'s/SET(DOCTYPE)/SET(ROOT_dax__adag)/'")
exec_program("${SED_EXE} -i ${CHAINE} simdag/dax_dtd.l" "${PROJECT_DIRECTORY}/src/") # DOCTYPE not mandatory

#simdag/dax_dtd.h: simdag/dax.dtd
exec_program("${FLEXML_EXE} --root-tags adag -P dax_ --sysid=dax.dtd -H simdag/dax_dtd.h -L simdag/dax.dtd" "${PROJECT_DIRECTORY}/src/")
set(CHAINE "'s/extern  *\\([^ ]*[ \\*]*\\)/XBT_PUBLIC_DATA(\\1) /'")
exec_program("${SED_EXE} ${CHAINE} -i simdag/dax_dtd.h" "${PROJECT_DIRECTORY}/src/")
set(CHAINE "'s/XBT_PUBLIC_DATA(\\([^)]*\\)) *\\([^(]*\\)(/XBT_PUBLIC(\\1) \\2(/'")	
exec_program("${SED_EXE} ${CHAINE} -i simdag/dax_dtd.h" "${PROJECT_DIRECTORY}/src/")

exec_program("${FLEX_EXE} -o gras/DataDesc/ddt_parse.yy.c -Pgras_ddt_parse_ --noline gras/DataDesc/ddt_parse.yy.l" "${PROJECT_DIRECTORY}/src/")

#surf/simgrid_dtd.c: surf/simgrid_dtd.l
exec_program("${CMAKE_COMMAND} -E make_directory surf" "${PROJECT_DIRECTORY}/src/")
exec_program("${FLEX_EXE} -o surf/simgrid_dtd.c -Psurf_parse_ --noline surf/simgrid_dtd.l" "${PROJECT_DIRECTORY}/src/")
set(CHAINE "'s/#include <unistd.h>/#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__TOS_WIN__)\\n#  ifndef __STRICT_ANSI__\\n#    include <io.h>\\n#    include <process.h>\\n#  endif\\n#else\\n#  include <unistd.h>\\n#endif/g' -i surf/simgrid_dtd.c")	
exec_program("${SED_EXE} ${CHAINE}" "${PROJECT_DIRECTORY}/src/")

#xbt/graphxml.c: xbt/graphxml.l
exec_program("${CMAKE_COMMAND} -E make_directory xbt" "${PROJECT_DIRECTORY}/src/")
exec_program("${FLEX_EXE} -o xbt/graphxml.c -Pxbt_graph_parse_ --noline xbt/graphxml.l" "${PROJECT_DIRECTORY}/src/")
set(CHAINE "'s/#include <unistd.h>/#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__TOS_WIN__)\\n#  ifndef __STRICT_ANSI__\\n#    include <io.h>\\n#    include <process.h>\\n#  endif\\n#else\\n#  include <unistd.h>\\n#endif/g' -i xbt/graphxml.c")			
exec_program("${SED_EXE} ${CHAINE}" "${PROJECT_DIRECTORY}/src/")

#simdag/dax_dtd.c: simdag/dax_dtd.l
exec_program("${CMAKE_COMMAND} -E make_directory simdag" "${PROJECT_DIRECTORY}/src/")
exec_program("${FLEX_EXE} -o simdag/dax_dtd.c -Pdax_ --noline simdag/dax_dtd.l" "${PROJECT_DIRECTORY}/src/")
set(CHAINE "'s/#include <unistd.h>/#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__TOS_WIN__)\\n#  ifndef __STRICT_ANSI__\\n#    include <io.h>\\n#    include <process.h>\\n#  endif\\n#else\\n#  include <unistd.h>\\n#endif/g' -i simdag/dax_dtd.c")	
exec_program("${SED_EXE} ${CHAINE}" "${PROJECT_DIRECTORY}/src/")

elseif(HAVE_FLEXML AND HAVE_FLEX  AND SED_EXE)
	message("  FLEX : 	${FLEX_EXE}")
	message("FLEXML : 	${FLEXML_EXE}")
	message("   SED : 	${SED_EXE}")
	message(FATAL_ERROR "Install flex or flexml or sed before use maintainer mode")
endif(HAVE_FLEXML AND HAVE_FLEX  AND SED_EXE)

#include $(top_srcdir)/acmacro/dist-files.mk

message("")
message("________________________________________________________________________________")
message("________________________________________________________________________________ FLEXML END")

message("")
message("________________________________________________________________________________")
message("________________________________________________________________________________ SG_UNIT_EXTRACTOR")

if(PERL_EXE)
	message("   PERL : 	${PERL_EXE}\n")
	exec_program("rm -f simgrid_units_main.c *_unit.c ${PROJECT_DIRECTORY}/src/simgrid_units_main.c ${PROJECT_DIRECTORY}/src/*_unit.c" "${PROJECT_DIRECTORY}/src/" OUTPUT_VARIABLE OKITOKI)

	#$(TEST_UNITS): $(TEST_CFILES)
	string(REPLACE ";" " " USE_TEST_CFILES "${TEST_CFILES}")
	exec_program("chmod a=rwx ${PROJECT_DIRECTORY}/tools/sg_unit_extractor.pl" "${PROJECT_DIRECTORY}/src/")
	exec_program("${PROJECT_DIRECTORY}/tools/sg_unit_extractor.pl ${USE_TEST_CFILES}" "${PROJECT_DIRECTORY}/src/")

	#@builddir@/simgrid_units_main.c: $(TEST_UNITS)
	exec_program("${PROJECT_DIRECTORY}/tools/sg_unit_extractor.pl ${PROJECT_DIRECTORY}/src/xbt/cunit.c" "${PROJECT_DIRECTORY}/src/")

else(PERL_EXE)
	message(FATAL_ERROR "Install perl before use maintainer mode")
endif(PERL_EXE)
message("")
message("________________________________________________________________________________")
message("________________________________________________________________________________ SG_UNIT_EXTRACTOR END")

#Those lines permit to remake a cmake configure if "sources to look" have been changed
foreach(file ${SRC_TO_LOOK})
	configure_file(${file} ${file} COPYONLY)
endforeach(file ${SRC_TO_LOOK})

endif(enable_maintainer_mode AND NOT WIN32)