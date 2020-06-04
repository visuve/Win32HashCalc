#include "PCH.hpp"

std::string ToUtf8(const std::wstring& unicode)
{
	std::string utf8;

	int required = WideCharToMultiByte(CP_UTF8, 0, unicode.c_str(), static_cast<int>(unicode.length()), nullptr, 0, nullptr, nullptr);

	if (required > 0)
	{
		utf8.resize(static_cast<size_t>(required));
		required = WideCharToMultiByte(CP_UTF8, 0, unicode.c_str(), static_cast<int>(unicode.length()), utf8.data(), required, nullptr, nullptr);
	}

	return utf8;
}

std::vector<uint8_t> ToUtf8ByteArray(const std::wstring& unicode)
{
	const std::string utf8 = ToUtf8(unicode);
	return { utf8.cbegin(), utf8.cend() };
}

class HashCalcException : public std::exception
{
public:
	HashCalcException(const std::wstring& what, const NTSTATUS status) :
		std::exception(Format(what, status).c_str()),
		Code(status)
	{
	}

	const int Code;

private:
	std::string Format(const std::wstring& what, const NTSTATUS status)
	{
		std::wstring message = what + L" failed.";

		std::array<wchar_t, 0x400> buffer;
		DWORD size = FormatMessageW(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			nullptr,
			status,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			buffer.data(),
			static_cast<DWORD>(buffer.size()),
			nullptr);

		if (size > 2)
		{
			message += L" Description: " + std::wstring(buffer.data(), size - 2); // Trim excess /r/n
		}

		return ToUtf8(message);
	}
};

class HashCalc
{
private:
	size_t GetPropSize(const std::wstring& property)
	{
		DWORD object = 0;
		DWORD bytesWritten = 0;
		const NTSTATUS status = BCryptGetProperty(_algorithmHandle, property.c_str(), reinterpret_cast<PUCHAR>(&object), sizeof(DWORD), &bytesWritten, 0);

		if (status != 0 || object == 0 || bytesWritten != sizeof(DWORD))
		{
			const std::string message = "BCryptGetProperty(" + ToUtf8(property) + ')';
			throw std::exception(message.c_str(), status);
		}

		return object;
	}

public:
	HashCalc(const std::wstring& algorithmName)
	{
		// Create provider
		{
			NTSTATUS status = BCryptOpenAlgorithmProvider(&_algorithmHandle, algorithmName.c_str(), nullptr, 0);

			if (status != 0)
			{
				throw HashCalcException(L"BCryptOpenAlgorithmProvider", status);
			}
		}

		// Create hash object
		{
			_hashObject.resize(GetPropSize(BCRYPT_OBJECT_LENGTH));

			NTSTATUS status = BCryptCreateHash(_algorithmHandle, &_hashHandle, _hashObject.data(), static_cast<ULONG>(_hashObject.size()), nullptr, 0, 0);

			if (status != 0)
			{
				throw HashCalcException(L"BCryptCreateHash", status);
			}
		}

		// Prepare hash data
		_hashData.resize(GetPropSize(BCRYPT_HASH_LENGTH));
	}

	~HashCalc()
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

	std::string CalculateChecksum(std::vector<uint8_t>& data)
	{
		if (_algorithmHandle == nullptr || _hashHandle == nullptr)
		{
			return {};
		}

		Update(data);
		Finish();

		return HashString();
	}

	std::string CalculateChecksum(const std::filesystem::path& path)
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

	std::string CalculateChecksum(const std::wstring& data)
	{
		std::vector<uint8_t> ba = ToUtf8ByteArray(data);
		return CalculateChecksum(ba);
	}

private:
	void Update(std::vector<uint8_t>& data)
	{
		if (_algorithmHandle == nullptr || _hashHandle == nullptr)
		{
			return;
		}

		NTSTATUS status = BCryptHashData(_hashHandle, data.data(), static_cast<ULONG>(data.size()), 0);

		if (status != 0)
		{
			throw HashCalcException(L"BCryptHashData", status);
		}
	}

	void Finish()
	{
		if (_algorithmHandle == nullptr || _hashHandle == nullptr)
		{
			return;
		}

		NTSTATUS status = BCryptFinishHash(_hashHandle, _hashData.data(), static_cast<ULONG>(_hashData.size()), 0);

		if (status != 0)
		{
			throw HashCalcException(L"BCryptFinishHash", status);
		}
	}

