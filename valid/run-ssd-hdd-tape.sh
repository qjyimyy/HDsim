#!/bin/sh

if [ "x${RUNVALID_PREFIX}" != "x" ];
then
PREFIX=${RUNVALID_PREFIX}
else
PREFIX=../src
fi

echo "Run ssd hdd and tape simulate"

echo ""
echo "ssd simulate"
${PREFIX}/disksim ssd.parv ssd.outv validate yuan-validate.trace 0
grep "IOdriver Response time average" ssd.outv
grep "IOdriver Response time Delay Fluctuation" ssd.outv

echo ""
echo "hdd simulate"
${PREFIX}/disksim seagate.parv seagate-hdd.outv validate yuan-validate.trace 0
grep "IOdriver Response time average" seagate-hdd.outv
grep "IOdriver Response time Delay Fluctuation" seagate-hdd.outv

echo ""
echo "tape simulate"
${PREFIX}/disksim tape.parv tape1.outv validate yuan-validate.trace 0
grep "IOdriver Response time average" tape1.outv
grep "IOdriver Response time Delay Fluctuation" tape1.outv