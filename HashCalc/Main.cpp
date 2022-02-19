#include "PCH.hpp"
#include "HashCalc.hpp"
#include "HashCalcException.hpp"
#include "StringConversion.hpp"

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

bool IsSupportedAlgorithm(const std::wstring& algorithm)
{
	return std::find(
		SupportedAlgorithms.cbegin(),
		SupportedAlgorithms.cend(),
		algorithm) != SupportedAlgorithms.cend();
}

void PrintUsage(const std::filesystem::path& exePath)
{
	std::wcerr << L"HashCalc v0.1" << std::endl;
	std::wcerr << L"Invalid arguments. Usage:" << std::endl;
	std::wcerr << exePath << " <string>" << std::endl;
	std::wcerr << exePath << " X:\\Path\\To\\File" << std::endl;
	std::wcerr << exePath << " <string> <algorithm>" << std::endl;
	std::wcerr << exePath << " X:\\Path\\To\\File <algorithm>" << std::endl;
	std::wcerr << L"Currently supported algorithms: " <<
		StringConversion::Join(SupportedAlgorithms) << std::endl;
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
			std::wcout << hashCalc.CalculateChecksumFrom(std::filesystem::path(argv[1]));
		}
		else
		{
			std::wcout << hashCalc.CalculateChecksum(std::wstring(argv[1]));
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