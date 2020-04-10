# CMakeList.txt: testCmake 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

PROJECT(CORE)
# 将源代码添加到此项目的可执行文件。

SET(CMAKE_BUILD_TYPE "release")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")



aux_source_directory(. SRC_LIST) #把当前目录(.)下所有源代码文件和头文件加入变量SRC_LIST
aux_source_directory(crypto CRYPTO) 
aux_source_directory(support SUPPORT) 



include_directories(.)
#include_directories(secp256k1/include)
#include_directories(libsodium/include)


link_directories(libs) 


IF (WIN32)
	#set(BOOST_ROOT E:/boost_1_72_0)
	find_package(BOOST COMPONENTS thread)
	if(Boost_FOUND)
	    message(Boost_INCLUDE_DIRS " ${Boost_INCLUDE_DIRS}")
	    message(Boost_LIBRARY_DIRS " ${Boost_LIBRARY_DIRS}")
	    include_directories(${Boost_INCLUDE_DIRS})
	    link_directories(${Boost_LIBRARY_DIRS})
	endif()

ELSEIF (UNIX)
	#set(BOOST_ROOT /mnt/f/boost_1_72_0)
	find_package(Boost COMPONENTS thread system)
	if(Boost_FOUND)
		include_directories(${Boost_INCLUDE_DIRS})
		MESSAGE( STATUS "Boost_INCLUDE_DIRS = ${Boost_INCLUDE_DIRS}.")
		MESSAGE( STATUS "Boost_LIBRARIES = ${Boost_LIBRARIES}.")
		MESSAGE( STATUS "Boost_LIB_VERSION = ${Boost_LIB_VERSION}.")
		add_executable(vds-privkey-transfer ${SRC_LIST} ${CRYPTO} ${SUPPORT}) 
		target_link_libraries (vds-privkey-transfer ${Boost_LIBRARIES})
		target_link_libraries(vds-privkey-transfer secp256k1)
		target_link_libraries(vds-privkey-transfer sodium)
		target_link_libraries(vds-privkey-transfer ssl)

		set(CMAKE_INSTALL_PREFIX /usr)
		install(TARGETS vds-privkey-transfer DESTINATION bin)
	endif()
	#set(Boost_INCLUDE_DIR  /mnt/f/boost_1_72_0)
	#include_directories(${Boost_INCLUDE_DIR})

ENDIF ()