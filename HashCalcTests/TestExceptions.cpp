#include "PCH.hpp"
#include "HashCalcException.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(ExceptionTests)
{
public:

	TEST_METHOD(Message)
	{
		{
			const HashCalcException ex(L"X", 0xC0E7000B);
			Assert::AreEqual("X failed. Description: There were not enough physical disks to complete the requested operation.", ex.what());
			Assert::AreEqual(int(0xC0E7000B), ex.Code);
		}
		Assert::AreEqual("STATUS_SUCCESS", HashCalcException(L"SNAFU", 0).what());
	}
};
