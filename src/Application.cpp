#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include <iostream>
#include <glew.h>
#include <GLFW/glfw3.h>
#include <string>

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Vertex Array Object REQUIRED to be explicitly created on Core Profile
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);  // G sync or frame limited based on monitor

    if (glewInit() != GLEW_OK)
        std::cout << "Error!" << std::endl;

    std::cout << glGetString(GL_VERSION) << std::endl;
    // Index buffer to reuse existing vertices's, don't repeat coordinate positions
    {
        float positions[]{
            -0.5f, -0.5f, // 0
             0.5f, -0.5f, // 1
             0.5f,  0.5f, // 2
            -0.5f,  0.5f, // 3
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        /*
         Two common methods for vao:
            - Use global vao where you rebind buffers to it before draw
            - Have one vao for each piece of geometry
         The better one depends on situation (maybe, 2017 vid lol), but opengl recommends you use them
         Best to run tests and see whether global vao or object specific vao is faster based on prod env
        */
        // Note: Use docs.gl to figure this stuff out, great docs
        VertexArray va;
        VertexBuffer vb(positions, sizeof(positions));
        VertexBufferLayout layout;
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);

        // Can use unsigned char to save memory, but limits to 0-255 indices
        IndexBuffer ib(indices, sizeof(indices) / sizeof(unsigned int));

        Shader shader("res/shaders/Basic.shader");
        shader.Bind();

        shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

        va.UnBind();
        vb.Unbind();
        ib.Unbind();
        shader.UnBind();

        float r = 0.0f;
        float increment = 0.05f;
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            GLCall(glClear(GL_COLOR_BUFFER_BIT));

            shader.Bind();
            // Uniforms are set PER DRAW
            shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);
            va.Bind();
            ib.Bind();
            // Draws CURRENTLY BOUND buffer
            // MUST USE UNSIGNED INT IN A BUFFER
            GLCall(glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT, nullptr));

            // Can clamp here but won't since it's quick practice
            if (r > 1.0f)
                increment = -0.01f;
            else if (r < 0.0f)
                increment = 0.01f;

            r += increment;

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }
    glfwTerminate();
    return 0;

}