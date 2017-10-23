#ifndef UTILS_H
#define UTILS_H

#include <Windows.h>
#include <string>

std::string GetDirectory()
{
	char working_directory[MAX_PATH + 1];
	GetCurrentDirectoryA(sizeof(working_directory), working_directory);
	return working_directory;
}


#endif // !UTILS_H
