进程内存操作工具 v0.1

v 为已经实现
x 为未实现

命令:
v info	读取 /proc/<pid>/maps 并格式化打印
v reload	重新读取进程的内存区域到缓存区
v dump	将缓存区的内存区域数据写入磁盘
v clean	清理缓存区
v exit	退出
x write	<address> <data>	往进程的内存<address>写入<data>
x read	<address> <length>	读取内存区域，使用16进制格式打印
x search	<address> <length> <data>	在内存区域中搜索内容

开发计划:

x 内置 lisp 解释器用于编写脚本
