#include <cstdlib>
#include "pseudoformatter.h"
#include "exception.h"

bool PseudoFormatter::is_opening_blockbracket(const std::string& s)
{
    //TODO: Begin, BEGIN etc
    return s == "{" || s == "begin";
}

void PseudoFormatter::indent_error_check(int expected_depth, int scale, int start)
{
    if(formatter_params.depth_by_fact < expected_depth * scale)
        results.add(0, "indent too small");
    else if(formatter_params.depth_by_fact > expected_depth * scale)
        results.add(0, "indent too deep");
}

void PseudoFormatter::blockbracket_check(const std::string& s, int start)
{
    //TODO: nonsensitive func in pascal grammar
    bool is_keyword_declaring_varblock = element == "keyword_declaring_varblock";
    if(is_opening_blockbracket(s) || is_keyword_declaring_varblock)
    {
        bool can_check = formatter_params.try_bind_to_title() || is_keyword_declaring_varblock;

        ExtendedBoolean& block_at_newline(settings.ext_bool_options["start_block_at_newline"]);
        if(can_check && block_at_newline == EB_CONSISTENT)
        {
            block_at_newline = start == formatter_params.indentation_end ? EB_TRUE : EB_FALSE;
        }
        else if(block_at_newline == EB_TRUE && start != formatter_params.indentation_end)
        {
            results.add(start, s + " is not at new line");
        }
        else if(can_check && block_at_newline == EB_FALSE && start == formatter_params.indentation_end)
        {
            results.add(start, s + " is at new line");
        }

        formatter_params.open_blockbracket();
    }
    else
    {
        formatter_params.close_blockbracket();
    }
}

void PseudoFormatter::nesting_depth_check(int start)
{
    int maximal_nesting_depth = settings.int_options["maximal_nesting_depth"];
    if(
        formatter_params.depth != formatter_params.previous_depth &&
        maximal_nesting_depth != IGNORE_OPTION &&
        formatter_params.depth > maximal_nesting_depth)
    {
        results.add(start, "maximal nesting depth exceeded");
    }
    formatter_params.previous_depth = formatter_params.depth;
}

void PseudoFormatter::whitespace_sequence_check(const std::string& s, int start)
{
    if(start + s.size() == formatter_params.current_line.size())
    {
        if(settings.ext_bool_options["forbid_trailing_whitespaces"] != EB_IGNORE)
            results.add(start, "trailing whitespaces");
    }
    else if(start == 0)
    {
        formatter_params.indentation_end = s.size();

        if(settings.indentation_style != IS_IGNORE)
            prefix_preprocessing(s);
    }
    else
    {
        if(s.size() > 1 && settings.ext_bool_options["forbid_vertical_alignment"] != EB_IGNORE)
            results.add(start, "more than one whitespace");
    }
}

