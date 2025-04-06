#pragma once
#include <windows.h>

class Timer {
public:
    Timer();
    void Start();
    float GetDeltaTime();

private:
    LARGE_INTEGER frequency{};
    LARGE_INTEGER prevTime{};
    bool initialized;
};