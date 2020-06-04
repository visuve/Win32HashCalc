#pragma once

namespace StringConversion
{
	std::string ToUtf8(const std::wstring& unicode);
	std::vector<uint8_t> ToUtf8ByteArray(const std::wstring& unicode);

	template<typename T, typename C>
	std::basic_string<C> Join(
		const T& array, 
		const std::basic_string<C>& separator, 
		const std::basic_string<C>& lastSeparator)
	{
		std::basic_string<C> joined;

		for (size_t i = 0; i < array.size(); ++i)
		{
			if (i < array.size() - 2)
			{
				joined += array[i] + separator;
				continue;
			}

			if (i < array.size() - 1)
			{
				joined += array[i] + lastSeparator;
				continue;
			}

			joined += array[i];
		}

		return joined;
	}

	template<typename T>
	std::wstring Join(const T& array)
	{
		return Join<T, wchar_t>(array, L", ", L" & ");
	}
}