set(SOURCES
    Directory.cpp
    Entry.cpp
    FatDirectory.cpp
    FatFile.cpp
    File.cpp
    Filesystem.cpp
)

add_library(flippy_lib STATIC ${SOURCES})
add_library(Flippy::Lib ALIAS flippy_lib)
target_include_directories(flippy_lib PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(flippy_lib PRIVATE Flippy::Common llfat fmt::fmt)

target_compile_options(flippy_lib PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
