cmake_minimum_required(VERSION 3.13)
project(2opt_RL)

add_compile_options(-D IL_STD -std=c++17)

set(CPLEX_DIR "/Applications/CPLEX_Studio1210")
set(dir_cplex "${CPLEX_DIR}/cplex/")
set(dir_cpoptimizer "${CPLEX_DIR}/cpoptimizer/")
set(dir_concert "${CPLEX_DIR}/concert/")

set(CPLEX_DIR2 "/Applications/CPLEX_Studio201")
set(dir_cplex2 "${CPLEX_DIR2}/cplex/")
set(dir_cpoptimizer2 "${CPLEX_DIR2}/cpoptimizer/")
set(dir_concert2 "${CPLEX_DIR2}/concert/")

find_library(
  LCPLEX
  NAMES libcplex.a 
  HINTS 
    ${dir_cplex}/lib/x86-64_osx/static_pic/
    ${dir_cplex2}/lib/x86-64_osx/static_pic/
  REQUIRED
)

find_library(
  LCPLEXDISTMIP
  NAMES libcplexdistmip.a
  HINTS 
    ${dir_cplex}/lib/x86-64_osx/static_pic
    ${dir_cplex2}/lib/x86-64_osx/static_pic
  REQUIRED
)

find_library(
  LILOCPLEX
  NAMES libilocplex.a
  HINTS 
    ${dir_cplex}/lib/x86-64_osx/static_pic
    ${dir_cplex2}/lib/x86-64_osx/static_pic
  REQUIRED
)

find_library(
  LCONCERT 
  NAMES libconcert.a
  HINTS 
    ${dir_concert}/lib/x86-64_osx/static_pic
    ${dir_concert2}/lib/x86-64_osx/static_pic
  REQUIRED
)

find_library(
  LCP
  NAMES libcp.a
  HINTS 
    ${dir_cpoptimizer}/lib/x86-64_osx/static_pic
    ${dir_cpoptimizer2}/lib/x86-64_osx/static_pic
  REQUIRED
)

add_executable(${PROJECT_NAME} 
  ./src/main.cpp 
  ./src/model/Graph.cpp 
  ./src/model/Node.cpp 
  ./src/model/Distance.cpp
  ./src/model/Tour.cpp
  ./src/model/Arguments.cpp
  ./src/RLmodel/LinearFittedQIteration.cpp
  ./src/helper/FileIOHelper.cpp
  ./src/helper/mt19937ar.cpp
  ./src/helper/RandVec.cpp
  ./src/helper/fitLP.cpp
  ./src/helper/Timer.cpp
  ./src/solver/initial_solution/GenerateInitialSolution.cpp
  ./src/solver/initial_solution/method/RandomTour.cpp
  ./src/solver/initial_solution/method/NearestNeighbor.cpp
  ./src/solver/initial_solution/method/FarthestInsertion.cpp
  ./src/solver/local_search/SearchLocalOpt.cpp
  ./src/solver/local_search/TestLocalOpt.cpp
  ./src/solver/local_search/method/TwoOpt.cpp
  ./src/solver/local_search/method/TwoOptOrdered.cpp
  ./src/solver/local_search/method/FastTwoOpt.cpp
)

target_include_directories(${PROJECT_NAME} 
PUBLIC
  ${dir_cplex}/include 
  ${dir_cpoptimizer}/include
  ${dir_concert}/include
  ${dir_cplex2}/include 
  ${dir_cpoptimizer2}/include
  ${dir_concert2}/include
)

target_link_libraries(${PROJECT_NAME}
  ${LCPLEX}
  ${LCPLEXDISTMIP}
  ${LILOCPLEX}
  ${LCONCERT}
  ${LCP}
)