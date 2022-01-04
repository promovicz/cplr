cmake_minimum_required(VERSION 3.0.0)

project(cplr)

find_package(PkgConfig REQUIRED)

pkg_search_module(LIBTCC REQUIRED libtcc)

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

add_compile_definitions(
  _GNU_SOURCE _POSIX_C_SOURCE=200809L
  )

target_link_libraries(
  cplr PUBLIC -pthread -lm -ldl -lreadline
  )

target_include_directories(
  cplr PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
  )
target_include_directories(
  cplr PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/source
  )

target_include_directories(
  cplr PUBLIC ${LIBTCC_INCLUDE_DIRS}
  )
target_link_libraries(
  cplr PUBLIC ${LIBTCC_LDFLAGS}
  )

install(
  TARGETS cplr
  RUNTIME DESTINATION bin
  )
