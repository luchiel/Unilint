#ifndef CHECKER_H
#define CHECKER_H

#include <fstream>
#include <map>
#include <string>
#include <list>
#include <utility>
#include "settings.h"
#include "language.h"
#include "token.h"
#include "result_collector.h"
#include "pseudoformatter.h"
#include "pseudoformatter_params.h"

#define LANG_PATH "langs"

class Checker
{
private:
    std::ifstream file_to_process;
    std::pair<Language, std::string> file_language;
    Settings settings;
    int empty_lines_counter;
    int previous_depth;

    Environment env;
    ResultCollector results;

    std::list<Token> tokens;
    std::map<std::string, TokenType> token_types;
    std::list<Token>::iterator token;

    void set_language(const std::string& language_);
    void init_token_types();
    void add_result(const std::string& error);
    void add_result(int index, const std::string& error);
    PseudoFormatterPtr new_formatter(const std::string& s_);

    void check_line_length();
    void check_if_empty();
    void check_newline_at_eof();

    void whitespace_sequence_check();
    void prefix_preprocessing();

    void brace_check();
    void unibracket_check();
    void spaces_in_unibrackets_check(int offset);
    void blockbracket_check();

    void name_style_check(NameType type);

    void nesting_depth_check();

    void keyword_and_brace_check();
    void else_check();

    bool keyword_equal(const std::string& to);

    void calculate_expected_depth();
    void check_indentation();
    void process_tokens();

    bool is_blockbracket();
    bool is_pascal_functional_begin();

    void check_if_declaration_block_is_over();
    bool try_bind_to_title();
    void try_add_indent();
    bool next_is_blockbracket(bool include_current);
    bool previous_is_blockbracket();

    struct LineInfo
    {
        std::string line;
        int size;
        int depth_by_fact;
        int expected_depth;
        int expected_extra_depth;
        bool can_check_indent;
        LineInfo(
            std::string line_, int size_, int depth_by_fact_, int expected_depth_, int expected_extra_depth_
        ):
            line(line_),
            size(size_),
            depth_by_fact(depth_by_fact_),
            expected_depth(expected_depth_),
            expected_extra_depth(expected_extra_depth_),
            can_check_indent(true)
        {}
    };
    std::vector<LineInfo> lines;

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
