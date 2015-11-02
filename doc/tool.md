MDT-TOOL
====
#简介
  mdt-tool用于trace系统的管理、查询

#特性
  mdt将日志数据抽象为一个类SQL的简化表格模型，拥有database、table、colume等概念。
 * database表示业务的产品线，用于不同产品线之间的隔离以及quota管理，例如在线音乐、线上购物属于不同的产品线。
 * table表示一个产品线内的不同系统，便于隔离不同系统间的日志数据，例如线上购物的卖家系统和买家系统可能是两个独立的系统。
 * column表示可被检索的日志字段，例如user_id、query_id等等。
  mdt支持常见的数据类型，如字符串、各种字长的整型等，方便用户构造检索条件。对于整型，支持大于、小于等比较语义。例如检索处理时间在5-6ms之间的trace，可以用cost_time>=5 and cost_time<=6表达。
  mdt支持组合条件检索。例如检索用户id是888且商品id是123的trace，可以用user_id=888 and product_id=123表达。
  
#用法
```
./mdt
>> help
<< open database <db_name>
<< open table <table_name>
<< create table <table_name> {index_name index_type}
<< get [start_timestamp] [end_timestamp] [limit] {index_name comparator(=, >=, >, <=, <) index_value}
```
 * open database：打开产品线，不存在时会创建
 * open table：打开产品线下的某系统
 * create table：创建表格，设置用于检索的日志字段和类型
 * get：检索日志。start_timestamp和end_timestamp限定日志时间，limit限定结果数量，检索表达式由多个检索条件组成，用空格分开，语义为“且”

#示例

创建表格

```
./mdt
>> open database my_db
<< current database is "my_db"

>> create table my_table query bytes qid uint64 user uint64
<< table "my_table" created
```

查询2015年10月15日14~15点之间，query为“apple”的日志，最多100条：

```
./mdt
>> open database my_db
<< current database is "my_db"

>> open table my_table
<< current table is "my_table"

>> get 20151015-140000 20151015-150000 100 query="apple"
<< 3 results match your query:

<< I1015 14:35:50.220968 9226 main.cc:168] MDT trace_id=110000000000012A span_id=80000000000099BF parent_id=700000000000CCBF span_name=search cost_time=3 qid=2200000000003380 user=660000000000A322 query=apple mem_used 5.07M net_rx 9K io_r 20K
<< I1015 14:35:50.221238 9226 main.cc:168] MDT trace_id=1100000000005901 span_id=800000000000EBDD parent_id=7000000000003C81 span_name=search cost_time=3 qid=22000000000011EC user=660000000000C0CA query=apple mem_used 4.33M net_rx 8K io_r 7K
<< I1015 14:35:50.221508 9226 main.cc:168] MDT trace_id=11000000000079DE span_id=8000000000005E0D parent_id=7000000000009AAB span_name=search cost_time=3 qid=220000000000034F user=660000000000B229 query=apple mem_used 0.66M net_rx 5K io_r 3K
```

查询2015年10月15日14点之后，trace_id为110000000000A400，且RPC名称为“search”的日志，最多100条：

```
>> get 20151015-140000 100 trace_id=110000000000A400  span_name=search
<< 1 results match your query:

<< I1015 14:35:50.221328 9226 main.cc:168] MDT trace_id=110000000000A400 span_id=8000000000000112 parent_id=7000000000005901 span_name=search cost_time=2 qid=220000000000FF90 user=6600000000007000 query=zip
```

查询service_name为nginx，且cost_time大于3的日志，最多1条

```
>> get 1 service_name=nginx cost_time>3
<< 1 results match your query:

<< I1015 14:35:50.221418 9226 main.cc:168] MDT trace_id=1100000000003128 span_id=8000000000002760 parent_id=7000000000007BDE span_name=search cost_time=4 qid=220000000000FE12 user=660000000000EE20 query=casio mem_used 6.76M net_rx 44K io_r 65K
```
