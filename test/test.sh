#! /usr/bin/bash

# should be run in the root directory of the project
pwd=$(pwd)
if [ "${pwd##*/}" != "G++" ]; then
  echo "Please run this script in the root directory of the project."
  exit 1
fi

cd test
make
cd ..

for test_sh in test/test_*.sh; do
  echo "Running $test_sh..."
  bash "$test_sh"
  if [ $? -ne 0 ]; then
    echo "$test_sh failed."
    exit 1
  else
    echo "$test_sh passed."
  fi
done
