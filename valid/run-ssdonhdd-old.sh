#!/bin/sh

if [ "x${RUNVALID_PREFIX}" != "x" ];
then
PREFIX=${RUNVALID_PREFIX}
else
PREFIX=../src
fi

echo "Run ssdonhdd simulate"

echo ""
echo "ssdonhdd cache: 2M"
${PREFIX}/disksim ssd-cache-2M.parv ssd-cache-2M.outv validate yuan-validate.trace 0
grep "IOdriver Response time average" ssd-cache-2M.outv
grep "IOdriver Response time Delay Fluctuation" ssd-cache-2M.outv

echo ""
echo "ssdonhdd cache: 4M"
${PREFIX}/disksim ssd-cache-4M.parv ssd-cache-4M.outv validate yuan-validate.trace 0
grep "IOdriver Response time average" ssd-cache-4M.outv
grep "IOdriver Response time Delay Fluctuation" ssd-cache-4M.outv

echo ""
echo "ssdonhdd cache: 6M"
${PREFIX}/disksim ssd-cache-6M.parv ssd-cache-6M.outv validate yuan-validate.trace 0
grep "IOdriver Response time average" ssd-cache-6M.outv
grep "IOdriver Response time Delay Fluctuation" ssd-cache-6M.outv

echo ""
echo "ssdonhdd cache: 8M"
${PREFIX}/disksim ssd-cache-8M.parv ssd-cache-8M.outv validate yuan-validate.trace 0
grep "IOdriver Response time average" ssd-cache-8M.outv
grep "IOdriver Response time Delay Fluctuation" ssd-cache-8M.outv

echo ""
echo "ssdonhdd cache: 12M"
${PREFIX}/disksim ssd-cache-12M.parv ssd-cache-12M.outv validate yuan-validate.trace 0
grep "IOdriver Response time average" ssd-cache-12M.outv
grep "IOdriver Response time Delay Fluctuation" ssd-cache-12M.outv

echo ""
echo "ssdonhdd cache: 16M"
${PREFIX}/disksim ssd-cache-16M.parv ssd-cache-16M.outv validate yuan-validate.trace 0
grep "IOdriver Response time average" ssd-cache-16M.outv
grep "IOdriver Response time Delay Fluctuation" ssd-cache-16M.outv

echo ""
echo "ssdonhdd cache: 20M"
${PREFIX}/disksim ssd-cache-20M.parv ssd-cache-20M.outv validate yuan-validate.trace 0
grep "IOdriver Response time average" ssd-cache-20M.outv
grep "IOdriver Response time Delay Fluctuation" ssd-cache-20M.outv

echo ""
echo "ssdonhdd cache: 24M"
${PREFIX}/disksim ssd-cache-24M.parv ssd-cache-24M.outv validate yuan-validate.trace 0
grep "IOdriver Response time average" ssd-cache-24M.outv
grep "IOdriver Response time Delay Fluctuation" ssd-cache-24M.outv

echo ""
echo "ssdonhdd cache: 28M"
${PREFIX}/disksim ssd-cache-28M.parv ssd-cache-28M.outv validate yuan-validate.trace 0
grep "IOdriver Response time average" ssd-cache-28M.outv
grep "IOdriver Response time Delay Fluctuation" ssd-cache-28M.outv

echo ""
echo "ssdonhdd cache: 32M"
${PREFIX}/disksim ssd-cache-32M.parv ssd-cache-32M.outv validate yuan-validate.trace 0
grep "IOdriver Response time average" ssd-cache-32M.outv
grep "IOdriver Response time Delay Fluctuation" ssd-cache-32M.outv

echo ""
echo "ssdonhdd cache: 36M"
${PREFIX}/disksim ssd-cache-36M.parv ssd-cache-36M.outv validate yuan-validate.trace 0
grep "IOdriver Response time average" ssd-cache-36M.outv
grep "IOdriver Response time Delay Fluctuation" ssd-cache-36M.outv

echo ""
echo "ssdonhdd cache: 40M"
${PREFIX}/disksim ssd-cache-40M.parv ssd-cache-40M.outv validate yuan-validate.trace 0
grep "IOdriver Response time average" ssd-cache-40M.outv
grep "IOdriver Response time Delay Fluctuation" ssd-cache-40M.outv

