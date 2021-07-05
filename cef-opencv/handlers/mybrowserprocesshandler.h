#pragma once

#include "include/cef_browser_process_handler.h"
#include "opencvhandler.h"
#include "iframehandler.h"

class MyBrowserProcessHandler : public CefBrowserProcessHandler,
                                public IFrameHandler {
public:
    MyBrowserProcessHandler();

public:
    void OnContextInitialized() override;
    CefRefPtr<CefClient> GetDefaultClient() override;

public:
    void OnFrame(const std::vector<uint8_t>& aFrame) override;

private:
    OpenCVHandler mOpenCVHandler;
    CefRefPtr<CefListValue> mFrameArray = nullptr;

private:
    void SendFrameToRender(const std::vector<uint8_t>& aFrame);

private:
    IMPLEMENT_REFCOUNTING(MyBrowserProcessHandler);
};