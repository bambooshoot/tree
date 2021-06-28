cmake_minimum_required(VERSION 2.8)

project(skelTree) # 设置项目名

set(ENV{DEVKIT_LOCATION} D:/asunlab/maya2022_sdk) # Maya开发包路径ENV变量

include($ENV{DEVKIT_LOCATION}/cmake/pluginEntry.cmake) # 包含一个plugin编译模板

set(PROJ_PATH "D:/asunlab/github/tree/MayaNodes") # 工程路径
set(SROUCEPATH ${PROJ_PATH}/src) # 源代码路径

aux_source_directory(${SROUCEPATH} SOURCE_FILES) # 这个命令可以把目录里的所有源代码列出来，放进SOURCE_FILES里

set(LIBRARIES
OpenMaya Foundation
) # 包含LIB

build_plugin() # 编译function