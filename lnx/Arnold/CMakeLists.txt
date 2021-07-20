cmake_minimum_required(VERSION 2.8)

project(skelTreeForArnold) # 设置项目名

set(ARNOLD_LOCATION /l/packages/pg/third_party/app/arnold/6.2.0.1/platform-linux)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "/data/asunlab/outsource/maya_devkit/devkitBase/cmake/modules")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY /data/asunlab/github/paramtree/src/bin)

include(/l/packages/pg/third_party/app/openexr/2.5.3/lib64/cmake/IlmBase/IlmBaseConfig.cmake)

set(SROUCEPATH /data/asunlab/github/paramtree/src/Arnold/src) 
set(EXRPATH /l/packages/pg/third_party/app/openexr/2.5.3)
set(COREPATH /data/asunlab/github/paramtree/src/Core)

include_directories(${SROUCEPATH} ${COREPATH} ${EXRPATH}/include ${ARNOLD_LOCATION}/include)

link_directories(${EXRPATH}/lib64)
link_libraries(Iex IexMath Imath)

aux_source_directory(${SROUCEPATH} SOURCE_FILES) 
aux_source_directory(${COREPATH} SOURCE_FILES)

find_package(Arnold)

add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${Arnold})