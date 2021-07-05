#include "mybrowserprocesshandler.h"

#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_helpers.h"
#include "../cefsimple/simple_handler.h"
#include "../handlers/myrenderprocesshandler.h"
#include "include/cef_base.h"
#include "include/base/cef_bind.h"
#include "include/wrapper/cef_closure_task.h"

namespace {

// When using the Views framework this object provides the delegate
// implementation for the CefWindow that hosts the Views-based browser.
class SimpleWindowDelegate : public CefWindowDelegate {
public:
    explicit SimpleWindowDelegate(CefRefPtr<CefBrowserView> browser_view)
        : browser_view_(browser_view) {}

    void OnWindowCreated(CefRefPtr<CefWindow> window) OVERRIDE {
        // Add the browser view and show the window.
        window->AddChildView(browser_view_);
        window->Show();

        // Give keyboard focus to the browser view.
        browser_view_->RequestFocus();
    }

    void OnWindowDestroyed(CefRefPtr<CefWindow> window) OVERRIDE {
        browser_view_ = nullptr;
    }

    bool CanClose(CefRefPtr<CefWindow> window) OVERRIDE {
        // Allow the window to close if the browser says it's OK.
        CefRefPtr<CefBrowser> browser = browser_view_->GetBrowser();
        if (browser) return browser->GetHost()->TryCloseBrowser();
        return true;
    }

    CefSize GetPreferredSize(CefRefPtr<CefView> view) OVERRIDE {
        return CefSize(1024, 768);
    }

private:
    CefRefPtr<CefBrowserView> browser_view_;

    IMPLEMENT_REFCOUNTING(SimpleWindowDelegate);
    DISALLOW_COPY_AND_ASSIGN(SimpleWindowDelegate);
};

class SimpleBrowserViewDelegate : public CefBrowserViewDelegate {
public:
    SimpleBrowserViewDelegate() {}

    bool OnPopupBrowserViewCreated(CefRefPtr<CefBrowserView> browser_view,
                                   CefRefPtr<CefBrowserView> popup_browser_view,
                                   bool is_devtools) OVERRIDE {
        // Create a new top-level Window for the popup. It will show itself
        // after creation.
        CefWindow::CreateTopLevelWindow(
            new SimpleWindowDelegate(popup_browser_view));

        // We created the Window.
        return true;
    }

private:
    IMPLEMENT_REFCOUNTING(SimpleBrowserViewDelegate);
    DISALLOW_COPY_AND_ASSIGN(SimpleBrowserViewDelegate);
};

}  // namespace

MyBrowserProcessHandler::MyBrowserProcessHandler() : mOpenCVHandler(*this) {}

void MyBrowserProcessHandler::OnContextInitialized() {
    CEF_REQUIRE_UI_THREAD();

    CefRefPtr<CefCommandLine> command_line =
        CefCommandLine::GetGlobalCommandLine();

    // Create the browser using the Views framework if "--use-views" is
    // specified via the command-line. Otherwise, create the browser using the
    // native platform framework.

    // SimpleHandler implements browser-level callbacks.
    CefRefPtr<SimpleHandler> handler(new SimpleHandler(true));

    // Specify CEF browser settings here.
    CefBrowserSettings browser_settings;

    std::string url;

    // Check if a "--url=" value was provided via the command-line. If so, use
    // that instead of the default URL.
    url = command_line->GetSwitchValue("url");
    url = "file:///index.html";

    // Create the BrowserView.
    CefRefPtr<CefBrowserView> browser_view = CefBrowserView::CreateBrowserView(
        handler, url, browser_settings, nullptr, nullptr,
        new SimpleBrowserViewDelegate());

    // Create the Window. It will show itself after creation.
    CefWindow::CreateTopLevelWindow(new SimpleWindowDelegate(browser_view));
    mOpenCVHandler.StartGrab();
}

CefRefPtr<CefClient> MyBrowserProcessHandler::GetDefaultClient() {
    return SimpleHandler::GetInstance();
}

void MyBrowserProcessHandler::OnFrame(const std::vector<uint8_t>& aFrame) {
    CefPostTask(TID_UI, base::Bind(&MyBrowserProcessHandler::SendFrameToRender,
                                   this, aFrame));
}

void MyBrowserProcessHandler::SendFrameToRender(
    const std::vector<uint8_t>& aFrame) {
    CefRefPtr<CefProcessMessage> lMessage =
        CefProcessMessage::Create("OnFrame");

    CefRefPtr<CefListValue> args = lMessage->GetArgumentList();

    CefRefPtr<CefBinaryValue> lFrame =
        CefBinaryValue::Create(aFrame.data(), aFrame.size());

    args->SetBinary(0, lFrame);

    CefRefPtr<CefBrowser> lBrowser = SimpleHandler::GetInstance()->mBrowser;
    if (lBrowser == nullptr) {
        return;
    }
    lBrowser->GetMainFrame()->SendProcessMessage(PID_RENDERER, lMessage);
}
