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
