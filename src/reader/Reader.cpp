#include "include/Reader.h"

Reader::Reader(char** buffer) : file_name(NULL), compressed(false), line(buffer)  {

}

Reader::~Reader() {
	free(file_name);
	file_name = NULL;
}

void Reader::set_file_name(const char* file_name) throw (Exception) {
	free(this->file_name);
	this->file_name = NULL;
	this->file_name = (char*)malloc((strlen(file_name) + 1) * sizeof(char));
	if (this->file_name == NULL) {
		throw Exception(__FILE__, __LINE__, "Error in memory allocation.");
	}
	strcpy(this->file_name, file_name);
}

const char* Reader::get_file_name() {
	return file_name;
}
