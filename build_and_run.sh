#!/bin/bash

cd build &&
cmake .. && make && ./reader
cd ..

cleanup() {
  echo -e "Keyboard Interrupt" &&
  cd ..
}
