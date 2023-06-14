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
#include "Mesh/HeightMap/HeightMap.h"
#include "Mesh/HeightMap/HeightMapUniform.h"
#include "Mesh/HeightMap/HeightMapNormal.h"
#include "Mesh/HeightMap/HeightMapExpo.h"
#include "Mesh/HeightMap/HeightMapGamma.h"
#include "Mesh/HeightMap/HeightMapBeta.h"
#include "Mesh/HeightMap/HeightMapChiSq.h"
#include "Mesh/HeightMap/HeightMapT.h"
#include "Mesh/HeightMap/HeightMapF.h"
#include "Mesh/Mesh.h"

    int main()
{
    unsigned int screenWidth = 1440;
    unsigned int screenHeight = 810;
    float aspectRatio = (float) screenWidth / screenHeight;
    Window window(screenWidth, screenHeight, "2D Probability Visualizer", NULL);

    VertexBufferLayout layout;
    layout.Push<float>(3); // 3d coordinates
    layout.Push<float>(3); // normals

    int mapWidth = 50;
    int mapLength = 50;

    enum probMode
    {
        UNIFORM,
        NORMAL,
        EXPONENTIAL,
        GAMMA,
        BETA,
        CHISQUARED,
        TDIST,
        FDIST
    };

    int xCurrMode = NORMAL;
    int yCurrMode = NORMAL;

    std::vector<int> x_range = { -5, 5 };
    std::vector<int> y_range = { -5, 5 };

    HeightMap xHeightMap = HeightMapNormal(mapWidth, x_range[0], x_range[1], 0, 1);
    HeightMap yHeightMap = HeightMapNormal(mapLength, y_range[0], y_range[1], 0, 1);

    Mesh functionMesh(xHeightMap, yHeightMap);
    
    VertexArray functionVA;
    VertexBuffer functionVB(functionMesh.m_Vertices, 2 * 3 * mapWidth * mapLength * sizeof(float), DRAW_MODE::STATIC);
    // bind vertex buffer to vertex array
    functionVA.AddBuffer(functionVB, layout);
    IndexBuffer functionIB(functionMesh.m_Indices, 6 * (mapWidth - 1) * (mapLength - 1), DRAW_MODE::STATIC);

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

    // uniform mode variables
    float x_start = -1;
    float x_end = 1;
    float y_start = -1;
    float y_end = 1;

    // normal mode variables
    float x_mu = 0;
    float x_sigma = 1;
    float y_mu = 0;
    float y_sigma = 1;

    // exponential mode variables
    float x_lambda = 1;
    float y_lambda = 1;

    // gamma mode variables
    float x_G_alpha = 1;
    float x_G_beta = 1;
    float y_G_alpha = 1;
    float y_G_beta = 1;

    // beta mode variables
    float x_B_alpha = 1;
    float x_B_beta = 1;
    float y_B_alpha = 1;
    float y_B_beta = 1;

    // chi sqaured mode variables
    int x_k = 1;
    int y_k = 1;

    // T mode variables
    float x_nu = 1;
    float y_nu = 1;

    // F mode variables
    int x_d1 = 1;
    int x_d2 = 1;
    int y_d1 = 1;
    int y_d2 = 1;

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

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(windowID))
    {
        // mouse movement
        glfwGetCursorPos(windowID, &currXpos, &currYpos);
        deltaX = (currXpos - lastXpos) / screenWidth;  // it is bounded by -1 and 1
        deltaY = (currYpos - lastYpos) / screenHeight; // it is bounded by -1 and 1
        lastXpos = currXpos;
        lastYpos = currYpos;

        // rotate model according to mouse movement
        if (Input::IsMouseButtonDown(GLFW_MOUSE_BUTTON_1) && !ImGui::GetIO().WantCaptureMouse)
        {
            rotationAngle = (float) deltaX * sens;
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
        ImGui::SliderInt2("X range", &x_range[0], -20, 20);
        ImGui::SliderInt2("Y range", &y_range[0], -20, 20);

        ImGui::Spacing();

        ImGui::SliderInt("X details", &mapWidth, 25, 75);
        ImGui::SliderInt("Y details", &mapLength, 25, 75);

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

        ImGui::Begin("X controls");
        // let user choose the probabiltiy distribution function
        ImGui::Text("Probability distribution Type:");
        ImGui::RadioButton("Uniform", &xCurrMode, UNIFORM);
        ImGui::RadioButton("Normal", &xCurrMode, NORMAL);
        ImGui::RadioButton("Exponential", &xCurrMode, EXPONENTIAL);
        ImGui::RadioButton("Gamma", &xCurrMode, GAMMA);
        ImGui::RadioButton("Beta", &xCurrMode, BETA);
        ImGui::RadioButton("Chi Sqaured", &xCurrMode, CHISQUARED);
        ImGui::RadioButton("T", &xCurrMode, TDIST);
        ImGui::RadioButton("F", &xCurrMode, FDIST);

        ImGui::Spacing();
        ImGui::Separator();

        ImGui::Text("Distribution parameters");
        switch (xCurrMode) // display controls for current probability dist function and regenerate heightmap
        {
        case UNIFORM:
            ImGui::SliderFloat("start", &x_start, -5, 5);
            ImGui::SliderFloat("end", &x_end, -5, 5);
            xHeightMap = HeightMapUniform(mapWidth, x_range[0], x_range[1], x_start, x_end);
            break;
        case NORMAL:
            ImGui::SliderFloat("mu", &x_mu, -5, 5);
            ImGui::SliderFloat("sigma", &x_sigma, 0, 5);
            xHeightMap = HeightMapNormal(mapWidth, x_range[0], x_range[1], x_mu, x_sigma);
            break;
        case EXPONENTIAL:
            ImGui::SliderFloat("lambda", &x_lambda, 0, 5);
            xHeightMap = HeightMapExpo(mapWidth, x_range[0], x_range[1], x_lambda);
            break;
        case GAMMA:
            ImGui::SliderFloat("alpha", &x_G_alpha, 0, 5);
            ImGui::SliderFloat("beta", &x_G_beta, 0, 5);
            xHeightMap = HeightMapGamma(mapWidth, x_range[0], x_range[1], x_G_alpha, x_G_beta);
            break;
        case BETA:
            ImGui::SliderFloat("alpha", &x_B_alpha, 0, 5);
            ImGui::SliderFloat("beta", &x_B_beta, 0, 5);
            xHeightMap = HeightMapBeta(mapWidth, x_range[0], x_range[1], x_B_alpha, x_B_beta);
            break;
        case CHISQUARED:
            ImGui::SliderInt("k", &x_k, 1, 10);
            xHeightMap = HeightMapChiSq(mapWidth, x_range[0], x_range[1], x_k);
            break;
        case TDIST:
            ImGui::SliderFloat("nu", &x_nu, 0, 10);
            xHeightMap = HeightMapT(mapWidth, x_range[0], x_range[1], x_nu);
            break;
        case FDIST:
            ImGui::SliderInt("d1", &x_d1, 1, 50);
            ImGui::SliderInt("d2", &x_d2, 1, 50);
            xHeightMap = HeightMapF(mapWidth, x_range[0], x_range[1], x_d1, x_d2);
            break;
        }
        ImGui::End();
        
        ImGui::Begin("Y controls");
        // let user choose the probabiltiy distribution function
        ImGui::Text("Probability distribution Type:");
        ImGui::RadioButton("Uniform", &yCurrMode, UNIFORM);
        ImGui::RadioButton("Normal", &yCurrMode, NORMAL);
        ImGui::RadioButton("Exponential", &yCurrMode, EXPONENTIAL);
        ImGui::RadioButton("Gamma", &yCurrMode, GAMMA);
        ImGui::RadioButton("Beta", &yCurrMode, BETA);
        ImGui::RadioButton("Chi Sqaured", &yCurrMode, CHISQUARED);
        ImGui::RadioButton("T", &yCurrMode, TDIST);
        ImGui::RadioButton("F", &yCurrMode, FDIST);

        ImGui::Spacing();
        ImGui::Separator();

        ImGui::Text("Distribution parameters");
        switch (yCurrMode) // display controls for current probability dist function and regenerate heightmap
        {
        case UNIFORM:
            ImGui::SliderFloat("start", &y_start, -5, 5);
            ImGui::SliderFloat("end", &y_end, -5, 5);
            yHeightMap = HeightMapUniform(mapLength, y_range[0], y_range[1], y_start, y_end);
            break;
        case NORMAL:
            ImGui::SliderFloat("mu", &y_mu, -5, 5);
            ImGui::SliderFloat("sigma", &y_sigma, 0, 5);
            yHeightMap = HeightMapNormal(mapLength, y_range[0], y_range[1], y_mu, y_sigma);
            break;
        case EXPONENTIAL:
            ImGui::SliderFloat("lambda", &y_lambda, 0, 5);
            yHeightMap = HeightMapExpo(mapLength, y_range[0], y_range[1], y_lambda);
            break;
        case GAMMA:
            ImGui::SliderFloat("alpha", &y_G_alpha, 0, 5);
            ImGui::SliderFloat("beta", &y_G_beta, 0, 5);
            yHeightMap = HeightMapGamma(mapLength, y_range[0], y_range[1], y_G_alpha, y_G_beta);
            break;
        case BETA:
            ImGui::SliderFloat("alpha", &y_B_alpha, 0, 5);
            ImGui::SliderFloat("beta", &y_B_beta, 0, 5);
            yHeightMap = HeightMapBeta(mapLength, y_range[0], y_range[1], y_B_alpha, y_B_beta);
            break;
        case CHISQUARED:
            ImGui::SliderInt("k", &y_k, 1, 10);
            yHeightMap = HeightMapChiSq(mapLength, y_range[0], y_range[1], y_k);
            break;
        case TDIST:
            ImGui::SliderFloat("nu", &y_nu, 0, 10);
            yHeightMap = HeightMapT(mapLength, y_range[0], y_range[1], y_nu);
            break;
        case FDIST:
            ImGui::SliderInt("d1", &y_d1, 1, 50);
            ImGui::SliderInt("d2", &y_d2, 1, 50);
            yHeightMap = HeightMapF(mapLength, y_range[0], y_range[1], y_d1, y_d2);
            break;
        }
        ImGui::End();

        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // regenerate mesh on user chosen functions
        functionMesh.Regenerate(xHeightMap, yHeightMap);
        functionVA.Bind(); // need to bind correct VA, otherwise it may add to waterVA
        functionVB.AssignData(functionMesh.m_Vertices, 2 * 3 * mapWidth * mapLength * sizeof(float), DRAW_MODE::STATIC);
        functionIB.AssignData(functionMesh.m_Indices, 6 * (mapWidth - 1) * (mapLength - 1), DRAW_MODE::STATIC);

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