#pragma once

namespace StringConversion
{
	std::string ToUtf8(std::wstring_view unicode);
	std::vector<uint8_t> ToUtf8ByteArray(std::wstring_view unicode);

	template<typename T, typename C>
	std::basic_string<C> Join(
		const T& array,
		std::basic_string_view<C> separator,
		std::basic_string_view<C> lastSeparator)
	{
		std::basic_string<C> joined;

		for (size_t i = 0; i < array.size(); ++i)
		{
			joined += array[i];

			if (i + 2 < array.size())
			{
				joined += separator;
				continue;
			}

			if (i + 1 < array.size())
			{
				joined += lastSeparator;
				continue;
			}
		}

		return joined;
	}

	template<typename T>
	std::wstring Join(const T& array)
	{
		return Join<T, wchar_t>(array, L", ", L" & ");
	}
}