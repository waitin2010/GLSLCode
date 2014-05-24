cmake_minimum_required (VERSION 2.6)
set (AppName wave)

project (${AppName})

source_group("Shader Files" FILES ${SHADER_PATH})
add_executable(${AppName} "${AppName}.cpp" ${SHADER_PATH})


##INSTALL(DIRECTORY data DESTINATION ${PROJECT_BINARY_DIR})
#INSTALL(DIRECTORY shader DESTINATION ${PROJECT_BINARY_DIR})
