cmake_minimum_required(VERSION 2.8)

project(skelTree) # 设置项目名

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY /data/asunlab/github/paramtree/src/bin)

set(ENV{DEVKIT_LOCATION} /data/asunlab/outsource/maya_devkit/devkitBase) # Maya开发包路径ENV变量

include($ENV{DEVKIT_LOCATION}/cmake/pluginEntry.cmake) # 包含一个plugin编译模板

# set(IlmBase_DIR "/l/packages/pg/third_party/app/openexr/2.5.3/lib64/cmake/IlmBase")

# find_package(IlmBase REQUIRED)

set(PROJ_PATH "/data/asunlab/github/paramtree/src/MayaNodes") # 工程路径
set(SROUCEPATH ${PROJ_PATH}/src) # 源代码路径
set(EXRPATH /l/packages/pg/third_party/app/openexr/2.5.3)
include_directories(${SROUCEPATH} ${EXRPATH}/include)

link_directories(${EXRPATH}/lib64)

link_libraries(Iex IexMath Imath)

aux_source_directory(${SROUCEPATH} SOURCE_FILES) # 这个命令可以把目录里的所有源代码列出来，放进SOURCE_FILES里

set(LIBRARIES
OpenMaya Foundation OpenMayaUI OpenMayaRender
) # 包含LIB

build_plugin() # 编译function