#include <cstdlib>
#include "pseudoformatter.h"
#include "exception.h"

bool PseudoFormatter::is_opening_blockbracket(const std::string& s_)
{
    return s_ == "{" || s_ == "begin";
}

void PseudoFormatter::indent_error_check(int expected_depth)
{
    if(formatter_params.depth_by_fact < expected_depth)
        results.add(0, "indent too small");
    else if(formatter_params.depth_by_fact > expected_depth)
        results.add(0, "indent too deep");
}

void PseudoFormatter::blockbracket_check(const std::string& s_, int start_)
{
    if(is_opening_blockbracket(s_))
    {
        formatter_params.depth++;
        ExtendedBoolean& block_at_newline(
            settings.ext_bool_options["start_block_at_newline"]);
        if(block_at_newline == EB_CONSISTENT)
        {
            block_at_newline = start_ == formatter_params.indentation_end ?
                EB_TRUE : EB_FALSE;
        }
        else if(
            block_at_newline == EB_TRUE &&
            start_ != formatter_params.indentation_end)
        {
            results.add(start_, s_ + " is not at new line");
        }
        else if(
            block_at_newline == EB_FALSE &&
            start_ == formatter_params.indentation_end)
        {
            results.add(start_, s_ + " is at new line");
        }
    }
    else
        formatter_params.depth--;

    nesting_depth_check(start_);
}

void PseudoFormatter::nesting_depth_check(int start_)
{
    int maximal_nesting_depth = settings.int_options["maximal_nesting_depth"];
    if(
        maximal_nesting_depth != IGNORE_OPTION &&
        formatter_params.depth > maximal_nesting_depth)
    {
        results.add(0, "maximal nesting depth exceeded");
    }
}

void PseudoFormatter::whitespace_sequence_check(
    const std::string& s_, int start_)
{
    if(start_ + s_.size() == formatter_params.current_line.size())
    {
        //suffix
        ExtendedBoolean trailing =
            settings.ext_bool_options["forbid_trailing_whitespaces"];
        if(trailing != EB_IGNORE)
            results.add(start_, "trailing whitespaces");
    }
    else if(start_ == 0)
    {
        prefix_preprocessing(s_);
    }
    else
    {
        //infix
        ExtendedBoolean vertical =
            settings.ext_bool_options["forbid_vertical_alignment"];
        if(vertical != EB_IGNORE && s_.size() > 1)
            results.add(start_, "more than one whitespace");
        if(s_[0] != ' ')
            results.add(start_, "unexpected non-space whitespace character");
    }
}

void PseudoFormatter::token_check(const std::string& s_, int start_)
{
    //check if compulsory block brackets are present
    if(
        formatter_params.indented_operation_expected &&
        settings.ext_bool_options["compulsory_block_braces"] == EB_TRUE &&
        element != "blockbracket")
    {
        results.add(start_, "block brace expected: { or begin");
    }

    //check indentation
    if(
        start_ == formatter_params.indentation_end && (
            formatter_params.perform_indentation_size_check ||
            start_ == 0 && settings.indentation_style != IS_IGNORE))
    {
        int indentation_size = 1;
        if(
            settings.indentation_style == IS_SPACES &&
            settings.indentation_policy == IP_BY_SIZE)
        {
            indentation_size = settings.int_options["indentation_size"];
        }

        if(element == "blockbracket")
        {
            ExtendedBoolean& ext_extra_indent(
                settings.ext_bool_options["extra_indent_for_blocks"]);

            if(ext_extra_indent == EB_CONSISTENT)
            {
                //TODO: pascal's main begin-end are exceptions
                ext_extra_indent = formatter_params.depth == 0 && start_ != 0 ?
                    EB_TRUE : EB_FALSE;
            }

            if(is_opening_blockbracket(s_))
            {
                if(ext_extra_indent == EB_TRUE)
                    formatter_params.depth++;
                indent_error_check(formatter_params.depth * indentation_size);
            }
            else
            {
                indent_error_check(
                    (formatter_params.depth - 1) * indentation_size);
                if(ext_extra_indent == EB_TRUE)
                    formatter_params.depth--;
            }
        }
        else if(formatter_params.indented_operation_expected)
        {
            //TODO: complex subexpression like in nested test
            //depth is ++'d 'till the end of operation
            indent_error_check((formatter_params.depth + 1) * indentation_size);
        }
        else
        {
            indent_error_check(formatter_params.depth * indentation_size);
        }
    }

    //check if expected brace is found
    if(
        formatter_params.indented_operation_expected_after_braces &&
        formatter_params.braces_opened == 0 &&
        element != "brace")
    {
        results.add(start_, "brace expected");
        formatter_params.indented_operation_expected_after_braces = false;
    }

    //indent was checked
    formatter_params.indented_operation_expected = false;;
}

