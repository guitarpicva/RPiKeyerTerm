#!/bin/bash
export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH
cd ~/RPiKeyerTerm
./RPiKeyerTerm & > /dev/null 2>&1
exit 0
