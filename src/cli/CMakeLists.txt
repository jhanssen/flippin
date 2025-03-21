set(SOURCES
    main.cpp
)

add_executable(flippy ${SOURCES})
target_link_libraries(flippy Flippy::Common Flippy::Lib fmt::fmt)
