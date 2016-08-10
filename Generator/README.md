#输入样例生成器

##注意
**编译项目前请先确保以下几点：**
>1. 编译器需支持c++11标准
>2. windows下，若使用Visual Studio，请使用2010以上版本，2010不清楚；若是dev c++，请使用5.6以后的版本；
>3. linux/unix下，若使用g++，请保证是g++ 4.8之后的版本，其余编译器(如clang)请参考官网提供的信息
>4. vc++6.0不支持，已经淘汰了
>5. vc++6.0不支持，已经淘汰了
>6. vc++6.0不支持，已经淘汰了
>7. 若想输出部分解析信息，可将config.h文件中的\_\_TEST\_\_置为0以外的值
>8. 创建文件夹在Generator构造函数中，向文件添加字符串位于fileAppend()函数中

**使用时注意点**
>1. 生成的输入样例不一定符合实际需求
>2. 生成的输入样例不一定符合实际需求
>3. 生成的输入样例不一定符合实际需求（具体产见样例版块）
>4. 没有去重机制，部分东西无法生成

##编译
windows: 
```
#未提供项目结构，比如vs中的sln文件
#建立项目前，首先使用提供的bat文件，转换文件编码和换行符
#使用ide建立项目后，无特殊要求，仅需将头文件(.h)放入头文件夹，源文件(.cpp)放入源文件夹即可
```
linux/unix: 
```
#已提供Makefile，依赖g++
make
./generator 参数1 参数2 ...
```

##支持
支持少部分基础正则表达式语法，仅支持两层圆括号"("嵌套，不能使用\*?.^$+等特殊符号<br>
表达式结束标识：end<br>
程序参数支持：<br>
>1. -h/--help 输出帮助信息，程序中断
>2. -times    =n，产生文件的数目
>3. -exp      =exp1 -link exp2 -link exp3 ... 若不考虑...，则表示连接前三行表达式，简易使用，之后程序直接解析，不再提供输入
>4. -dir      =dirname **未编写**
>4. -input    =num 起始文件号**未编写**

行表达式参数（处理优先级，依次降低）：<br>
>0. lxcy        并非参数，仅为参数值的一种形式，表示使用第x(1~n)行y(1~n)列的数值（本待参数值的检测并不完备，请注意使用）
>1. -numcolumn  =n/lxcy 产成n列该行表达式所对应的字串（自动添加空格分割）
>2. -numline    =n/lxcy 产生n列该行表达式所对应的字串
>3. -rectangle  ={line,step,n/lxcy} 表示从第line(1~n)行开始的step(1~n)行，重复n次


##样例
ip地址
```
##正则表达式，随便搜的，[0-1]{1}[0-9]{2}这里会包含前导0
((25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]).){3}(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])
```
![ip地址](https://raw.githubusercontent.com/Alrash/OnlineJudge/master/data/images/Part02-Generator_example.png)
<br><p align="center">ip地址多行生成</p>

生成矩阵:<br>
![矩阵](https://github.com/Alrash/OnlineJudge/blob/master/data/images/Part02-Generator_example02.png)
<br><p align="center">重复产生矩阵</p>

##运行原理
[数据结构](https://github.com/Alrash/OnlineJudge/wiki/Part-of-Generator#%E5%8E%9F%E7%90%86%E8%AF%B4%E6%98%8E)
