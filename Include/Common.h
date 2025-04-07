#pragma once

#include <windows.h>
#include <string>
#include <DirectXMath.h>

using namespace DirectX;

std::wstring ConvertToWString(const char* str);
void p(std::string s);
void printMatrix(const XMMATRIX& mat);
void printFloat3(const XMFLOAT3& v);
void printFloat4(const XMFLOAT4& v);