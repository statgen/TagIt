/*
 * Tag(ging) It(erative)
 * Created on: Oct 31, 2014
 *      Author: dtaliun
 */

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <iterator>

#include "auxiliary/include/auxiliary.h"
#include "commandline/include/TagItCommandLine.h"
#include "structures/include/ArrayDictionary.h"
#include "structures/include/ArrayGraph.h"
#include "reader/include/ReaderFactory.h"
#include "tagger/include/Tagger.h"

using namespace std;

// Constants
static const double EPSILON = 0.000000001;
static const unsigned int BUCKET_SIZE = 1000000u;
static const unsigned int HEAP_INITIAL_SIZE = 1000000u;
static const unsigned int HEAP_INCREMENT = 100000u;

// File headers
static const char AF_FILE_SEPARATOR = '\t';
static const char* AF_FILE_CHROM = "CHROM";
static const char* AF_FILE_POS = "POS";
static const char* AF_FILE_N_ALLELES = "N_ALLELES";
static const char* AF_FILE_N_CHR = "N_CHR";
static const char* AF_FILE_ALLELE_FREQ = "{ALLELE:FREQ}";
static const unsigned int AF_FILE_N_COLUMNS = 5u;
static const char* AF_FILE_COLUMNS[AF_FILE_N_COLUMNS] = {
		AF_FILE_CHROM, AF_FILE_POS, AF_FILE_N_ALLELES, AF_FILE_N_CHR, AF_FILE_ALLELE_FREQ
};

static const char LD_FILE_SEPARATOR = '\t';
static const char* LD_FILE_MARKER1 = "#MARKER1";
static const char* LD_FILE_MARKER2 = "MARKER2";
static const char* LD_FILE_AF1 = "AF1";
static const char* LD_FILE_AF2 = "AF2";
static const char* LD_FILE_R2 = "R2";
static const char* LD_FILE_R = "R";
static const unsigned int LD_FILE_N_COLUMNS = 6u;
static const char* LD_FILE_COLUMNS[LD_FILE_N_COLUMNS] = {
		LD_FILE_MARKER1, LD_FILE_MARKER2, LD_FILE_AF1, LD_FILE_AF2, LD_FILE_R2, LD_FILE_R
};

static const char WEIGHTS_FILE_SEPARATOR = '\t';
static const unsigned int WEIGHTS_FILE_N_COLUMNS = 2u;

// Timers
clock_t start_time = 0;
clock_t end_time = 0;
double execution_time = 0.0;

// Input data
ArrayDictionary fix_markers(BUCKET_SIZE, HEAP_INITIAL_SIZE, HEAP_INCREMENT);
ArrayDictionary exclude_markers(BUCKET_SIZE, HEAP_INITIAL_SIZE, HEAP_INCREMENT);
ArrayDictionary hide_markers(BUCKET_SIZE, HEAP_INITIAL_SIZE, HEAP_INCREMENT);
ArrayDictionary dirty_markers(BUCKET_SIZE, HEAP_INITIAL_SIZE, HEAP_INCREMENT);
vector<unordered_set<unsigned int>> dirty_markers_per_file;
unordered_set<unsigned int> dirty_markers_to_exclude;
vector<bool*> dirty_passed_maf_per_file;

// Filtered Data
ArrayDictionary markers(BUCKET_SIZE, HEAP_INITIAL_SIZE, HEAP_INCREMENT);
vector<bool*> passed_maf_per_file;
vector<ArrayGraph> graphs_per_file;

