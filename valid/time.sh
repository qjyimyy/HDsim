#!/bin/bash

start=$(date +%s)

echo Start
../src/disksim ssd-cache.parv hm_trace.outv validate hm_0_exp_validate.trace 0

end=$(date +%s)
take=$((end-start))
echo time taken is ${take} seconds.
