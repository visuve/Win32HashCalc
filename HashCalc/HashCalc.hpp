#pragma once

class HashCalc
{
private:
	size_t GetPropSize(const std::wstring& property);

public:
	HashCalc(const std::wstring& algorithmName);
	~HashCalc();

	std::wstring CalculateChecksum(std::vector<uint8_t>& data);
	std::wstring CalculateChecksum(const std::filesystem::path& path);
	std::wstring CalculateChecksum(const std::wstring& data);

private:
	void Update(std::vector<uint8_t>& data);
	void Finish();
	std::wstring HashString();

	BCRYPT_ALG_HANDLE _algorithmHandle = nullptr;
	BCRYPT_HASH_HANDLE _hashHandle = nullptr;
	std::vector<uint8_t> _hashObject;
	std::vector<uint8_t> _hashData;
};