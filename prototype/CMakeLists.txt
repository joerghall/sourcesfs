cmake_minimum_required(VERSION 3.0)


include(filelist.cmake)

include_directories(
    SYSTEM /usr/local/include/osxfuse/fuse
)

link_directories(
    /usr/local/lib
)

add_definitions(
    -DFUSE_USE_VERSION=26
    -D_FILE_OFFSET_BITS=64
    -D_DARWIN_USE_64_BIT_INODE
)

add_executable(sourcesfs
    ${prototype_sources}
    ${prototype_sources}
)

target_link_libraries(sourcesfs
    osxfuse
)
