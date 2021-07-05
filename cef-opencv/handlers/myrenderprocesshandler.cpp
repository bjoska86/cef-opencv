#include <vector>
#include "myrenderprocesshandler.h"
#include "functionhandler.h"

void MyRenderProcessHandler::OnContextCreated(CefRefPtr<CefBrowser> browser,
                                              CefRefPtr<CefFrame> frame,
                                              CefRefPtr<CefV8Context> context) {
    CefRefPtr<CefV8Value> lWindow = context->GetGlobal();

    // Create an instance of my CefV8Handler object.

    mFunctionHandler = new FunctionHandler();

    // Create the "myfunc" function.
    CefRefPtr<CefV8Value> lGetFrameFunction =
        CefV8Value::CreateFunction("getFrame", mFunctionHandler);

    lWindow->SetValue("getFrame", lGetFrameFunction,
                      V8_PROPERTY_ATTRIBUTE_NONE);
}

bool MyRenderProcessHandler::OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
    CefProcessId source_process, CefRefPtr<CefProcessMessage> message) {
    if (message->GetName() != "OnFrame") {
        return true;
    }

    CefRefPtr<CefListValue> lArgs = message->GetArgumentList();
    if (lArgs->GetSize() > 1 || lArgs->GetSize() == 0) {
        return false;
    }

    if (lArgs->GetType(0) != VTYPE_BINARY) {
        return true;
    }

    CefRefPtr<CefBinaryValue> lFrameArray = lArgs->GetBinary(0);
    if (lFrameArray == nullptr) {
        return true;
    }
    std::vector<uint8_t> lFrame;
    lFrame.resize(lFrameArray->GetSize());
    lFrameArray->GetData(lFrame.data(), lFrameArray->GetSize(), 0);
    if (mFunctionHandler == nullptr) {
        return true;
    }
    mFunctionHandler->SetFrame(lFrame);

    return true;
}
