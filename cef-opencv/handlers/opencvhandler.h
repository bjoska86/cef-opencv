#pragma once

#include <atomic>
#include <thread>
#include <opencv2/opencv.hpp>

#include "iframehandler.h"

class OpenCVHandler {
public:
    OpenCVHandler(IFrameHandler& aFrameHandler);

public:
    void StartGrab();
    void StopGrab();

    ~OpenCVHandler() { StopGrab(); }

private:
    IFrameHandler& mFrameHandler;

private:
    cv::VideoCapture mCap;

private:
    std::atomic<bool> mIsRunning = false;
    std::thread mGrabber;
};