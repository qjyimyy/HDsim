# HMsim

## 介绍
存储模拟器Hybridsim的源码，包括一些运行、计算结果的脚本；Hybridsim目前可用于模拟SSD、HDD、tape、SSD和HDD异构、SSD和tape异构等情况。

## 配置环境与安装
配置环境
```markdown
gcc >= 11.2.1
GLIBCXX >= 3.4.29
```
下载离线包后，解压文件，然后为一些文件添加权限：
```shell
chmod +x ./libparam/*.pl
```
删除一些路径文件：
```shell
rm -f ./diskmodel/*.d
rm -f ./ssdmodel/*.d
```
编译：
```shell
make
```
编译完成后会在```/src```文件夹中生成```disksim```可执行程序。

## 主要模块介绍
### HDD
对于HDD的模拟，主要会用到```/diskmodel```文件夹下的文件；包括布局模块和机械模块， 布局模块执行 LBN 到 DBN 的转化，机械模块执行 HDD 传输时间估计。
### SSD
对于SSD的模拟，主要会用到```/ssdmodel```文件夹下的文件；主要的函数都在```ssd.c```中，具体计算访问时间的函数定义在```ssd.timing.c```中，其他还包括ssd中关于GC、gang模式（一个gang中多个element）、初始化的函数。
### 模拟全局
对于执行模拟主函数、读取trace等，主要会用到```/src```文件夹下的文件；```disksim_global.h```文件中保存着一些全局的数据结构；主函数中用到的函数，定义在```disksim.c```文件中；读取trace的函数定义在```disksim_iotrace.c```文件中。
### 运行所需文件
运行程序时需要用到配置文件和trace，这些均在```/valid```文件夹中。此外，```/valid```还保存有一些可用的脚本，如执行脚本```run*.sh```、统计用途的脚本```qos.sh, comp.cpp```等。
### 其他
- doc：存放说明文档。 
- include：存放头文件。
- libddbg：debug 相关的代码。
- libparam：存放参数解析的 lex 和 yacc 程序，用于解析各个模块的参数。 
- Makefile：项目编译文件。
- src/disksim_tape.c：对 tape 进行模拟

## 运行说明
运行命令格式：
```shell
../src/disksim xxx.parv xxx.outv traceformat trace 0
```
参数解释：
```markdown
../src/dismsim: 编译源码后生成的可执行文件，文件默认生成在该路径下，名称可以自行修改。
xxx.parv: 模拟所需的配置文件，具体配置信息如何设置参考/doc/disksim4-manual.pdf。
xxx.outv: 模拟的输出文件，里面保存在具体的统计信息。
traceformat: 模拟的trace格式，目前支持acsii、raw、validate、hpl、hpl2、dec、emcsymm、encbackend、
batch、ocssd（进行中）
```

### 运行模式
对于不同的运行模式，主要更改的是对应的配置文件。运行某个模式时，只需要将第二个参数改为对应的配置文件即可，配置文件对应关系如下：
valid文件夹中的ssd.parv、seagate.parv、tape.parv、ssd-cache-xx.parv(xx代表缓存大小)、ssdontape-xx.parv(xx代表缓存大小)分别代表纯ssd、纯hdd、纯tape、ssd和hdd异构、ssd和tape异构的配置文件。
### IO trace和real使用
validate trace格式：
trace 格式为一个六元组。

- 第一列：R 表示读操作，W 表示写操作
- 第二列：固定字段 Hit，无实际含义（不是缓存命中的意思），无需改变
- 第三列：offset，单位为块（扇区，512B）
- 第四列：length，单位为块（扇区，512B）
- 第五列：该请求服务时间，设置为 0 即可
- 第六列：距离下一个请求到达时间间隔，单位为 ms

在运行HMsim结束后，会生成一个driver_response.txt文件，该文件中保存着每个请求的请求延迟、请求opid、请求块号、请求块数、请求读写类型等信息，调用```comp```来进行比较，命令为
```shell
./comp  real.txt  driver_response.txt;
```
其中，real.txt为收集到的真实数据。结果会返回模拟的平均延迟、真实的平均延迟以及二者的偏差。

## 模拟结果输出与统计
模拟结果的所有输出保存在xxx.outv输出文件中，统计时则需要调用相应的脚本来进行计算。
run.sh和hs_run-ssdontape.sh，分别是运行ssd和hdd异构、ssd和tape异构并进行统计的脚本。可以通过修改run.sh中对应的配置文件参数，来运行统计非异构的模拟。
脚本头部都有脚本的说明。


