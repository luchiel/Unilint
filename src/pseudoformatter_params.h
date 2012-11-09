#ifndef PSEUDOFORMATTER_PARAMS_H
#define PSEUDOFORMATTER_PARAMS_H

#include <map>
#include <stack>
#include <string>
#include <vector>
#include "settings.h"
#include "language.h"

enum Section { S_TYPE, S_VAR, S_CODE };

struct Environment
{
    std::stack<int> open_if_count_before_blockbracket;

    std::string current_line;
    int current_line_index;

    std::map<std::string, NameType> list_of_names;

    bool line_closed;

    bool indented_operation_expected;
    bool indented_operation_expected_after_braces;

    int braces_opened;

    bool case_unmatched;

    std::stack<Section> section;
    std::stack<int> declaration_depth;
    std::stack< std::pair<int, bool> > if_depth;
    std::stack<int> close_on_end;

    int expected_depth;
    int expected_extra_depth;

    Environment():
        current_line_index(0),
        line_closed(false),
        indented_operation_expected(false),
        indented_operation_expected_after_braces(false),
        braces_opened(0),
        case_unmatched(false),
        expected_depth(0),
        expected_extra_depth(0)
    {
        close_on_end.push(0);
        section.push(S_CODE);
    }

    void open_blockbracket();
    void close_blockbracket();

    void close_opened_statements();
};

#endif