void PseudoFormatter::spaces_in_unibrackets_check(
    char c_, int start_, int offset_)
{
    ExtendedBoolean* opt_ptr = NULL;
    char opposite;

    switch(c_)
    {
        case '(':
        case ')':
            opt_ptr = &settings.ext_bool_options["spaces_inside_braces"];
            opposite = c_ == '(' ? ')' : '(';
            break;
        case '[':
        case ']':
            opt_ptr = &settings.ext_bool_options["spaces_inside_brackets"];
            opposite = c_ == '[' ? ']' : '[';
            break;
        case '{':
        case '}':
            opt_ptr = &settings.ext_bool_options["spaces_inside_curly_braces"];
            opposite = c_ == '{' ? '}' : '{';
            break;
        default:
            throw UnilintException("Not a bracket symbol");
    }

    ExtendedBoolean& spaces_inside(*opt_ptr);

    //empty brackets are exception
    if(formatter_params.current_line[start_ + offset_] == opposite)
        return;

    if(spaces_inside == EB_CONSISTENT)
    {
        spaces_inside = formatter_params.current_line[start_ + offset_] == ' ' ?
            EB_TRUE : EB_FALSE;
    }
    else if(
        spaces_inside == EB_TRUE &&
        formatter_params.current_line[start_ + offset_] != ' ')
    {
        results.add(
            start_ + offset_,
            std::string("no space ") +
            (offset_ > 0 ? "after " : "before ") + c_);
    }
    else if(
        spaces_inside == EB_FALSE &&
        formatter_params.current_line[start_ + offset_] == ' ')
    {
        results.add(
            start_ + offset_,
            std::string("unexpected space ") +
            (offset_ > 0 ? "after " : "before ") + c_);
    }
}

void PseudoFormatter::unibracket_check(char c_, int start_)
{
    if(c_ == '(' || c_ == '[' || c_ == '{')
    {
        if(start_ != formatter_params.current_line.size() - 1)
            spaces_in_unibrackets_check(c_, start_, 1);
    }
    else
    {
        if(start_ != 0 && start_ != formatter_params.indentation_end)
            spaces_in_unibrackets_check(c_, start_, -1);
    }
}

void PseudoFormatter::brace_check(const std::string& s_, int start_)
{
    if(s_ == "(")
    {
        formatter_params.braces_opened++;
    }
    else
    {
        formatter_params.braces_opened--;
        if(formatter_params.braces_opened < 0)
        {
            results.add(start_, "unexpected " + s_);
        }
        else if(
            formatter_params.braces_opened == 0 &&
            formatter_params.indented_operation_expected_after_braces)
        {
            formatter_params.indented_operation_expected_after_braces = false;
            formatter_params.indented_operation_expected = true;
        }
    }
}

void PseudoFormatter::prefix_preprocessing(const std::string& s)
{
    if(settings.indentation_style == IS_IGNORE)
        return;

    formatter_params.indentation_end = s.size();

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

    if(tabs)
    {
        if(settings.indentation_style == IS_SPACES)
        {
            results.add(0, "tabs found, spaces expected");
            //indentation_size can be undefined
            //so tabs cannot be translated to spaces
            return;
        }
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
            //TODO: does first indent found always have depth one? NO
            settings.int_options["indentation_size"] = s.size();
            settings.indentation_policy = IP_BY_SIZE;
        }
    }

    formatter_params.depth_by_fact = s.size();
    formatter_params.perform_indentation_size_check = true;
}

