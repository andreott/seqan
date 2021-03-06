# ===========================================================================
#                  SeqAn - The Library for Sequence Analysis
# ===========================================================================
# File: /core/tests/arg_parse/CMakeLists.txt
#
# CMakeLists.txt file for the arg_parse module tests.
# ===========================================================================

cmake_minimum_required (VERSION 2.8.2)
project (seqan_core_tests_arg_parse)
message (STATUS "Configuring core/tests/arg_parse")

# ----------------------------------------------------------------------------
# Dependencies
# ----------------------------------------------------------------------------

# Search SeqAn and select dependencies.
set (SEQAN_FIND_DEPENDENCIES NONE)
find_package (SeqAn REQUIRED)

# ----------------------------------------------------------------------------
# Build Setup
# ----------------------------------------------------------------------------

# Add include directories.
include_directories (${SEQAN_INCLUDE_DIRS})

# Add definitions set by find_package (SeqAn).
add_definitions (${SEQAN_DEFINITIONS})

# Update the list of file names below if you add source files to your test.
add_executable (test_arg_parse
               test_arg_parse.cpp
               test_extensions.h
               test_arg_parse.h
               test_arg_parse_argument.h
               test_arg_parse_option.h
               test_argument_parser.h
               test_arg_parse_ctd_support.h)

# Add dependencies found by find_package (SeqAn).
target_link_libraries (test_arg_parse ${SEQAN_LIBRARIES})

# Add CXX flags found by find_package (SeqAn).
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SEQAN_CXX_FLAGS}")

# ----------------------------------------------------------------------------
# Register with CTest
# ----------------------------------------------------------------------------

add_test (NAME test_test_arg_parse COMMAND $<TARGET_FILE:test_arg_parse>)
