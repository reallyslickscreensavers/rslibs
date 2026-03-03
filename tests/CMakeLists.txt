cmake_minimum_required(VERSION 3.14)
project(rslibs_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.17.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Repo root for <rsMath/rsMath.h> style includes
set(REPO_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/..")

# rsMath library sources
set(RSMATH_SOURCES
    ${REPO_ROOT}/rsMath/rsVec.cpp
    ${REPO_ROOT}/rsMath/rsVec4.cpp
    ${REPO_ROOT}/rsMath/rsQuat.cpp
    ${REPO_ROOT}/rsMath/rsMatrix.cpp
)

# Rgbhsl library sources
set(RGBHSL_SOURCES
    ${REPO_ROOT}/Rgbhsl/Rgbhsl.cpp
)

# Implicit library sources (excluding impSurface and impCubeVolume which depend on OpenGL)
set(IMPLICIT_SOURCES
    ${REPO_ROOT}/Implicit/impShape.cpp
    ${REPO_ROOT}/Implicit/impSphere.cpp
    ${REPO_ROOT}/Implicit/impEllipsoid.cpp
    ${REPO_ROOT}/Implicit/impTorus.cpp
    ${REPO_ROOT}/Implicit/impKnot.cpp
    ${REPO_ROOT}/Implicit/impCapsule.cpp
    ${REPO_ROOT}/Implicit/impRoundedHexahedron.cpp
    ${REPO_ROOT}/Implicit/impHexahedron.cpp
    ${REPO_ROOT}/Implicit/impCubeTables.cpp
)

# Test executable
add_executable(rslibs_tests
    test_rsMath.cpp
    test_rsTrigonometry.cpp
    test_Rgbhsl.cpp
    test_Implicit.cpp
    ${RSMATH_SOURCES}
    ${RGBHSL_SOURCES}
    ${IMPLICIT_SOURCES}
)

target_include_directories(rslibs_tests PRIVATE
    ${REPO_ROOT}             # for <rsMath/rsMath.h> style
    ${REPO_ROOT}/rsMath      # for "rsMath.h" style (used by .cpp files)
    ${REPO_ROOT}/Rgbhsl      # for "Rgbhsl.h" style (used by .cpp files)
    ${REPO_ROOT}/Implicit    # for "impShape.h" style (used by .cpp files)
)

target_link_libraries(rslibs_tests PRIVATE GTest::gtest_main)

enable_testing()
include(GoogleTest)
gtest_discover_tests(rslibs_tests DISCOVERY_MODE PRE_TEST)