void load_dirty_markers(const char* input_file, unordered_set<unsigned int>& markers, unordered_set<unsigned int>& markers_to_exclude) throw (Exception) {
	Reader* reader = NULL;

	char* line = NULL;
	int line_length = 0;
	unsigned int line_number = 0u;
	unsigned int column_number = 0u;

	char* token = NULL;
	vector<char*> tokens;

	unsigned long int n_alleles = 0ul;
	char name[8000u];
	unsigned int name_id = 0u;

	try {
		reader = ReaderFactory::create(input_file);
		reader->open();

//		BEGIN: check header.
		if ((line_length = reader->read_line()) > 0) {
			++line_number;
			line = *(reader->line);
			column_number = 0u;

			while ((token = auxiliary::strtok(&line, AF_FILE_SEPARATOR)) != NULL) {
				if (column_number < AF_FILE_N_COLUMNS) {
					if (auxiliary::strcmp_case_insensitive(token, AF_FILE_COLUMNS[column_number]) != 0) {
						throw Exception(__FILE__, __LINE__, "Column '%s' was not found on its expected position No. %d in '%s' file.",
								AF_FILE_COLUMNS[column_number], column_number + 1u, input_file);
					}
				} else {
					throw Exception(__FILE__, __LINE__, "Too many columns in '%s' file.", input_file);
				}

				++column_number;
			}
		} else {
			throw Exception(__FILE__, __LINE__, "Missing header in '%s' file.", input_file);
		}
//		END: check header.

//		BEGIN: read marker names.
		while ((line_length = reader->read_line()) > 0) {
			++line_number;
			line = *(reader->line);
			column_number = 0u;
			tokens.clear();

			while ((token = auxiliary::strtok(&line, AF_FILE_SEPARATOR)) != NULL) {
				tokens.push_back(token);
				++column_number;
			}

			if (column_number <= AF_FILE_N_COLUMNS) {
				throw Exception(__FILE__, __LINE__, "Number of columns (%d) on line No. %d in '%s' file is less than expected (>%d).",
						column_number, line_number, input_file, AF_FILE_N_COLUMNS);
			}

			if (!auxiliary::to_ulong_int(tokens.at(2), &n_alleles)) {
				throw Exception(__FILE__, __LINE__, "Invalid number of alleles '%s' on line No. %d in '%s' file.",
						tokens.at(2), line_number, input_file);
			}

			if (n_alleles != 2ul) {
				continue;
			}

			name[0u] = '\0';
			strcat(name, tokens.at(0));
			strcat(name, ":");
			strcat(name, tokens.at(1));

			name_id = dirty_markers.insert(name);

			if (markers.count(name_id) > 0) {
				markers_to_exclude.insert(name_id);
			} else {
				markers.insert(name_id);
			}
		}

		if (line_length == 0) {
			throw Exception(__FILE__, __LINE__, "Line %d is empty in '%s' file.", line_number + 1u, input_file);
		}
//		END: read marker names.

		reader->close();
		delete reader;
		reader = NULL;
	} catch (Exception &e1) {
		if (reader != NULL) {
			try {
				reader->close();
			} catch (Exception &e2) {}
			delete reader;
			reader = NULL;
		}

		e1.add_message(__FILE__, __LINE__, "Error while reading '%s' file.", input_file);
		throw;
	}
}

void synchronize_dirty_markers() {
	for (unsigned int i = 0u; i < dirty_markers.n; ++i) {
		if (dirty_markers_to_exclude.count(i) > 0) {
			dirty_markers.remove(dirty_markers.names[i]);
			for (auto&& markers : dirty_markers_per_file) {
				markers.erase(i);
			}
		}
	}
	dirty_markers_to_exclude.clear();
}


void exclude_dirty_markers() {
	long int name_id = 0u;

	for (unsigned int i = 0u; i < exclude_markers.n; ++i) {
		if ((name_id = dirty_markers.find(exclude_markers.names[i])) >= 0) {
			dirty_markers_to_exclude.insert(name_id);
		}
	}

	synchronize_dirty_markers();
}

void exclude_maf_dirty_markers() {
	bool passed = false;

	for (unsigned int i = 0u; i < dirty_markers.n; ++i) {
		if (dirty_markers.names[i] == NULL) {
			continue;
		}

		for (auto&& passed_maf : dirty_passed_maf_per_file) {
			passed |= passed_maf[i];
		}

		if (!passed) {
			dirty_markers_to_exclude.insert(i);
		} else {
			passed = false;
		}
	}

	synchronize_dirty_markers();
}

