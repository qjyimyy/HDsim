'''
该 python 脚本按照指数分布修改 hm_o_valitate.trace 的时间戳
每两个 trace 之间的间隔时间满足指数分布
在 trace 字段中，时间间隔字段为第六列（最后一列）

运行方式: python generate_exp_timestamp.py
'''
import numpy as np

# 平均到达率
lambda_value = 100
# 请求总个数
# num_request = 50000

with open('hm_0_validate.trace', 'r') as file:
    lines = file.readlines()

# trace 请求总个数
num_request = len(lines)

timestamp = []
time = 0

for i in range(0, num_request):
    line = lines[i].split(' ')
    timestamp.append(line[5])

np.set_printoptions(suppress=True)

# 生成指数分布的时间间隔
time_intervals = np.random.exponential(scale=1/lambda_value, size=num_request - 1)

for i, interval in enumerate(time_intervals):
    # time += interval
    timestamp[i + 1] = interval

with open('hm_0_exp_validate.trace', 'w') as file:
    for i in range(0, num_request):
        line = lines[i].split(' ')
        file.write(line[0] + ' ' + line[1] + ' ' + line[2] + ' ' + line[3] + ' ' + line[4] + ' ' + str(timestamp[i]) + '\n')

