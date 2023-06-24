#include <iostream>
#include <string>
#include <vector>

#include "external/imgui/imgui.h"
#include "external/imgui/imgui_impl_glfw.h"
#include "external/imgui/imgui_impl_opengl3.h"

#include "Window.h"
#include "Input.h"
#include "renderer/VertexArray.h"
#include "renderer/VertexBuffer.h"
#include "renderer/IndexBuffer.h"
#include "renderer/Shader.h"
#include "renderer/Camera.h"

#include "frameVar.h"
#include "Mesh/Mesh.h"

int main()
{
    unsigned int screenWidth = 1440;
    unsigned int screenHeight = 810;
    float aspectRatio = (float)screenWidth / screenHeight;
    Window window(screenWidth, screenHeight, "2D Probability Visualizer", NULL);

    VertexBufferLayout layout;
    layout.Push<float>(3); // 3d coordinates
    layout.Push<float>(3); // normals

    frameVar xCurrVars = frameVar();
    frameVar yCurrVars = frameVar();
    frameVar xNextVars = frameVar();
    frameVar yNextVars = frameVar();

    HeightMap xHeightMap = HeightMapNormal(xCurrVars.m_size, xCurrVars.m_domain_start, xCurrVars.m_domain_end, 0, 1);
    HeightMap yHeightMap = HeightMapNormal(yCurrVars.m_size, yCurrVars.m_domain_start, yCurrVars.m_domain_end, 0, 1);

    Mesh functionMesh(xHeightMap, yHeightMap);

    VertexArray functionVA;
    VertexBuffer functionVB(functionMesh.m_Vertices, 2 * 3 * xCurrVars.m_size * yCurrVars.m_size * sizeof(float), DRAW_MODE::STATIC);
    // bind vertex buffer to vertex array
    functionVA.AddBuffer(functionVB, layout);
    IndexBuffer functionIB(functionMesh.m_Indices, 6 * (xCurrVars.m_size - 1) * (yCurrVars.m_size - 1), DRAW_MODE::STATIC);

    // shaders
    std::string vertexFilepath = "res/shaders/map.vert";
    std::string fragmentFilepath = "res/shaders/map.frag";

    Shader shader(vertexFilepath, fragmentFilepath);

    // camera setup
    float FOV = 45.0f;
    double yaw = -90.0;
    double pitch = -45.0;
    glm::vec3 cameraPosition = { 0.0f, 20.0f, 22.5f };
    Camera camera(cameraPosition, yaw, pitch);

    float rotationAngle = 0.0f; // rotation angle of the function mesh
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projMatrix = glm::perspective(glm::radians(FOV), aspectRatio, 0.1f, 1000.0f);

    // upload uniforms
    shader.Bind();
    shader.SetUniformMat4f("u_Model", modelMatrix);
    shader.SetUniformMat4f("u_View", camera.GetViewMatrix());
    shader.SetUniformMat4f("u_Projection", projMatrix);

    // openGL settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // wireframe mode
    bool wireframe = false;

    GLFWwindow* windowID = window.GetID();
    // input initialization
    Input::Init(windowID);

    // mouse movement variables
    double currXpos, currYpos, deltaX, deltaY;
    double lastXpos = 0.0;
    double lastYpos = 0.0;
    double sens = 200.0;

    // Setup Dear ImGui context
    ImGui::CreateContext();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(windowID, true);
    ImGui_ImplOpenGL3_Init();
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    bool quit = false;
    bool regenerate = false;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(windowID) && !quit)
    {
        // close the window
        if (Input::IsKeyDown(GLFW_KEY_Q))
        {
            quit = true;
        }

        // reset frame, set next frame data to be equal to current frame
        xNextVars = xCurrVars;
        yNextVars = yCurrVars;

        // mouse movement
        glfwGetCursorPos(windowID, &currXpos, &currYpos);
        deltaX = (currXpos - lastXpos) / screenWidth;  // it is bounded by -1 and 1
        deltaY = (currYpos - lastYpos) / screenHeight; // it is bounded by -1 and 1
        lastXpos = currXpos;
        lastYpos = currYpos;

        // rotate model according to mouse movement
        if (Input::IsMouseButtonDown(GLFW_MOUSE_BUTTON_1) && !ImGui::GetIO().WantCaptureMouse)
        {
            rotationAngle = (float)deltaX * sens;
            rotationAngle > 360.0f ? rotationAngle -= 360.0f : NULL;
            modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            shader.SetUniformMat4f("u_Model", modelMatrix);
        }

        // adjust FOV using vertical scroll
        FOV -= Input::GetScrollY() * 2.0f;
        FOV < 25.0f ? FOV = 25.0f : NULL;
        FOV > 65.0f ? FOV = 65.0f : NULL;
        projMatrix = glm::perspective(glm::radians(FOV), aspectRatio, 0.1f, 1000.0f);
        shader.SetUniformMat4f("u_Projection", projMatrix);
        Input::ResetScroll();

        // clearing per frame
        glClearColor(0.80f, 0.90f, 0.96f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Render here */
        functionVA.Bind();
        shader.Bind();
        glDrawElements(GL_TRIANGLES, functionIB.GetCount(), GL_UNSIGNED_INT, 0);

        // imgui new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Display parameters");
        ImGui::SliderInt2("X range", &xNextVars.m_domain_start, -20, 20);
        ImGui::SliderInt2("Y range", &yNextVars.m_domain_start, -20, 20);

        ImGui::Spacing();

        ImGui::SliderInt("X details", &xNextVars.m_size, 25, 75);
        ImGui::SliderInt("Y details", &yNextVars.m_size, 25, 75);

        ImGui::Spacing();

        ImGui::Text("Options:");
        if (ImGui::Checkbox("Wireframe Mode", &wireframe))
        {
            if (wireframe)
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe mode
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // surface mode
        }
        ImGui::End();

        ImGui::Begin("Application insight");
        ImGui::Text("Application average %.1f FPS", ImGui::GetIO().Framerate);
        ImGui::End();

        ImGui::Begin("X controls");
        // let user choose the probabiltiy distribution function
        ImGui::Text("Probability distribution Type:");
        ImGui::RadioButton("Uniform", &xNextVars.m_mode, UNIFORM);
        ImGui::RadioButton("Normal", &xNextVars.m_mode, NORMAL);
        ImGui::RadioButton("Exponential", &xNextVars.m_mode, EXPONENTIAL);
        ImGui::RadioButton("Gamma", &xNextVars.m_mode, GAMMA);
        ImGui::RadioButton("Beta", &xNextVars.m_mode, BETA);
        ImGui::RadioButton("Chi Sqaured", &xNextVars.m_mode, CHISQUARED);
        ImGui::RadioButton("T", &xNextVars.m_mode, TDIST);
        ImGui::RadioButton("F", &xNextVars.m_mode, FDIST);

        ImGui::Spacing();
        ImGui::Separator();

        ImGui::Text("Distribution parameters");
        switch (xNextVars.m_mode) // display controls for current probability dist function and regenerate heightmap
        {
        case UNIFORM:
            ImGui::SliderFloat("start", &xNextVars.m_uniform_start, -5, 5);
            ImGui::SliderFloat("end", &xNextVars.m_uniform_end, -5, 5);
            break;
        case NORMAL:
            ImGui::SliderFloat("mu", &xNextVars.m_normal_mu, -5, 5);
            ImGui::SliderFloat("sigma", &xNextVars.m_normal_sigma, 0, 5);
            break;
        case EXPONENTIAL:
            ImGui::SliderFloat("lambda", &xNextVars.m_exponential_lambda, 0, 5);
            break;
        case GAMMA:
            ImGui::SliderFloat("alpha", &xNextVars.m_gamma_alpha, 0, 5);
            ImGui::SliderFloat("beta", &xNextVars.m_gamma_beta, 0, 5);
            break;
        case BETA:
            ImGui::SliderFloat("alpha", &xNextVars.m_beta_alpha, 0, 5);
            ImGui::SliderFloat("beta", &xNextVars.m_beta_beta, 0, 5);
            break;
        case CHISQUARED:
            ImGui::SliderInt("k", &xNextVars.m_chi_sq_k, 1, 10);
            break;
        case TDIST:
            ImGui::SliderFloat("nu", &xNextVars.m_T_nu, 0, 10);
            break;
        case FDIST:
            ImGui::SliderInt("d1", &xNextVars.m_F_d1, 1, 50);
            ImGui::SliderInt("d2", &xNextVars.m_F_d2, 1, 50);
            break;
        }
        ImGui::End();

        ImGui::Begin("Y controls");
        // let user choose the probabiltiy distribution function
        ImGui::Text("Probability distribution Type:");
        ImGui::RadioButton("Uniform", &yNextVars.m_mode, UNIFORM);
        ImGui::RadioButton("Normal", &yNextVars.m_mode, NORMAL);
        ImGui::RadioButton("Exponential", &yNextVars.m_mode, EXPONENTIAL);
        ImGui::RadioButton("Gamma", &yNextVars.m_mode, GAMMA);
        ImGui::RadioButton("Beta", &yNextVars.m_mode, BETA);
        ImGui::RadioButton("Chi Sqaured", &yNextVars.m_mode, CHISQUARED);
        ImGui::RadioButton("T", &yNextVars.m_mode, TDIST);
        ImGui::RadioButton("F", &yNextVars.m_mode, FDIST);

        ImGui::Spacing();
        ImGui::Separator();

        ImGui::Text("Distribution parameters");
        switch (yNextVars.m_mode) // display controls for current probability dist function and regenerate heightmap
        {
        case UNIFORM:
            ImGui::SliderFloat("start", &yNextVars.m_uniform_start, -5, 5);
            ImGui::SliderFloat("end", &yNextVars.m_uniform_end, -5, 5);
            break;
        case NORMAL:
            ImGui::SliderFloat("mu", &yNextVars.m_normal_mu, -5, 5);
            ImGui::SliderFloat("sigma", &yNextVars.m_normal_sigma, 0, 5);
            break;
        case EXPONENTIAL:
            ImGui::SliderFloat("lambda", &yNextVars.m_exponential_lambda, 0, 5);
            break;
        case GAMMA:
            ImGui::SliderFloat("alpha", &yNextVars.m_gamma_alpha, 0, 5);
            ImGui::SliderFloat("beta", &yNextVars.m_gamma_beta, 0, 5);
            break;
        case BETA:
            ImGui::SliderFloat("alpha", &yNextVars.m_beta_alpha, 0, 5);
            ImGui::SliderFloat("beta", &yNextVars.m_beta_beta, 0, 5);
            break;
        case CHISQUARED:
            ImGui::SliderInt("k", &yNextVars.m_chi_sq_k, 1, 10);
            break;
        case TDIST:
            ImGui::SliderFloat("nu", &yNextVars.m_T_nu, 0, 10);
            break;
        case FDIST:
            ImGui::SliderInt("d1", &yNextVars.m_F_d1, 1, 50);
            ImGui::SliderInt("d2", &yNextVars.m_F_d2, 1, 50);
            break;
        }
        ImGui::End();

        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // regenerate mesh on user chosen functions
        if (!(xNextVars == xCurrVars && yNextVars == yCurrVars)) // only regenerate on user update, instead of every frame
        {
            xHeightMap = xNextVars.generateCorrectMap();
            yHeightMap = yNextVars.generateCorrectMap();

            functionMesh.Regenerate(xHeightMap, yHeightMap);
            functionVA.Bind(); // need to bind correct VA, otherwise it may add to waterVA
            functionVB.AssignData(functionMesh.m_Vertices, 2 * 3 * xNextVars.m_size * yNextVars.m_size * sizeof(float), DRAW_MODE::STATIC);
            functionIB.AssignData(functionMesh.m_Indices, 6 * (xNextVars.m_size - 1) * (yNextVars.m_size - 1), DRAW_MODE::STATIC);

            xCurrVars = xNextVars;
            yCurrVars = yNextVars;
            regenerate = false;
        }

        /* Swap front and back buffers */
        glfwSwapBuffers(windowID);

        /* Poll for and process events */
        glfwPollEvents();
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    window.~Window();
    return 0;
}