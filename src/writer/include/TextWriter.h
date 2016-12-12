#ifndef TEXTWRITER_H_
#define TEXTWRITER_H_

#include "Writer.h"
#include <fstream>

using namespace std;

class TextWriter : public Writer {
private:
	ofstream ofile_stream;

	char* buffer;

public:
	static const unsigned int DEFAULT_BUFFER_SIZE;

	TextWriter(unsigned int buffer_size = DEFAULT_BUFFER_SIZE) throw (Exception);
	virtual ~TextWriter();

	void open() throw (Exception);
	void close() throw (Exception);
	void write(const char* format, ...) throw (Exception);
};

#endif
