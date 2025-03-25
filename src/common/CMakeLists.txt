set(SOURCES
    File.cpp
)

add_library(flippy_common OBJECT ${SOURCES})
add_library(Flippy::Common ALIAS flippy_common)

get_target_property(FMT_INCLUDES fmt::fmt INCLUDE_DIRECTORIES)

target_include_directories(flippy_common PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${FMT_INCLUDES})
