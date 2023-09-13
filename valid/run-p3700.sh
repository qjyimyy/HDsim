#!/bin/bash
rm -rf driver_response.txt;
../src/disksim  p3700.parv  p3700.outv   validate  $1  0;
./comp  $2  driver_response.txt;
