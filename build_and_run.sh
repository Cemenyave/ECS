#!/bin/bash

cd build
cmake --build .
compilation_result=$?

if [ $compilation_result -eq 0 ]; then
  echo "compilation SUCCESS"
  ./samples/sandbox/sandbox
else
  echo "compilation FAILED %compilation_result"
fi

cd ..
