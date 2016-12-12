#include "include/TextWriter.h"

const unsigned int TextWriter::DEFAULT_BUFFER_SIZE = 16777216;

TextWriter::TextWriter(unsigned int buffer_size) throw (Exception) : buffer(NULL) {
	buffer = (char*)malloc((buffer_size + 1u) * sizeof(char));
	if (buffer == NULL) {
		throw Exception(__FILE__, __LINE__, "Error in memory allocation.");
	}

	buffer[0] = '\0';
}

TextWriter::~TextWriter() {
	free(buffer);
	buffer = NULL;
}

void TextWriter::open() throw (Exception) {
	if (ofile_stream.is_open()) {
		close();
	}

	ofile_stream.clear();
	ofile_stream.open(file_name, ios::binary);

	if (ofile_stream.fail()) {
		throw Exception(__FILE__, __LINE__, "Error while opening '%s' file.", file_name);
	}
}

void TextWriter::close() throw (Exception) {
	if (ofile_stream.is_open()) {
		ofile_stream.clear();
		ofile_stream.close();

		if (ofile_stream.fail()) {
			throw Exception(__FILE__, __LINE__, "Error while closing '%s' file.", file_name);
		}
	}
}

void TextWriter::write(const char* format, ...) throw (Exception) {
	va_list arguments;

	va_start(arguments, format);
	if (vsprintf(buffer, format, arguments) < 0) {
		throw Exception(__FILE__, __LINE__, "Error while writing '%s' file.", file_name);
	}
	va_end(arguments);

	ofile_stream << buffer;

	if (ofile_stream.fail()) {
		throw Exception(__FILE__, __LINE__, "Error while writing '%s' file.", file_name);
	}
}
