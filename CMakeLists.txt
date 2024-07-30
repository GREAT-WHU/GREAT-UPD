#Minimum requirement of CMake version : 3.0.0
cmake_minimum_required(VERSION 3.0.0)

#Project name and version number
project(GREAT-UPD VERSION 0.1.0)

# Compile default settings, more settings can be selected in the cmake command
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
	add_compile_options(-O)
	add_compile_options(-pthread)
	add_compile_options(-w)
	add_compile_options(-m64)
	add_compile_options(-lz)
	add_compile_options(-lstdc++)
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
	add_compile_options(-g)
	add_compile_options(-O2)
	add_compile_options(-Wall)
	add_compile_options(-pthread)
endif()

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
	set(CMAKE_CXX_STANDARD 11)	
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -pthread")
else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

#Set the ROOT and subdirectory, you should put the CMakeList.txt in these file directories
set(ROOT ${PROJECT_SOURCE_DIR})
set(EXECUTABLE_OUTPUT_PATH ${ROOT}/build/Bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ROOT}/build/Bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ROOT}/build/Lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${ROOT}/build/Lib)

set(CMAKE_DEBUG_POSTFIX "d")
set(CMAKE_RELEASE_POSTFIX "")
set(CMAKE_RELWITHDEBINFO_POSTFIX "rd")
set(CMAKE_MINSIZEREL_POSTFIX "s")

#Choose different compilation configurations according to VS compilation
if(CMAKE_BUILD_TYPE MATCHES "Release")
	set(CMAKE_BUILD_POSTFIX "${CMAKE_RELEASE_POSTFIX}") 
elseif(CMAKE_BUILD_TYPE MATCHES "Debug")
	set(CMAKE_BUILD_POSTFIX "${CMAKE_DEBUG_POSTFIX}") 
elseif(CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
	set(CMAKE_BUILD_POSTFIX "${CMAKE_RELWITHDEBINFO_POSTFIX}")
elseif(CMAKE_BUILD_TYPE MATCHES "MinSizeRel")
	set(CMAKE_BUILD_POSTFIX "${CMAKE_MINSIZEREL_POSTFIX}")
else()
	set(CMAKE_BUILD_POSTFIX "")
endif()	

# global value
# set lib name
set( libMat              libMat)
set( libGnut             libGnut)
set( libUPD              libUPD)
# set lib path
set( libMatSrc      ${PROJECT_SOURCE_DIR}/src/LibMat)
set( libGnutSrc     ${PROJECT_SOURCE_DIR}/src/LibGnut)
set( libUPDSrc      ${PROJECT_SOURCE_DIR}/src/LibUPD)
# For lib source files
add_subdirectory(${libMatSrc}           ${ROOT}/build/${libMat})
add_subdirectory(${libGnutSrc}          ${ROOT}/build/${libGnut})
add_subdirectory(${libUPDSrc}           ${ROOT}/build/${libUPD})
# group
SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)

SET_PROPERTY(TARGET ${libGnut}          PROPERTY FOLDER "LIB")
SET_PROPERTY(TARGET ${libMat}		    PROPERTY FOLDER "LIB")
SET_PROPERTY(TARGET ${libUPD}		    PROPERTY FOLDER "LIB")

set (UPDLSQ                 GREAT-UPD )
# for main file of the UPD estimation
add_subdirectory(${ROOT}/src/app         ${ROOT}/build/${UPDLSQ})
SET_PROPERTY(TARGET ${UPDLSQ}            PROPERTY FOLDER "UPD")

#Output Messages for debug the Cmake
message(STATUS "operation system is : ${CMAKE_SYSTEM}") 
message(STATUS "current platform is : ${CMAKE_SYSTEM_NAME}")		
message(STATUS "CMake version is    : ${CMAKE_SYSTEM_VERSION}") 
message(STATUS "C compiler is : ${CMAKE_C_COMPILER}")
message(STATUS "C++ compiler is : ${CMAKE_CXX_COMPILER}")	
message(STATUS "The program main directory is : ${ROOT}")