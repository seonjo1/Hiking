#include "Common.h"

std::wstring ConvertToWString(const char* str)
{
	// ACP949 -> �����ڵ�� ��ȯ (���̸� Ȯ��)
	int len = MultiByteToWideChar(CP_ACP, 0, str, -1, nullptr, 0);
	if (len == 0) return L"";

	// ACP949 -> �����ڵ�� ��ȯ (result�� ����)
	std::wstring result(len, L'\0');
	MultiByteToWideChar(CP_ACP, 0, str, -1, &result[0], len);

	// ���� wstring�� '\0'�� �־��µ� ���� �����Ҷ� '\0'�� �� �����Ƿ� 1�� ����
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