#!/bin/bash
rm -rf driver_response.txt;
../src/disksim  ocssd.parv  ocssd.outv   validate  $1  0;
echo |grep "Overall I/O System Response time average" ocssd.outv