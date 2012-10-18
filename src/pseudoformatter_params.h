#ifndef PSEUDOFORMATTER_PARAMS_H
#define PSEUDOFORMATTER_PARAMS_H

struct PseudoFormatterParams
{
    int depth;
    int current_line_index;
    std::string current_line;
    std::map<std::string, NameType> list_of_names;
    //TODO: pascal vars -> indentation
    bool indented_operation_expected;
    bool indented_operation_expected_after_braces;
    bool indent_was_depth_plus_one;
    int braces_opened;
    int indentation_end;

    PseudoFormatterParams():
        depth(0), current_line_index(0),
        indented_operation_expected(false),
        indented_operation_expected_after_braces(false),
        indent_was_depth_plus_one(false),
        braces_opened(0),
        indentation_end(0) {}
};

#endif
