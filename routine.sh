#!/bin/sh
gcc -o  main main.c

for input_file in test/*.in; do

    # Extract the base filename without the directory and extension
    base_name=$(basename "$input_file" .in)
    
    # Run your command: ./main < input file > out2
    ./main < "$input_file" > out2
    
    # Compute the diff between out2 and expected output
    diff_output=$(diff -w out2 "test/${base_name}.out")
    
    # Check if diff output is empty
    if [ -z "$diff_output" ]; then
        echo "$base_name: PASSED"
    else
        echo "$base_name: FAIL"
        echo "$diff_output"
    fi
done