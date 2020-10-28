#include "PCH.hpp"
#include "StringConversion.hpp"

namespace StringConversion
{
	std::string ToUtf8(const std::wstring& unicode)
	{
		std::string utf8;

		int required = WideCharToMultiByte(
			CP_UTF8,
			0,
			unicode.c_str(),
			static_cast<int>(unicode.length()),
			nullptr,
			0,
			nullptr,
			nullptr);

		if (required > 0)
		{
			utf8.resize(static_cast<size_t>(required));

			required = WideCharToMultiByte(
				CP_UTF8,
				0,
				unicode.c_str(),
				static_cast<int>(unicode.length()),
				utf8.data(),
				required,
				nullptr,
				nullptr);
		}

		return utf8;
	}

	std::vector<uint8_t> ToUtf8ByteArray(const std::wstring& unicode)
	{
		const std::string utf8 = ToUtf8(unicode);
		return { utf8.cbegin(), utf8.cend() };
	}
}