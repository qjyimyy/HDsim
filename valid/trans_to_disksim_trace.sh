#!/bin/bash

# 该脚本作用是将微软原始 trace 格式转化为 disksim 格式的 trace
# 运行方式：./trans_to_disksim_trace.sh {raw_trace_name} > {disksim_trace_name}

# 原始 trace 格式有 7 列，含义分别如下：
# Col 1: 时间戳(timestamp)
# Col 2: 主机名(hostname)
# Col 3: 设备名称(devname)
# Col 4: 读写(rw)
# Col 5: 偏移量(offset)
# Col 6: 长度(length)
# Col 7: 响应时间(responsetime)

# disksim 格式的 trace 各列含义如下：
# Col 1: 读写(RW)
# Col 2: Hit(暂时固定为 Hit)
# Col 3: 偏移量(offset)
# Col 4: 长度(length，单位：块，即 512B)
# Col 5: 服务时间(servtime，即完成该次请求的总时间)
# Col 6: 时间戳(源码中的字段名为 nextinter)

TRACE_LIST=$@

if [ -n "$TRACE_LIST" ]; then
    TRACE_FILE+=${TRACE_LIST[0]}
else
    echo "Please input filename..."
    exit
fi

awk -F ',' '
BEGIN {
    rw[0] = "R";
    rw[1] = "W";
    servtime = 0.0;
    nextinter = 0.0;
}
{
    offset = $5;
    len = $6;
    if ($4 == "Write") {
        rw_flag = 1;
    } else if ($4 == "Read") {
        rw_flag = 0;
    }
    printf("%s Hit %s %s %f %f\n", rw[rw_flag], offset, len, servtime, nextinter);
}
' ${TRACE_FILE}