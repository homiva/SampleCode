# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.


#idf_component_register(
    #SRCS "main.c"
    #INCLUDE_DIRS "."
    #REQUIRES hcsr04 wifi relay firebase
#)
FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)
idf_component_register(SRCS ${app_sources})
