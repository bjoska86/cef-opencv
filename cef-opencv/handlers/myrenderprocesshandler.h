#pragma once

#include "include/cef_render_process_handler.h"
#include "functionhandler.h"

class MyRenderProcessHandler : public CefRenderProcessHandler {
public:
    void OnContextCreated(CefRefPtr<CefBrowser> browser,
                          CefRefPtr<CefFrame> frame,
                          CefRefPtr<CefV8Context> context) override;

    bool OnProcessMessageReceived(
        CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
        CefProcessId source_process,
        CefRefPtr<CefProcessMessage> message) override;

private:
    CefRefPtr<FunctionHandler> mFunctionHandler;

private:
    IMPLEMENT_REFCOUNTING(MyRenderProcessHandler);
};