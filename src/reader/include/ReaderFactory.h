#ifndef READERFACTORY_H_
#define READERFACTORY_H_

#include "TextReader.h"
#include "GzipReader.h"

class ReaderFactory {
private:
	static bool is_gzip(const char* file_name) throw (Exception);

public:
	ReaderFactory();
	virtual ~ReaderFactory();

	static Reader* create(const char* file_name) throw (Exception);
};

#endif
