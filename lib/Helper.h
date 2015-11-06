//
// Created by Lars Bilke on 26.10.15.
//

#ifndef PROJECT_HELPER_H
#define PROJECT_HELPER_H

#include <string>
#include <vtkVariant.h>

class Helper
{
public:
	static int CanReadFile(const char *name, const char *extension);

	static bool invalidChar (char c)
	{
		return !(c>=0 && c <128);
	}

	// From http://stackoverflow.com/a/1798170/80480
	static std::string trim(const std::string& str, const std::string& whitespace = " \t")
	{
		const std::string::size_type strBegin = str.find_first_not_of(whitespace);
		if (strBegin == std::string::npos)
			return ""; // no content


		const std::string::size_type strEnd = str.find_last_not_of(whitespace);
		const std::string::size_type strRange = strEnd - strBegin + 1;

		return str.substr(strBegin, strRange);
	}

	static std::string reduce(const std::string& str,
					   const std::string& fill = " ",
					   const std::string& whitespace = " \t")
	{
		// trim first
		std::string result = trim(str, whitespace);

		// replace sub ranges
		std::string::size_type beginSpace = result.find_first_of(whitespace);
		while (beginSpace != std::string::npos)
		{
			const std::string::size_type endSpace = result.find_first_not_of(whitespace, beginSpace);
			const std::string::size_type range = endSpace - beginSpace;

			result.replace(beginSpace, range, fill);

			const std::string::size_type newStart = beginSpace + fill.length();
			beginSpace = result.find_first_of(whitespace, newStart);
		}

		return result;
	}

	static std::string getDirectory(const std::string& str)
	{
#if WIN32
		std::string separator = "\\";
#else
		std::string separator = "/";
#endif
		const std::size_t pos = str.find_last_of("/\\");
		if(pos == std::string::npos)
			return "." + separator;
		return str.substr(0, pos + 1);
	}

	static int StringToInt(const char *string)
	{
		vtkVariant variant(string);
		return variant.ToInt();
	}
};

#endif //PROJECT_HELPER_H
