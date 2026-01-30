# FLOAT 库
set(FLOAT_SRC 
    ${CMAKE_CURRENT_SOURCE_DIR}/FLOAT.c
    ${CMAKE_CURRENT_SOURCE_DIR}/FLOAT_vfprintf.c
)

add_library(FLOAT STATIC ${FLOAT_SRC})

target_compile_options(FLOAT PRIVATE
    -m32
    -O2
    -fno-builtin
    -fno-stack-protector
    -U_FORTIFY_SOURCE
    -D_FORTIFY_SOURCE=0
)

target_include_directories(FLOAT PUBLIC
    ${CMAKE_SOURCE_DIR}/lib-common
)

# 设置输出目录
set_target_properties(FLOAT PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib-common/FLOAT
)
