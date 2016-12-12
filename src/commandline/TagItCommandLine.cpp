#include "include/TagItCommandLine.h"

const char* TagItCommandLine::AF = "--af";
const char* TagItCommandLine::LD = "--ld";
const char* TagItCommandLine::LABEL = "--label";
const char* TagItCommandLine::FIX = "--fix";
const char* TagItCommandLine::EXCLUDE = "--exclude";
const char* TagItCommandLine::HIDE = "--hide";
const char* TagItCommandLine::EXCLUDE_MAF = "--exclude-maf";
const char* TagItCommandLine::HIDE_MAF = "--hide-maf";
const char* TagItCommandLine::R2 = "--r2";
const char* TagItCommandLine::WEIGHT = "--weight";
const char* TagItCommandLine::MARKER_WEIGHT = "--marker-weight";
const char* TagItCommandLine::UNIQUE = "--unique";
const char* TagItCommandLine::OUT_SUMMARY = "--out-summary";
const char* TagItCommandLine::OUT_TAGS = "--out-tags";
const char* TagItCommandLine::OUT_TAGGED = "--out-tagged";

TagItCommandLine::TagItCommandLine() : CommandLine(), print_help(false),
		fix_file(NULL), exclude_file(NULL), hide_file(NULL),
		maf_threshold(0.0), hide_maf(false), r2_threshold(0.0),
		output_summary_file(NULL), output_tags_file(NULL), output_tagged_file(NULL),
		weights_all(true) {

}

TagItCommandLine::~TagItCommandLine() {

}

void TagItCommandLine::initialize_commands_dictionary() {
	CommandLine::initialize_commands_dictionary();

	commands_dictionary.insert(AF);
	commands_dictionary.insert(LD);
	commands_dictionary.insert(LABEL);
	commands_dictionary.insert(FIX);
	commands_dictionary.insert(EXCLUDE);
	commands_dictionary.insert(HIDE);
	commands_dictionary.insert(EXCLUDE_MAF);
	commands_dictionary.insert(HIDE_MAF);
	commands_dictionary.insert(R2);
	commands_dictionary.insert(WEIGHT);
	commands_dictionary.insert(MARKER_WEIGHT);
	commands_dictionary.insert(UNIQUE);
	commands_dictionary.insert(OUT_SUMMARY);
	commands_dictionary.insert(OUT_TAGS);
	commands_dictionary.insert(OUT_TAGGED);
}

