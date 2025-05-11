# 启用测试
enable_testing()

set(TEST_ROOT_DIR "${CMAKE_SOURCE_DIR}/test")
set(TEST_EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/output/test_program)

include(${CMAKE_SOURCE_DIR}/test/script/auto_add_test_by_dir.cmake) # 注册测试
include(${CMAKE_SOURCE_DIR}/test/script/auto_add_exec.cmake)

# if defined(_MSC_VER)
if(WIN32)
    auto_add_exec("draft")
elseif(APPLE)
elseif(UNIX)
endif()

# endif
auto_add_test_by_dir("base")
auto_add_test_by_dir("ip")

auto_add_test_by_dir("abnf")
auto_add_test_by_dir("abnf/operators")
auto_add_test_by_dir("abnf/uri")
auto_add_test_by_dir("abnf/imf")

auto_add_test_by_dir("abnf/http")
auto_add_test_by_dir("abnf/core")