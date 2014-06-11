#ifndef _DEPENDENCIES_H_
#define _DEPENDENCIES_H_
// OpenGL extension and framework
#include "gl/glew.h"
#include "gl/freeglut.h"

/// Mathematics Library for matrix manipulation
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

// texture library
#include "SOIL.h"
#pragma comment(lib, "soil")

// UI library
#include "AntTweakBar.h"

#ifdef __cplusplus
extern "C" {
#endif
      #include "lua\lua.h"
	#include "lua\lauxlib.h"
	#include "lua\lualib.h"
 #ifdef __cplusplus
 }
 #endif

#pragma comment(lib,"lua")


using namespace glm;
#endif