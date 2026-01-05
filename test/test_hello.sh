#! /usr/bin/bash

mkdir tmp
./test/hello > tmp/output.txt
printf "Hello, World!\n" > tmp/expected_output.txt
diff -u tmp/expected_output.txt tmp/output.txt
if [ $? -ne 0 ]; then
    echo "Expected"
    cat tmp/expected_output.txt
    echo "Got"
    cat tmp/output.txt
    rm -rf tmp
    exit 1
fi

rm -rf tmp
