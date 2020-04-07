cmake_minimum_required(VERSION 3.2)

PROJECT(dmsnowflake)

LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
INCLUDE(cmake/ModuleImport.cmake)
INCLUDE(cmake/ModuleCompileOptions.cmake)
ModuleSetCompileOptions()

ModuleImport("dmtest" "thirdparty/dmtest")
LibImport("dmsnowflake" "src/dmsnowflake")
ExeImport("test" "dmsnowflake;dmtest")

#ADD_CUSTOM_COMMAND(
#TARGET dmsnowflaketest
#PRE_BUILD
#COMMAND ${CMAKE_COMMAND} -E copy_if_different
#${CMAKE_CURRENT_SOURCE_DIR}/config/test.csv
#$<TARGET_FILE_DIR:dmsnowflaketest>
#)

#INSTALL(DIRECTORY include/ DESTINATION include)
#INSTALL(TARGETS dmsnowflake
#        RUNTIME DESTINATION bin
#        LIBRARY DESTINATION lib
#        ARCHIVE DESTINATION lib)

#SET(DMSNOWFLAKE_LIBS "dmsnowflake")
#MARK_AS_ADVANCED(${DMSNOWFLAKE_LIBS})