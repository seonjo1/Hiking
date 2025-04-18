#include "Common.h"

std::wstring ConvertToWString(const char* str)
{
	// ACP949 -> 유니코드로 변환 (길이만 확인)
	int len = MultiByteToWideChar(CP_ACP, 0, str, -1, nullptr, 0);
	if (len == 0) return L"";

	// ACP949 -> 유니코드로 변환 (result에 복사)
	std::wstring result(len, L'\0');
	MultiByteToWideChar(CP_ACP, 0, str, -1, &result[0], len);

	// 기존 wstring에 '\0'이 있었는데 값을 복사할때 '\0'이 또 들어오므로 1개 삭제
	result.pop_back();
	return result;
}

void p(std::string s)
{
	return;
	OutputDebugStringA(s.c_str());
}

void printMatrix(const XMMATRIX& mat)
{
	std::string s;
	for (int i = 0; i < 4; ++i)
	{
		XMVECTOR row = mat.r[i];
		float f[4];
		XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(f), row);
		s += std::to_string(f[0]) + " " +
			std::to_string(f[1]) + " " +
			std::to_string(f[2]) + " " +
			std::to_string(f[3]) + "\n";
	}

	p(s);
}

void printFloat3(const XMFLOAT3& v)
{
	std::string s;
	s += std::to_string(v.x) + " " +
		std::to_string(v.y) + " " +
		std::to_string(v.z) + "\n";
	p(s);
}

void printFloat4(const XMFLOAT4& v)
{
	std::string s;
	s += std::to_string(v.x) + " " +
		std::to_string(v.y) + " " +
		std::to_string(v.z) + " " +
		std::to_string(v.w) + "\n";
	p(s);
}