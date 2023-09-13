#!/bin/sh

if [ "x${RUNVALID_PREFIX}" != "x" ];
then
PREFIX=${RUNVALID_PREFIX}
else
PREFIX=../src
fi

echo "Run ssdontape simulate"

echo ""
echo "ssdontape cache: 2M"
${PREFIX}/disksim ssdontape-2M.parv ssdontape-2M.outv validate yuan-validate.trace 0\
&& grep "IOdriver Response time average" ssdontape-2M.outv
grep "IOdriver Response time Delay Fluctuation" ssdontape-2M.outv

echo ""
echo "ssdontape cache: 4M"
${PREFIX}/disksim ssdontape-4M.parv ssdontape-4M.outv validate yuan-validate.trace 0\
&& grep "IOdriver Response time average" ssdontape-4M.outv
grep "IOdriver Response time Delay Fluctuation" ssdontape-4M.outv

echo ""
echo "ssdontape cache: 8M"
${PREFIX}/disksim ssdontape-8M.parv ssdontape-8M.outv validate yuan-validate.trace 0\
&& grep "IOdriver Response time average" ssdontape-8M.outv
grep "IOdriver Response time Delay Fluctuation" ssdontape-8M.outv

echo ""
echo "ssdontape cache: 16M"
${PREFIX}/disksim ssdontape-16M.parv ssdontape-16M.outv validate yuan-validate.trace 0\
&& grep "IOdriver Response time average" ssdontape-16M.outv
grep "IOdriver Response time Delay Fluctuation" ssdontape-16M.outv

echo ""
echo "ssdontape cache: 32M"
${PREFIX}/disksim ssdontape-32M.parv ssdontape-32M.outv validate yuan-validate.trace 0\
&& grep "IOdriver Response time average" ssdontape-32M.outv
grep "IOdriver Response time Delay Fluctuation" ssdontape-32M.outv

echo ""
echo "ssdontape cache: 64M"
${PREFIX}/disksim ssdontape-64M.parv ssdontape-64M.outv validate yuan-validate.trace 0\
&& grep "IOdriver Response time average" ssdontape-64M.outv
grep "IOdriver Response time Delay Fluctuation" ssdontape-64M.outv

echo ""
echo "ssdontape cache: 128M"
${PREFIX}/disksim ssdontape-128M.parv ssdontape-128M.outv validate yuan-validate.trace 0\
&& grep "IOdriver Response time average" ssdontape-128M.outv
grep "IOdriver Response time Delay Fluctuation" ssdontape-128M.outv

echo ""
echo "ssdontape cache: 256M"
${PREFIX}/disksim ssdontape-256M.parv ssdontape-256M.outv validate yuan-validate.trace 0\
&& grep "IOdriver Response time average" ssdontape-256M.outv
grep "IOdriver Response time Delay Fluctuation" ssdontape-256M.outv

echo ""
echo "ssdontape cache: 512M"
${PREFIX}/disksim ssdontape-512M.parv ssdontape-512M.outv validate yuan-validate.trace 0\
&& grep "IOdriver Response time average" ssdontape-512M.outv
grep "IOdriver Response time Delay Fluctuation" ssdontape-512M.outv

echo ""
echo "ssdontape cache: 1G"
${PREFIX}/disksim ssdontape-1G.parv ssdontape-1G.outv validate yuan-validate.trace 0\
&& grep "IOdriver Response time average" ssdontape-1G.outv
grep "IOdriver Response time Delay Fluctuation" ssdontape-1G.outv

echo ""
echo "ssdontape cache: 2G"
${PREFIX}/disksim ssdontape-2G.parv ssdontape-2G.outv validate yuan-validate.trace 0\
&& grep "IOdriver Response time average" ssdontape-2G.outv
grep "IOdriver Response time Delay Fluctuation" ssdontape-2G.outv

echo ""
echo "ssdontape cache: 4G"
${PREFIX}/disksim ssdontape-4G.parv ssdontape-4G.outv validate yuan-validate.trace 0\
&& grep "IOdriver Response time average" ssdontape-4G.outv
grep "IOdriver Response time Delay Fluctuation" ssdontape-4G.outv

