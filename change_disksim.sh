#!/bin/bash
rm -f ./src/disksim
make
ls -l ./src/disksim | awk '{print $6, $7, $8}'