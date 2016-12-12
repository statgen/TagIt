#include "include/WriterFactory.h"

const char* WriterFactory::TEXT = "TEXT";
const char* WriterFactory::GZIP = "GZIP";

WriterFactory::WriterFactory() {

}

WriterFactory::~WriterFactory() {

}

Writer* WriterFactory::create(const char* type) throw (Exception) {
	if (auxiliary::strcmp_case_insensitive(type, TEXT) == 0) {
		return new TextWriter();
	} else if (auxiliary::strcmp_case_insensitive(type, GZIP) == 0) {
		return new GzipWriter();
	} else {
		throw Exception(__FILE__, __LINE__, "Unrecognized file type '%s'.", type);
	}
}
