# Warning settings shared by every target in the project.
# 
# Usage:
#   target_link_libraries(my_target PRIVATE gsim::warnings)

option(GSIM_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

add_library(gsim_warnings INTERFACE)
add_library(gsim::warnings ALIAS gsim_warnings)

if(MSVC)
    target_compile_options(gsim_warnings INTERFACE
        /W4             # high warning level
        /permissive-    # strict standard conformance
        /utf-8)
    if(GSIM_WARNINGS_AS_ERRORS)
        target_compile_options(gsim_warnings INTERFACE /WX)
    endif()
else()
    target_compile_options(gsim_warnings INTERFACE
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow              # local variable shadows an outer one
        -Wnon-virtual-dtor    # polymorphic base class without virtual dtor
        -Wold-style-cast      # (int)x instead of static_cast<int>(x)
        -Wcast-align
        -Woverloaded-virtual  # accidentally hiding a virtual function
        -Wconversion          # implicit narrowing conversion
        -Wsign-conversion
        -Wdouble-promotion    # float silently promoted to double
        -Wnull-dereference)
    if(GSIM_WARNINGS_AS_ERRORS)
        target_compile_options(gsim_warnings INTERFACE -Werror)
    endif()
endif()
