set(SOURCES
    Directory.cpp
    Entry.cpp
    File.cpp
)

add_library(flippy_lib STATIC ${SOURCES})
add_library(Flippy::Lib ALIAS flippy_lib)
target_include_directories(flippy_lib PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(flippy_lib PRIVATE Flippy::Common)
