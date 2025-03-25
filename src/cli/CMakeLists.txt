set(SOURCES
    main.cpp
    CatCommand.cpp
    CopyCommand.cpp
    CreateCommand.cpp
    DelCommand.cpp
    DirCommand.cpp
    FilesystemCommand.cpp
    ReadCommand.cpp
    WriteCommand.cpp
    Slashes.cpp
)

add_executable(flippy ${SOURCES})
target_link_libraries(flippy Flippy::Common Flippy::Lib fmt::fmt)
