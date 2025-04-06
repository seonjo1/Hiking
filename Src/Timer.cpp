#include "Timer.h"

Timer::Timer() : initialized(false) {}

void Timer::Start() {
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&prevTime);
    initialized = true;
}

float Timer::GetDeltaTime() {
    if (!initialized) {
        Start(); // 자동 초기화
        return 0.0f;
    }

    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime);

    float delta = static_cast<float>(currentTime.QuadPart - prevTime.QuadPart) / frequency.QuadPart;
    prevTime = currentTime;
    return delta;
}
