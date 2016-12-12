#ifndef GZIPREADER_H_
#define GZIPREADER_H_

#include "Reader.h"
#include <zlib.h>

class GzipReader: public Reader {
private:
	gzFile infile;

	int buffer_size;
	char* buffer;

public:
	static const unsigned int DEFAULT_BUFFER_SIZE;

	GzipReader(unsigned int buffer_size = DEFAULT_BUFFER_SIZE) throw (Exception);
	virtual ~GzipReader();

	void open() throw (Exception);
	void close() throw (Exception);
	int read_line() throw (Exception);
	void reset() throw (Exception);
	bool eof();
	bool is_compressed();
};

#endif
