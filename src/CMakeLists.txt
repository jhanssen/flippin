set(SOURCES
    main.cpp
)

add_executable(flippy ${SOURCES})
target_link_libraries(flippy llfat fmt::fmt)
target_compile_features(flippy PRIVATE cxx_std_20)