void PseudoFormatter::token_check(const std::string& s, int start)
{
    if(s == "else")
    {
        formatter_params.close_opened_statements();

        if(settings.ext_bool_options["forbid_multiple_expressions_per_line"] == EB_TRUE)
            formatter_params.line_closed =
                start != formatter_params.indentation_end && !formatter_params.if_had_blockbracket;

        formatter_params.restore_last_if_depth();
    }

    //check multiple expressions per line
    if(formatter_params.line_closed && element != "comment" && element != "blockbracket")
    {
        results.add(start, "two or more expressions at one line");
        formatter_params.line_closed = false;
    }

    //check if compulsory block brackets are present
    if(
        formatter_params.indented_operation_expected &&
        settings.ext_bool_options["compulsory_block_braces"] == EB_TRUE &&
        element != "blockbracket")
    {
        results.add(start, "block brace expected: { or begin");
    }

    //calculate depth and check indentation
    bool call_indent_error_check =
        start == formatter_params.indentation_end && (
            formatter_params.perform_indentation_size_check ||
            start == 0 && settings.indentation_style != IS_IGNORE);

    bool is_inside_indented_block_without_end =
        formatter_params.section.top() != S_CODE &&
        element != "varblock" && element != "typeblock" &&
        element != "keyword_declaring_func" &&
        !(element == "blockbracket" && is_opening_blockbracket(s));

    ExtendedBoolean& ext_extra_indent(settings.ext_bool_options["extra_indent_for_blocks"]);

    if(is_inside_indented_block_without_end)
    {
        if(formatter_params.try_bind_to_title() && ext_extra_indent == EB_TRUE)
            formatter_params.depth++;

        formatter_params.depth++;
    }

    int indentation_size = 1;
    if(settings.indentation_style == IS_SPACES && settings.indentation_policy == IP_BY_SIZE)
        indentation_size = settings.int_options["indentation_size"];

    bool is_keyword_declaring_varblock = element == "keyword_declaring_varblock";

    if(element == "blockbracket" || is_keyword_declaring_varblock)
    {
        if(ext_extra_indent == EB_CONSISTENT && start == formatter_params.indentation_end)
        {
            if(is_keyword_declaring_varblock)
            {
                //depth == 1 here, because otherwise we'd have extra_indent set
                ext_extra_indent =
                    formatter_params.depth == 1 && start != indentation_size ? EB_TRUE : EB_FALSE;
            }
            else if(formatter_params.try_bind_to_title())
            {
                ext_extra_indent = formatter_params.depth == 0 && start != 0 ? EB_TRUE : EB_FALSE;
            }
        }

        bool extra_indent_required = formatter_params.try_bind_to_title() || is_keyword_declaring_varblock;

        if(is_opening_blockbracket(s) || is_keyword_declaring_varblock)
        {
            if(ext_extra_indent == EB_TRUE && extra_indent_required)
                formatter_params.depth++;
            if(call_indent_error_check)
                indent_error_check(formatter_params.depth, indentation_size, start);
        }
        else
        {
            if(call_indent_error_check)
                indent_error_check(formatter_params.depth - 1, indentation_size, start);
            if(ext_extra_indent == EB_TRUE && extra_indent_required)
            {
                formatter_params.depth--;
                if(formatter_params.depth == 1)
                    formatter_params.try_close_title();
            }
        }
    }
    else if(element == "varblock" || element == "typeblock")
    {
        if(
            ext_extra_indent == EB_CONSISTENT &&
            start == formatter_params.indentation_end &&
            formatter_params.try_bind_to_title())
        {
            ext_extra_indent = formatter_params.depth == 0 && start != 0 ? EB_TRUE : EB_FALSE;
        }

        if(call_indent_error_check)
            indent_error_check(
                formatter_params.depth + (
                    formatter_params.try_bind_to_title() && ext_extra_indent == EB_TRUE ? 1 : 0),
                indentation_size, start);
    }
    else
    {
        if(formatter_params.indented_operation_expected)
            formatter_params.open_statement();

        if(call_indent_error_check)
            indent_error_check(formatter_params.depth, indentation_size, start);
    }

    if(start == formatter_params.indentation_end)
        nesting_depth_check(start);

    if(is_inside_indented_block_without_end)
    {
        formatter_params.depth--;

        if(formatter_params.try_bind_to_title() && ext_extra_indent == EB_TRUE)
            formatter_params.depth--;
    }

    if(s == "if")
    {
        //TODO: nonsensitive
        formatter_params.save_if_depth();
    }

    //check if expected brace is found
    if(
        formatter_params.indented_operation_expected_after_braces &&
        formatter_params.braces_opened == 0 &&
        element != "brace")
    {
        results.add(start, "brace expected");
        formatter_params.indented_operation_expected_after_braces = false;
    }

    //indent was checked
    formatter_params.indented_operation_expected = false;
}

void PseudoFormatter::spaces_in_unibrackets_check(char c, int start, int offset)
{
    ExtendedBoolean* opt_ptr = NULL;
    char opposite;

    switch(c)
    {
        case '(':
        case ')':
            opt_ptr = &settings.ext_bool_options["spaces_inside_braces"];
            opposite = c == '(' ? ')' : '(';
            break;
        case '[':
        case ']':
            opt_ptr = &settings.ext_bool_options["spaces_inside_brackets"];
            opposite = c == '[' ? ']' : '[';
            break;
        case '{':
        case '}':
            opt_ptr = &settings.ext_bool_options["spaces_inside_curly_braces"];
            opposite = c == '{' ? '}' : '{';
            break;
        default:
            throw UnilintException("Not a bracket symbol");
    }

    ExtendedBoolean& spaces_inside(*opt_ptr);

    //empty brackets are exception
    if(formatter_params.current_line[start + offset] == opposite)
        return;

    if(spaces_inside == EB_CONSISTENT)
    {
        spaces_inside = formatter_params.current_line[start + offset] == ' ' ? EB_TRUE : EB_FALSE;
    }
    else if(spaces_inside == EB_TRUE && formatter_params.current_line[start + offset] != ' ')
    {
        results.add(
            start + offset,
            std::string("no space ") + (offset > 0 ? "after " : "before ") + c);
    }
    else if(spaces_inside == EB_FALSE && formatter_params.current_line[start + offset] == ' ')
    {
        results.add(
            start + offset,
            std::string("unexpected space ") + (offset > 0 ? "after " : "before ") + c);
    }
}

