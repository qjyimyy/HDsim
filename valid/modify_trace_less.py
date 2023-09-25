'''
Author      : Cherry
Date        : 2023-09-15 13:27:01
FilePath    : /disksimfifo_cache/valid/modify_trace_less.py
Description : 该脚本作用是修改 trace 文件中的记录个数，为了验证是否记录个数
            会影响产生的事件个数以及模拟器运行时间。保证 trace 表示的数据
            量大小不变，仅修改记录条数和每次请求的大小
'''

index = 0
file = open('hm_0_validate.trace', 'r')
num_request = 1048576
lines = []

for line in file.readlines():
    index += 1
    line = line.replace("\n", "")
    lines.append(line)
    # 1M 条记录(1024 * 1024)
    if index == num_request:
        break
    
file.close()

with open('hm_0_expand_1M_validate.trace', 'w') as file:
    for i in range(0, num_request):
        line = lines[i].split(' ')
        file.write(line[0] + ' ' + line[1] + ' ' + line[2] + ' ' + "2048" + ' ' + line[4] + ' ' + line[5] + '\n')

