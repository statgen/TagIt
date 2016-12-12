#ifndef WRITERFACTORY_H_
#define WRITERFACTORY_H_

#include "TextWriter.h"
#include "GzipWriter.h"
#include "../../auxiliary/include/auxiliary.h"

using namespace std;

class WriterFactory {
private:
	WriterFactory();

public:
	static const char* TEXT;
	static const char* GZIP;

	virtual ~WriterFactory();

	static Writer* create(const char* type) throw (Exception);
};

#endif
