# 安装
1. 安装Eigen
      只需要移植文件即可 
      当前Eigen已经记录到git中,可以直接clone,后面需要修改CMakeLists.txt中的路径
2. 安装MKL库
      2.00 下载离线版的安装包,最后几个字符为offline.sh
      2.0 特别注意,安装mkl库需要有图形界面,最好在code终端中试安装
      2.1 sh 执行安装
      2.2 按照操作执行
      2.3 在~/.bashrc 中增加 source /path/to/intel/oneapi/setvars.sh
      2.4 执行 source ~/.bashrc
      2.5 执行 echo $MKLROOT, 查询是否正确安装MKL库
3. 修改CMakeLists.txt
4. git push origin HEAD username: Ma_2023 password: mjw091212
<br><br>

# 设计理念:
1. 关于input格式   
   参考knowledeg中的input模板

   
2. mesh类
   mesh类的设计支持多part,支持节点编号从非0开始,为了实现这一目的,设计了如下的成员变量
   mesh中的单元和节点维护两套编号：对接输入的称为全局编号，用于计算的称为局部编号

3. 约束中的重复约束
   约束基类中定义了unordered_set m_set: 参考了NSC手册
   在构造约束时,每个约束的自由度都会跟已经施加过约束的自由度进行搜索,
   如果已经施加过约束,则跳过该约束




新增单元：
1. 单元头文件和实现文件
2. 单元接口上注意引用
2. 注意后处理处需要将单元写入进去
3. 单元组装的时候需要知道单元类型



新增材料：
1. 材料头文件和实现文件：
    1.1 takeDB 读材料参数
    1.2 updateStress 更新应力
    1.3 getDSDu 更新应力增量

2. 材料接口h文件上引用新材料
3. 在单元基类中派生该材料












C++编程常用知识:
基类的函数需要定义为虚函数,或纯虚函数
派生子类的构造函数需要先调用基类的派生函数
区分重载和多态

构造函数的调用逻辑 先调用基类的构造函数,再调用子类的构造函数
析构函数的调用逻辑 先调用子类的析构函数 再调用基类的析构函数



useful head file
#include <Eigen/Sparse>
#include <Eigen/PardisoSupport>
#include <Eigen/Dense>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <omp.h>
#include <chrono>



注意一点很重要的CMake概念:
1. 有时候提示#include不到某个头文件时,是因为这个库不仅需要被包含在当前目录下的库中,还要在它所有的上级路径中都要包含
2. 举一个例子,我希望在element这个目录的代码中包含material目录下的头文件,但它老是提示找不到该目录或文件,这是因为还得在element的上层调用关系中包含这个materiallib库,如最重要的main函数同级目录下的CMakeLists.txt和solver目录下的CMakeLists.txt中也要包含这个库





并行调试可以学习一下gdb
openmp并行中有个很常见的问题：内存竞争
类的方法可以并行，但类的数据最好不要通过openmp并行，尤其是累加操作
openmp会访问同一片地址，造成数据冲突
