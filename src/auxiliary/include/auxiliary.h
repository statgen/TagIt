#ifndef AUXILIARY_H_
#define AUXILIARY_H_

#include <cstdlib>
#include <cctype>
#include <cstring>
#include <cmath>

using namespace std;

namespace auxiliary {

	char* strtok(char** start, char separator);

	inline int fcmp(double x, double y, double epsilon) {
		int max_exponent = 0;
		double delta = 0.0;
		double diff = 0.0;

		frexp(fabs(x) > fabs(y) ? x : y, &max_exponent);
		delta = ldexp(epsilon, max_exponent);

		diff = x - y;

		if (diff > delta) {
			return 1;
		} else if (diff < -delta) {
			return -1;
		} else {
			return 0;
		}
	}

	inline bool to_ulong_int(const char* value, unsigned long int* ulong_int) {
		if ((value != NULL) && (strlen(value) <= 0)) {
			return false;
		}

		char* end_ptr = NULL;

		*ulong_int = strtoul(value, &end_ptr, 10);

		return (*end_ptr == '\0');
	}

	inline bool to_dbl(const char* value, double* dbl) {
		if ((value != NULL) && (strlen(value) <= 0)) {
			return false;
		}

		char* end_ptr = NULL;

		*dbl = strtod(value, &end_ptr);

		return (*end_ptr == '\0');
	}

	int strcmp_case_insensitive(const char* s1, const char* s2);

	inline bool bool_strcmp_case_insensitive(const char* s1, const char* s2) {
		return strcmp_case_insensitive(s1, s2) < 0;
	}

	inline bool bool_str_equal_to_case_insensitive(const char* s1, const char* s2) {
		return strcmp_case_insensitive(s1, s2) == 0;
	}

	unsigned long int str_jenkins_hash_case_insensitive(const char *s);
}

#endif
