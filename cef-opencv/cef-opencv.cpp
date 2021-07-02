#include <windows.h>

#include <thread>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "cefsimple/simple_app.h"
#include "include/cef_command_line.h"
#include "include/cef_sandbox_win.h"

using namespace cv;
using namespace std;

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                      LPTSTR lpCmdLine, int nCmdShow) {
  // Enable High-DPI support on Windows 7 or newer.
  CefEnableHighDPISupport();

  // Provide CEF with command-line arguments.
  CefMainArgs main_args(hInstance);

  // CEF applications have multiple sub-processes (render, plugin, GPU, etc)
  // that share the same executable. This function checks the command-line and,
  // if this is a sub-process, executes the appropriate logic.
  int exit_code = CefExecuteProcess(main_args, nullptr, nullptr);
  if (exit_code >= 0) {
    // The sub-process has completed so return here.
    return exit_code;
  }

  // Parse command-line arguments for use in this method.
  CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
  command_line->InitFromString(::GetCommandLineW());

  // Specify CEF global settings here.
  CefSettings settings;

  if (command_line->HasSwitch("enable-chrome-runtime")) {
    // Enable experimental Chrome runtime. See issue #2969 for details.
    settings.chrome_runtime = true;
  }

  settings.no_sandbox = true;

  // SimpleApp implements application-level callbacks for the browser process.
  // It will create the first browser instance in OnContextInitialized() after
  // CEF has initialized.

  auto lCefFunction = [&]() -> void {
    CefRefPtr<SimpleApp> app(new SimpleApp);

    // Initialize CEF.
    CefInitialize(main_args, settings, app.get(), nullptr);

    // Run the CEF message loop. This will block until CefQuitMessageLoop() is
    // called.
    CefRunMessageLoop();

    // Shut down CEF.
    CefShutdown();
  };

  std::thread lThread = std::thread(lCefFunction);

  // Open the default video camera
  VideoCapture cap(0);

  // if not success, exit program
  if (cap.isOpened() == false) {
    return -1;
  }

  double dWidth =
      cap.get(CAP_PROP_FRAME_WIDTH);  // get the width of frames of the video
  double dHeight =
      cap.get(CAP_PROP_FRAME_HEIGHT);  // get the height of frames of the video

  string window_name = "My Camera Feed";
  namedWindow(window_name);  // create a window called "My Camera Feed"

  while (true) {
    Mat frame;
    bool bSuccess = cap.read(frame);  // read a new frame from video

    // Breaking the while loop if the frames cannot be captured
    if (bSuccess == false) {
      break;
    }

    // show the frame in the created window
    imshow(window_name, frame);

    // wait for for 10 ms until any key is pressed.
    // If the 'Esc' key is pressed, break the while loop.
    // If the any other key is pressed, continue the loop
    // If any key is not pressed withing 10 ms, continue the loop
    if (waitKey(10) == 27) {
      break;
    }
  }
  return 0;
}