void finalize_dirty_markers() throw (Exception) {
	unsigned int n_passed = 0u;
	unsigned int name_id = 0u;
	bool* passed = NULL;

	for (unsigned int i = 0u; i < dirty_markers.n; ++i) {
		if (dirty_markers.names[i] != NULL) {
			++n_passed;
		}
	}

	for (unsigned int i = 0u; i < dirty_markers_per_file.size(); ++i) {
		passed = (bool*)malloc(n_passed * sizeof(bool));
		if (passed == NULL) {
			throw Exception(__FILE__, __LINE__, "Error in memory allocation.");
		}
		passed_maf_per_file.push_back(passed);
	}

	for (unsigned int i = 0u; i < dirty_markers.n; ++i) {
		if (dirty_markers.names[i] == NULL) {
			continue;
		}

		name_id = markers.insert(dirty_markers.names[i]);
		dirty_markers.remove(dirty_markers.names[i]);

		for (unsigned int j = 0u; j < passed_maf_per_file.size(); ++j) {
			passed_maf_per_file.at(j)[name_id] = dirty_passed_maf_per_file.at(j)[i];
		}
	}

	for (unsigned int i = 0u; i < fix_markers.n; ++i) {
		name_id = markers.find(fix_markers.names[i]);
		for (unsigned int j = 0u; j < passed_maf_per_file.size(); ++j) {
			passed_maf_per_file.at(j)[name_id] = true;
		}
	}
}

unsigned int load_allele_frequencies(const char* input_file, unordered_set<unsigned int>& markers, bool** passed_maf, double maf_threshold) throw (Exception) {
	Reader* reader = NULL;

	char* line = NULL;
	int line_length = 0;
	unsigned int line_number = 0u;
	unsigned int column_number = 0u;

	char* token = NULL;
	vector<char*> tokens;

	unsigned long int n_alleles = 0ul;
	char name[8000u];
	long int name_id = 0;

	double freq = 0.0;
	double maf = 0.0;

	unsigned int n_passed_maf = 0u;

	try {
		*passed_maf = (bool*)malloc(dirty_markers.n * sizeof(bool));
		if (*passed_maf == NULL) {
			throw Exception(__FILE__, __LINE__, "Error in memory allocation,");
		}

		for (unsigned int i = 0u; i < dirty_markers.n; ++i) {
			(*passed_maf)[i] = false;
		}

		reader = ReaderFactory::create(input_file);
		reader->open();

//		BEGIN: check header.
		if ((line_length = reader->read_line()) > 0) {
			++line_number;
			line = *(reader->line);
			column_number = 0u;

			while ((token = auxiliary::strtok(&line, AF_FILE_SEPARATOR)) != NULL) {
				if (column_number < AF_FILE_N_COLUMNS) {
					if (auxiliary::strcmp_case_insensitive(token, AF_FILE_COLUMNS[column_number]) != 0) {
						throw Exception(__FILE__, __LINE__, "Column '%s' was not found on its expected position No. %d in '%s' file.",
								AF_FILE_COLUMNS[column_number], column_number + 1u, input_file);
					}
				} else {
					throw Exception(__FILE__, __LINE__, "Too many columns in '%s' file.", input_file);
				}

				++column_number;
			}
		} else {
			throw Exception(__FILE__, __LINE__, "Missing header in '%s' file.", input_file);
		}
//		END: check header.

//		BEGIN: read marker names.
		while ((line_length = reader->read_line()) > 0) {
			++line_number;
			line = *(reader->line);
			column_number = 0u;
			tokens.clear();

			while ((token = auxiliary::strtok(&line, AF_FILE_SEPARATOR)) != NULL) {
				tokens.push_back(token);
				++column_number;
			}

			if (column_number <= AF_FILE_N_COLUMNS) {
				throw Exception(__FILE__, __LINE__, "Number of columns (%d) on line No. %d in '%s' file is less than expected (>%d).",
						column_number, line_number, input_file, AF_FILE_N_COLUMNS);
			}

			if (!auxiliary::to_ulong_int(tokens.at(2), &n_alleles)) {
				throw Exception(__FILE__, __LINE__, "Invalid number of alleles '%s' on line No. %d in '%s' file.",
						tokens.at(2), line_number, input_file);
			}

			if (n_alleles != 2ul) {
				continue;
			}

			name[0u] = '\0';
			strcat(name, tokens.at(0));
			strcat(name, ":");
			strcat(name, tokens.at(1));

			name_id = dirty_markers.find(name);

			if (name_id < 0) {
				continue;
			}

			token = strchr(tokens.at(4), ':');
			if (token == NULL) {
				token = tokens.at(4);
			} else {
				++token;
			}

			if (!auxiliary::to_dbl(token, &freq) || (auxiliary::fcmp(freq, 0.0, EPSILON) < 0) || (auxiliary::fcmp(freq, 1.0, EPSILON) > 0)) {
				throw Exception(__FILE__, __LINE__, "Invalid allele frequency value '%s' on line No. %d in '%s' file.", token, line_number, input_file);
			}

			if (auxiliary::fcmp(freq, 0.5, EPSILON) > 0) {
				maf = 1.0 - freq;
			} else {
				maf = freq;
			}

			if (auxiliary::fcmp(maf, maf_threshold, EPSILON) < 0) {
				(*passed_maf)[name_id] = false;
			} else {
				(*passed_maf)[name_id] = true;
			}
		}
//		END: read marker names.

		for (auto id : markers) {
			if ((*passed_maf)[id]) {
				++n_passed_maf;
			}
		}

		reader->close();
		delete reader;
		reader = NULL;
	} catch (Exception &e1) {
		if (reader != NULL) {
			try {
				reader->close();
			} catch (Exception &e2) {}
			delete reader;
			reader = NULL;
		}

		if (*passed_maf != NULL) {
			free(*passed_maf);
			*passed_maf = NULL;
		}

		e1.add_message(__FILE__, __LINE__, "Error while reading '%s' file.", input_file);
		throw;
	}

	return n_passed_maf;
}

