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
#include "Mesh/HeightMap/HeightMapNormal.h"
#include "Mesh/HeightMap/HeightMapUniform.h"
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
        NORMAL
    };

    int xCurrMode = NORMAL;
    int yCurrMode = NORMAL;

    HeightMap xHeightMap = HeightMapNormal(mapWidth);
    HeightMap yHeightMap = HeightMapNormal(mapLength);

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

    // normal mode variables
    float x_mu = 0;
    float x_sigma = 1;
    float y_mu = 0;
    float y_sigma = 1;

    // uniform mode variables
    float x_start = -1;
    float x_end = 1;
    float y_start = -1;
    float y_end = 1;

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
        ImGui::SliderInt("Width", &mapWidth, 25, 75);
        ImGui::SliderInt("Length", &mapLength, 25, 75);
        ImGui::Text("Options:");
        if (ImGui::Checkbox("Wireframe", &wireframe))
        {
            if (wireframe)
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // wireframe mode
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // surface mode
        }
        ImGui::End();

        ImGui::Begin("X parameters");
        // let user choose the probabiltiy distribution function
        ImGui::Text("x-axis Probability distribution Type:");
        ImGui::RadioButton("Uniform", &xCurrMode, UNIFORM);
        ImGui::RadioButton("Normal", &xCurrMode, NORMAL);
        switch (xCurrMode) // display controls for current probability dist function and regenerate heightmap
        {
        case UNIFORM:
            ImGui::SliderFloat("x start", &x_start, -5, 5);
            ImGui::SliderFloat("x end", &x_end, -5, 5);
            xHeightMap = HeightMapUniform(mapWidth, x_start, x_end);
            break;
        case NORMAL:
            ImGui::SliderFloat("x mu", &x_mu, -5, 5);
            ImGui::SliderFloat("x sigma", &x_sigma, 0, 5);
            xHeightMap = HeightMapNormal(mapWidth, x_mu, x_sigma);
            break;
        }
        ImGui::End();
        
        ImGui::Begin("Y parameters");
        // let user choose the probabiltiy distribution function
        ImGui::Text("y-axis Probability distribution Type:");
        ImGui::RadioButton("Uniform", &yCurrMode, UNIFORM);
        ImGui::RadioButton("Normal", &yCurrMode, NORMAL);
        switch (yCurrMode) // display controls for current probability dist function and regenerate heightmap
        {
        case UNIFORM:
            ImGui::SliderFloat("y start", &y_start, -5, 5);
            ImGui::SliderFloat("y end", &y_end, -5, 5);
            yHeightMap = HeightMapUniform(mapLength, y_start, y_end);
            break;
        case NORMAL:
            ImGui::SliderFloat("y mu", &y_mu, -5, 5);
            ImGui::SliderFloat("y sigma", &y_sigma, 0, 5);
            yHeightMap = HeightMapNormal(mapLength, y_mu, y_sigma);
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