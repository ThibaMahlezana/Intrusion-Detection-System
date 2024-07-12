#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include "main_window.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "IconsFontAwesome6.h"
#include <opencv2/opencv.hpp>
#include "select_area.h"
#include "detect_person.h"

int MainWindow::window_width = 700;
int MainWindow::window_height = 540;
int glfw_win_w = 1150, glfw_win_h = 600;

// Set screen states
bool show_welcome_screen = true;
bool show_mode_screen = false;
bool show_camera_screen = false;
bool show_alert_screen = false;
bool show_trespassing_screen = false;

// set camera status
bool show_camera = false;
GLuint imageTexture;
int frameWidth = 640;
int frameHeight = 480;

// select roi
bool select_roi = false;
std::vector<cv::Point> Points;
bool ShapeDrawn = false;

SelectArea select_area;
DetectPerson detect_person;
bool detection = false;

MainWindow::MainWindow() : window(nullptr) {}

MainWindow::~MainWindow()
{
    if (window)
    {
        // Cleanup openv
        cap.release();
        cv::destroyAllWindows();

        // Cleanup ImGui and GLFW
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        // Terminate GLFW
        glfwDestroyWindow(window);
        glfwTerminate();
    }
}

void MainWindow::error_callback(int error, const char *description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void MainWindow::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void MainWindow::resize_callback(GLFWwindow *window, int new_width, int new_height)
{
    glViewport(0, 0, window_width = new_width, window_height = new_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, window_width, window_height, 0.0, 0.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void TextCenter(std::string text)
{
    auto windowWidth = ImGui::GetWindowSize().x;
    auto textWidth = ImGui::CalcTextSize(text.c_str()).x;

    ImGui::SetCursorPosX((ImGui::GetStyle().WindowPadding.x / 2) + (windowWidth - textWidth) * 0.5f);
    ImGui::Text(text.c_str());
}

void MainWindow::initOpenGL(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, w, h, 0.0, 0.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void MainWindow::initGraphics()
{
    // Initialize GLFW
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        return;

    // Calculate the screen dimensions
    int screenWidth, screenHeight;
    glfwGetMonitorWorkarea(glfwGetPrimaryMonitor(), NULL, NULL, &screenWidth, &screenHeight);

    // Calculate the window position to center it
    int windowX = (screenWidth - glfw_win_w) / 2;
    int windowY = (screenHeight - glfw_win_h) / 2;

    // Creating the window
    // glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    window = glfwCreateWindow(glfw_win_w, glfw_win_h, "SECURITY SYSTEM", NULL, NULL);
    if (window == NULL)
        return;

    // Set the window position
    glfwSetWindowPos(window, windowX, windowY);

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

// Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char *glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char *glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac
#else
    // GL 3.0 + GLSL 130
    // const char *glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0); // 3.0+ only
#endif

    // Initialize GLEW
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        std::cout << "[ERROR] GLEW initialization error: " << glewGetErrorString(err) << std::endl;
        return;
    }
    std::cout << "[INFO] Linked GLEW using version: " << glewGetString(GLEW_VERSION) << std::endl;
}

void MainWindow::imguiStyles()
{
    ImGuiStyle &style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(8, 8);
    style.WindowBorderSize = 0.0f;
    style.FrameRounding = 5.0f;
    style.ItemSpacing = ImVec2(20, 20);
    style.FramePadding = ImVec2(8, 8);
}

GLuint MainWindow::matToTexture(const cv::Mat &mat, GLenum minFilter, GLenum magFilter, GLenum wrapFilter)
{
    // Generate a number for our textureID's unique handle
    GLuint textureID;
    glGenTextures(1, &textureID);

    // Bind to our texture handle
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Catch silly-mistake texture interpolation method for magnification
    if (magFilter == GL_LINEAR_MIPMAP_LINEAR ||
        magFilter == GL_LINEAR_MIPMAP_NEAREST ||
        magFilter == GL_NEAREST_MIPMAP_LINEAR ||
        magFilter == GL_NEAREST_MIPMAP_NEAREST)
    {
        std::cout << "[INFO] You can't use MIPMAPs for magnification - setting filter to GL_LINEAR" << std::endl;
        magFilter = GL_LINEAR;
    }

    // Set texture interpolation methods for minification and magnification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

    // Set texture clamping method
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapFilter);

    // Set incoming texture format to:
    // GL_BGR       for CV_CAP_OPENNI_BGR_IMAGE,
    // GL_LUMINANCE for CV_CAP_OPENNI_DISPARITY_MAP,
    // Work out other mappings as required ( there's a list in comments in main() )
    GLenum inputColourFormat = GL_BGR;
    if (mat.channels() == 1)
    {
        inputColourFormat = GL_LUMINANCE;
    }

    // Create the texture
    glTexImage2D(GL_TEXTURE_2D,     // Type of texture
                 0,                 // Pyramid level (for mip-mapping) - 0 is the top level
                 GL_RGB,            // Internal colour format to convert to
                 mat.cols,          // Image width  i.e. 640 for Kinect in standard mode
                 mat.rows,          // Image height i.e. 480 for Kinect in standard mode
                 0,                 // Border width in pixels (can either be 1 or 0)
                 inputColourFormat, // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
                 GL_UNSIGNED_BYTE,  // Image data type
                 mat.ptr());        // The actual image data itself

    // If we're using mipmaps then generate them. Note: This requires OpenGL 3.0 or higher
    if (minFilter == GL_LINEAR_MIPMAP_LINEAR ||
        minFilter == GL_LINEAR_MIPMAP_NEAREST ||
        minFilter == GL_NEAREST_MIPMAP_LINEAR ||
        minFilter == GL_NEAREST_MIPMAP_NEAREST)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    return textureID;
}

void MainWindow::drawFrame(const cv::Mat &frame)
{
    // Clear color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW); // Operate on model-view matrix

    glEnable(GL_TEXTURE_2D);
    GLuint image_tex = matToTexture(frame, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_CLAMP);

    /* Draw a quad */
    glBegin(GL_QUADS);
    glTexCoord2i(0, 0);
    glVertex2i(0, 0);
    glTexCoord2i(0, 1);
    glVertex2i(0, window_height);
    glTexCoord2i(1, 1);
    glVertex2i(window_width, window_height);
    glTexCoord2i(1, 0);
    glVertex2i(window_width, 0);
    glEnd();

    glDeleteTextures(1, &image_tex);
    glDisable(GL_TEXTURE_2D);
}

void MainWindow::WelcomeScreen(ImFont *fontawsome, ImFont *NunitoSans, ImFont *BigNunitoSans, ImFont *SmallNunitoSans)
{
    ImGui::SetNextWindowSize(ImVec2(600, 600));
    ImGui::SetNextWindowPos(ImVec2((1150 - 600) / 2, 0));
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;
    ImGui::Begin("Welcome Screen", NULL, window_flags);

    // Welcome text
    ImGui::Dummy(ImVec2(0.0f, 100.0f));
    ImGui::PushFont(NunitoSans);
    TextCenter("Welcome!");
    ImGui::PopFont();

    // Application name
    ImGui::Dummy(ImVec2(0.0f, 20.0f));
    ImGui::PushFont(BigNunitoSans);
    TextCenter("Security System v1.0.0");
    ImGui::PopFont();

    // Start button
    ImGui::Dummy(ImVec2(0.0f, 20.0f));
    ImGui::Spacing();
    ImGui::SameLine(0, 220);
    ImGui::PushFont(NunitoSans);
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1 / 7.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(1 / 7.0f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(1 / 7.0f, 0.8f, 0.8f));
    if (ImGui::Button("Start", ImVec2(150, 60)))
    {
        std::cout << "[INFO] Loading mode screen ..." << std::endl;
        show_welcome_screen = false;
        show_mode_screen = true;
    }
    ImGui::PopStyleColor(3);
    ImGui::PopFont();

    // Footer text
    ImGui::Dummy(ImVec2(0.0f, 100.0f));
    ImGui::PushFont(SmallNunitoSans);
    TextCenter("Powered by Tiye Technologies");
    ImGui::PopFont();

    ImGui::End();
}

void MainWindow::ModeSetupScreen(ImFont *fontawsome, ImFont *NunitoSans, ImFont *BigNunitoSans, ImFont *SmallNunitoSans)
{
    ImGui::SetNextWindowSize(ImVec2(600, 600));
    ImGui::SetNextWindowPos(ImVec2((1150 - 600) / 2, 0));
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;
    ImGui::Begin("Mode Setup Screen", NULL, window_flags);

    // Application name
    ImGui::Dummy(ImVec2(0.0f, 30.0f));
    ImGui::PushFont(BigNunitoSans);
    TextCenter("What do you want the system to do?");
    ImGui::PopFont();

    // Mode options buttons
    ImGui::Dummy(ImVec2(0.0f, 20.0f));
    ImGui::PushFont(NunitoSans);
    static int selectedOption = 0;

    // Trespassing Monitoring Button
    if (ImGui::RadioButton(" Monitor Trespassing", selectedOption == 0))
        selectedOption = 0;

    // Traffic Monitoring Button
    if (ImGui::RadioButton(" Monitor Traffic", selectedOption == 1))
        selectedOption = 1;
    ImGui::PopFont();

    // Next button
    ImGui::Dummy(ImVec2(0.0f, 20.0f));
    ImGui::Spacing();
    ImGui::SameLine(0, 50);
    ImGui::PushFont(NunitoSans);
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1 / 7.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(1 / 7.0f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(1 / 7.0f, 0.8f, 0.8f));
    if (ImGui::Button("Next", ImVec2(100, 60)))
    {
        std::cout << "[INFO] Loading camera screen ..." << std::endl;
        show_mode_screen = false;
        show_camera_screen = true;
    }
    ImGui::PopStyleColor(3);
    ImGui::PopFont();

    // Footer text
    ImGui::Dummy(ImVec2(0.0f, 100.0f));
    ImGui::PushFont(SmallNunitoSans);
    TextCenter("Powered by Tiye Technologies");
    ImGui::PopFont();

    ImGui::End();
}

void MainWindow::CameraSetupScreen(ImFont *fontawsome, ImFont *NunitoSans, ImFont *BigNunitoSans, ImFont *SmallNunitoSans)
{
    // Set ImGui window size to match GLFW window size
    ImGui::SetNextWindowSize(ImVec2(600, 100));
    // ImGui::SetNextWindowPos(ImVec2((1150 - 600) / 2, 0));
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBackground;
    ImGui::Begin("Title Window", NULL, window_flags);

    // Menu Bar
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Menu"))
        {

            if (ImGui::MenuItem("Close Application"))
            {
                std::cout << "[INFO] Closing Application..." << std::endl;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Camera"))
        {
            if (ImGui::MenuItem("Add Camera"))
            {
                std::cout << "[INFO] Adding Camera..." << std::endl;
            }
            if (ImGui::MenuItem("Remove Camera"))
            {
                std::cout << "[INFO] Removing Camera..." << std::endl;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Area"))
        {
            if (ImGui::MenuItem("Add Area"))
            {
                std::cout << "[INFO] Adding Area..." << std::endl;
            }
            if (ImGui::MenuItem("Remove Area"))
            {
                std::cout << "[INFO] Removing Area..." << std::endl;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Connectivity"))
        {
            if (ImGui::MenuItem("Test Alarm"))
            {
                std::cout << "[INFO] Testing Alarm..." << std::endl;
            }
            if (ImGui::MenuItem("Connect to an App"))
            {
                std::cout << "[INFO] Connecting to an App..." << std::endl;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    // Application name
    ImGui::PushFont(NunitoSans);
    ImGui::Text("SECURITY SYSTEM V1.0.0");
    ImGui::PopFont();
    ImGui::End();

    ImGui::SetNextWindowSize(ImVec2(300, 650));
    ImGui::SetNextWindowPos(ImVec2(glfw_win_w - 320, 0));
    ImGui::Begin("Camera Setup", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

    ImGui::PushFont(NunitoSans);
    ImGui::Text("CAMERA");
    ImGui::PopFont();
    ImGui::PushFont(SmallNunitoSans);
    static char camIndex_link[50];
    int cameraIndex;
    ImGui::Text("Input Camera index / RTSP link");
    ImGui::PushItemWidth(280.0);
    ImGui::InputText("##cam", camIndex_link, IM_ARRAYSIZE(camIndex_link));
    if (ImGui::Button("Open", ImVec2(60, 35)))
    {
        std::cout << "[INFO] Opening the camera ..." << std::endl;
        std::cout << camIndex_link << std::endl;
        if (std::isdigit(camIndex_link[0]))
        {
            std::cout << "Camera input is a digit" << std::endl;
            cameraIndex = std::atoi(camIndex_link);
            cap.open(cameraIndex);
        }
        else
            cap.open(camIndex_link);

        // Setting up Webcam / Camera
        videoSettings();
        // Loading a single frame needed to calculate GPU memory allocation
        loadFrame();
        // Setting up OpenGL (Allocate GPU memory)
        initTexture(frame, imageTexture);
        // Release frames is needed before loading new frame within the showWebcam() method
        frame.release();
        show_camera = true;
    }
    ImGui::SameLine(0, 40);
    if (ImGui::Button("Close", ImVec2(60, 35)))
    {
        std::cout << "[INFO] Closing the camera ..." << std::endl;
        show_camera = false;
    }
    ImGui::PopFont();

    ImGui::Separator();

    ImGui::PushFont(NunitoSans);
    ImGui::Text("Area of Interest");
    ImGui::PopFont();
    ImGui::PushFont(SmallNunitoSans);
    ImGui::Text("Right click to select the points.");
    ImGui::Text("Press 'c' to connect the points.");
    ImGui::Text("Press 'r' to reset.");
    if (ImGui::Button("Start", ImVec2(60, 35)))
    {
        std::cout << "[INFO] Selecting the area ..." << std::endl;
        select_roi = true;
    }
    ImGui::SameLine(0, 40);
    if (ImGui::Button("Stop", ImVec2(60, 35)))
    {
        std::cout << "[INFO] Selecting the area ..." << std::endl;
        select_roi = false;
    }
    ImGui::PopFont();

    ImGui::Separator();

    // Detect Button
    ImGui::PushFont(NunitoSans);
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1 / 7.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(1 / 7.0f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(1 / 7.0f, 0.8f, 0.8f));
    if (ImGui::Button("Detect", ImVec2(100, 50)))
    {
        std::cout << "[INFO] Starting Detection ..." << std::endl;
        detection = true;
    }
    ImGui::PopStyleColor(3);
    ImGui::PopFont();

    ImGui::SameLine(0, 40);

    // Stop Button
    ImGui::PushFont(NunitoSans);
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(1 / 7.0f, 0.6f, 0.6f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(1 / 7.0f, 0.7f, 0.7f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(1 / 7.0f, 0.8f, 0.8f));
    if (ImGui::Button("Stop", ImVec2(100, 50)))
    {
        std::cout << "[INFO] Stop Detection" << std::endl;
        detection = false;
    }
    ImGui::PopStyleColor(3);
    ImGui::PopFont();

    ImGui::End();
}

void MainWindow::AlertSetupScreen(ImFont *fontawsome, ImFont *NunitoSans, ImFont *BigNunitoSans, ImFont *SmallNunitoSans) {}

void MainWindow::TresspassingScreen(ImFont *fontawsome, ImFont *NunitoSans, ImFont *BigNunitoSans, ImFont *SmallNunitoSans)
{
    ImGui::Begin("Trespassing");
    ImGui::PushFont(BigNunitoSans);
    TextCenter("Trespassing");
    ImGui::PopFont();
    ImGui::End();
}

void MainWindow::videoSettings()
{
    cap.set(cv::CAP_PROP_FRAME_WIDTH, frameWidth);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, frameHeight);
}

void MainWindow::loadFrame()
{
    // Check if the camera was opened successfully
    if (!(cap.isOpened()))
    {
        std::cout << "Error opening camera" << std::endl;
        glfwTerminate();
    }
    // set correct resolution accoring to camera type
    cap.read(frame);
}

void MainWindow::initTexture(cv::Mat &image, GLuint &imageTexture)
{

    if (image.empty())
    {
        std::cout << "image empty" << std::endl;
        glfwTerminate();
    }
    else
    {
        // These settings stick with the texture that's bound. You only need to set them once.
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
        glGenTextures(1, &imageTexture);            // Gen a new texture and store the handle
        glBindTexture(GL_TEXTURE_2D, imageTexture); // Allocate GPU memory for handle (Texture ID)

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Set texture clamping method
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }
}

void MainWindow::BindCVMat2GLTexture(cv::Mat &image, GLuint &imageTexture)
{
    if (image.empty())
    {
        std::cout << "image empty" << std::endl;
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, imageTexture); // Allocate GPU memory for handle (Texture ID)

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Set texture clamping method
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

        cv::cvtColor(image, image, cv::COLOR_RGB2BGR);

        glTexImage2D(GL_TEXTURE_2D,    // Type of texture
                     0,                // Pyramid level (for mip-mapping) - 0 is the top level
                     GL_RGB,           // Internal colour format to convert to
                     image.cols,       // Image width  i.e. 640 for Kinect in standard mode
                     image.rows,       // Image height i.e. 480 for Kinect in standard mode
                     0,                // Border width in pixels (can either be 1 or 0)
                     GL_RGB,           // Input image format (i.e. GL_RGB, GL_RGBA, GL_BGR etc.)
                     GL_UNSIGNED_BYTE, // Image data type
                     image.ptr());     // The actual image data itself
    }
}

void MainWindow::ShowWebcam(ImFont *NunitoSans)
{
    // Set the frame position
    float frameX = 10.0f;
    float frameY = 30.0f;
    int ImGuiWinPosX = 10, ImGuiWinPosY = 50;
    cv::Point minPoint;
    cv::Point maxPoint;

    loadFrame();
    if (select_roi)
    {
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            ImVec2 mousePos = ImGui::GetMousePos();
            Points.push_back(cv::Point(mousePos.x - frameX - ImGuiWinPosX, mousePos.y - frameY - ImGuiWinPosY));
        }

        ImGuiIO &io = ImGui::GetIO();
        if (io.KeysDown['r'] || io.KeysDown['R'])
        {
            Points.clear();
            ShapeDrawn = false;
        }
        if (io.KeysDown['c'] || io.KeysDown['C'])
        {
            if (Points.size() > 2)
            {
                ShapeDrawn = true;
                select_roi = false;
            }
        }
        select_area.drawPointsAndShape(frame, Points, ShapeDrawn);
    }
    if (ShapeDrawn)
    {
        select_area.drawPointsAndShape(frame, Points, ShapeDrawn);
        // Determining the maximum point and the minimum point
        if (!Points.empty())
        {
            minPoint = Points[0];
            maxPoint = Points[0];

            for (const auto &point : Points)
            {
                if (point.x < minPoint.x || (point.x == minPoint.x && point.y < minPoint.y))
                    minPoint = point;

                if (point.x > maxPoint.x || (point.x == maxPoint.x && point.y > maxPoint.y))
                    maxPoint = point;
            }

            // std::cout << "Minimum Point: (" << minPoint.x << ", " << minPoint.y << ")" << std::endl;
            // std::cout << "Maximum Point: (" << maxPoint.x << ", " << maxPoint.y << ")" << std::endl;
        }
        else
            std::cout << "No points in the vector." << std::endl;

        if (detection)
            detect_person.detect(frame, minPoint, maxPoint);
    }

    // detecting people
    // if (detection)
    // {
    //     detect_person.detect(frame);
    // }

    BindCVMat2GLTexture(frame, imageTexture);
    frame.release();

    // Show video Webcam
    ImGui::SetNextWindowSize(ImVec2(650, 600));
    ImGui::SetNextWindowPos(ImVec2(ImGuiWinPosX, ImGuiWinPosY));
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBackground;
    ImGui::Begin("CAM0", NULL, window_flags);
    // ImVec2 framePosition(frameX, frameY);
    // ImGui::SetCursorPos(framePosition);
    ImGui::PushFont(NunitoSans);
    TextCenter("CAM 0");
    ImGui::PopFont();
    ImGui::Image((void *)(intptr_t)imageTexture, ImVec2(frameWidth, frameHeight));
    ImGui::End();
}

GLuint CreateOpenGLTexture(const cv::Mat &image)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, image.ptr());
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureID;
}

void RenderImGuiFrame(cv::Mat &frame)
{
    ImGui::SetNextWindowPos(ImVec2(10, 50));
    ImGui::Begin("OpenCV Frame");

    if (select_roi)
    {
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            ImVec2 mousePos = ImGui::GetMousePos();
            Points.push_back(cv::Point(mousePos.x, mousePos.y));
        }

        ImGuiIO &io = ImGui::GetIO();
        if (io.KeysDown['r'] || io.KeysDown['R'])
        {
            Points.clear();
            ShapeDrawn = false;
        }
        if (io.KeysDown['c'] || io.KeysDown['C'])
        {
            if (Points.size() > 2)
            {
                ShapeDrawn = true;
            }
        }
        select_area.drawPointsAndShape(frame, Points, ShapeDrawn);
    }

    // Drawing a cv::Mat frame to ImGui texture
    GLuint frameTexture = CreateOpenGLTexture(frame);
    ImGui::Image((void *)(intptr_t)frameTexture, ImVec2(frame.cols, frame.rows));

    ImGui::End();
}

void MainWindow::StartWebcamThread()
{
    cap.open(0);
    if (!cap.isOpened())
    {
        std::cerr << "Error: Cannot open the webcam!" << std::endl;
        return;
    }

    while (!glfwWindowShouldClose(window))
    {
        cap >> frame;
        frameMutex.lock();
        frame.copyTo(frame);
        frameMutex.unlock();
    }

    cap.release();
}

void MainWindow::displayMainWindow()
{
    // Initialize graphics
    initGraphics();

    // Set GL+GLSL version
    const char *glsl_version = "#version 130";

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    // Setting up Font types
    (void)io;
    const char *fontFileDir = "../fonts/Nunito_Sans/static/NunitoSans_10pt-Regular.ttf";
    const char *iconFileDir = "../fonts/fa-regular-400.ttf";
    const char *bigFontFileDir = "../fonts/Nunito_Sans/static/NunitoSans_7pt_Condensed-Bold.ttf";
    float baseFontSize = 30.0f;
    float iconFontSize = baseFontSize * 2.0f / 3.0f;
    ImFont *SmallNunitoSans = io.Fonts->AddFontFromFileTTF(fontFileDir, 16.0f);
    ImFont *NunitoSans = io.Fonts->AddFontFromFileTTF(fontFileDir, baseFontSize);
    ImFont *BigNunitoSans = io.Fonts->AddFontFromFileTTF(bigFontFileDir, 40.0f);

    // Setting up icons
    static const ImWchar icons_range[] = {ICON_MIN_FA, ICON_MAX_16_FA, 0};
    ImFontConfig icons_config;
    icons_config.MergeMode = false;
    icons_config.PixelSnapH = true;
    icons_config.GlyphMinAdvanceX = iconFontSize;
    ImFont *fontawsome = io.Fonts->AddFontFromFileTTF(iconFileDir, iconFontSize, &icons_config, icons_range);

    imguiStyles();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImVec4 clear_color = ImVec4(0.07f, 0.27f, 0.35f, 1.00f);

    // Frames Per Second
    // float final_time;
    // float FPS[16];
    // for (size_t i = 0; i < 16; i++)
    //     FPS[i] = 0.0;

    // OpenCV frame
    // cv::Mat frame;
    // cv::VideoCapture cap;
    // cap.open(0);

    // glfwMakeContextCurrent(window);

    // Start the webcam thread
    // std::thread webcamThread([this]()
    //                          { StartWebcamThread(); });

    // Setting up Webcam
    // videoSettings();
    // loadFrame(); // single frame needed to calculate GPU memory allocation

    // setting up OpenGL (Allocate GPU memory)
    // initTexture(frame, imageTexture);
    // release frames is needed before loading new frame within the .Update() method
    // frame.release();

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        // Clear the color buffer
        // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);

        // Rendering the scene
        // cap >> frame;
        // if (show_camera)
        //     drawFrame(frame);
        // if (select_roi)
        //     select_area.selectROI(frame);

        // Starting the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Render Screens
        if (show_welcome_screen)
            WelcomeScreen(fontawsome, NunitoSans, BigNunitoSans, SmallNunitoSans);
        else if (show_mode_screen)
            ModeSetupScreen(fontawsome, NunitoSans, BigNunitoSans, SmallNunitoSans);
        else if (show_camera_screen)
            CameraSetupScreen(fontawsome, NunitoSans, BigNunitoSans, SmallNunitoSans);
        else if (show_trespassing_screen)
            TresspassingScreen(fontawsome, NunitoSans, BigNunitoSans, SmallNunitoSans);

        if (show_camera)
            ShowWebcam(NunitoSans);

        /**
                // Create 'Window 2' to display the live video feed
                ImGui::Begin("Window 2");
                ImGui::Text("CAM 01");
                ImGui::Text("FPS: ");
                frameMutex.lock();
                if (!frame.empty())
                {
                    // Detect a person
                    // detect_person.detect(frame);
                    // Render the video frame here using OpenGL
                    // Drawing a cv::Mat frame to ImGui texture
                    GLuint frameTexture = CreateOpenGLTexture(frame);
                    ImGui::Image((void *)(intptr_t)frameTexture, ImVec2(frame.cols, frame.rows));
                }
                frameMutex.unlock();
                ImGui::End();
        **/

        // RenderImGuiFrame(frame);

        // Rendering ImGui
        // ImGui::Render();
        // glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        // glfwPollEvents();
    }
    // Clean up threads
    // webcamThread.join();
}