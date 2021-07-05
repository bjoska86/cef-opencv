#pragma once

#include <vector>
#include "include/cef_v8.h"

class FunctionHandler : public CefV8Handler,
                        public CefV8ArrayBufferReleaseCallback {
public:
    bool Execute(const CefString& name, CefRefPtr<CefV8Value> object,
                 const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval,
                 CefString& exception) override;

public:
    void SetFrame(const std::vector<uint8_t>& aFrame);

    void ReleaseBuffer(void* buffer) { delete[] buffer; };

private:
    std::vector<uint8_t> mFrame;

private:
    IMPLEMENT_REFCOUNTING(FunctionHandler);
};