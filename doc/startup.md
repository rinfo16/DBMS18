支持的语句:
> CREATE TABLE table_name ( column_define_list );  
> SELECT column_name_list FROM table_name;  
> LOAD table_name ( column_name_list) FROM ‘csv_file_path’;  

支持的数据类型：
> INTEGER, VARCHAR，FLOAT

例:
启动服务端:
> nutshell --port=8432

使用psql连接到服务端：
> psql -h localhost -p 8432 -d postgres

创建表product
> create table product (id INTEGER, price INTEGER, name VARCHAR(256));

导入数据：
> copy product from '/tmp/data.csv';

SELECT查询：
> select name, id, price from product;