void load_ld(const char* input_file, ArrayGraph& graph, double r2_threshold, bool* passed_maf, bool hide_maf) throw (Exception) {
	Reader* reader = NULL;

	char* line = NULL;
	int line_length = 0;
	unsigned int line_number = 0u;
	unsigned int column_number = 0u;

	char* token = NULL;
	vector<char*> tokens;

	const char* name = NULL;
	long int first_name_id = 0;
	long int second_name_id = 0;
	double r2 = 0.0;

	try {
		reader = ReaderFactory::create(input_file);
		reader->open();

//		BEGIN: check header.
		if ((line_length = reader->read_line()) > 0) {
			++line_number;
			line = *(reader->line);
			column_number = 0u;

			while ((token = auxiliary::strtok(&line, LD_FILE_SEPARATOR)) != NULL) {
				if (column_number < LD_FILE_N_COLUMNS) {
					if (auxiliary::strcmp_case_insensitive(token, LD_FILE_COLUMNS[column_number]) != 0) {
						throw Exception(__FILE__, __LINE__, "Column '%s' was not found on its expected position No. %d in '%s' file.",
								LD_FILE_COLUMNS[column_number], column_number + 1u, input_file);
					}
				} else {
					throw Exception(__FILE__, __LINE__, "Too many columns in '%s' file.", input_file);
				}
				++column_number;
			}
		} else {
			throw Exception(__FILE__, __LINE__, "Missing header in '%s' file.", input_file);
		}
//		END: check header.

//		BEGIN: read LD.
		while ((line_length = reader->read_line()) > 0) {
			++line_number;
			line = *(reader->line);
			column_number = 0u;
			tokens.clear();

			while ((token = auxiliary::strtok(&line, LD_FILE_SEPARATOR)) != NULL) {
				tokens.push_back(token);
				++column_number;
			}

			if (column_number < LD_FILE_N_COLUMNS) {
				throw Exception(__FILE__, __LINE__, "Number of columns (%d) on line No. %d in '%s' file is less than expected (%d).",
						column_number, line_number, input_file, LD_FILE_N_COLUMNS);
			}

			if (column_number > LD_FILE_N_COLUMNS) {
				throw Exception(__FILE__, __LINE__, "Number of columns (%d) on line No. %d in '%s' file is less than expected (%d).",
						column_number, line_number, input_file, LD_FILE_N_COLUMNS);
			}

			if (!auxiliary::to_dbl(tokens.at(4), &r2)) {
				throw Exception(__FILE__, __LINE__, "Invalid r^2 value '%s' on line No. %d in '%s' file.", tokens.at(4), line_number, input_file);
			}

			name = auxiliary::strtok(&(tokens.at(0)), '_');
			if ((first_name_id = markers.find(name)) < 0) {
				continue;
			}

			name = auxiliary::strtok(&(tokens.at(1)), '_');
			if ((second_name_id = markers.find(name)) < 0) {
				continue;
			}

			if (first_name_id == second_name_id) {
				continue;
			}

			if (!hide_maf && !(passed_maf[first_name_id] && passed_maf[second_name_id])) {
				continue;
			}

			if (auxiliary::fcmp(r2, r2_threshold, EPSILON) < 0) {
				continue;
			}

			if ((passed_maf[first_name_id]) && (hide_markers.find(markers.names[first_name_id]) < 0)) {
				graph.add_edge(first_name_id, second_name_id);
			}

			if ((passed_maf[second_name_id]) && (hide_markers.find(markers.names[second_name_id]) < 0)) {
				graph.add_edge(second_name_id, first_name_id);
			}
		}

		if (line_length == 0) {
			throw Exception(__FILE__, __LINE__, "Line %d is empty in '%s' file.", line_number + 1u, input_file);
		}
//		END: read LD.

		reader->close();
		delete reader;
		reader = NULL;
	} catch (Exception &e1) {
		if (reader != NULL) {
			try {
				reader->close();
			} catch (Exception &e2) {}
			delete reader;
		}
		e1.add_message(__FILE__, __LINE__, "Error while reading '%s' file.", input_file);
		throw;
	}
}

