# ---------------------------------------------------------------------------
# GoogleTest
#
# Uses a system-installed GTest when one is available (FIND_PACKAGE_ARGS),
# otherwise downloads a pinned release. SYSTEM suppresses GTest's own warnings
# so that GSIM_WARNINGS_AS_ERRORS only applies to our code.
# ---------------------------------------------------------------------------
include(FetchContent)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)  # MSVC: match our /MD runtime
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.18.0
    GIT_SHALLOW    TRUE
    SYSTEM
    EXCLUDE_FROM_ALL
    FIND_PACKAGE_ARGS NAMES GTest)

FetchContent_MakeAvailable(googletest)

include(GoogleTest)

# ---------------------------------------------------------------------------
# gsim_add_test(<target> <source>...)
#
# Builds one test executable and registers every TEST() inside it with CTest.
# ---------------------------------------------------------------------------
function(gsim_add_test target)
    add_executable(${target})

    target_sources(${target} PRIVATE ${ARGN})

    target_link_libraries(${target}
        PRIVATE
            gsim::core
            gsim::warnings
            GTest::gtest_main)

    gtest_discover_tests(${target}
        DISCOVERY_TIMEOUT 20
        PROPERTIES LABELS unit)
endfunction()

# ---------------------------------------------------------------------------
# Test targets
# ---------------------------------------------------------------------------
gsim_add_test(core_tests
    core/AssertTest.cpp
    core/LogTest.cpp
    core/VersionTest.cpp)
