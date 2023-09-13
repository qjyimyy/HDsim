#########################################################
# 该脚本功能：
# 计算 trace 文件表示的请求数据总大小，支持多个文件作为参数
# 用法：bash trace_size.sh <file1> <file2> ... <filen>
#########################################################
TRACE_LIST=$@

if [ -n "$TRACE_LIST" ]; then
    TRACE_FILE+=${TRACE_LIST[0]}
else
    echo "Please input filename"
    exit
fi

awk '
BEGIN {
    suf[0] = "B";
    suf[1] = "KB";
    suf[2] = "MB";
    suf[3] = "GB";
    suf[4] = "TB";
    suf[5] = "PB";

    # 检测 trace 类型
    # if (NF == 6) {
    #     TYPE = 1
    # }

    cnt = 1;
    printf("Trace file data size: \n");
}
{
    if (FNR == 1) {
        CUR_FILE = FILENAME;
        if (cnt == 1) {
            cnt = 0;
        } else {
            getSize();
        }
        PRE_FILE = CUR_FILE;
    }
    sum += $4;
}
END {
    getSize();
}
# 按照 KB、MB 格式输出，满 512 则用下一个单位表示
function getSize() {
    cnt = 0;
    # 单位为块(512B)
    sum *= 512;
    while (1) {
        if (sum < 512) {
            printf("%-20s: %6.2f%s\n", PRE_FILE, sum, suf[cnt]);
            break;
        }
        sum /= 1024;
        cnt++;
    }
    cnt = 0;
}
' ${TRACE_FILE}
