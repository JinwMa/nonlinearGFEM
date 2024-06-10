1. 安装Eigen
      只需要移植文件即可 
      当前Eigen已经记录到git中,可以直接clone,后面需要修改CMakeLists.txt中的路径
2. 安装MKL库
      2.0 特别注意，安装mkl库需要有图形界面，最好在code终端中试安装
      2.1 sh 执行安装
      2.2 按照操作执行
      2.3 在~/.bashrc 中增加 source /path/to/intel/oneapi/setvars.sh
      2.4 执行 source ~/.bashrc
      2.5 执行 echo $MKLROOT, 查询是否正确安装MKL库
3. 修改CMakeLists.txt
4. git push origin HEAD username: Ma_2032 password: mjw091212