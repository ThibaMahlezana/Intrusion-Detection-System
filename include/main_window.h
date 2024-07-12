#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include <opencv2/opencv.hpp>
#include <mutex>
#include <vector>
#include <chrono>

class MainWindow
{
public:
    MainWindow();
    ~MainWindow();
    void displayMainWindow();

private:
    static void error_callback(int error, const char *description);
    static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void resize_callback(GLFWwindow *window, int new_width, int new_height);
    void initOpenGL(int w, int h);
    void initGraphics();
    GLuint matToTexture(const cv::Mat &mat, GLenum minFilter, GLenum magFilter, GLenum wrapFilter);
    void drawFrame(const cv::Mat &frame);
    void StartWebcamThread();

    void videoSettings();
    void loadFrame();
    void initTexture(cv::Mat &image, GLuint &imageTexture);
    void BindCVMat2GLTexture(cv::Mat &image, GLuint &imageTexture);
    void ShowWebcam(ImFont *NunitoSans);

    void imguiStyles();
    void WelcomeScreen(ImFont *fontawsome, ImFont *NunitoSans, ImFont *BigNunitoSans, ImFont *SmallNunitoSans);
    void ModeSetupScreen(ImFont *fontawsome, ImFont *NunitoSans, ImFont *BigNunitoSans, ImFont *SmallNunitoSans);
    void CameraSetupScreen(ImFont *fontawsome, ImFont *NunitoSans, ImFont *BigNunitoSans, ImFont *SmallNunitoSans);
    void AlertSetupScreen(ImFont *fontawsome, ImFont *NunitoSans, ImFont *BigNunitoSans, ImFont *SmallNunitoSans);
    void TresspassingScreen(ImFont *fontawsome, ImFont *NunitoSans, ImFont *BigNunitoSans, ImFont *SmallNunitoSans);

    static int window_width;
    static int window_height;
    GLFWwindow *window;
    cv::VideoCapture cap;
    cv::Mat frame;
    std::mutex frameMutex;
};

#endif