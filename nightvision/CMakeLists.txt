cmake_minimum_required (VERSION 2.6)
set (AppName nightvision)

project (${AppName})

set (SHADER_DIR "${Path}/shader")
set (GL_SHADER_GTC "${AppName}.vert" "${AppName}.frag")

foreach(FILE ${GL_SHADER_GTC})
		set(SHADER_PATH ${SHADER_PATH} ${SHADER_DIR}/${FILE})
	endforeach(FILE)

source_group("Shader Files" FILES ${SHADER_PATH})

add_executable(${AppName} "${AppName}.cpp" ${SHADER_PATH})
target_link_libraries(${AppName} Shader)
target_link_libraries(${AppName} Model)

##INSTALL(DIRECTORY data DESTINATION ${PROJECT_BINARY_DIR})
#INSTALL(DIRECTORY shader DESTINATION ${PROJECT_BINARY_DIR})
#INSTALL(FILES libnoise.dll DESTINATION ${PROJECT_BINARY_DIR})
