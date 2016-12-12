#ifndef GZIPWRITER_H_
#define GZIPWRITER_H_

#include "Writer.h"
#include <zlib.h>

using namespace std;

class GzipWriter : public Writer {
private:
	gzFile outfile;

	char* buffer;

public:
	static const unsigned int DEFAULT_BUFFER_SIZE;

	GzipWriter(unsigned int buffer_size = DEFAULT_BUFFER_SIZE) throw (Exception);
	virtual ~GzipWriter();

	void open() throw (Exception);
	void close() throw (Exception);
	void write(const char* format, ...) throw (Exception);
};

#endif
