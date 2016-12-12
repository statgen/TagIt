#include "include/TextReader.h"

const unsigned int TextReader::DEFAULT_BUFFER_SIZE = 16777216u;

TextReader::TextReader(unsigned int buffer_size) throw (Exception) : Reader(&buffer),
	buffer_size(buffer_size), buffer(NULL) {

	buffer = (char*)malloc((buffer_size + 1) * sizeof(char));
	if (buffer == NULL) {
		throw Exception(__FILE__, __LINE__, "Error in memory allocation.");
	}

	buffer[0] = '\0';
}

TextReader::~TextReader() {
	buffer_size = 0;

	free(buffer);
	buffer = NULL;
}


void TextReader::open() throw (Exception) {
	if (ifile_stream.is_open()) {
		close();
	}

	ifile_stream.clear();
	ifile_stream.open(file_name, ios::binary);

	if (ifile_stream.fail()) {
		throw Exception(__FILE__, __LINE__, "Error while opening '%s' file.", file_name);
	}
}

void TextReader::close() throw (Exception) {
	if (ifile_stream.is_open()) {
		ifile_stream.clear();
		ifile_stream.close();

		if (ifile_stream.fail()) {
			throw Exception(__FILE__, __LINE__, "Error while closing '%s' file.", file_name);
		}
	}

	buffer[0] = '\0';
}

int TextReader::read_line() throw (Exception) {
	int i = 0;
	int c = 0;

	while ((i < buffer_size) && ((c = ifile_stream.get()) != ifstream::traits_type::eof())) {
		buffer[i] = (char)c;

		if (buffer[i] == '\n') {
			buffer[i] = '\0';
			return i;
		} else if (buffer[i] == '\r') {
			buffer[i] = '\0';
			if ((c = ifile_stream.peek()) != ifstream::traits_type::eof()) {
				if ((char)c == '\n') {
					ifile_stream.ignore();
				}
			}
			return i;
		}

		i += 1;
	}

	buffer[i] = '\0';

	if (!ifile_stream.eof() && ifile_stream.fail()) {
		throw Exception(__FILE__, __LINE__, "Error while reading '%s' file.", file_name);
	}

	return (i == 0 ? -1 : i);
}

void TextReader::reset() throw (Exception) {
	if (ifile_stream.is_open()) {
		ifile_stream.clear();
		ifile_stream.seekg((streampos)0, ifstream::beg);

		if (ifile_stream.fail()) {
			throw Exception(__FILE__, __LINE__, "Error while resetting '%s' file.", file_name);
		}
	}

	buffer[0] = '\0';
}

bool TextReader::eof() {
	return ifile_stream.eof();
}

bool TextReader::is_compressed() {
	return false;
}
