cmake_minimum_required(VERSION 3.8)

PROJECT(dmtest)
INCLUDE(cmake/ModuleImport.cmake)
INCLUDE(cmake/ModuleCompileOptions.cmake)
ModuleSetCompileOptions()

LibImport("dmtest" "src/dmtest")
