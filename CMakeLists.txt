cmake_minimum_required(VERSION 3.8)

PROJECT(dmsnowflake)

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
INCLUDE(cmake/ModuleImport.cmake)
INCLUDE(cmake/ModuleCompileOptions.cmake)
ModuleSetCompileOptions()

ModuleImport("dmtest" "thirdparty/dmtest")

InterfaceImport("dmsnowflake" "include" "")

if (PROJECT_IS_TOP_LEVEL)
    ExeImport("test" "dmsnowflake;dmtest")
endif(PROJECT_IS_TOP_LEVEL)
