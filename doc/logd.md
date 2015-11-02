logd
====
Copyright 2015, Baidu, Inc.

#简介
 * logd的作用是从用户程序的日志中采集有效数据。

#特性
logd对日志格式没有严格的规定，给用户提供了尽可能大的灵活度，对于glog或其它日志库产生的日志都能做到很好的兼容。

 * 只采集指定的日志行
 
	用户日志不会被全部收集，只有包含特定标识的行才会被logd收集，标识所在的列叫做“识别列”。通过启动参数--magic_col可以指定识别列的序号。

 * 自定义的日期和时间格式
	
	不同日志库产生的日志的日期和时间格式都不同，以2015年10月15日14时35分为例，GLOG会打印“I1015 14:35:50.220968”，而其他日志库可能打印“2015-10-15 14:35:50”。通过启动参数--date_col和--time_col可以分别指定日期和时间所在的列序号；而--date_format和--time_format可以分别指定日期和时间的格式。对“I1015 14:35:50.220968”来说，--date_format=I%mmdd，--time_format=%HH:MM:SS:NNNNNN；对2015-10-15 14:35:50来说，--date_format=%yyyymmdd，--time_format=%HH:MM:SS

 * 可能用于检索的信息以key-value对的形式打印
	
	以web服务为例，query id、query、user id都是重要信息，可能会用于检索，只需要将它们用如下格式打印出来即可query_id=123 [space] query="abc" [space] user_id=888 [space] … 

 * 内置了trace模型常用关键词
	
	trace模型有如下常用关键词：trace_id用于唯一的标识一个trace；span_id用于标识一次模块间的RPC调用；parent_id是上游RPC调用的ID，据此可构建出一个trace的完整调用树；span_name是指RPC的名字

 * 可以包含raw数据
	
	对于不用于检索的信息，即trace系统不关心的数据，可以以任意格式出现在日志中，只要保证这些列不切割key-value形式的列即可。参数--kv_start_col和--kv_end_col指定key-value列的起始和结束序号。

#示例
以一个web服务为例，每次query会产生多个rpc调用，依次访问ui、nginx、business、database这四个服务。以其中的nginx服务为例，说明日志格式和启动参数。
 * 日志格式说明
```
I1015 14:35:50.220968 9226 main.cc:168] MDT trace_id=110000000000012A span_id=80000000000099BF parent_id=700000000000CCBF span_name=search cost_time=3 qid=2200000000003380 user=660000000000A322 query=apple mem_used 5.07M net_rx 9K io_r 20K
I1015 14:35:50.221058 9226 main.cc:168] MDT trace_id=110000000000E24C span_id=800000000000A679 parent_id=700000000000E099 span_name=search cost_time=2 qid=2200000000004822 user=6600000000008471 query=dog mem_used 6.07M net_rx 23K io_r 22K
I1015 14:35:50.221148 9226 main.cc:168] MDT trace_id=110000000000773D span_id=800000000000C100 parent_id=7000000000000520 span_name=search cost_time=2 qid=2200000000005060 user=6600000000009CA8 query=email mem_used 2.12 M net_rx 2K io_r 15K
I1015 14:35:50.221238 9226 main.cc:168] MDT trace_id=1100000000005901 span_id=800000000000EBDD parent_id=7000000000003C81 span_name=search cost_time=3 qid=22000000000011EC user=660000000000C0CA query=apple mem_used 4.33M net_rx 8K io_r 7K
I1015 14:35:50.221328 9226 main.cc:168] MDT trace_id=110000000000A400 span_id=8000000000000112 parent_id=7000000000005901 span_name=search cost_time=2 qid=220000000000FF90 user=6600000000007000 query=zip mem_used 3.56M net_rx 52K io_r 43K
I1015 14:35:50.221418 9226 main.cc:168] MDT trace_id=1100000000003128 span_id=8000000000002760 parent_id=7000000000007BDE span_name=search cost_time=2 qid=220000000000FE12 user=660000000000EE20 query=casio mem_used 6.76M net_rx 44K io_r 65K
I1015 14:35:50.221508 9226 main.cc:168] MDT trace_id=11000000000079DE span_id=8000000000005E0D parent_id=7000000000009AAB span_name=search cost_time=3 qid=220000000000034F user=660000000000B229 query=apple mem_used 0.66M net_rx 5K io_r 3K
```
 1. trace_id、span_id、parent_id、span_name是内置关键词。trace_id串起一次query请求在各模块间的传递，span_id是ui模块调用nginx模块的rpc id，parent_id是httpd调用ui模块的rpc id，span_name是rpc名字。
 2. 除此之外，还希望用query id、query、user id、cost_time这四个值作为查找这次query的查询条件。因此它们也以key=value的格式输出。
 3. 而mem_userd、net_rx、io_r这三个值只需要在trace查询结果中展示出来，不会以它们作为检索条件。因此它们可以用任意格式输出。

 * 参数说明
对于上面的nginx日志，logd的启动命令如下：
```
./logd --log_path=/home/work/nginx/log/nginx.log --host=10.81.5.113:9412 --service_name=nginx \
       --magic_col=5 --date_col=1 --date_format=I%mmdd --time_col=2 --time_format=%HH:MM:SS:NNNNNN \
       --kv_start_col=6 --kv_end_col=13
```
 1. log_path=/home/work/nginx/log/nginx.log指定日志文件路径。
 2. host=10.81.5.113:9412和service_name=nginx分别指定机器名和服务名，对于一个模块实例来说它们是不变的，因此可以用参数指定而不需要打印在每一行日志里。
 3. magic_col=5指定标识列“MDT”位于第5列。
 4. kv_start_col=6和kv_end_col=13指定key-value所在列的序号范围，即从第6列到第13列。
