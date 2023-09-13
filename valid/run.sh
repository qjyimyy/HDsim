#!/bin/bash
rm -rf driver_response.txt;
../src/disksim  ssd-cache.parv  ssd-cache.outv   validate  $1  0;
./comp  $2  driver_response.txt;
