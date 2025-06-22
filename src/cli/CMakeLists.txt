set(SOURCES
    main.cpp
    CopyCommand.cpp
    CreateCommand.cpp
    DelCommand.cpp
    DeltreeCommand.cpp
    DirCommand.cpp
    FilesystemCommand.cpp
    MdCommand.cpp
    RdCommand.cpp
    ReadCommand.cpp
    RenCommand.cpp
    TypeCommand.cpp
    WriteCommand.cpp
    Options.cpp
    Slashes.cpp
)

add_executable(flippin ${SOURCES})
target_link_libraries(flippin Flippin::Common Flippin::Lib fmt::fmt)

target_compile_options(flippin PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
