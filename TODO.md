
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
可以参考开源代码中的实现，如nginx, mysql, postgres, linux kernel

##### 4.list
双链表，支持从左端或右端链接，移除，根据情况扩展接口

##### 5.singleton
实现单体模式，不需考虑多线程竞争条件(在更高层次上处理)  


### Deadline 2016.11.13 实验2完成  
####1. 实现B+树索引  
####2. Parser Interface SELECT/INSERT/DELETE/UPDATE/CREATE ...
####3. Client && Server Protocal
####4. Executer Interface

实现B+树时可参考的代码   
[Tokyo Cabinet] (http://fallabs.com/tokyocabinet/)  
[SQLite] (http://sqlite.org/version3.html)  
[BerkeleyDB] (http://www.oracle.com/technetwork/database/database-technologies/berkeleydb/downloads/index.html)  
[MySQL InnoDB] (http://www.mysql.com/)  
[PostgreSQL] (https://www.postgresql.org/)  


