#pragma once
#include "platform.h"

#ifdef HIVE_ANDROID
#include <GLES3/gl3.h>
#elif defined HIVE_UNIT_TEST
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#endif