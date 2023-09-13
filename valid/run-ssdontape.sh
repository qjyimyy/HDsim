#!/bin/bash

# Run ssd on hdd simulation

# filename
SSDONTAPE="ssdontape"
# tracefile
TRACE_FILE="yuan-validate.trace"
VALIDATE="validate"
SYNTHGEN="0"
SUFFIX_PARV=".parv"
SUFFIX_OUTV=".outv"
GREP_AVG_TIME="IOdriver Response time average"
GREP_DELAY_FLUC="IOdriver Response time Delay Fluctuation"

cachesize=("2M" "4M" "8M" "16M" "32M" "64M" "128M" "256M" "512M" "1G" "2G" "4G")


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
	echo "ssdontape cache: "${cachesize[$i]}
	${PREFIX}/disksim ${PARV_FILE} ${OUTV_FILE} ${VALIDATE} ${TRACE_FILE} ${SYNTHGEN}\
	&& grep "${GREP_AVG_TIME}" ${OUTV_FILE}
	grep "${GREP_DELAY_FLUC}" ${OUTV_FILE}
    # echo $i
done