void TagItCommandLine::parse_command_line() throw (Exception) {
	vector<const char*>* arguments = NULL;
	double dbl_value = 0.0;

	command_line_it = command_line.find(HELP);
	if (command_line_it != command_line.end()) {
		arguments = command_line_it->second;
		if (arguments->size() > 0) {
			throw Exception(NULL, 0, "Extra arguments after the '%s' command.\nSpecify '%s' (without any arguments) for the command line description.", HELP, HELP);
		}
		print_help = true;
		return;
	}

	command_line_it = command_line.find(AF);
	if (command_line_it != command_line.end()) {
		arguments = command_line_it->second;
		if (arguments->size() == 0) {
			throw Exception(NULL, 0, "Specify at least one file with markers and allele frequencies after the '%s' command.\nSpecify '%s' for the command line description.", AF, HELP);
		}
		for (unsigned int i = 0u; i < arguments->size(); ++i) {
			af_files.push_back(arguments->at(i));
		}
	} else {
		throw Exception(NULL, 0, "Specify file(s) with markers and allele frequencies after the '%s' command.\nSpecify '%s' for the command line description.", AF, HELP);
	}

	command_line_it = command_line.find(LD);
	if (command_line_it != command_line.end()) {
		arguments = command_line_it->second;
		if (arguments->size() != af_files.size()) {
			throw Exception(NULL, 0, "Number of LD files after the '%s' command must match number of files after the '%s' command.\nSpecify '%s' for the command line description.", LD, AF, HELP);
		}
		for (unsigned int i = 0u; i < arguments->size(); ++i) {
			ld_files.push_back(arguments->at(i));
		}
	} else {
		throw Exception(NULL, 0, "Specify LD file(s) after the '%s' command.\nSpecify '%s' for the command line description.", LD, HELP);
	}

	command_line_it = command_line.find(LABEL);
	if (command_line_it != command_line.end()) {
		arguments = command_line_it->second;
		if (arguments->size() != af_files.size()) {
			throw Exception(NULL, 0, "Number of labels after the '%s' command must match number of files after the '%s' command.\nSpecify '%s' for the command line description.", LABEL, AF, HELP);
		}
		for (unsigned int i = 0u; i < arguments->size(); ++i) {
			labels.push_back(arguments->at(i));
		}
	}

	command_line_it = command_line.find(FIX);
	if (command_line_it != command_line.end()) {
		arguments = command_line_it->second;
		if (arguments->size() > 1) {
			throw Exception(NULL, 0, "Extra arguments after the '%s' command.\nSpecify '%s' for the command line description.", FIX, HELP);
		} else if (arguments->size() < 1) {
			throw Exception(NULL, 0, "Missing argument after the '%s' command.\nSpecify '%s' for the command line description.", FIX, HELP);
		}
		fix_file = arguments->at(0);
	}

	command_line_it = command_line.find(EXCLUDE);
	if (command_line_it != command_line.end()) {
		arguments = command_line_it->second;
		if (arguments->size() > 1) {
			throw Exception(NULL, 0, "Extra arguments after the '%s' command.\nSpecify '%s' for the command line description.", EXCLUDE, HELP);
		} else if (arguments->size() < 1) {
			throw Exception(NULL, 0, "Missing arguments after the '%s' command.\nSpecify '%s' for the command line description.", EXCLUDE, HELP);
		}
		exclude_file = arguments->at(0);
	}

	command_line_it = command_line.find(HIDE);
	if (command_line_it != command_line.end()) {
		arguments = command_line_it->second;
		if (arguments->size() > 1) {
			throw Exception(NULL, 0, "Extra arguments after the '%s' command.\nSpecify '%s' for the command line description.", HIDE, HELP);
		} else if (arguments->size() < 1) {
			throw Exception(NULL, 0, "Missing arguments after the '%s' command.\nSpecify '%s' for the command line description.", HIDE, HELP);
		}
		hide_file = arguments->at(0);
	}

	if ((command_line.count(EXCLUDE_MAF) > 0) && (command_line.count(HIDE_MAF) > 0)) {
		throw Exception(NULL, 0, "The '%s' and '%s' commands can't be specified simultaneously.\nSpecify '%s' for the command line description", EXCLUDE_MAF, HIDE_MAF, HELP);
	}

	command_line_it = command_line.find(EXCLUDE_MAF);
	if (command_line_it != command_line.end()) {
		arguments = command_line_it->second;
		if (arguments->size() > 1) {
			throw Exception(NULL, 0, "Extra arguments after the '%s' command.\nSpecify '%s' for the command line description.", EXCLUDE_MAF, HELP);
		} else if (arguments->size() < 1) {
			throw Exception(NULL, 0, "Missing argument after the '%s' command.\nSpecify '%s' for the command line description.", EXCLUDE_MAF, HELP);
		}

		if (!auxiliary::to_dbl(arguments->at(0), &maf_threshold) || (maf_threshold < 0.0) || (maf_threshold >= 0.5)) {
			throw Exception(NULL, 0, "Invalid minor allele frequency threshold after the '%s' command.\nSpecify '%s' for the command line description", EXCLUDE_MAF, HELP);
		}

		hide_maf = false;
	}

	command_line_it = command_line.find(HIDE_MAF);
	if (command_line_it != command_line.end()) {
		arguments = command_line_it->second;
		if (arguments->size() > 1) {
			throw Exception(NULL, 0, "Extra arguments after the '%s' command.\nSpecify '%s' for the command line description.", HIDE_MAF, HELP);
		} else if (arguments->size() < 1) {
			throw Exception(NULL, 0, "Missing argument after the '%s' command.\nSpecify '%s' for the command line description.", HIDE_MAF, HELP);
		}

		if (!auxiliary::to_dbl(arguments->at(0), &maf_threshold) || (maf_threshold < 0.0) || (maf_threshold >= 0.5)) {
			throw Exception(NULL, 0, "Invalid minor allele frequency threshold after the '%s' command.\nSpecify '%s' for the command line description", HIDE_MAF, HELP);
		}

		hide_maf = true;
	}

	command_line_it = command_line.find(R2);
	if (command_line_it != command_line.end()) {
		arguments = command_line_it->second;
		if (arguments->size() > 1) {
			throw Exception(NULL, 0, "Extra arguments after the '%s' command.\nSpecify '%s' for the command line description.", R2, HELP);
		} else if (arguments->size() < 1) {
			throw Exception(NULL, 0, "Missing arguments after the '%s' command.\nSpecify '%s' for the command line description.", R2, HELP);
		}

		if (!auxiliary::to_dbl(arguments->at(0), &r2_threshold) || (r2_threshold <= 0.0) || (r2_threshold > 1.0)) {
			throw Exception(NULL, 0, "Invalid r^2 threshold after the '%s' command.\nSpecify '%s' for the command line description.", R2, HELP);
		}
	} else {
		throw Exception(NULL, 0, "Specify r^2 threshold after the '%s' command.\nSpecify '%s' for the command line description.", R2, HELP);
	}

	if ((command_line.count(WEIGHT) > 0) && (command_line.count(MARKER_WEIGHT) > 0)) {
		throw Exception(NULL, 0, "The '%s' and '%s' commands can't be specified simultaneously.\nSpecify '%s' for the command line description", WEIGHT, MARKER_WEIGHT, HELP);
	}

	command_line_it = command_line.find(WEIGHT);
	if (command_line_it != command_line.end()) {
		arguments = command_line_it->second;
		if (arguments->size() != af_files.size()) {
			throw Exception(NULL, 0, "Number of weights after the '%s' command must match number of files after the '%s' command.\nSpecify '%s' for the command line description.", WEIGHT, AF, HELP);
		}
		for (unsigned int i = 0u; i < arguments->size(); ++i) {
			if (!auxiliary::to_dbl(arguments->at(i), &dbl_value) || (dbl_value <= 0.0)) {
				throw Exception(NULL, 0, "Invalid weight(s) after the '%s' command.\nSpecify '%s' for the command line description", WEIGHT, HELP);
			}
			weights.push_back(dbl_value);
		}

	}

	command_line_it = command_line.find(MARKER_WEIGHT);
	if (command_line_it != command_line.end()) {
		arguments = command_line_it->second;
		if (arguments->size() != af_files.size()) {
			throw Exception(NULL, 0, "Number of marker weights files after the '%s' command must match number of files after the '%s' command.\nSpecify '%s' for the command line description.", MARKER_WEIGHT, AF, HELP);
		}
		for (unsigned int i = 0u; i < arguments->size(); ++i) {
			marker_weights_files.push_back(arguments->at(i));
		}
	}

	command_line_it = command_line.find(OUT_SUMMARY);
	if (command_line_it != command_line.end()) {
		arguments = command_line_it->second;
		if (arguments->size() > 1) {
			throw Exception(NULL, 0, "Extra arguments after the '%s' command.\nSpecify '%s' for the command line description.", OUT_SUMMARY, HELP);
		} else if (arguments->size() < 1) {
			throw Exception(NULL, 0, "Missing arguments after the '%s' command.\nSpecify '%s' for the command line description.", OUT_SUMMARY, HELP);
		}
		output_summary_file = arguments->at(0);
	}

	command_line_it = command_line.find(OUT_TAGS);
	if (command_line_it != command_line.end()) {
		arguments = command_line_it->second;
		if (arguments->size() > 1) {
			throw Exception(NULL, 0, "Extra arguments after the '%s' command.\nSpecify '%s' for the command line description.", OUT_TAGS, HELP);
		} else if (arguments->size() < 1) {
			throw Exception(NULL, 0, "Missing arguments after the '%s' command.\nSpecify '%s' for the command line description.", OUT_TAGS, HELP);
		}
		output_tags_file = arguments->at(0);
	}

	command_line_it = command_line.find(OUT_TAGGED);
	if (command_line_it != command_line.end()) {
		arguments = command_line_it->second;
		if (arguments->size() > 1) {
			throw Exception(NULL, 0, "Extra arguments after the '%s' command.\nSpecify '%s' for the command line description.", OUT_TAGGED, HELP);
		} else if (arguments->size() < 1) {
			throw Exception(NULL, 0, "Missing arguments after the '%s' command.\nSpecify '%s' for the command line description.", OUT_TAGGED, HELP);
		}
		output_tagged_file = arguments->at(0);
	}

	command_line_it = command_line.find(UNIQUE);
	if (command_line_it != command_line.end()) {
		if (command_line_it->second->size() > 0) {
			throw Exception(NULL, 0, "Extra arguments after the '%s' command.\nSpecify '%s' for the command line description.", UNIQUE, HELP);
		}
		weights_all = false;
	}
}