void PseudoFormatter::name_style_check(
    const std::string& s_, int start_, const std::string& type_)
{
    NamingStyle ns = settings.naming_styles[type_];
    bool style_error = false;
    if(ns == NS_CAMEL || ns == NS_PASCAL)
    {
        style_error =
            ns == NS_CAMEL && !std::islower(s_[0]) ||
            ns == NS_PASCAL && !std::isupper(s_[0]);

        for(int i = 1; !style_error && i < s_.size(); ++i)
        {
            style_error = !std::isalpha(s_[i]) && !std::isdigit(s_[i]);
        }
    }
    if(ns == NS_UNDERSCORE || ns == NS_CAPS_UNDERSCORE)
    {
        for(int i = 0; !style_error && i < s_.size(); ++i)
        {
            style_error =
                s_[i] != '_' &&
                !std::isdigit(s_[i]) && (
                    ns == NS_UNDERSCORE && !islower(s_[i]) ||
                    ns == NS_CAPS_UNDERSCORE && !isupper(s_[i]));
        }
    }
    if(style_error)
    {
        std::string ns_string(
            ns == NS_CAMEL ? "camel" :
            ns == NS_PASCAL ? "pascal" :
            ns == NS_UNDERSCORE ? "underscore" :
                "caps_underscore");
        results.add(start_, "name " + s_ + " is not of style " + ns_string);
    }
}

void PseudoFormatter::keyword_and_brace_check(const std::string& s_, int start_)
{
    ExtendedBoolean& space(
        settings.ext_bool_options["space_between_keyword_and_brace"]);

    int keyword_end = start_ + s_.size();
    if(keyword_end == formatter_params.current_line.size())
        return;

    if(space == EB_CONSISTENT)
    {
        space = formatter_params.current_line[keyword_end] == ' ' ?
            EB_TRUE : EB_FALSE;
    }
    else if(
        space == EB_TRUE &&
        formatter_params.current_line[keyword_end] != ' ')
    {
        results.add(keyword_end, "no space after " + s_);
    }
    else if(
        space == EB_FALSE &&
        formatter_params.current_line[keyword_end] == ' ')
    {
        results.add(keyword_end, "space after " + s_);
    }
}

void PseudoFormatter::else_check(const std::string s_, int start_)
{
    ExtendedBoolean& at_newline(settings.ext_bool_options["else_at_newline"]);
    if(at_newline == EB_CONSISTENT)
    {
        at_newline = start_ == formatter_params.indentation_end ?
            EB_TRUE : EB_FALSE;
    }
    else if(at_newline == EB_TRUE && start_ != formatter_params.indentation_end)
    {
        results.add(start_, "else is not at new line");
    }
    else if(
        at_newline == EB_FALSE && start_ == formatter_params.indentation_end)
    {
        //TODO: previous was not brace => oops
        results.add(start_, "else is at new line");
    }
}

void PseudoFormatter::format(
    const std::string& s, const srchilite::FormatterParams* params)
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
        blockbracket_check(s, params->start);
        if(s == "{" || s == "}")
            unibracket_check(s[0], params->start);
    }
    else if(element == "classname" || element == "function")
    {
        NameType name_type = (element == "classname" ? NT_CLASS : NT_FUNCTION);
        auto name_in_list = formatter_params.list_of_names.find(s);
        //TODO: constructors in C++ are exceptions:
        //look at other langs and repair them
        if(
            name_in_list == formatter_params.list_of_names.end() ||
            name_in_list->second != name_type)
        {
            formatter_params.list_of_names[s] = name_type;
            name_style_check(
                s, params->start, (name_type == NT_CLASS ?
                "class_naming_style" : "function_naming_style"));
        }
    }
    else if(element == "identifier")
    {
        //check if identifier is not known classname or function
        if(
            formatter_params.list_of_names.find(s) ==
            formatter_params.list_of_names.end())
        {
            name_style_check(s, params->start, "variable_naming_style");
        }
    }
    else if(element == "keyword_with_following_operation")
    {
        formatter_params.indented_operation_expected = true;
        if(s == "else")
            else_check(s, params->start);
    }
    else if(element == "keyword_with_following_operation_after_braces")
    {
        formatter_params.indented_operation_expected_after_braces = true;
        keyword_and_brace_check(s, params->start);
    }
    else if(element == "switch_labels")
    {
        //it means case of default; for possible alignment options
    }
    else if(element == "semicolon")
    {
        //formatter_params.semicolon_count++;
        //lang-dependent
        //trig operation to false
        //TODO: for(;;)
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
