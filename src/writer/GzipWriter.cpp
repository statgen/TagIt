#include "include/GzipWriter.h"

const unsigned int GzipWriter::DEFAULT_BUFFER_SIZE = 16777216;

GzipWriter::GzipWriter(unsigned int buffer_size) throw (Exception) : outfile(NULL), buffer(NULL) {
	buffer = (char*)malloc((buffer_size + 1u) * sizeof(char));
	if (buffer == NULL) {
		throw Exception(__FILE__, __LINE__, "Error in memory allocation.");
	}

	buffer[0] = '\0';
}

GzipWriter::~GzipWriter() {
	free(buffer);
	buffer = NULL;
}

void GzipWriter::open() throw (Exception) {
	outfile = gzopen(file_name, "wb");
	if (outfile == NULL) {
		throw Exception(__FILE__, __LINE__, "Error while opening '%s' file.", file_name);
	}
}

void GzipWriter::close() throw (Exception) {
	int gzerrno = 0;

	gzerrno = gzclose(outfile);
	if (gzerrno != Z_OK) {
		throw Exception(__FILE__, __LINE__, "Error while closing '%s' file.", file_name);
	}
}

void GzipWriter::write(const char* format, ...) throw (Exception) {
	va_list arguments;

	va_start(arguments, format);
	if (vsprintf(buffer, format, arguments) < 0) {
		throw Exception(__FILE__, __LINE__, "Error while writing '%s' file.", file_name);
	}
	va_end(arguments);

	if (gzputs(outfile, buffer) < 0) {
		throw Exception(__FILE__, __LINE__, "Error while writing '%s' file.", file_name);
	}
}
