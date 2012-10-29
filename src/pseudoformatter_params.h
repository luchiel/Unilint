#ifndef PSEUDOFORMATTER_PARAMS_H
#define PSEUDOFORMATTER_PARAMS_H

#include <stack>

enum Section { S_TYPE, S_VAR, S_CODE };

struct PseudoFormatterParams
{
    int depth;
    int current_line_index;
    std::string current_line;
    std::map<std::string, NameType> list_of_names;
    //TODO: pascal vars -> indentation
    bool indented_operation_expected;
    bool indented_operation_expected_after_braces;

    int braces_opened;
    int indentation_end;
    int operation_per_line_count;

    int depth_by_fact;

    bool perform_indentation_size_check;

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
        perform_indentation_size_check(false)
    {
        section.push(S_CODE);
    }
};

#endif
