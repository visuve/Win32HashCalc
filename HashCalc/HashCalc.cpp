#include "PCH.hpp"
#include "HashCalc.hpp"
#include "HashCalcException.hpp"
#include "StringConversion.hpp"

size_t HashCalc::GetPropSize(const std::wstring& property)
{
	DWORD object = 0;
	DWORD bytesWritten = 0;

	const NTSTATUS status = BCryptGetProperty(
		_algorithmHandle,
		property.c_str(),
		reinterpret_cast<PUCHAR>(&object),
		sizeof(DWORD),
		&bytesWritten,
		0);

	if (status != 0 || object == 0 || bytesWritten != sizeof(DWORD))
	{
		const std::string message = 
			"BCryptGetProperty(" + StringConversion::ToUtf8(property) + ')';
		throw std::exception(message.c_str(), status);
	}

	return object;
}

HashCalc::HashCalc(const std::wstring& algorithmName)
{
	// Create provider
	{
		const NTSTATUS status = BCryptOpenAlgorithmProvider(
			&_algorithmHandle,
			algorithmName.c_str(),
			nullptr,
			0);

		if (status != 0)
		{
			throw HashCalcException(L"BCryptOpenAlgorithmProvider", status);
		}
	}

	// Create hash object
	{
		_hashObject.resize(GetPropSize(BCRYPT_OBJECT_LENGTH));

		const NTSTATUS status = BCryptCreateHash(
			_algorithmHandle,
			&_hashHandle,
			_hashObject.data(),
			static_cast<ULONG>(_hashObject.size()),
			nullptr,
			0,
			0);

		if (status != 0)
		{
			throw HashCalcException(L"BCryptCreateHash", status);
		}
	}

	// Prepare hash data
	_hashData.resize(GetPropSize(BCRYPT_HASH_LENGTH));
}

HashCalc::~HashCalc()
{
	if (_algorithmHandle)
	{
		BCryptCloseAlgorithmProvider(_algorithmHandle, 0);
		_algorithmHandle = nullptr;
	}

	if (_hashHandle)
	{
		BCryptDestroyHash(_hashHandle);
		_hashHandle = nullptr;
	}
}

std::wstring HashCalc::CalculateChecksum(std::vector<uint8_t>& data)
{
	if (_algorithmHandle == nullptr || _hashHandle == nullptr)
	{
		return {};
	}

	Update(data);
	Finish();

	return HashString();
}

std::wstring HashCalc::CalculateChecksum(const std::filesystem::path& path)
{
	std::basic_ifstream<uint8_t> file(path, std::ios::in | std::ios::binary);

	if (!file)
	{
		return {};
	}

	file.exceptions(std::istream::failbit | std::istream::badbit);

	std::vector<uint8_t> buffer(0x400);
	uint64_t bytesLeft = std::filesystem::file_size(path);

	while (bytesLeft && file)
	{
		if (buffer.size() > bytesLeft)
		{
			buffer.resize(bytesLeft);
		}

		file.read(buffer.data(), buffer.size());
		bytesLeft -= buffer.size();

		Update(buffer);
	}

	Finish();

	return HashString();
}

std::wstring HashCalc::CalculateChecksum(const std::wstring& data)
{
	std::vector<uint8_t> ba = StringConversion::ToUtf8ByteArray(data);
	return CalculateChecksum(ba);
}

void HashCalc::Update(std::vector<uint8_t>& data)
{
	if (_algorithmHandle == nullptr || _hashHandle == nullptr)
	{
		return;
	}

	const NTSTATUS status = BCryptHashData(
		_hashHandle,
		data.data(),
		static_cast<ULONG>(data.size()),
		0);

	if (status != 0)
	{
		throw HashCalcException(L"BCryptHashData", status);
	}
}

void HashCalc::Finish()
{
	if (_algorithmHandle == nullptr || _hashHandle == nullptr)
	{
		return;
	}

	const NTSTATUS status = BCryptFinishHash(
		_hashHandle,
		_hashData.data(),
		static_cast<ULONG>(_hashData.size()),
		0);

	if (status != 0)
	{
		throw HashCalcException(L"BCryptFinishHash", status);
	}
}

std::wstring HashCalc::HashString()
{
	std::wstringstream ss;
	ss << std::hex << std::setfill(L'0');

	for (int x : _hashData)
	{
		ss << std::setw(2) << x;
	}

	return ss.str();
}
