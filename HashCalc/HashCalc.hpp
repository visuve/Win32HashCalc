#pragma once

class HashCalc
{
private:
	size_t GetPropSize(std::wstring_view property);

public:
	HashCalc(std::wstring_view algorithmName);
	~HashCalc();

	std::wstring CalculateChecksum(std::vector<uint8_t>& data);
	std::wstring CalculateChecksumFrom(const std::filesystem::path& path);
	std::wstring CalculateChecksum(std::wstring_view data);

private:
	void Update(std::vector<uint8_t>& data);
	void Finish();
	std::wstring HashString();

	BCRYPT_ALG_HANDLE _algorithmHandle = nullptr;
	BCRYPT_HASH_HANDLE _hashHandle = nullptr;
	std::vector<uint8_t> _hashObject;
	std::vector<uint8_t> _hashData;
};