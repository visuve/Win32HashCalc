#pragma once

class HashCalcException : public std::exception
{
public:
	HashCalcException(const std::wstring& what, const NTSTATUS status);
	const int Code;

private:
	std::string Format(const std::wstring& what, const NTSTATUS status);
};