echo ""
echo "ssdonhdd cache: 44M"
${PREFIX}/disksim ssd-cache-44M.parv ssd-cache-44M.outv validate yuan-validate.trace 0
grep "IOdriver Response time average" ssd-cache-44M.outv
grep "IOdriver Response time Delay Fluctuation" ssd-cache-44M.outv

echo ""
echo "ssdonhdd cache: 48M"
${PREFIX}/disksim ssd-cache-48M.parv ssd-cache-48M.outv validate yuan-validate.trace 0
grep "IOdriver Response time average" ssd-cache-48M.outv
grep "IOdriver Response time Delay Fluctuation" ssd-cache-48M.outv

echo ""
echo "ssdonhdd cache: 52M"
${PREFIX}/disksim ssd-cache-52M.parv ssd-cache-52M.outv validate yuan-validate.trace 0
grep "IOdriver Response time average" ssd-cache-52M.outv
grep "IOdriver Response time Delay Fluctuation" ssd-cache-52M.outv

echo ""
echo "ssdonhdd cache: 56M"
${PREFIX}/disksim ssd-cache-56M.parv ssd-cache-56M.outv validate yuan-validate.trace 0
grep "IOdriver Response time average" ssd-cache-56M.outv
grep "IOdriver Response time Delay Fluctuation" ssd-cache-56M.outv

echo ""
echo "ssdonhdd cache: 60M"
${PREFIX}/disksim ssd-cache-60M.parv ssd-cache-60M.outv validate yuan-validate.trace 0
grep "IOdriver Response time average" ssd-cache-60M.outv
grep "IOdriver Response time Delay Fluctuation" ssd-cache-60M.outv

echo ""
echo "ssdonhdd cache: 64M"
${PREFIX}/disksim ssd-cache-64M.parv ssd-cache-64M.outv validate yuan-validate.trace 0
grep "IOdriver Response time average" ssd-cache-64M.outv
grep "IOdriver Response time Delay Fluctuation" ssd-cache-64M.outv

echo ""
echo "ssdonhdd cache: 68M"
${PREFIX}/disksim ssd-cache-68M.parv ssd-cache-68M.outv validate yuan-validate.trace 0
grep "IOdriver Response time average" ssd-cache-68M.outv
grep "IOdriver Response time Delay Fluctuation" ssd-cache-68M.outv

echo ""
echo "ssdonhdd cache: 72M"
${PREFIX}/disksim ssd-cache-72M.parv ssd-cache-72M.outv validate yuan-validate.trace 0
grep "IOdriver Response time average" ssd-cache-72M.outv
grep "IOdriver Response time Delay Fluctuation" ssd-cache-72M.outv

echo ""
echo "ssdonhdd cache: 76M"
${PREFIX}/disksim ssd-cache-76M.parv ssd-cache-76M.outv validate yuan-validate.trace 0
grep "IOdriver Response time average" ssd-cache-76M.outv
grep "IOdriver Response time Delay Fluctuation" ssd-cache-76M.outv

echo ""
echo "ssdonhdd cache: 80M"
${PREFIX}/disksim ssd-cache-80M.parv ssd-cache-80M.outv validate yuan-validate.trace 0
grep "IOdriver Response time average" ssd-cache-80M.outv
grep "IOdriver Response time Delay Fluctuation" ssd-cache-80M.outv

echo ""
echo "ssdonhdd cache: 128M"
${PREFIX}/disksim ssd-cache-128M.parv ssd-cache-128M.outv validate yuan-validate.trace 0
grep "IOdriver Response time average" ssd-cache-128M.outv
grep "IOdriver Response time Delay Fluctuation" ssd-cache-128M.outv

echo ""
echo "ssdonhdd cache: 256M"
${PREFIX}/disksim ssd-cache-256M.parv ssd-cache-256M.outv validate yuan-validate.trace 0
grep "IOdriver Response time average" ssd-cache-256M.outv
grep "IOdriver Response time Delay Fluctuation" ssd-cache-256M.outv

echo ""
echo "ssdonhdd cache: 512M"
${PREFIX}/disksim ssd-cache-512M.parv ssd-cache-512M.outv validate yuan-validate.trace 0
grep "IOdriver Response time average" ssd-cache-512M.outv
grep "IOdriver Response time Delay Fluctuation" ssd-cache-512M.outv