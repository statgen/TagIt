#include "include/auxiliary.h"

char* auxiliary::strtok(char** start, char separator) {
	if ((*start != NULL) && (**start != '\0')) {
		char* token = *start;

		do {
			if (**start == separator) {
				**start = '\0';
				(*start)++;
				return token;
			}
			(*start)++;
		} while (**start != '\0');

		return token;
	}

	return NULL;
}

int auxiliary::strcmp_case_insensitive(const char* s1, const char* s2) {
	int i = 0;

	while ((s1[i] != '\0') && (s2[i] != '\0')) {
		if (tolower(s1[i]) < tolower(s2[i])) {
			return -1;
		} else if (tolower(s1[i]) > tolower(s2[i])) {
			return 1;
		}
		++i;
	}

	if (s1[i] == '\0') {
		if (s2[i] == '\0') {
			return 0;
		}
		return -1;
	}

	return 1;
}

unsigned long int auxiliary::str_jenkins_hash_case_insensitive(const char *s) {
	unsigned int h = 0u;
	unsigned int i = 0u;

	while (s[i] != '\0') {
		h += tolower(s[i]);
		h += (h << 10);
		h ^= (h >> 6);
		++i;
	}

    h += (h << 3);
    h ^= (h >> 11);
    h += (h << 15);

    return h;
}
