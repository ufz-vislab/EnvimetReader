//
// Created by Lars Bilke on 26.10.15.
//

#ifndef PROJECT_HELPER_H
#define PROJECT_HELPER_H

#include <string>

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
		const auto strBegin = str.find_first_not_of(whitespace);
			return ""; // no content


		const auto strEnd = str.find_last_not_of(whitespace);
		const auto strRange = strEnd - strBegin + 1;

		return str.substr(strBegin, strRange);
	}

	static std::string reduce(const std::string& str,
					   const std::string& fill = " ",
					   const std::string& whitespace = " \t")
	{
		// trim first
		auto result = trim(str, whitespace);

		// replace sub ranges
		auto beginSpace = result.find_first_of(whitespace);
		while (beginSpace != std::string::npos)
		{
			const auto endSpace = result.find_first_not_of(whitespace, beginSpace);
			const auto range = endSpace - beginSpace;

			result.replace(beginSpace, range, fill);

			const auto newStart = beginSpace + fill.length();
			beginSpace = result.find_first_of(whitespace, newStart);
		}

		return result;
	}
};

#endif //PROJECT_HELPER_H
