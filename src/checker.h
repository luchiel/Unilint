#ifndef CHECKER_H
#define CHECKER_H

#include <fstream>
#include <map>
#include <string>
#include <utility>
#include "settings.h"
#include "result_collector.h"
#include "pseudoformatter.h"
#include "pseudoformatter_params.h"

#define LANG_PATH "langs"

enum Language
{
    L_BASIC,
    L_C,
    L_CPP,
    L_CSHARP,
    L_HASKELL,
    L_JAVA,
    L_PASCAL,
    L_PERL,
    L_PHP,
    L_PYTHON,
    L_RUBY,
};

class Checker
{
private:
    std::ifstream file_to_process;
    std::pair<Language, std::string> file_language;
    Settings settings;
    int empty_lines_counter;
    ResultCollector results;
    PseudoFormatterParams formatter_params;

    void set_language(const std::string& language_);
    PseudoFormatterPtr new_formatter(const std::string& s_);

    void check_line_length();
    void check_if_empty();
    void check_newline_at_eof();

public:
    Checker(
        const std::string& filename_,
        const std::string& language_,
        const std::string& settings_
    );
    ~Checker();
    void process_file();
    void output_results_to_file(const std::string& results_);
};

#endif
