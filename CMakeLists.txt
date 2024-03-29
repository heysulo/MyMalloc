cmake_minimum_required(VERSION 3.5)

project(MyMalloc)

set(CMAKE_CXX_STANDARD 11)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_compile_definitions(GTEST_ACTIVE)

add_executable(
  ${PROJECT_NAME}
  MyApplication.cpp
  MyMalloc.cpp
)

target_link_libraries(
    ${PROJECT_NAME}
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})

# add_executable(${PROJECT_NAME} MyApplication.cpp MyMalloc.cpp)
