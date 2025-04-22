#pragma once
#include "platform.h"

#ifdef HIVE_ANDROID
#include <GLES3/gl32.h>
#include <GLES3/gl31.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3ext.h>
#elif defined HIVE_UNIT_TEST
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#endif