include(ExternalProject)
ExternalProject_Add(
    libisofs_build
    GIT_REPOSITORY https://github.com/jhanssen/libisofs.git
    GIT_TAG bootstrap
    SOURCE_DIR "${CMAKE_CURRENT_BINARY_DIR}/libisofs-src"
    CONFIGURE_COMMAND ./configure --enable-static --disable-shared
    BUILD_COMMAND make
    BUILD_IN_SOURCE ON
    BUILD_BYPRODUCTS
        "${CMAKE_CURRENT_BINARY_DIR}/libisofs-src/libisofs/.libs/libisofs.a"
        "${CMAKE_CURRENT_BINARY_DIR}/libisofs-src/libisofs/.libs/libisofs.la"
    INSTALL_COMMAND ""
    UPDATE_COMMAND "")

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/libisofs-src/libisofs")

add_library(libisofs STATIC IMPORTED GLOBAL)
set_target_properties(libisofs PROPERTIES
    IMPORTED_LOCATION "${CMAKE_CURRENT_BINARY_DIR}/libisofs-src/libisofs/.libs/libisofs.a"
    INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}/libisofs-src/libisofs"
)
add_dependencies(libisofs libisofs_build)
add_library(Isofs::Lib ALIAS libisofs)
