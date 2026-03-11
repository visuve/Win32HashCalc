#pragma once

class HashCalc
{
private:
	size_t GetPropSize(std::wstring_view property);

public:
	HashCalc(std::wstring_view algorithmName);
	~HashCalc();

	std::wstring CalculateChecksum(std::span<uint8_t> data);
	std::wstring CalculateChecksum(std::wstring_view data);
	std::wstring CalculateChecksumFromFile(const std::filesystem::path& path);
	std::map<std::filesystem::path, std::wstring> CalculateChecksumFromFolder(const std::filesystem::path& path);

private:
	void Update(std::span<uint8_t> data);
	void Finish();
	std::wstring HashString() const;

	BCRYPT_ALG_HANDLE _algorithmHandle = nullptr;
	BCRYPT_HASH_HANDLE _hashHandle = nullptr;
	std::vector<uint8_t> _hashData;
};