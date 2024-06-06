cmake_minimum_required(VERSION 3.23)
project(FaultInjectorSimGrid)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) 

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(MLPACK REQUIRED)
find_package(SimGrid 3.35 REQUIRED)
find_package(Armadillo REQUIRED)
find_package(BLAS REQUIRED)
find_package(LAPACK REQUIRED)

include_directories(${SimGrid_INCLUDE_DIR})
include_directories(${MLPACK_INCLUDE_DIR})
include_directories("/usr/local/include")
link_directories("/usr/local/lib")

set(PROJECT_SOURCES
        main.cpp

        FaultInjector/FaultInjector.h FaultInjector/FaultInjector.cpp
        DeployManager/DeployManager.h DeployManager/DeployManager.cpp
        Entity/Worker.h Entity/Worker.cpp
        Entity/Task.h Entity/Task.cpp
        Entity/Job.h Entity/Job.cpp
        Controller/Controller.h Controller/Controller.cpp
        GraphGenerator/GraphGenerator.h GraphGenerator/GraphGenerator.cpp
        Utils/json.hpp
        Utils/jngen.hpp
)

add_executable(FaultInjectorSimGrid ${PROJECT_SOURCES})
target_link_libraries(FaultInjectorSimGrid ${SimGrid_LIBRARY} ${MLPACK_LIBRARY} ${ARMADILLO_LIBRARIES} ${BLAS_LIBRARIES} ${LAPACK_LIBRARIES} )