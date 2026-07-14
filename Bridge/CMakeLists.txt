cmake_minimum_required(VERSION 3.31)

add_executable(SonyXM5Bridge WIN32
    src/main.cpp
)

target_link_libraries(SonyXM5Bridge PRIVATE mdr Shell32.lib)
target_compile_features(SonyXM5Bridge PRIVATE cxx_std_20)

if(MSVC)
    target_compile_options(SonyXM5Bridge PRIVATE /W4 /permissive- /EHsc)
else()
    target_compile_options(SonyXM5Bridge PRIVATE -Wall -Wextra -Wpedantic)
endif()

set_target_properties(SonyXM5Bridge PROPERTIES
    OUTPUT_NAME "SonyXM5Bridge"
)
