set(SOURCES
    FileIO.cpp
)

add_library(flippin_common OBJECT ${SOURCES})
add_library(Flippin::Common ALIAS flippin_common)

get_target_property(FMT_INCLUDES fmt::fmt INCLUDE_DIRECTORIES)

target_include_directories(flippin_common PUBLIC ${CMAKE_CURRENT_LIST_DIR} ${FMT_INCLUDES})

target_compile_options(flippin_common PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
