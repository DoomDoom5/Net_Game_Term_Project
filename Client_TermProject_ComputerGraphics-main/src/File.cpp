#include "stdafx.h"
#include "File.h"

GLchar* FileToBuffer(const GLchar* path)
{
	//ifstream file(path);
	//if (file.fail())
	//{
	//	assert(0);
	//}

	//file.seekg(0, ios::end);
	//size_t size = static_cast<size_t>(file.tellg());
	//file.seekg(0, ios::beg);

	//GLchar* buffer = nullptr;
	//buffer = (GLchar*)malloc(size + 1);
	//file.read(buffer, size);
	//buffer[size] = 0;
	//file.close();

	//return buffer;


	FILE* fptr;
	size_t length = 0;
	GLchar* buf;
	fptr = fopen(path, "rb"); // Open file for reading
	if (!fptr) // Return NULL on failure
		return NULL;
	fseek(fptr, 0, SEEK_END); // Seek to the end of the file
	length = ftell(fptr); // Find out how many bytes GLinto the file we are
	buf = (char*)malloc(length + 1); // Allocate a buffer for the entire length of the file and a null terminator
	fseek(fptr, 0, SEEK_SET); // Go back to the beginning of the file
	fread(buf, length, 1, fptr); // Read the contents of the file in to the buffer
	fclose(fptr); // Close the file
	buf[length] = 0; // Null terminator
	return buf; // Return the buffer
}