void PseudoFormatter::unibracket_check(char c, int start)
{
    if(c == '(' || c == '[' || c == '{')
    {
        if(start != formatter_params.current_line.size() - 1)
            spaces_in_unibrackets_check(c, start, 1);
    }
    else
    {
        if(start != 0 && start != formatter_params.indentation_end)
            spaces_in_unibrackets_check(c, start, -1);
    }
}

void PseudoFormatter::brace_check(const std::string& s, int start)
{
    if(s == "(")
    {
        formatter_params.braces_opened++;
        formatter_params.depth++;
    }
    else
    {
        formatter_params.braces_opened--;
        formatter_params.depth--;
        if(formatter_params.braces_opened < 0)
        {
            results.add(start, "unexpected " + s);
        }
        else if(
            formatter_params.braces_opened == 0 &&
            formatter_params.indented_operation_expected_after_braces)
        {
            formatter_params.indented_operation_expected_after_braces = false;
            formatter_params.indented_operation_expected = true;

            if(settings.ext_bool_options["forbid_multiple_expressions_per_line"] == EB_TRUE)
                formatter_params.line_closed = true;
        }
    }
}

void PseudoFormatter::prefix_preprocessing(const std::string& s)
{
    bool tabs = false;
    bool spaces = false;
    for(unsigned int i = 0; i < s.size(); ++i)
    {
        tabs = tabs || (s[i] == '\t');
        spaces = spaces || (s[i] == ' ');
        if(!tabs && !spaces)
        {
            results.add(i, "strange whitespace character");
            return;
        }
    }
    if(tabs && spaces)
    {
        results.add(0, "tabs and spaces mixed");
        return;
    }

    //replace IS_CONSISTENT with first detected style
    if(settings.indentation_style == IS_CONSISTENT)
        settings.indentation_style = tabs ? IS_TABS : IS_SPACES;

    if(tabs && settings.indentation_style == IS_SPACES)
    {
        results.add(0, "tabs found, spaces expected");
        //indentation_size can be undefined
        //so tabs cannot be translated to spaces
        return;
    }

    if(spaces)
    {
        if(settings.indentation_style == IS_TABS)
        {
            results.add(0, "spaces found, tabs expected");
            //indentation_size is not stored for style = tabs
            //so spaces cannot be translated to tabs
            return;
        }

        if(settings.indentation_policy == IP_IGNORE)
            return;

        //detect tab_size for IP_CONSISTENT
        if(settings.indentation_policy == IP_CONSISTENT)
        {
            settings.int_options["indentation_size"] = s.size();
            settings.indentation_policy = IP_BY_SIZE;
        }
    }

    formatter_params.depth_by_fact = s.size();
    formatter_params.perform_indentation_size_check = true;
}

void PseudoFormatter::name_style_check(const std::string& s, int start, const std::string& type)
{
    NamingStyle ns = settings.naming_styles[type];
    bool style_error = false;
    if(ns == NS_CAMEL || ns == NS_PASCAL)
    {
        style_error =
            ns == NS_CAMEL && !std::islower(s[0]) ||
            ns == NS_PASCAL && !std::isupper(s[0]);

        for(int i = 1; !style_error && i < s.size(); ++i)
        {
            style_error = !std::isalpha(s[i]) && !std::isdigit(s[i]);
        }
    }
    if(ns == NS_UNDERSCORE || ns == NS_CAPS_UNDERSCORE)
    {
        for(int i = 0; !style_error && i < s.size(); ++i)
        {
            style_error =
                s[i] != '_' &&
                !std::isdigit(s[i]) && (
                    ns == NS_UNDERSCORE && !islower(s[i]) ||
                    ns == NS_CAPS_UNDERSCORE && !isupper(s[i]));
        }
    }
    if(style_error)
    {
        std::string ns_string(
            ns == NS_CAMEL ? "camel" :
            ns == NS_PASCAL ? "pascal" :
            ns == NS_UNDERSCORE ? "underscore" :
                "caps_underscore");
        results.add(start, "name " + s + " is not of style " + ns_string);
    }
}

void PseudoFormatter::keyword_and_brace_check(const std::string& s, int start)
{
    ExtendedBoolean& space(settings.ext_bool_options["space_between_keyword_and_brace"]);

    int keyword_end = start + s.size();
    if(keyword_end == formatter_params.current_line.size())
        return;

    if(space == EB_CONSISTENT)
    {
        space = formatter_params.current_line[keyword_end] == ' ' ? EB_TRUE : EB_FALSE;
    }
    else if(space == EB_TRUE && formatter_params.current_line[keyword_end] != ' ')
    {
        results.add(keyword_end, "no space after " + s);
    }
    else if(space == EB_FALSE && formatter_params.current_line[keyword_end] == ' ')
    {
        results.add(keyword_end, "space after " + s);
    }
}

