set(SOURCES
    main.cpp
    CopyCommand.cpp
    CreateCommand.cpp
    FilesystemCommand.cpp
    WriteCommand.cpp
    Slashes.cpp
)

add_executable(flippy ${SOURCES})
target_link_libraries(flippy Flippy::Common Flippy::Lib fmt::fmt)
