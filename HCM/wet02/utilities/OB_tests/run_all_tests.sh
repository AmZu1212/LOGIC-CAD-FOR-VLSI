#!/bin/bash

# SAT-Based Formal Equivalence Checker - Test Runner
# Runs all test cases and compares results to expected outcomes
#
# USAGE:
#   From WSL bash terminal:
#     cd /mnt/c/Users/ogal/OneDrive\ -\ NVIDIA\ Corporation/Documents/VsCode/vlsi-cad/Wet/HCM/wet02
#     chmod +x run_all_tests.sh
#     ./run_all_tests.sh
#
#   Or from PowerShell:
#     wsl
#     cd "/mnt/c/Users/ogal/OneDrive - NVIDIA Corporation/Documents/VsCode/vlsi-cad/Wet/HCM/wet02"
#     bash run_all_tests.sh

echo "=========================================================================="
echo "SAT-Based Formal Equivalence Checker - Test Suite"
echo "=========================================================================="
echo ""

# Color codes for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Counters
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Function to run a test
run_test() {
    local test_num=$1
    local test_name=$2
    local command=$3
    local expected=$4
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    echo "----------------------------------------------------------------------"
    echo "TEST $test_num: $test_name"
    echo "Command: $command"
    echo "Expected: $expected"
    echo ""
    
    # Run the command and capture output
    output=$(eval $command 2>&1)
    
    # Check result
    if [[ $output == *"NOT SATISFIABLE"* ]]; then
        actual="UNSAT"
    elif [[ $output == *"SATISFIABLE"* ]]; then
        actual="SAT"
    else
        actual="ERROR"
    fi
    
    echo "Actual: $actual"
    
    # Compare with expected
    if [[ $expected == $actual ]]; then
        echo -e "${GREEN}RESULT: PASS ✓${NC}"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo -e "${RED}RESULT: FAIL ✗${NC}"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        echo "Output:"
        echo "$output" | head -20
    fi
    echo ""
}

# TEST 1: Equivalent Circuits (test0000.v vs test0001.v)
run_test 1 "test0000 vs test0001 (Equivalent)" \
    "./gl_verilog_fev -s TopLevel0000 stdcell.v test0000.v -i TopLevel0001 stdcell.v test0001.v" \
    "UNSAT"

# TEST 2: Non-Equivalent Circuits (test1110.v vs test1111.v)
run_test 2 "test1110 vs test1111 (Non-Equivalent)" \
    "./gl_verilog_fev -s TopLevel1110 stdcell.v test1110.v -i TopLevel1111 stdcell.v test1111.v" \
    "SAT"

# TEST 3: Self-Check (circuit vs itself)
run_test 3 "test0000 vs test0000 (Self-Check)" \
    "./gl_verilog_fev -s TopLevel0000 stdcell.v test0000.v -i TopLevel0000 stdcell.v test0000.v" \
    "UNSAT"

# TEST 4: Large NAND (test_nand5_a.v vs test_nand5_b.v)
run_test 4 "test_nand5_a vs test_nand5_b (Equivalent NAND5)" \
    "./gl_verilog_fev -s TopLevelNand5A stdcell.v test_nand5_a.v -i TopLevelNand5B stdcell.v test_nand5_b.v" \
    "UNSAT"

# TEST 5: Large OR (test_or7_a.v vs test_or7_b.v)
run_test 5 "test_or7_a vs test_or7_b (Equivalent OR7)" \
    "./gl_verilog_fev -s TopLevelOr7A stdcell.v test_or7_a.v -i TopLevelOr7B stdcell.v test_or7_b.v" \
    "UNSAT"

# TEST 6: Sequential Circuits with DFF (test4440.v vs test4441.v)
run_test 6 "test4440 vs test4441 (Sequential with DFF)" \
    "./gl_verilog_fev -s TopLevel4440 stdcell.v test4440.v -i TopLevel4441 stdcell.v test4441.v" \
    "UNSAT"

# TEST 7: Comprehensive Gate Test (test5550.v vs test5551.v)
run_test 7 "test5550 vs test5551 (All gate types)" \
    "./gl_verilog_fev -s TopLevel5550 stdcell.v test5550.v -i TopLevel5551 stdcell.v test5551.v" \
    "UNSAT"

# TEST 8: ISCAS c0409 vs c0410 (Non-Equivalent)
run_test 8 "c0409 vs c0410 (ISCAS - Non-Equivalent)" \
    "./gl_verilog_fev -s TopLevel0409 stdcell.v c0409.v -i TopLevel0410 stdcell.v c0410.v" \
    "SAT"

# TEST 9: ISCAS c1355 vs c1356 (Equivalent)
run_test 9 "c1355 vs c1356 (ISCAS - Equivalent)" \
    "./gl_verilog_fev -s TopLevel1355 stdcell.v c1355.v -i TopLevel1356 stdcell.v c1356.v" \
    "UNSAT"

# TEST 10: ISCAS c1404 vs c1405 (Equivalent)
run_test 10 "c1404 vs c1405 (ISCAS - Equivalent)" \
    "./gl_verilog_fev -s TopLevel1404 stdcell.v c1404.v -i TopLevel1405 stdcell.v c1405.v" \
    "UNSAT"

# TEST 11: ISCAS c2670 vs c2671 (Non-Equivalent)
run_test 11 "c2670 vs c2671 (ISCAS - Non-Equivalent)" \
    "./gl_verilog_fev -s TopLevel2670 stdcell.v c2670.v -i TopLevel2671 stdcell.v c2671.v" \
    "SAT"

# TEST 12: ISCAS c2670 vs c2672 (Equivalent)
run_test 12 "c2670 vs c2672 (ISCAS - Equivalent)" \
    "./gl_verilog_fev -s TopLevel2670 stdcell.v c2670.v -i TopLevel2672 stdcell.v c2672.v" \
    "UNSAT"

# TEST 13: ISCAS c2806 vs c2807 (Non-Equivalent)
run_test 13 "c2806 vs c2807 (ISCAS - Non-Equivalent)" \
    "./gl_verilog_fev -s TopLevel2806 stdcell.v c2806.v -i TopLevel2807 stdcell.v c2807.v" \
    "SAT"

# Summary
echo "=========================================================================="
echo "TEST SUITE SUMMARY"
echo "=========================================================================="
echo "Total Tests:  $TOTAL_TESTS"
echo -e "${GREEN}Passed:       $PASSED_TESTS${NC}"
echo -e "${RED}Failed:       $FAILED_TESTS${NC}"
echo ""

if [ $FAILED_TESTS -eq 0 ]; then
    echo -e "${GREEN}ALL TESTS PASSED! ✓✓✓${NC}"
    exit 0
else
    echo -e "${RED}SOME TESTS FAILED!${NC}"
    exit 1
fi
