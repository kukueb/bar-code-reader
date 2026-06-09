#!/bin/bash

if [[ -z "$1" ]]; then
  echo "no arg" && exit
fi

./build_and_run.sh &&
  echo && echo &&
  sed -n '/^Total/,$p' data/compare_$1.txt