void TagItCommandLine::help() {
	cout << endl;
	cout << "Tag(ging)It(erative) v 1.0.8" << endl;
	cout << endl << endl;

	cout << "Usage: ";
	cout << "tagit ";
	cout << AF << " <file_1> ... <file_N> ";
	cout << LD << " <file_1> ... <file_N> ";
	cout << R2 << " <value>";
	cout << endl << endl;

	cout << "Mandatory arguments" << endl;
	cout << " " << AF << "\t\t\tList of tab delimited files (*.gz is supported) with markers and allele frequencies. ";
	cout <<	"The mandaroty columns are: CHROM, POS, N_ALLELES, N_CHR, {ALLELE:FREQ}. ";
	cout << "Typically, such files are generated with the VCFtools software using '--freq' option.";
	cout << endl << endl;

	cout << " " << LD << "\t\t\tList of tab delimited LD files (*.gz is supported) with the pairwise r2 correlation coefficients. ";
	cout << "The mandatory columns are: MARKER1, MARKER2, AF1, AF2, R2, R. ";
	cout << "The header starts with the '#' symbol.";
	cout << endl << endl;

	cout << " " << R2 << "\t\t\tAn LD threshold. ";
	cout << "Threshold values are from the (0, 1] interval. ";
	cout << "Marker M1 tags marker M2 if r^2 between them is greater or equal than the specified threshold.";
	cout << endl << endl;

	cout << "Output files" << endl;
	cout << " " << OUT_SUMMARY << "\t\tTab delimited output file (gzip compressed) of all markers before the tagging process. " << endl;
	cout << "Output format is identical to " << OUT_TAGS << ", but stores the information for all markers.";
	cout << endl << endl;

	cout << " " << OUT_TAGS << "\t\tTab delimited output file (gzip compressed) of tag markers with columns: MARKER, WEIGHT.ALL, WEIGHT.UNIQUE, [1/<LABEL_1>].WEIGHT, [2/<LABEL_2>].WEIGHT, ..., [N/<LABEL_N>].WEIGHT. ";
	cout << "The MARKER column stores the marker name. ";
	cout << "The WEIGHT.ALL column stores the tag weight which is the sum of weights of all tagged markers across populations. ";
	cout << "The WEIGHT.UNIQUE column stores the tag weight which is the sum of weights of unique tagged markers across populations. ";
	cout << "The order of the [i/<LABEL_i>].WEIGHT columns follows the order of specified files in " << AF << ", " << LD << " and " << LABEL << "(if specified) commands. ";
	cout << "Every [i/<LABEL_i>].WEIGHT column stores the tag weight (analogous to WEIGHT.ALL) only for population i.";
	cout << endl << endl;

	cout << " " << OUT_TAGGED << "\t\tTab delimited output file (gzip compressed) of tagged markers with columns: MARKER, 1/<LABEL_1>, 2/<LABEL_2>, ..., N/<LABEL_N>. ";
	cout << "The MARKER column stores the marker name. ";
	cout << "The order of the i/<LABEL_i> columns follows the order of specified files in " << AF << ", " << LD << " and " << LABEL << "(if specified) commands. ";
	cout << "Every i/<LABEL_i> column stores 0 or 1, where 1 means that the marker was tagged in population i.";
	cout << endl << endl;

	cout << "Filtering markers" << endl;
	cout << " " << FIX << "\t\t\tOptional file (*.gz is supported) with fixed tag markers. ";
	cout << "These markers are set as tags before any other. ";
	cout << "One marker per line. Marker identifiers should be in the '[chr]:[position]' format. No header.";
	cout << endl << endl;

	cout << " " << EXCLUDE << "\t\tOptional file (*.gz is supported) with excluded markers. ";
	cout << "These markers are set as non-tag and can't ne tagged by other markers. One marker per line. Marker identifiers should be in the '[chr]:[position]' format. No header.";
	cout << endl << endl;

	cout << " " << HIDE << "\t\t\tOptional file (*.gz is supported) with markers to hide. ";
	cout << "These markers are set as non-tag, but can be tagged by other markers. One marker per line. Marker identifiers should be in the '[chr]:[position]' format. No header.";
	cout << endl << endl;

	cout << "Filtering markers by frequency" << endl;
	cout << " " << EXCLUDE_MAF << "\t\tA minor allele frequency (MAF) threshold. ";
	cout << "Threshold values are from the [0.0, 0.5) interval, default threshold is >= 0.0. ";
	cout << "If marker doesn't satisfy the MAF threshold, then it will not tag any marker in a population and will be not tagged by other markers in that population. ";
	cout << "This is the default option with the default MAF threshold >= 0.0.";
	cout << endl << endl;

	cout << " " << HIDE_MAF << "\t\tA minor allele frequency (MAF) threshold. ";
	cout << "Threshold values are from the [0.0, 0.5) interval." ;
	cout << "If marker doesn't satisfy the MAF threshold, then it will not tag any marker in a population but can be tagged by other markers in that population.";
	cout << endl << endl;

	cout << "Weighting" <<  endl;
	cout << " " << WEIGHT << "\t\tOptional list of weights for every specific LD file. ";
	cout << "Weights must be greater than 0. Default weights are all set to 1.";
	cout << endl << endl;

	cout << " " << MARKER_WEIGHT << "\tOptional list of files (*.gz is supported) with SNP weights. ";
	cout << "Weights must be greater than 0. For those SNPs that are not in the specified files, the weight is set to 1.";
	cout << endl << endl;

	cout << " " << UNIQUE << "\t\tWhen computing tag weight, consider only unique markers tagged across all populations. ";
	cout << "By default all markers are considered.";
	cout << endl << endl;

	cout << "Other" << endl;
	cout << " " << LABEL << "\t\tOptional list of short labels for every specified LD file. The short labels will be used in the output.";
	cout << endl << endl;

	cout << " " << HELP << "\t\t\tPrints this message.";
	cout << endl << endl;
}
