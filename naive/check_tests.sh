#!/bin/bash

# ====================================================
# Script to check and summarize the status of all tests
# ====================================================

echo ""
echo "🔎 Verifying test suite execution..."
echo ""

# Run the test suite
./run_all_tests.sh
STATUS=$?

if [ $STATUS -eq 0 ]; then
    echo -e "\nAll tests PASSED successfully.\n"
    exit 0
else
    echo -e "\nOne or more tests FAILED.\n"
    exit 1
fi
