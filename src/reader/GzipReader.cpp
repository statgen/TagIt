#include "include/GzipReader.h"

const unsigned int GzipReader::DEFAULT_BUFFER_SIZE = 16777216u;

GzipReader::GzipReader(unsigned int buffer_size) throw (Exception) : Reader(&buffer),
	infile(NULL), buffer_size(buffer_size), buffer(NULL) {

	if (auxiliary::strcmp_case_insensitive(zlibVersion(), ZLIB_VERSION) != 0) {
		throw Exception(__FILE__, __LINE__, "zLib library version %s is not compatible with system's zlib library version %s.", ZLIB_VERSION, zlibVersion());
	}

	buffer = (char*)malloc((buffer_size + 1) * sizeof(char));
	if (buffer == NULL) {
		throw Exception(__FILE__, __LINE__, "Error in memory allocation.");
	}

	buffer[0] = '\0';
}

GzipReader::~GzipReader() {
	buffer_size = 0;

	free(buffer);
	buffer = NULL;
}

void GzipReader::open() throw (Exception) {
	infile = gzopen(file_name, "rb");
	if (infile == NULL) {
		throw Exception(__FILE__, __LINE__, "Error while opening '%s' file.", file_name);
	}
}

void GzipReader::close() throw (Exception) {
	int gzerrno = 0;

	gzerrno = gzclose(infile);
	if (gzerrno != Z_OK) {
		throw Exception(__FILE__, __LINE__, "Error while closing '%s' file.", file_name);
	}
}

int GzipReader::read_line() throw (Exception) {
	int i = 0;
	int c = 0;

	while ((i < buffer_size) && ((c = gzgetc(infile)) >= 0)) {
		buffer[i] = (char)c;

		if (buffer[i] == '\n') {
			buffer[i] = '\0';
			return i;
		} else if (buffer[i] == '\r') {
			buffer[i] = '\0';
			if ((c = gzgetc(infile)) >= 0) {
				if ((char)c != '\n') {
					c = gzungetc(c, infile);
				}
			}
			return i;
		}

		i += 1;
	}

	buffer[i] = '\0';

	if ((c < 0) && (gzeof(infile) < 1)) {
		throw Exception(__FILE__, __LINE__, "Error while reading '%s' file.", file_name);
	}

	return (i == 0 ? -1 : i);
}

void GzipReader::reset() throw (Exception) {
	if (gzseek(infile, 0L, SEEK_SET) < 0) {
		throw Exception(__FILE__, __LINE__, "Error while resetting '%s' file.", file_name);
	}
}

bool GzipReader::eof() {
	return gzeof(infile) > 0;
}

bool GzipReader::is_compressed() {
	return true;
}
