cmake_minimum_required(VERSION 3.0.0)

project(cplr)

include(CheckIncludeFile)
include(ExternalProject)

find_package(PkgConfig)

# TODO detect libm, libdl, libreadline, libpthread

set(CPLR_VERSION_STRING "0.1")

# build directory for dependencies
set(DEPBLD ${CMAKE_BINARY_DIR}/depbld)

# stage directory for dependencies
set(STAGE ${CMAKE_BINARY_DIR}/stage)

# Build optional BDW garbage collector
#
# We must build the collector ourselves because we want replace-malloc.
#
option(CPLR_ENABLE_BDWGC "Enable BDW garbage collector as malloc replacement" ON)
if(CPLR_ENABLE_BDWGC)
  set(BDWGC_CONFIG --prefix=/bdwgc --enable-static --disable-shared --enable-replace-malloc)
  set(BDWGC_SOURCE_DIR ${CMAKE_SOURCE_DIR}/deps/bdwgc)
  # build bdwgc and install to stage
  ExternalProject_Add(bdwgc
    PREFIX ${DEPBLD}
    SOURCE_DIR ${BDWGC_SOURCE_DIR}
    DOWNLOAD_COMMAND true
    PATCH_COMMAND autoreconf -vi ${BDWGC_SOURCE_DIR}
    CONFIGURE_COMMAND ${BDWGC_SOURCE_DIR}/configure ${BDWGC_CONFIG} && make clean
    BUILD_COMMAND make all
    INSTALL_COMMAND make install DESTDIR=${STAGE}
    )
  # determine includes and libs
  set(BDWGC_INCLUDE_DIRS ${STAGE}/bdwgc/include)
  set(BDWGC_LDFLAGS ${STAGE}/bdwgc/lib/libgc.a)
endif()

# Build TinyCC or find using pkg-config
#
# We prefer building our own libtcc to prevent issues with stale versions.
#
# Use of external libtcc builds requires yet-unmerged pkg-config support.
#
option(CPLR_ENABLE_TINYCC "Enable TinyCC backend" ON)
if(CPLR_ENABLE_TINYCC)
  option(CPLR_TINYCC_EXTERNAL "TinyCC: use system libtcc" OFF)
  if(CPLR_TINYCC_EXTERNAL)
    pkg_search_module(LIBTCC libtcc)
    if(NOT LIBTCC_FOUND)
      check_include_file(libtcc.h LIBTCC_INCLUDE_DIRS)
      find_library(LIBTCC_LDFLAGS tcc RERQUIRED)
    endif()
  else()
    set(TINYCC_CONFIG --prefix=/tinycc --enable-static)
    set(TINYCC_SOURCE_DIR ${CMAKE_SOURCE_DIR}/deps/tinycc)
    # build tinycc and install to stage
    ExternalProject_Add(tinycc
      PREFIX ${DEPBLD}
      SOURCE_DIR ${TINYCC_SOURCE_DIR}
      DOWNLOAD_COMMAND true
      CONFIGURE_COMMAND ${TINYCC_SOURCE_DIR}/configure ${TINYCC_CONFIG} && make clean
      BUILD_COMMAND make all
      INSTALL_COMMAND make install DESTDIR=${STAGE}
      )
    # determine includes and libs
    set(LIBTCC_INCLUDE_DIRS ${STAGE}/tinycc/include)
    set(LIBTCC_LDFLAGS ${STAGE}/tinycc/lib/libtcc.a)
    # install tcc libdir
    set(CPLR_TINYCC_LIBDIR ${CMAKE_INSTALL_PREFIX}/lib/cplr/tcc)
    install(
      DIRECTORY ${CMAKE_BINARY_DIR}/stage/tinycc/lib/tcc
      DESTINATION lib/cplr)
  endif()
endif()

# Generate config header
configure_file(source/cplr_config.h.in source/cplr_config.h)

# Compatibility defines
add_compile_definitions(
  _GNU_SOURCE _POSIX_C_SOURCE=200809L
  )

# Main executable
add_executable(
  cplr
  source/cext_abort.c
  source/cext_list.c
  source/cext_memory.c
  source/cext_string.c
  source/cext_value.c
  source/cpkg.c
  source/cplr_command.c
  source/cplr_compile.c
  source/cplr_defaults.c
  source/cplr_execute.c
  source/cplr_generate.c
  source/cplr_interact.c
  source/cplr_main.c
  source/cplr_optparse.c
  source/cplr_package.c
  source/cplr_prepare.c
  source/cplr_run.c
  source/cplr_state.c
  source/cplr_symbols.c
  )
# Include internal headers
target_include_directories(
  cplr PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
  )
target_include_directories(
  cplr PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/source
  )
target_include_directories(
  cplr PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/source
  )
# Take care of bdwgc
if(CPLR_ENABLE_BDWGC)
  target_include_directories(
    cplr PUBLIC ${BDWGC_INCLUDE_DIRS}
    )
  target_link_libraries(
    cplr PUBLIC ${BDWGC_LDFLAGS}
    )
  if(NOT CPLR_BDWGC_EXTERNAL)
    add_dependencies(cplr bdwgc)
  endif()
endif()
# Take care of tinycc
if(CPLR_ENABLE_TINYCC)
  target_include_directories(
    cplr PUBLIC ${LIBTCC_INCLUDE_DIRS}
    )
  target_link_libraries(
    cplr PUBLIC ${LIBTCC_LDFLAGS}
    )
  if(NOT CPLR_TINYCC_EXTERNAL)
    add_dependencies(cplr tinycc)
  endif()
endif()
# Required system libraries
target_link_libraries(
  cplr PUBLIC -pthread -lm -ldl -lreadline
  )
# Install the binary
install(
  TARGETS cplr
  RUNTIME DESTINATION bin
  )
