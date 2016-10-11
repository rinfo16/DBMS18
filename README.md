# __Nutshell__

一个数据库POC ...

### __Roadmap__
##### __Common__  
- [ ] Commont Utils/ADTs

##### __RRPortal__  
- [ ] Start Up Initial
- [ ] Session Manager
- [ ] C&S Protocol

##### __Parser__  
- [ ] Lex && Yacc
- [ ] AST


##### __Realizer__  
- [ ] Semantic Check
- [ ] Optimizer RBO/CBO
- [ ] Build Execute Tree


##### __Executor__  
- [ ] Scan
- [ ] Sorter 
- [ ] Aggregation
- [ ] Merge 
- [ ] Hash
- [ ] Join


##### __Storage__  
- [ ] Space Manager
- [ ] Buffer Manager
- [ ] Meta Data Manager
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

### __TODO__

### Deadline 2016.10.23 实验1完成  

### 部署开发环境;  

### 熟悉技术栈:  
Linux/C++/git/数据结构/数据库实现;  

### 开发任务:   

#### Build Tool  

#### Unit Test  

#### Storage  
空间及缓冲区管理，顺序访问接口(Read/Write)    

#### Common  
公共utils/ADTs，内存管理(可选)  

##### 1.bitmap
用于管理存储空间,位图索引  
可以参考的开源代码:  
PostgreSQL, arrayfuncs.c, bitmapset struct  
linux kernel, bitmap.c  

##### 2.clock
计时器类，提供ms级别计时  
Start开始计时，Stop停止计时，可以反复调用Start/Stop，计累计时间  
支持格式化输出

##### 3.hash
hash函数实现，寻找几个不同的Hash实现方式  
可以参考开源代码中的实现，如nginx, mysql,postgres,linux kernel

##### 4.list
双链表，支持从左端或右端链接，移除，根据情况扩展接口

##### 5.singleton
实现单体模式，不需考虑多线程竞争条件(在更高层次上处理)  

### __编码风格__


参考[Google Style Guide](http://zh-google-styleguide.readthedocs.io/en/latest/google-cpp-styleguide/)
 