void load_weights(const char* input_file, ArrayGraph& graph) throw (Exception) {
	Reader* reader = NULL;

	char* line = NULL;
	int line_length = 0;
	unsigned int line_number = 0u;

	char* token = NULL;
	vector<char*> tokens;

	double weight = 0.0;
	long int name_id = 0;

	try {
		graph.load_weights(1.0);

		reader = ReaderFactory::create(input_file);
		reader->open();

		while ((line_length = reader->read_line()) > 0) {
			++line_number;
			line = *(reader->line);
			tokens.clear();

			while ((token = auxiliary::strtok(&line, WEIGHTS_FILE_SEPARATOR)) != NULL) {
				tokens.push_back(token);
			}

			if (tokens.size() < WEIGHTS_FILE_N_COLUMNS) {
				throw Exception(__FILE__, __LINE__, "Number of columns (%d) on line No. %d in '%s' file is less than expected (%d).",
						tokens.size(), line_number, input_file, WEIGHTS_FILE_N_COLUMNS);
			}

			if (tokens.size() > WEIGHTS_FILE_N_COLUMNS) {
				throw Exception(__FILE__, __LINE__, "Number of columns (%d) on line No. %d in '%s' file is greater than expected (%d).",
						tokens.size(), line_number, input_file, WEIGHTS_FILE_N_COLUMNS);
			}

			if ((name_id = markers.find(tokens.at(0))) < 0) {
				continue;
			}

			if (!auxiliary::to_dbl(tokens.at(1), &weight)) {
				throw Exception(__FILE__, __LINE__, "Invalid r^2 value '%s' on line No. %d in '%s' file.", tokens.at(1), line_number, input_file);
			}

			graph.load_weight(name_id, weight);
		}

		if (line_length == 0) {
			throw Exception(__FILE__, __LINE__, "Line %d is empty in '%s' file.", line_number + 1u, input_file);
		}

		reader->close();
		delete reader;
		reader = NULL;
	} catch (Exception &e1) {
		if (reader != NULL) {
			try {
				reader->close();
			} catch (Exception &e2) {}
			delete reader;
		}

		e1.add_message(__FILE__, __LINE__, "Error while reading '%s' file.", input_file);
		throw;
	}
}

