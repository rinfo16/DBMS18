<img src="doc/owl.jpg" height=160></img>
[![Build Status](https://travis-ci.org/rinfo16/owl.svg?branch=master)](https://travis-ci.org/rinfo16/owl)

# Welcome to Owl

A database POC
 
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
- [x] Build Execution Tree


##### __Executor__  
- [x] Scan
- [ ] Select
- [x] Porject
- [x] Nest Loop Join
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
#### Support SQL: 
> CREATE TABLE table_name ( column_define_list );  
> SELECT column_name_list FROM table_name;  
> COPY table_name ( column_name_list) FROM ‘csv_file_path’;  

#### Support data type: 
> INTEGER, VARCHAR，FLOAT

#### Example:
Backend start 
> nutshell --port=8432

Connect to the backend use psql 
> psql -h localhost -p 8432 -d postgres

Creat table product 
> create table product (id INTEGER, price INTEGER, name VARCHAR(256));

Load from cvs file 
> copy product from '/tmp/data.csv';

SELECT query 
> select name, id, price from product;
