#!/bin/bash

# Run ssd on hdd simulation

# filename
SSDONTAPE="ssd-cache"
# tracefile
TRACE_FILE="yuan-validate.trace"
VALIDATE="validate"
SYNTHGEN="0"
SUFFIX_PARV=".parv"
SUFFIX_OUTV=".outv"
GREP_AVG_TIME="IOdriver Response time average"
GREP_DELAY_FLUC="IOdriver Response time Delay Fluctuation"

cachesize=("2M" "4M" "6M" "8M" "12M" "14M" "16M" "18M" "20M" "24M" "28M" "32M" "36M" "40M" "44M" "48M" "52M" "56M" "60M" "64M" "68M" "72M" "76M" "80M" "128M" "256M" "512M")

if [ "x${RUNVALID_PREFIX}" != "x" ]
then
	PREFIX=${RUNVALID_PREFIX}
else
	PREFIX=../src
fi

# len=${#cachesize[@]}

for i in $(seq 0 $((${#cachesize[@]}-1)))
do
    PARV_FILE=${SSDONTAPE}-${cachesize[$i]}${SUFFIX_PARV}
    OUTV_FILE=${SSDONTAPE}-${cachesize[$i]}${SUFFIX_OUTV}
	echo ""
	echo "ssdonhdd cache: "${cachesize[$i]}
	${PREFIX}/disksim ${PARV_FILE} ${OUTV_FILE} ${VALIDATE} ${TRACE_FILE} ${SYNTHGEN}
	grep "${GREP_AVG_TIME}" ${OUTV_FILE}
	grep "${GREP_DELAY_FLUC}" ${OUTV_FILE}
    # echo $i
done
