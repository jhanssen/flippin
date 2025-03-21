add_library(flippy_common INTERFACE)
add_library(Flippy::Common ALIAS flippy_common)

target_include_directories(flippy_common INTERFACE ${CMAKE_CURRENT_LIST_DIR})
