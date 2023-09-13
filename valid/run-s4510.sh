#!/bin/bash
rm -rf driver_response.txt;
../src/disksim  s4510.parv  s4510.outv   validate  $1  0;
./comp  $2  driver_response.txt;
