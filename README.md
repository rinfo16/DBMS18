# __nutshell__

一个数据库POC
 
### __Roadmap__
##### __Common__  
- [ ] Common Utils/ADTs
- [ ] Memory Manager

##### __RRPortal__  
- [x] Start Up Initial
- [x] Session Manager
- [x] C&S Protocol, support PostgreSQL protocol

##### __Parser__  
- [x] Lex && Yacc
- [x] AST


##### __Realizer__  
- [ ] Semantic Check
- [ ] Optimizer RBO/CBO
- [ ] Build Execution Tree


##### __Executor__  
- [x] Scan
- [ ] Porject
- [ ] Nest Loop Join
- [ ] Hash Join
- [ ] Merge Join
- [ ] Sort
- [ ] Hash
- [ ] Aggregate
- [ ] Union
- [ ] Union All
- [ ] Intersect
- [ ] Except
- [ ] Sub Query 
- [ ] Expression Calculate

##### __Storage__  
- [x] Space Manager
- [x] Buffer Manager
- [x] Meta Data Manager
- [ ] Lock Manager
- [ ] Index Access
    - [ ] B+ Tree
    - [ ] Linear Hash
    - [ ] Bitmap
- [ ] Transaction
	- [ ] WAL


##### __Client__  
- [ ] UI
- [ ] Tools

### Start Up  
#### 支持的语句:
> CREATE TABLE table_name ( column_define_list );  
> SELECT column_name_list FROM table_name;  
> LOAD table_name ( column_name_list) FROM ‘csv_file_path’;  

#### 支持的数据类型：
> INTEGER, VARCHAR，FLOAT

#### 例:
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
