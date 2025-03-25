set(SOURCES
    main.cpp
    CatCommand.cpp
    CopyCommand.cpp
    CreateCommand.cpp
    DelCommand.cpp
    DirCommand.cpp
    FilesystemCommand.cpp
    ReadCommand.cpp
    RenCommand.cpp
    WriteCommand.cpp
    Slashes.cpp
)

add_executable(flippy ${SOURCES})
target_link_libraries(flippy Flippy::Common Flippy::Lib fmt::fmt)

target_compile_options(flippy PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
