#ifndef TEXTTABLEREADER_H_
#define TEXTTABLEREADER_H_

#include <fstream>
#include <vector>
#include <set>
#include <math.h>
#include <limits>

#include "Reader.h"

class TextReader : public Reader {
private:
	ifstream ifile_stream;

	int buffer_size;
	char* buffer;

public:
	static const unsigned int DEFAULT_BUFFER_SIZE;

	TextReader(unsigned int buffer_size = DEFAULT_BUFFER_SIZE) throw (Exception);
	virtual ~TextReader();

	void open() throw (Exception);
	void close() throw (Exception);
	int read_line() throw (Exception);
	void reset() throw (Exception);
	bool eof();
	bool is_compressed();
};

#endif