void PseudoFormatter::else_check(const std::string s, int start)
{
    ExtendedBoolean& at_newline(settings.ext_bool_options["else_at_newline"]);
    if(at_newline == EB_CONSISTENT)
    {
        at_newline = start == formatter_params.indentation_end ? EB_TRUE : EB_FALSE;
    }
    else if(at_newline == EB_TRUE && start != formatter_params.indentation_end)
    {
        results.add(start, "else is not at new line");
    }
    else if(at_newline == EB_FALSE && start == formatter_params.indentation_end)
    {
        results.add(start, "else is at new line");
    }
    formatter_params.if_had_blockbracket = false;
}

void PseudoFormatter::format(const std::string& s, const srchilite::FormatterParams* params)
{
    if(params == NULL)
        throw UnilintException("Something strange happened");

    if(element != "normal")
    {
        token_check(s, params->start);
    }
    else
    {
        whitespace_sequence_check(s, params->start);
        return;
    }

    if(element == "blockbracket")
    {
        if(is_opening_blockbracket(s))
            formatter_params.section.push(S_CODE);
        else
            formatter_params.section.pop();

        blockbracket_check(s, params->start);
        if(s == "{" || s == "}")
            unibracket_check(s[0], params->start);
    }
    else if(element == "varblock" || element == "typeblock")
    {
        if(formatter_params.braces_opened == 0)
            formatter_params.section.top() = element == "varblock" ? S_VAR : S_TYPE;
    }
    else if(element == "keyword_declaring_func")
    {
        if(formatter_params.depth == 0)
            formatter_params.create_title();
        formatter_params.section.top() = S_CODE;
    }
    else if(element == "of")
    {
        if(formatter_params.case_unmatched)
        {
            formatter_params.case_unmatched = false;
            formatter_params.section.push(S_CODE);
            formatter_params.open_blockbracket();
        }
    }
    else if(element == "keyword_declaring_varblock")
    {
        formatter_params.section.push(S_VAR);
        blockbracket_check(s, params->start);
    }
    else if(element == "classname" || element == "function")
    {
        NameType name_type = (element == "classname" ? NT_CLASS : NT_FUNCTION);
        auto name_in_list = formatter_params.list_of_names.find(s);
        //TODO: constructors in C++ are exceptions:
        //look at other langs and repair them
        if(name_in_list == formatter_params.list_of_names.end() || name_in_list->second != name_type)
        {
            //TODO: map is not multimap. Find out more about func&class namespaces
            formatter_params.list_of_names[s] = name_type;
            name_style_check(s, params->start, (
                name_type == NT_CLASS ? "class_naming_style" : "function_naming_style"));
        }
    }
    else if(element == "identifier")
    {
        if(formatter_params.section.top() == S_TYPE)
        {
            formatter_params.list_of_names[s] = NT_CLASS;
            name_style_check(s, params->start, "class_naming_style");
        }
        else if(formatter_params.list_of_names.find(s) == formatter_params.list_of_names.end())
        {
            name_style_check(s, params->start, "variable_naming_style");
        }
    }
    else if(element == "keyword_with_following_operation")
    {
        formatter_params.indented_operation_expected = true;
        if(settings.ext_bool_options["forbid_multiple_expressions_per_line"] == EB_TRUE)
            formatter_params.line_closed = true;

        if(s == "else" && formatter_params.if_had_blockbracket)
            else_check(s, params->start);
    }
    else if(element == "keyword_with_following_operation_after_braces")
    {
        formatter_params.indented_operation_expected_after_braces = true;
        keyword_and_brace_check(s, params->start);
    }
    else if(element == "case")
    {
        formatter_params.case_unmatched = true;
    }
    else if(element == "switch_labels")
    {
        //it means case of default; for possible alignment options
    }
    else if(element == "semicolon")
    {
        if(formatter_params.braces_opened == 0)
        {
            formatter_params.close_opened_statements();
            if(settings.ext_bool_options["forbid_multiple_expressions_per_line"] == EB_TRUE)
                formatter_params.line_closed = true;
        }
    }
    else if(element == "brace")
    {
        brace_check(s, params->start);
        unibracket_check(s[0], params->start);
    }
    else if(element == "bracket")
    {
        unibracket_check(s[0], params->start);
    }
}
