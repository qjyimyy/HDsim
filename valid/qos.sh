#!/bin/bash
for((i=1;i<=128;i=i*2))
do
    other=2 
    value= expr $i \* $other;
    echo ${value};
    rm -rf driver_response.txt;
    # echo "ssd-cache-"`expr ${i} \* ${other}`"M.parv"
    ./cachesize-qos "ssd-cache.parv"  "ssd-cache-"`expr ${i} \* ${other}`"M.parv"  `expr $i \* $other`  "512";

    ../src/disksim "ssd-cache-"`expr ${i} \* ${other}`".parv"  "ssd-cache-"`expr ${i} \* ${other}`".outv" validate $1 0;

    # cp "ssd-cache-"`expr $i \* $other`".outv" qos-data/;
    # cp driver_response.txt qos/`expr $i \* $other`".txt"; 
    # ./comp real.txt  driver_response.txt;
done
