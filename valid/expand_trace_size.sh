#########################################################
# 该脚本功能：
# 扩大 trace 所表示的数据量的大小，将其扩大到指定量级
# 用法: bash expand_trace_size.sh [file] [size]
# size 为具体量级大小，如 500GB，1TB，2TB
# 例如: bash expand_trace_size.sh hm_0_validate.sh 2TB
#########################################################

awk '
BEGIN {
    # block_size[0] = 536
    sign[0] = -1;
    sign[1] = 1;
    avg = 537;
    srand();
}
{
    size = avg + int(536 * rand()) * sign[int(2 * rand())]
    printf("%s %s %s %d %s %s\n", $1, $2, $3, size, $5, $6);
    # print size
}
END {

}
' hm_0_validate.trace > hm_0_expand_validate.trace