set(SOURCES
    Directory.cpp
    Entry.cpp
    FatDirectory.cpp
    FatFile.cpp
    File.cpp
    Filesystem.cpp
    Utf8.cpp
)

find_package(Iconv REQUIRED)
if (NOT Iconv_FOUND)
    message(FATAL_ERROR "iconv not found")
endif()

find_package(ZLIB REQUIRED)
if (NOT ZLIB_FOUND)
    message(FATAL_ERROR "zlib not found")
endif()

add_library(flippin_lib STATIC ${SOURCES})
add_library(Flippin::Lib ALIAS flippin_lib)
target_include_directories(flippin_lib PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(flippin_lib PRIVATE Flippin::Common llfat fmt::fmt ${Iconv_LIBRARIES} ${ZLIB_LIBRARIES})

target_compile_options(flippin_lib PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
