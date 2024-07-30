#Minimum requirement of CMake version : 3.0.0
cmake_minimum_required(VERSION 3.0.0)

#Project name and version number
project(${libUPD} VERSION 0.1.0)

# set include path
include_directories(${libUPDSrc})
include_directories(${libGnutSrc})
include_directories(${libMatSrc})

# for all the header files and source_files
file(GLOB_RECURSE header_files *.h *.hpp)
file(GLOB_RECURSE source_files *.cpp)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	# find files and put them into different groups
	file(GLOB gall_header_files 	gall/*.h)
	file(GLOB gcoders_header_files 	gcoders/*.h)
	file(GLOB gdata_header_files 	gdata/*.h)
	file(GLOB gproc_header_files 	gproc/*.h)
	file(GLOB gset_header_files 	gset/*.h)
	file(GLOB gutils_header_files 	gutils/*.h)
	file(GLOB gambfix_header_files 	gambfix/*.h)

	#file(GLOB newmat_header_files	newmat/*.h)

	file(GLOB gall_source_files 	gall/*.cpp)
	file(GLOB gcoders_source_files 	gcoders/*.cpp)
	file(GLOB gdata_source_files 	gdata/*.cpp)
	file(GLOB gproc_source_files 	gproc/*.cpp)
	file(GLOB gset_source_files 	gset/*.cpp)
	file(GLOB gutils_source_files 	gutils/*.cpp)
	file(GLOB gambfix_source_files 	gambfix/*.cpp)

	# add files to project
	source_group("CMake Files"  FILES CMakeLists.txt)
	source_group("Header Files" FILES header_files)
	source_group("Soruce Files" FILES source_files)
	
	source_group("Header Files\\gall"	FILES ${gall_header_files})
	source_group("Header Files\\gcoders"	FILES ${gcoders_header_files})
	source_group("Header Files\\gdata"		FILES ${gdata_header_files})
	source_group("Header Files\\gproc"		FILES ${gproc_header_files})
	source_group("Header Files\\gset"		FILES ${gset_header_files})
	source_group("Header Files\\gutils"		FILES ${gutils_header_files})
	source_group("Header Files\\gambfix"	FILES ${gambfix_header_files})
	
	source_group("Source Files\\gall"		FILES ${gall_source_files})
	source_group("Source Files\\gcoders"	FILES ${gcoders_source_files})
	source_group("Source Files\\gdata"		FILES ${gdata_source_files})
	source_group("Source Files\\gproc"		FILES ${gproc_source_files})
	source_group("Source Files\\gset"		FILES ${gset_source_files})
	source_group("Source Files\\gutils"		FILES ${gutils_source_files})
	source_group("Source Files\\gambfix"	FILES ${gambfix_source_files})
else()
   
endif()

add_definitions(-D GREATUE_UPD_LIBRARY)
add_library(${PROJECT_NAME} SHARED ${header_files} ${source_files})

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	link_directories(${ROOT}/build/Bin/Debug)
	link_directories(${ROOT}/build/Bin/Release)
	link_directories(${ROOT}/build/Bin/RelWithDebInfo)
	link_directories(${ROOT}/build/Bin/MinSizeRel)
else()
	link_directories(${ROOT}/build/Lib)
endif()

target_link_libraries(${PROJECT_NAME} ${libMat})
target_link_libraries(${PROJECT_NAME} ${libGnut})
add_dependencies(${PROJECT_NAME} ${libMat} ${libGnut})





	
