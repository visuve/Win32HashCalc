#include "PCH.hpp"
#include "StringConversion.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(StringConversionTests)
{
public:
		
	TEST_METHOD(ToUtf8)
	{
		const std::vector<uint8_t> result = StringConversion::ToUtf8ByteArray(L"ABC");
		Assert::AreEqual(result.size(), size_t(3));
		Assert::AreEqual(result[0], uint8_t(0x41));
		Assert::AreEqual(result[1], uint8_t(0x42));
		Assert::AreEqual(result[2], uint8_t(0x43));
	}

	TEST_METHOD(Join)
	{
		std::vector<std::wstring> elements = { L"hydrogen", L"helium", L"lithium", L"beryllium" };
		Assert::AreEqual(StringConversion::Join(elements).c_str(), L"hydrogen, helium, lithium & beryllium");
	}
};
