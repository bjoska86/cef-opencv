#include <opencv2/opencv.hpp>
#include <string>
#include "opencvhandler.h"

using namespace cv;
using namespace std;

OpenCVHandler::OpenCVHandler(IFrameHandler& aFrameHandler)
    : mFrameHandler(aFrameHandler) {}

void OpenCVHandler::StartGrab() {  // Open the default video camera
    auto lGrabFunction = [&]() -> void {
        mCap = VideoCapture(0);

        if (mCap.isOpened() == false) {
            return;
        }

        mCap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
        mCap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

        std::string window_name = "Camera";
        namedWindow(window_name);
        mIsRunning = true;
        while (mIsRunning) {
            Mat frame;
            bool bSuccess = mCap.read(frame);

            if (bSuccess == false) {
                mIsRunning = false;
                break;
            }

            imshow(window_name, frame);

            std::vector<uint8_t> lOutputFrame;
            const bool lConvertSuccess =
                cv::imencode(".jpeg", frame, lOutputFrame);
            if (lConvertSuccess) {
                mFrameHandler.OnFrame(lOutputFrame);
            } else {
                break;
            }

            if (waitKey(10) == 27) {
                mIsRunning = false;
            }
        }
    };

    mIsRunning = true;
    mGrabber = std::thread(lGrabFunction);
}

void OpenCVHandler::StopGrab() {
    mIsRunning = false;
    if (mGrabber.joinable()) {
        mGrabber.join();
    }
}
