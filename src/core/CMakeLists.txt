add_library(gsim_core)
add_library(gsim::core ALIAS gsim_core)

target_sources(gsim_core
    PRIVATE
        src/Version.cpp)

target_include_directories(gsim_core
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_compile_features(gsim_core PUBLIC cxx_std_20)

target_compile_definitions(gsim_core
    PRIVATE
        GSIM_VERSION_MAJOR=${PROJECT_VERSION_MAJOR}
        GSIM_VERSION_MINOR=${PROJECT_VERSION_MINOR}
        GSIM_VERSION_PATCH=${PROJECT_VERSION_PATCH}
        GSIM_VERSION_STRING="${PROJECT_VERSION}")

target_link_libraries(gsim_core PRIVATE gsim::warnings)
