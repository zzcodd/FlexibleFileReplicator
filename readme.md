###功能：
对于服务器上的录像文件，根据时间范围进行拷贝；基于rsync
对日志进行拷贝；
支持自定义拷贝；

demo中：
源地址：/tmp/record
目标地址： /tmp/record_dest

std::string dfString;
// 输出 ： 挂载点 总大小 可用大小
df | grep /tmp/record_dest | grep -v loop | awk '{print $6 "|" $2 "|" $4}'


###date的格式
20230830210000000|20230830215000000