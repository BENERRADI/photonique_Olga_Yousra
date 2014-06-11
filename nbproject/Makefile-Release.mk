#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=MinGW64-Windows
CND_DLIB_EXT=dll
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/correction.o \
	${OBJECTDIR}/exposure.o \
	${OBJECTDIR}/main.o \
	${OBJECTDIR}/matcher.o \
	${OBJECTDIR}/stitching.o


# C Compiler Flags
CFLAGS=-fopenmp -march=native

# CC Compiler Flags
CCFLAGS=-fopenmp -march=native -O2
CXXFLAGS=-fopenmp -march=native -O2

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-Llib -lm -lopencv_calib3d249.dll -lopencv_contrib249.dll -lopencv_core249.dll -lopencv_features2d249.dll -lopencv_flann249.dll -lopencv_gpu249.dll -lopencv_highgui249.dll -lopencv_imgproc249.dll -lopencv_legacy249.dll -lopencv_ml249.dll -lopencv_nonfree249.dll -lopencv_objdetect249.dll -lopencv_ocl249.dll -lopencv_photo249.dll -lopencv_stitching249.dll -lopencv_superres249.dll -lopencv_ts249 -lopencv_video249.dll -lopencv_videostab249.dll

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/photonique2.exe

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/photonique2.exe: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/photonique2 ${OBJECTFILES} ${LDLIBSOPTIONS} -fopenmp -march=native -O2 -s

${OBJECTDIR}/correction.o: correction.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -s -Iinclude -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/correction.o correction.cpp

${OBJECTDIR}/exposure.o: exposure.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -s -Iinclude -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/exposure.o exposure.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -s -Iinclude -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

${OBJECTDIR}/matcher.o: matcher.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -s -Iinclude -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/matcher.o matcher.cpp

${OBJECTDIR}/stitching.o: stitching.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -Wall -s -Iinclude -I. -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/stitching.o stitching.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/photonique2.exe

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
