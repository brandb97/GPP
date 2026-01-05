#! /usr/bin/bash

mkdir tmp
./test/pingpong > tmp/output.txt
printf "ping\npong\nping\npong\nping\npong\n" > tmp/expected_output.txt
diff -u tmp/expected_output.txt tmp/output.txt
if [ $? -ne 0 ]; then
    echo "Expected"
    cat tmp/expected_output.txt
    echo "Got"
    cat tmp/output.txt
    exit 1
fi

rm -rf tmp