int main(int args, char** argv) {
	try {
		TagItCommandLine commands;
		vector<unsigned int> n_per_file;

		commands.read_command_line(args, argv);
		commands.parse_command_line();

		if (commands.print_help) {
			commands.help();
			return 0;
		}

		if ((commands.fix_file != NULL) || (commands.exclude_file != NULL) || (commands.hide_file != NULL)) {
			fill_n(std::ostream_iterator<char>(std::cout), 78, '-');
			cout << endl << "Loading fixed/excluded/hidden markers... " << endl;
			start_time = clock();

			if (commands.fix_file != NULL) {
				fix_markers.load(commands.fix_file);
				cout << " Fixed: " << fix_markers.n << endl;
			}

			if (commands.exclude_file != NULL) {
				exclude_markers.load(commands.exclude_file);
				cout << " Excluded: " << exclude_markers.n << endl;
			}

			if (commands.hide_file != NULL) {
				hide_markers.load(commands.hide_file);
				cout << " Hidden: " << hide_markers.n << endl;
			}

			if (exclude_markers.intersect(fix_markers) || (hide_markers.intersect(fix_markers)) || (hide_markers.intersect(exclude_markers))) {
				throw Exception(__FILE__, __LINE__, "Sets of fixed/excluded/hidden markers overlap.");
			}

			end_time = clock();
			execution_time = (end_time - start_time) / (double) CLOCKS_PER_SEC;
			cout << "Done (" << execution_time << " sec)" << endl;
		}

		fill_n(std::ostream_iterator<char>(std::cout), 78, '-');
		cout << endl << "Loading markers... " << endl;
		start_time = clock();

		for (unsigned int i = 0u; i < commands.af_files.size(); ++i) {
			dirty_markers_per_file.emplace_back();
			load_dirty_markers(commands.af_files.at(i), dirty_markers_per_file.back(), dirty_markers_to_exclude);
			if (!commands.labels.empty()) {
				cout << " " << commands.labels.at(i) << ": " << flush;
			} else {
				cout << " " << commands.af_files.at(i) << ": " << flush;
			}
			cout << dirty_markers_per_file.back().size() << endl;
		}

		end_time = clock();
		execution_time = (end_time - start_time) / (double) CLOCKS_PER_SEC;
		cout << "Done (" << execution_time << " sec)" << endl;

		cout << "Synchronizing markers... " << endl;
		start_time = clock();

		synchronize_dirty_markers();

		for (unsigned int i = 0u; i < commands.af_files.size(); ++i) {
			if (!commands.labels.empty()) {
				cout << " " << commands.labels.at(i) << ": " << flush;
			} else {
				cout << " " << commands.af_files.at(i) << ": " << flush;
			}
			cout << dirty_markers_per_file.at(i).size() << endl;
		}

		end_time = clock();
		execution_time = (end_time - start_time) / (double) CLOCKS_PER_SEC;
		cout << "Done (" << execution_time << " sec)" << endl;

		if (exclude_markers.n > 0) {
			fill_n(std::ostream_iterator<char>(std::cout), 78, '-');
			cout << endl << "Excluding markers... " << endl;
			start_time = clock();

			exclude_dirty_markers();

			for (unsigned int i = 0u; i < commands.af_files.size(); ++i) {
				if (!commands.labels.empty()) {
					cout << " " << commands.labels.at(i) << ": " << flush;
				} else {
					cout << " " << commands.af_files.at(i) << ": " << flush;
				}
				cout << dirty_markers_per_file.at(i).size() << endl;
			}

			end_time = clock();
			execution_time = (end_time - start_time) / (double) CLOCKS_PER_SEC;
			cout << "Done (" << execution_time << " sec)" << endl;
		}

		fill_n(std::ostream_iterator<char>(std::cout), 78, '-');
		cout << endl << "Loading allele frequencies... " << endl;
		start_time = clock();

		for (unsigned int i = 0u; i < commands.af_files.size(); ++i) {
			if (!commands.labels.empty()) {
				cout << " " << commands.labels.at(i) << " (MAF >= " << commands.maf_threshold << "): " << flush;
			} else {
				cout << " " << commands.af_files.at(i) << " (MAF >= " << commands.maf_threshold << "): " << flush;
			}
			dirty_passed_maf_per_file.emplace_back();
			cout << load_allele_frequencies(commands.af_files.at(i), dirty_markers_per_file.at(i), &(dirty_passed_maf_per_file.back()), commands.maf_threshold) << endl;
		}

		end_time = clock();
		execution_time = (end_time - start_time) / (double) CLOCKS_PER_SEC;
		cout << "Done (" << execution_time << " sec)" << endl;

		if (!commands.hide_maf) {
			cout << "Synchronizing markers... " << endl;
			start_time = clock();

			exclude_maf_dirty_markers();

			for (unsigned int i = 0u; i < commands.af_files.size(); ++i) {
				if (!commands.labels.empty()) {
					cout << " " << commands.labels.at(i) << ": " << flush;
				} else {
					cout << " " << commands.af_files.at(i) << ": " << flush;
				}
				cout << dirty_markers_per_file.at(i).size() << endl;
			}

			end_time = clock();
			execution_time = (end_time - start_time) / (double) CLOCKS_PER_SEC;
			cout << "Done (" << execution_time << " sec)" << endl;
		}

		if (fix_markers.n > 0) {
			fill_n(std::ostream_iterator<char>(std::cout), 78, '-');
			cout << endl << "Adding fixed markers... " << endl;
			start_time = clock();

			for (unsigned int i = 0u, name_id = 0u; i < fix_markers.n; ++i) {
				name_id = dirty_markers.insert(fix_markers.names[i]);
				for (auto&& markers : dirty_markers_per_file) {
					markers.insert(name_id);
				}
			}

			for (unsigned int i = 0u; i < commands.af_files.size(); ++i) {
				if (!commands.labels.empty()) {
					cout << " " << commands.labels.at(i) << ": " << flush;
				} else {
					cout << " " << commands.af_files.at(i) << ": " << flush;
				}
				cout << dirty_markers_per_file.at(i).size() << endl;
			}

			end_time = clock();
			execution_time = (end_time - start_time) / (double) CLOCKS_PER_SEC;
			cout << "Done (" << execution_time << " sec)" << endl;
		}


		finalize_dirty_markers();

		for (auto&& passed_maf : dirty_passed_maf_per_file) {
			delete passed_maf;
		}
		dirty_passed_maf_per_file.clear();
		dirty_markers_per_file.clear();
		dirty_markers_to_exclude.clear();

		fill_n(std::ostream_iterator<char>(std::cout), 78, '=');
		cout << endl << "FINAL DATASET HAS " << markers.n << " MARKER(S)." << endl;

		fill_n(std::ostream_iterator<char>(std::cout), 78, '=');
		cout << endl << "Loading LD... " << endl;
		start_time = clock();

		for (unsigned int i = 0u; i < commands.ld_files.size(); ++i) {
			graphs_per_file.emplace_back(markers.n);
			if (!commands.labels.empty()) {
				cout << " Strong LD pairs in " << commands.labels.at(i) << ": " << flush;
			} else {
				cout << " Strong LD pairs in " << commands.af_files.at(i) << ": " << flush;
			}
			load_ld(commands.ld_files.at(i), graphs_per_file.at(i), commands.r2_threshold, passed_maf_per_file.at(i), commands.hide_maf);
			graphs_per_file.at(i).check_order_consistency();
			cout << graphs_per_file.at(i).get_n_connections() << " (" << graphs_per_file.at(i).get_memory_usage() << " Mb)" << endl;
		}

		end_time = clock();
		execution_time = (end_time - start_time) / (double) CLOCKS_PER_SEC;
		cout << "Done (" << execution_time << " sec)" << endl;

		fill_n(std::ostream_iterator<char>(std::cout), 78, '-');
		cout << endl << "Setting weights... " << endl;
		start_time = clock();

		if (commands.weights.size() > 0) {
			for (unsigned int i = 0; i < commands.af_files.size(); ++i) {
				graphs_per_file.at(i).load_weights(commands.weights.at(i));
				if (!commands.labels.empty()) {
					cout << " " << commands.labels.at(i) << ": " << flush;
				} else {
					cout << " " << commands.af_files.at(i) << ": " << flush;
				}
				cout << commands.weights.at(i) << endl;
			}
		} else if (commands.marker_weights_files.size() > 0) {
			for (unsigned int i = 0; i < commands.af_files.size(); ++i) {
				load_weights(commands.marker_weights_files.at(i), graphs_per_file.at(i));
				if (!commands.labels.empty()) {
					cout << " " << commands.labels.at(i) << ": " << flush;
				} else {
					cout << " " << commands.af_files.at(i) << ": " << flush;
				}
				cout << commands.marker_weights_files.at(i) << endl;
			}
		} else {
			for (unsigned int i = 0; i < commands.af_files.size(); ++i) {
				graphs_per_file.at(i).load_weights(1.0);
				if (!commands.labels.empty()) {
					cout << " " << commands.labels.at(i) << ": " << flush;
				} else {
					cout << " " << commands.af_files.at(i) << ": " << flush;
				}
				cout << 1.0 << endl;
			}
		}

		end_time = clock();
		execution_time = (end_time - start_time) / (double) CLOCKS_PER_SEC;
		cout << "Done (" << execution_time << " sec)" << endl;

		Tagger tagger(&markers, &graphs_per_file);

		if (commands.output_summary_file != NULL) {
			fill_n(std::ostream_iterator<char>(std::cout), 78, '-');
			cout << endl << "Writing summary... " << endl;
			start_time = clock();

			if (commands.output_summary_file != NULL) {
				cout << " Summary file: " << commands.output_summary_file << endl;
				tagger.print_summary(commands.output_summary_file, true, commands.labels);
			}

			end_time = clock();
			execution_time = (end_time - start_time) / (double) CLOCKS_PER_SEC;
			cout << "Done (" << execution_time << " sec)" << endl;
		}

		fill_n(std::ostream_iterator<char>(std::cout), 78, '=');
		if (commands.weights_all) {
			cout << endl << "Tagging (weighting by all tagged markers)... " << endl;
		} else {
			cout << endl << "Tagging (weighting by unique tagged markers)... " << endl;
		}
		start_time = clock();

		tagger.tag(fix_markers, commands.weights_all);

		cout << " Tags: " << tagger.get_n_tags() << endl;
		cout << " Tagged: " << tagger.get_n_tagged() << endl;
		for (unsigned int i = 0; i < commands.af_files.size(); ++i) {
			if (!commands.labels.empty()) {
				cout << " Tagged in " << commands.labels.at(i) << ": " << flush;
			} else {
				cout << " Tagged in " << commands.af_files.at(i) << ": " << flush;
			}
			cout << tagger.get_n_tagged(graphs_per_file.at(i)) << endl;
		}

		end_time = clock();
		execution_time = (end_time - start_time) / (double) CLOCKS_PER_SEC;
		cout << "Done (" << execution_time << " sec)" << endl;

		if ((commands.output_tags_file != NULL) || (commands.output_tagged_file != NULL)) {
			fill_n(std::ostream_iterator<char>(std::cout), 78, '-');
			cout << endl << "Writing results... " << endl;
			start_time = clock();

			if (commands.output_tags_file != NULL) {
				cout << " Tags file: " << commands.output_tags_file << endl;
				tagger.print_tags(commands.output_tags_file, true, commands.labels);
			}

			if (commands.output_tagged_file != NULL) {
				cout << " Tagged file: " << commands.output_tagged_file << endl;
				tagger.print_tagged(commands.output_tagged_file, true, commands.labels);
			}

			end_time = clock();
			execution_time = (end_time - start_time) / (double) CLOCKS_PER_SEC;
			cout << "Done (" << execution_time << " sec)" << endl;
		}
		fill_n(std::ostream_iterator<char>(std::cout), 78, '=');
		cout << endl << "FINAL DATASET HAS " << tagger.get_n_tags() << " TAG(S)." << endl;
		fill_n(std::ostream_iterator<char>(std::cout), 78, '=');
		cout << endl;
	} catch (Exception &e) {
		cout << e.what() << endl;
	}

	for (unsigned int i = 0u; i < passed_maf_per_file.size(); ++i) {
		free(passed_maf_per_file.at(i));
	}
	passed_maf_per_file.clear();

	return 0;
}



