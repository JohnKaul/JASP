cmake_minimum_required( VERSION 2.8 )
project( JASP )

# ==---------------------------== #
# Product version...
# ==---------------------------== #

# set( PROJECT_VERSION 1.0 )
# ...or...
set( VERSION_MAJOR 0 )
set( VERSION_MINOR 1 )
set( VERSION_PATCH 0 )

# ==---------------------------== #
# Directory setup
# ==---------------------------== #
set( SRCS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src )
set( SRCS_INCLUDE_DIR ${SRCS_DIR}/include )
set( EXTRA_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../include )
# EXECUTABLE_OUTPUT_PATH 
# -----------------------
# set this variable to specify a common place where CMake should put all
# executable files (instead of CMAKE_CURRENT_BINARY_DIR)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})

# LIBRARY_OUTPUT_PATH 
# -----------------------
# set this variable to specify a common place where CMake should put all
# libraries (instead of CMAKE_CURRENT_BINARY_DIR)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})


# ==---------------------------== #
# Adding/using the LLVM libraries
# ==---------------------------== #
# 
# ===
# locate LLVM
# ===
# A LLVM_ROOT is a convenience variable...using this instead of "FIND_PACKAGE"
set( LLVM_ROOT "C:/LLVM" )

# Sanity check:
if(NOT EXISTS ${LLVM_ROOT}/include/llvm)
  message( FATAL_ERROR "LLVM_ROOT (${LLVM_ROOT}) is not a valid LLVM install" )
endif()

# ===
# Incorporate the CMake features provided by LLVM:
# ===
set( CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    "${LLVM_ROOT}/share/llvm/cmake"
    )

include(AddLLVM)
include(AddLLVMDefinitions)
include(HandleLLVMOptions)
include(LLVMConfig)
include(TableGen)
include(LLVMConfigVersion)
include(LLVMParseArguments)
include(LLVMProcessSources)

# ==---------------------------== #
# Misc.
# ==---------------------------== #

# enable warnings
add_definitions( -DWall )

# ==---------------------------== #
# Locations to required libs and headers.
# ==---------------------------== #
set( LLVM_INCLUDE_DIRS
    "${LLVM_ROOT}/include"
    )
set( LLVM_LIBRARY_DIRS
    "${LLVM_ROOT}/lib"
    )
set( LLVM_LIB_SEARCH_PATH
    "${LLVM_ROOT}/lib"
    )

# Now set the header and library paths for project and LLVM:
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${SRCS_INCLUDE_DIR}
    ${EXTRA_INCLUDE_DIR}
    ${LLVM_INCLUDE_DIRS}
    )

link_directories( 
    ${LLVM_LIBRARY_DIRS} 
    )

add_definitions( 
    ${LLVM_DEFINITIONS} 
    )

# ==---------------------------== #
# Source file list.
# ==---------------------------== #
set( SRCS
    ${SRCS_DIR}/main.cpp
)

# ==---------------------------== #
# Building.
# ==---------------------------== #

# We need to add the LLVM libs:
llvm_map_components_to_libraries(
    REQ_LLVM_LIBRARIES
    core
    support
    )

# copy the LICENCE file to the build directory
configure_file( 
    ${CMAKE_CURRENT_SOURCE_DIR}/doc/LICENSE.txt 
    ${EXECUTABLE_OUTPUT_PATH}/LICENSE.txt @ONLY 
    )

# copy the README file to the build directory
configure_file( 
    ${CMAKE_CURRENT_SOURCE_DIR}/doc/ReadMe.txt 
    ${EXECUTABLE_OUTPUT_PATH}/ReadMe.txt @ONLY 
    )

add_executable( ${PROJECT_NAME} ${SRCS} )

target_link_libraries(${PROJECT_NAME} ${REQ_LLVM_LIBRARIES})

install(TARGETS ${PROJECT_NAME}
    RUNTIME
    DESTINATION bin
    )

install(FILES 
    ${EXECUTABLE_OUTPUT_PATH}/LICENSE.txt 
    DESTINATION bin
    )

# install(FILES 
#     ${SRCS_DIR}/etc/libgcc_s_dw2-1.dll
#     DESTINATION bin
#     )

install(FILES 
    ${EXECUTABLE_OUTPUT_PATH}/ReadMe.txt 
    DESTINATION doc
    )

add_subdirectory( tests )

# ==---------------------------== #
# Packaging.
# ==---------------------------== #

if( WIN32 )
    set( CPACK_GENERATOR NSIS )
endif()

set( PACKAGE_STRING "${PROJECT_NAME}_${PROJECT_VERSION}" )
set( PACKAGE_BUGREPORT "${CONTACT_EMAIL}" )

set( CPACK_PACKAGE_NAME "${PACKAGE_STRING}" )
set( CPACK_PACKAGE_VENDOR "${CONTACT_COMPANY}" )
set( CPACK_PACKAGE_DESCRIPTION_SUMMARY "A summary of my project." )
set( CPACK_PACKAGE_DESCRIPTION_FILE ${EXECUTABLE_OUTPUT_PATH}/ReadMe.txt )
set( CPACK_PACKAGE_CONTACT "${PACKAGE_BUGREPORT}")
set( CPACK_RESOURCE_FILE_LICENSE ${EXECUTABLE_OUTPUT_PATH}/LICENSE.txt )
set( CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR} )
set( CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR} )
set( CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH} )

include(CPack)
