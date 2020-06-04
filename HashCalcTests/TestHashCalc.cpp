#include "PCH.hpp"
#include "HashCalc.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

TEST_CLASS(HashCalcTests)
{
public:
	TEST_METHOD(StringChecksumSame)
	{
		Assert::AreEqual(L"a9046c73e00331af68917d3804f70655", HashCalc(BCRYPT_MD2_ALGORITHM).CalculateChecksum(std::wstring(L"hello")).c_str());
		Assert::AreEqual(L"866437cb7a794bce2b727acc0362ee27", HashCalc(BCRYPT_MD4_ALGORITHM).CalculateChecksum(std::wstring(L"hello")).c_str());
		Assert::AreEqual(L"5d41402abc4b2a76b9719d911017c592", HashCalc(BCRYPT_MD5_ALGORITHM).CalculateChecksum(std::wstring(L"hello")).c_str());
		Assert::AreEqual(L"aaf4c61ddcc5e8a2dabede0f3b482cd9aea9434d", HashCalc(BCRYPT_SHA1_ALGORITHM).CalculateChecksum(std::wstring(L"hello")).c_str());
		Assert::AreEqual(L"2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824", HashCalc(BCRYPT_SHA256_ALGORITHM).CalculateChecksum(std::wstring(L"hello")).c_str());
		Assert::AreEqual(L"59e1748777448c69de6b800d7a33bbfb9ff1b463e44354c3553bcdb9c666fa90125a3c79f90397bdf5f6a13de828684f", HashCalc(BCRYPT_SHA384_ALGORITHM).CalculateChecksum(std::wstring(L"hello")).c_str());
		Assert::AreEqual(L"9b71d224bd62f3785d96d46ad3ea3d73319bfbc2890caadae2dff72519673ca72323c3d99ba5c11d7c7acc6e14b8c5da0c4663475c2e5c3adef46f73bcdec043", HashCalc(BCRYPT_SHA512_ALGORITHM).CalculateChecksum(std::wstring(L"hello")).c_str());
	}

	TEST_METHOD(StringChecksumAlternating)
	{
		Assert::AreEqual(L"80a4442c57b9ae254e262ba19c1c832c", HashCalc(BCRYPT_MD2_ALGORITHM).CalculateChecksum(std::wstring(L"hydrogen")).c_str());
		Assert::AreEqual(L"61bd12ec5d9d13a2956d8e0a33be5bdd", HashCalc(BCRYPT_MD4_ALGORITHM).CalculateChecksum(std::wstring(L"helium")).c_str());
		Assert::AreEqual(L"42cacb235328b80cd4548840e57f89b6", HashCalc(BCRYPT_MD5_ALGORITHM).CalculateChecksum(std::wstring(L"lithium")).c_str());
		Assert::AreEqual(L"bada19a17b86234ca21377e07317132fadfb853f", HashCalc(BCRYPT_SHA1_ALGORITHM).CalculateChecksum(std::wstring(L"beryllium")).c_str());
		Assert::AreEqual(L"25b049a5984206dd6fe6eca9c02b468677615a568855010013d4960873c86527", HashCalc(BCRYPT_SHA256_ALGORITHM).CalculateChecksum(std::wstring(L"boron")).c_str());
		Assert::AreEqual(L"c0df5e67ee6a34d2cee9f6c80d45c8e136ff363404027db863cc301acd4bf01aedfe8ea408dc3654bab4689db94f4585", HashCalc(BCRYPT_SHA384_ALGORITHM).CalculateChecksum(std::wstring(L"carbon")).c_str());
		Assert::AreEqual(L"44e75d435bf4e8c41552a61bc9df08baede971a4470ec79e99408f8a4644e393dc56f7d7a1001a075e763e8773a368ed5a960323f3ff050d0e7f1d4cd237549c", HashCalc(BCRYPT_SHA512_ALGORITHM).CalculateChecksum(std::wstring(L"nitrogen")).c_str());
	}
};