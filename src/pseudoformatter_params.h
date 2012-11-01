#ifndef PSEUDOFORMATTER_PARAMS_H
#define PSEUDOFORMATTER_PARAMS_H

#include <map>
#include <stack>
#include <string>
#include "settings.h"

enum Section { S_TYPE, S_VAR, S_CODE };

struct PseudoFormatterParams
{
private:
    std::stack<int> open_if_count_before_blockbracket;
    std::stack<int> close_on_end;

public:
    int depth;
    int current_line_index;
    std::string current_line;
    std::map<std::string, NameType> list_of_names;
    bool indented_operation_expected;
    bool indented_operation_expected_after_braces;

    int braces_opened;
    int indentation_end;
    int operation_per_line_count;
    int depth_by_fact;
    int previous_depth;

    bool perform_indentation_size_check;

    std::stack<int> if_depth;
    std::stack<Section> section;

    PseudoFormatterParams():
        depth(0),
        current_line_index(0),
        indented_operation_expected(false),
        indented_operation_expected_after_braces(false),
        braces_opened(0),
        indentation_end(0),
        operation_per_line_count(0),
        depth_by_fact(0),
        previous_depth(0),
        perform_indentation_size_check(false)
    {
        close_on_end.push(0);
        section.push(S_CODE);
    }

    void restore_last_if_depth();
    void close_blockbracket();
    void open_blockbracket();
    void open_statement();
    void close_opened_statements();
};

#endif
