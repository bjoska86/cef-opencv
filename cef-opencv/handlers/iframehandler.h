#pragma once
#include <vector>

class IFrameHandler {
public:
    IFrameHandler() = default;
    virtual ~IFrameHandler() = default;

public:
    virtual void OnFrame(const std::vector<uint8_t> &aFrame) = 0;
};
