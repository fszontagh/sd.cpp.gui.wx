find_package(CpuFeatures)

if (NOT TARGET CpuFeatures::CpuFeatures)
# CPU Features dependency
FetchContent_Declare(
    CpuFeatures
    GIT_REPOSITORY https://github.com/google/cpu_features.git
    GIT_TAG v0.9.0

)
set(BUILD_TESTING OFF)

FetchContent_MakeAvailable(CpuFeatures)

endif()