	std::string HashString()
	{
		std::stringstream ss;
		ss << std::hex << std::setw(2) << std::setfill('0');

		for (int x : _hashData)
		{
			ss << x;
		}

		return ss.str();
	}

	BCRYPT_ALG_HANDLE _algorithmHandle = nullptr;
	BCRYPT_HASH_HANDLE _hashHandle = nullptr;
	std::vector<uint8_t> _hashObject;
	std::vector<uint8_t> _hashData;
};

bool IsReadableFile(const std::wstring& value)
{
	try
	{
		const std::filesystem::path path(value);
		const std::filesystem::file_status status = std::filesystem::status(path);

		if (status.type() != std::filesystem::file_type::regular)
		{
			return false;
		}

		const std::filesystem::perms perms = status.permissions();

		if ((perms & std::filesystem::perms::owner_read) == std::filesystem::perms::none ||
			(perms & std::filesystem::perms::group_read) == std::filesystem::perms::none ||
			(perms & std::filesystem::perms::others_read) == std::filesystem::perms::none)
		{
			return false;
		}

		return true;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return false;
}

const std::array<std::wstring, 7> SupportedAlgorithms =
{
	BCRYPT_MD2_ALGORITHM,
	BCRYPT_MD4_ALGORITHM,
	BCRYPT_MD5_ALGORITHM,
	BCRYPT_SHA1_ALGORITHM,
	BCRYPT_SHA256_ALGORITHM,
	BCRYPT_SHA384_ALGORITHM,
	BCRYPT_SHA512_ALGORITHM
};

template<typename T>
std::wstring Join(const T& array)
{
	std::wstring joined;	
	
	for (size_t i = 0; i < array.size(); ++i)
	{ 
		if (i < array.size() - 2)
		{
			joined += array[i] + L", ";
			continue;
		}

		if (i < array.size() - 1)
		{
			joined += array[i] + L" & ";
			continue;
		}

		joined += array[i];
	}

	return joined;
}

bool IsSupportedAlgorithm(const std::wstring& algorithm)
{
	return std::find(SupportedAlgorithms.cbegin(), SupportedAlgorithms.cend(), algorithm) != SupportedAlgorithms.cend();
}

void PrintUsage(const std::filesystem::path& exePath)
{
	std::wcerr << L"HashCalc v0.1" << std::endl;
	std::wcerr << L"Invalid arguments. Usage:" << std::endl;
	std::wcerr << exePath << " <string>" << std::endl;
	std::wcerr << exePath << " X:\\Path\\To\\File" << std::endl;
	std::wcerr << exePath << " <string> <algorithm>" << std::endl;
	std::wcerr << exePath << " X:\\Path\\To\\File <algorithm>" << std::endl;
	std::wcerr << L"Currently supported algorithms: " << Join(SupportedAlgorithms) << std::endl;
}

int wmain(int argc, wchar_t* argv[])
{
	if (argc == 1 || argc > 3)
	{
		PrintUsage(argv[0]);
		return ERROR_BAD_ARGUMENTS;
	}

	std::wstring selectedAlgorithm = BCRYPT_SHA256_ALGORITHM;

	if (argc == 3)
	{
		if (IsSupportedAlgorithm(argv[2]))
		{
			selectedAlgorithm = argv[2];
		}
		else
		{
			PrintUsage(argv[0]);
			return ERROR_BAD_ARGUMENTS;
		}
	}

	try
	{
		HashCalc hashCalc(selectedAlgorithm);

		if (IsReadableFile(argv[1]))
		{
			std::cout << hashCalc.CalculateChecksum(std::filesystem::path(argv[1]));
		}
		else
		{
			std::cout << hashCalc.CalculateChecksum(std::wstring(argv[1]));
		}
	}
	catch (const HashCalcException& e)
	{
		std::cerr << "An exception occurred: " << e.what() << std::endl;
		return e.Code;
	}
	catch (const std::ios::failure& e)
	{
		std::cerr << "An I/O exception occurred: " << e.what() << std::endl;
		return ERROR_IO_DEVICE;
	}

	return 0;
}