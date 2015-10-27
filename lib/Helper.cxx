//
// Created by Lars Bilke on 26.10.15.
//

#include "Helper.h"

#include <iostream>
#include <sys/stat.h>

int Helper::CanReadFile(const char *name, const char *extension)
{
	// Check if file exists
	struct stat fs;
	if (stat(name, &fs) != 0)
	{
		std::cout << "The file " << name << " does not exist." << std::endl;
		return 0;
	}

	if(std::string(name).rfind(extension) == std::string::npos)
	{
		std::cout << "The file " <<name << " has the wrong extension. Expected: "
			<< extension << std::cout;
		return 0;
	}
	else
		return 1;
}
