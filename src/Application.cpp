#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Shader.h"
#include "Texture.h"

#include <GLFW/glfw3.h>
#include <glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <iostream>
#include <string>

// Note: Use docs.gl to figure this stuff out, great docs
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
    window = glfwCreateWindow(960, 540, "Hello World", NULL, NULL);
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
            -50.0f, -50.0f, 0.0f, 0.0f, // 0
            50.0f, -50.0f, 1.0f, 0.0f, // 1
            50.0f, 50.0f, 1.0f, 1.0f, // 2
            -50.0f, 50.0f, 0.0f, 1.0f  // 3
        };

        unsigned int indices[]{
            0, 1, 2,
            2, 3, 0
        };

        GLCall(glEnable(GL_BLEND));
        GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        VertexArray va;
        VertexBuffer vb(positions, sizeof(positions));
        VertexBufferLayout layout;
        layout.Push<float>(2);
        layout.Push<float>(2);
        va.AddBuffer(vb, layout);

        // Can use unsigned char to save memory, but limits to 0-255 indices
        IndexBuffer ib(indices, sizeof(indices) / sizeof(unsigned int));

        // Proj maps our objects from our pixel ratio to the -1 to 1 range opengl understands (projects our data onto screen)
        glm::mat4 proj{ glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f) };
        // View controls the camera, moving the camera left by 100.0f pixels moves the objects right by 100.0f
        // Redundant here, but can be changed in future
        glm::mat4 view{ glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)) };

        Shader shader("res/shaders/Basic.shader");
        shader.Bind();
        shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

        Texture texture("res/textures/Maya.jpg");
        texture.Bind();
        shader.SetUniform1i("u_Texture", 0);

        va.UnBind();
        vb.Unbind();
        ib.Unbind();
        shader.UnBind();

        float r{ 0.0f };
        float increment{ 0.05f };
        const char* glsl_version{ "#version 130" };
        Renderer renderer;
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init(glsl_version);
        glm::vec3 translationA{ (200.0f, 200.0f, 0.0f) };
        glm::vec3 translationB{ (400.0f, 200.0f, 0.0f) };
        /* Loop until the user closes the window */
        while (!glfwWindowShouldClose(window))
        {
            /* Render here */
            renderer.Clear();

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            {
				glm::mat4 model{ glm::translate(glm::mat4(1.0f), translationA) };
				glm::mat4 mvp{ proj * view * model };
                shader.Bind();
				shader.SetUniformMat4f("u_MVP", mvp); 
                renderer.Draw(va, ib, shader);
            }
            {
				glm::mat4 model{ glm::translate(glm::mat4(1.0f), translationB) };
				glm::mat4 mvp{ proj * view * model };
                shader.Bind();
				shader.SetUniformMat4f("u_MVP", mvp); 
                renderer.Draw(va, ib, shader);
            }
            // Can clamp here but won't since it's quick practice
            if (r > 1.0f)
                increment = -0.01f;
            else if (r < 0.0f)
                increment = 0.01f;

            r += increment;
			{
                ImGui::Begin("Apple");
                ImGui::SliderFloat3("TranslationA", &translationA.x, 0.0f, 960.0f);
                ImGui::SliderFloat3("TranslationB", &translationB.x, 0.0f, 960.0f);
				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				ImGui::End();
			}
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;

}