#include "functionhandler.h"

bool FunctionHandler::Execute(const CefString& name,
                              CefRefPtr<CefV8Value> object,
                              const CefV8ValueList& arguments,
                              CefRefPtr<CefV8Value>& retval,
                              CefString& exception) {
    if (name != "getFrame") {
        return false;
    }

    // Return my string value.

    unsigned char* lBuffer = new unsigned char[mFrame.size()];

    for (size_t index = 0; index < mFrame.size(); ++index) {
        lBuffer[index] = mFrame[index];
    }

    retval = CefV8Value::CreateArrayBuffer(lBuffer, mFrame.size(), this);
    // retval = CefV8Value::CreateString("asdf");

    return true;
}

void FunctionHandler::SetFrame(const std::vector<uint8_t>& aFrame) {
    mFrame = aFrame;
}
