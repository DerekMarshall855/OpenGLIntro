#include "Renderer.h"
#include <iostream>

void GLClearError()
{
    // glDebugMessageCallback > glGetError for most debugging (v 4.3 and up only)
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << ")" <<
            function << " " << file << " : " << line << std::endl;
        return false;
    }
    return true;
}