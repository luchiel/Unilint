#ifndef PSEUDOFORMATTER_PARAMS_H
#define PSEUDOFORMATTER_PARAMS_H

#include <map>
#include <stack>
#include <string>
#include "settings.h"
#include "language.h"

enum Section { S_TYPE, S_VAR, S_CODE };

struct PseudoFormatterParams
{
private:
    std::stack<int> open_if_count_before_blockbracket;
    std::stack<int> close_on_end;
    std::stack< std::pair<int, bool> > if_depth;
    std::stack<int> title_opened_at_depth;

public:
    int depth;
    int current_line_index;
    std::string current_line;
    std::map<std::string, NameType> list_of_names;
    bool line_closed;
    bool indented_operation_expected;
    bool indented_operation_expected_after_braces;

    int braces_opened;
    int indentation_end;
    int operation_per_line_count;
    int depth_by_fact;
    int previous_depth;

    bool perform_indentation_size_check;
    bool if_had_blockbracket;
    bool case_unmatched;

    std::stack<Section> section;
    Language language;

    PseudoFormatterParams():
        depth(0),
        current_line_index(0),
        line_closed(false),
        indented_operation_expected(false),
        indented_operation_expected_after_braces(false),
        braces_opened(0),
        indentation_end(0),
        operation_per_line_count(0),
        depth_by_fact(0),
        previous_depth(0),
        perform_indentation_size_check(false),
        if_had_blockbracket(false),
        case_unmatched(false)
    {
        close_on_end.push(0);
        section.push(S_CODE);
    }

    void open_blockbracket();
    void close_blockbracket();
    void save_if_depth();
    void restore_last_if_depth();
    void open_statement();
    void close_opened_statements();
    void create_title();
    bool try_bind_to_title();
    int title_depth();
    void try_close_title();

    bool is_pascal_main_begin();
    bool is_pascal_main_end();

    void init_new_line();
};

#endif
