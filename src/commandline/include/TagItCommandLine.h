#ifndef TAGITCOMMANDLINE_H_
#define TAGITCOMMANDLINE_H_

#include "CommandLine.h"

class TagItCommandLine: public CommandLine {
protected:
	void initialize_commands_dictionary();

public:
	static const char* AF;
	static const char* LD;
	static const char* LABEL;
	static const char* FIX;
	static const char* EXCLUDE;
	static const char* HIDE;
	static const char* EXCLUDE_MAF;
	static const char* HIDE_MAF;
	static const char* R2;
	static const char* WEIGHT;
	static const char* MARKER_WEIGHT;
	static const char* UNIQUE;
	static const char* OUT_SUMMARY;
	static const char* OUT_TAGS;
	static const char* OUT_TAGGED;

	bool print_help;

	vector<const char*> af_files;
	vector<const char*> ld_files;
	vector<const char*> labels;
	const char* fix_file;
	const char* exclude_file;
	const char* hide_file;
	double maf_threshold;
	bool hide_maf;
	double r2_threshold;
	vector<double> weights;
	vector<const char*> marker_weights_files;
	const char* output_summary_file;
	const char* output_tags_file;
	const char* output_tagged_file;
	bool weights_all;

	TagItCommandLine();
	virtual ~TagItCommandLine();

	void parse_command_line() throw (Exception);

	void help();
};

#endif
