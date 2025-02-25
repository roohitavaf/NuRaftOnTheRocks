#!/bin/bash
set -e

# Find and execute all test executables that end with _test
for test_file in ./*_test; do
    if [[ -x "$test_file" ]]; then
        echo "Running test: $test_file"
        "$test_file"
    fi
done