cmake_minimum_required(VERSION 3.16)

project(TJU_NEMU2025 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
enable_language(ASM)
enable_testing()

# Index-only configuration for CLion.

set(TJU_COMMON_INCLUDES
  "${CMAKE_SOURCE_DIR}/lib-common"
  "${CMAKE_SOURCE_DIR}/lib-common/x86-inc"
  "${CMAKE_SOURCE_DIR}/lib-common/uclibc/include"
)

set(TJU_NEMU_INCLUDES
  "${CMAKE_SOURCE_DIR}/nemu/include"
  "${CMAKE_SOURCE_DIR}/nemu/src"
)

set(TJU_KERNEL_INCLUDES
  "${CMAKE_SOURCE_DIR}/kernel/include"
  "${CMAKE_SOURCE_DIR}/kernel/src"
)

set(TJU_GAME_INCLUDES
  "${CMAKE_SOURCE_DIR}/game/include"
  "${CMAKE_SOURCE_DIR}/game/src"
  "${CMAKE_SOURCE_DIR}/game/src/typing/include"
  "${CMAKE_SOURCE_DIR}/game/src/nemu-pal/include"
)

set(TJU_TESTCASE_INCLUDES
  "${CMAKE_SOURCE_DIR}/testcase/include"
)

set(TJU_BASE_CFLAGS -Wall -Werror)

set(TJU_NEMU_CFLAGS -ggdb3 -O2)
set(TJU_KERNEL_CFLAGS -m32 -O2 -fno-builtin -fno-omit-frame-pointer -fno-stack-protector -fno-pie -fno-pic)
set(TJU_GAME_CFLAGS -m32 -O2 -fno-builtin -fno-omit-frame-pointer -fno-stack-protector -Wno-format)
set(TJU_TESTCASE_CFLAGS -m32 -fno-builtin -fno-stack-protector -Wno-format -fno-pie -fno-pic)
set(TJU_FLOAT_CFLAGS -m32 -O2 -fno-builtin -fno-stack-protector -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0)
set(TJU_TESTCASE_LINK_FLAGS -Wl,-m,elf_i386 -Wl,-e,main)
set(TJU_TESTCASE_MOV_LINK_FLAGS -Wl,-m,elf_i386 -Wl,-Ttext,0x100000)
set(TJU_PRINT_FLOAT_LINUX_DEFS LINUX_RT)
set(TJU_NEMU_BIN "${CMAKE_SOURCE_DIR}/obj/nemu/nemu" CACHE FILEPATH "Path to NEMU executable")
set(TJU_NEMU_ARGS "" CACHE STRING "Extra args for NEMU")
set(TJU_RUN_NEMU_SCRIPT "${CMAKE_SOURCE_DIR}/scripts/run_nemu.sh")
set(TJU_BUILD_NEMU_WITH_MAKE ON CACHE BOOL "Build NEMU via Makefile before running NEMU targets")

set(TJU_TESTCASE_TARGETS "")

add_custom_target(check
  COMMAND "${CMAKE_CTEST_COMMAND}" --output-on-failure -R nemu_tc_
  WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
  USES_TERMINAL
)

if(TJU_BUILD_NEMU_WITH_MAKE)
  add_custom_target(nemu_build
    COMMAND "${CMAKE_MAKE_PROGRAM}" nemu
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    USES_TERMINAL
  )
  add_dependencies(check nemu_build)
endif()

file(GLOB_RECURSE TJU_NEMU_SOURCES CONFIGURE_DEPENDS
  "nemu/src/*.c"
  "nemu/src/*.S"
)

file(GLOB_RECURSE TJU_KERNEL_SOURCES CONFIGURE_DEPENDS
  "kernel/src/*.c"
  "kernel/src/*.S"
)

file(GLOB_RECURSE TJU_TESTCASE_SOURCES CONFIGURE_DEPENDS
  "testcase/src/*.c"
  "testcase/src/*.S"
)

file(GLOB_RECURSE TJU_GAME_COMMON_SOURCES CONFIGURE_DEPENDS
  "game/src/common/*.c"
  "game/src/common/*.S"
)

set(TJU_GAME_VARIANT "typing" CACHE STRING "game variant: typing or nemu-pal")
set_property(CACHE TJU_GAME_VARIANT PROPERTY STRINGS "typing" "nemu-pal")

if(TJU_GAME_VARIANT STREQUAL "nemu-pal")
  file(GLOB_RECURSE TJU_GAME_VARIANT_SOURCES CONFIGURE_DEPENDS
    "game/src/nemu-pal/*.c"
    "game/src/nemu-pal/*.S"
  )
else()
  file(GLOB_RECURSE TJU_GAME_VARIANT_SOURCES CONFIGURE_DEPENDS
    "game/src/typing/*.c"
    "game/src/typing/*.S"
  )
endif()

file(GLOB_RECURSE TJU_FLOAT_SOURCES CONFIGURE_DEPENDS
  "lib-common/FLOAT/*.c"
)

add_library(tju_nemu_index OBJECT ${TJU_NEMU_SOURCES})
target_include_directories(tju_nemu_index PRIVATE ${TJU_COMMON_INCLUDES} ${TJU_NEMU_INCLUDES})
target_compile_options(tju_nemu_index PRIVATE ${TJU_BASE_CFLAGS} ${TJU_NEMU_CFLAGS})

add_library(tju_kernel_index OBJECT ${TJU_KERNEL_SOURCES})
target_include_directories(tju_kernel_index PRIVATE ${TJU_COMMON_INCLUDES} ${TJU_KERNEL_INCLUDES})
target_compile_options(tju_kernel_index PRIVATE ${TJU_BASE_CFLAGS} ${TJU_KERNEL_CFLAGS})

add_library(tju_testcase_index OBJECT ${TJU_TESTCASE_SOURCES})
target_include_directories(tju_testcase_index PRIVATE ${TJU_COMMON_INCLUDES} ${TJU_TESTCASE_INCLUDES})
target_compile_options(tju_testcase_index PRIVATE ${TJU_BASE_CFLAGS} ${TJU_TESTCASE_CFLAGS})

add_library(tju_game_index OBJECT ${TJU_GAME_COMMON_SOURCES} ${TJU_GAME_VARIANT_SOURCES})
target_include_directories(tju_game_index PRIVATE ${TJU_COMMON_INCLUDES} ${TJU_GAME_INCLUDES})
target_compile_options(tju_game_index PRIVATE ${TJU_BASE_CFLAGS} ${TJU_GAME_CFLAGS})

add_library(tju_float_index OBJECT ${TJU_FLOAT_SOURCES})
target_include_directories(tju_float_index PRIVATE ${TJU_COMMON_INCLUDES})
target_compile_options(tju_float_index PRIVATE ${TJU_BASE_CFLAGS} ${TJU_FLOAT_CFLAGS})

# Build libs used by testcase executables.
add_library(tju_float STATIC ${TJU_FLOAT_SOURCES})
target_include_directories(tju_float PRIVATE ${TJU_COMMON_INCLUDES})
target_compile_options(tju_float PRIVATE ${TJU_BASE_CFLAGS} ${TJU_FLOAT_CFLAGS})

set(TJU_LIBC_PATH "${CMAKE_SOURCE_DIR}/lib-common/uclibc/lib/libc.a")
add_library(tju_libc STATIC IMPORTED)
set_target_properties(tju_libc PROPERTIES IMPORTED_LOCATION "${TJU_LIBC_PATH}")

# Generate testcase executables for debugging.
file(GLOB TJU_TESTCASE_C_SOURCES CONFIGURE_DEPENDS "testcase/src/*.c")
file(GLOB TJU_TESTCASE_S_SOURCES CONFIGURE_DEPENDS "testcase/src/*.S")

set(TJU_TESTCASE_START_SRC "${CMAKE_SOURCE_DIR}/testcase/src/start.S")
set(TJU_TESTCASE_MOV_SRC "${CMAKE_SOURCE_DIR}/testcase/src/mov.S")

function(tju_add_testcase_exe src)
  get_filename_component(base "${src}" NAME_WE)
  string(REGEX REPLACE "[^A-Za-z0-9_]" "_" target "tc_${base}")

  set(use_start TRUE)
  set(link_flags "${TJU_TESTCASE_LINK_FLAGS}")
  if("${src}" STREQUAL "${TJU_TESTCASE_MOV_SRC}")
    set(use_start FALSE)
    set(link_flags "${TJU_TESTCASE_MOV_LINK_FLAGS}")
  endif()

  if(use_start)
    add_executable(${target} "${TJU_TESTCASE_START_SRC}" "${src}")
  else()
    add_executable(${target} "${src}")
  endif()

  set_target_properties(${target} PROPERTIES
    OUTPUT_NAME "${base}"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/testcase"
  )

  target_include_directories(${target} PRIVATE ${TJU_COMMON_INCLUDES} ${TJU_TESTCASE_INCLUDES})
  target_compile_options(${target} PRIVATE ${TJU_BASE_CFLAGS} ${TJU_TESTCASE_CFLAGS})
  target_link_options(${target} PRIVATE ${link_flags} -nostdlib -nodefaultlibs -nostartfiles -no-pie)
  target_link_libraries(${target} PRIVATE tju_float tju_libc)

  add_custom_target(run_nemu_${target}
    COMMAND /bin/sh "${TJU_RUN_NEMU_SCRIPT}" "${TJU_NEMU_BIN}" "${CMAKE_BINARY_DIR}/testcase/${base}" ${TJU_NEMU_ARGS}
    DEPENDS ${target}
    USES_TERMINAL
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
  )
  if(TJU_BUILD_NEMU_WITH_MAKE)
    add_dependencies(run_nemu_${target} nemu_build)
  endif()

  add_test(NAME nemu_${target}
    COMMAND /bin/sh "${TJU_RUN_NEMU_SCRIPT}" "${TJU_NEMU_BIN}" "${CMAKE_BINARY_DIR}/testcase/${base}" ${TJU_NEMU_ARGS}
  )
  set_tests_properties(nemu_${target} PROPERTIES WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

  list(APPEND TJU_TESTCASE_TARGETS ${target})
  set(TJU_TESTCASE_TARGETS "${TJU_TESTCASE_TARGETS}" PARENT_SCOPE)
endfunction()

foreach(src IN LISTS TJU_TESTCASE_C_SOURCES)
  if(src MATCHES "testcase/src/print-FLOAT.c")
    continue()
  endif()
  tju_add_testcase_exe("${src}")
endforeach()

foreach(src IN LISTS TJU_TESTCASE_S_SOURCES)
  if(src MATCHES "testcase/src/start.S")
    continue()
  endif()
  if(src MATCHES "testcase/src/mov.S")
    continue()
  endif()
  tju_add_testcase_exe("${src}")
endforeach()

# Special cases for additional compile flags.
if(TARGET tc_integral)
  target_compile_options(tc_integral PRIVATE -O2 -msoft-float -mno-80387 -mno-fp-ret-in-387)
endif()
if(TARGET tc_quadratic_eq)
  target_compile_options(tc_quadratic_eq PRIVATE -O2 -msoft-float -mno-80387 -mno-fp-ret-in-387)
endif()

# Linux runtime variant for print-FLOAT.
if(EXISTS "${CMAKE_SOURCE_DIR}/testcase/src/print-FLOAT.c")
  add_executable(tc_print_FLOAT_linux "${CMAKE_SOURCE_DIR}/testcase/src/print-FLOAT.c")
  set_target_properties(tc_print_FLOAT_linux PROPERTIES
    OUTPUT_NAME "print-FLOAT-linux"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/testcase"
  )
  target_compile_definitions(tc_print_FLOAT_linux PRIVATE ${TJU_PRINT_FLOAT_LINUX_DEFS})
  target_include_directories(tc_print_FLOAT_linux PRIVATE ${TJU_COMMON_INCLUDES} ${TJU_TESTCASE_INCLUDES})
  target_compile_options(tc_print_FLOAT_linux PRIVATE ${TJU_BASE_CFLAGS} ${TJU_TESTCASE_CFLAGS})
  target_link_options(tc_print_FLOAT_linux PRIVATE ${TJU_TESTCASE_LINK_FLAGS} -nostdlib -nodefaultlibs -nostartfiles -no-pie)
  target_sources(tc_print_FLOAT_linux PRIVATE
    "${CMAKE_SOURCE_DIR}/lib-common/uclibc/lib/crt1.o"
    "${CMAKE_SOURCE_DIR}/lib-common/uclibc/lib/crti.o"
    "${CMAKE_SOURCE_DIR}/lib-common/uclibc/lib/crtn.o"
  )
  target_link_libraries(tc_print_FLOAT_linux PRIVATE tju_float tju_libc)
  list(APPEND TJU_TESTCASE_TARGETS tc_print_FLOAT_linux)
endif()

# Special handling for mov.S: link with ld directly to avoid libgcc dependency.
if(EXISTS "${CMAKE_SOURCE_DIR}/testcase/src/mov.S")
  set(TJU_MOV_OBJ "${CMAKE_BINARY_DIR}/CMakeFiles/tc_mov.dir/testcase/src/mov.S.o")
  set(TJU_MOV_BIN "${CMAKE_BINARY_DIR}/testcase/mov")
  add_custom_command(
    OUTPUT "${TJU_MOV_BIN}"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_BINARY_DIR}/CMakeFiles/tc_mov.dir/testcase/src"
    COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_BINARY_DIR}/testcase"
    COMMAND "${CMAKE_C_COMPILER}" -m32 -c
      -I${CMAKE_SOURCE_DIR}/lib-common
      -I${CMAKE_SOURCE_DIR}/testcase/include
      ${CMAKE_SOURCE_DIR}/testcase/src/mov.S
      -o ${TJU_MOV_OBJ}
    COMMAND "${CMAKE_LINKER}" -m elf_i386 -Ttext=0x100000 -o "${TJU_MOV_BIN}" "${TJU_MOV_OBJ}"
    DEPENDS "${CMAKE_SOURCE_DIR}/testcase/src/mov.S"
    VERBATIM
  )
  add_custom_target(tc_mov DEPENDS "${TJU_MOV_BIN}")
  if(TJU_BUILD_NEMU_WITH_MAKE)
    add_dependencies(tc_mov nemu_build)
  endif()
  add_test(NAME nemu_tc_mov
    COMMAND /bin/sh "${TJU_RUN_NEMU_SCRIPT}" "${TJU_NEMU_BIN}" "${TJU_MOV_BIN}" ${TJU_NEMU_ARGS}
  )
  set_tests_properties(nemu_tc_mov PROPERTIES WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
  list(APPEND TJU_TESTCASE_TARGETS tc_mov)
endif()

if(TJU_TESTCASE_TARGETS)
  add_dependencies(check ${TJU_TESTCASE_TARGETS})
endif()
