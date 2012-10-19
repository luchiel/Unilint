#include <cstdlib>
#include "pseudoformatter.h"
#include "exception.h"

void PseudoFormatter::blockbracket_check(const std::string& s_, int start_)
{
    if(s_ == "{" || s_ == "begin")
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
    //TODO: not only place to change depth
    int maximal_nesting_depth = settings.int_options["maximal_nesting_depth"];
    if(
        maximal_nesting_depth != IGNORE_OPTION &&
        formatter_params.depth > maximal_nesting_depth)
    {
        results.add(0, "maximal nesting depth exceeded");
    }
    //TODO: is it from here?
    formatter_params.indented_operation_expected = false;
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

void PseudoFormatter::prefix_check(const std::string& s)
{
    if(settings.indentation_style == IS_IGNORE)
        return;

    formatter_params.indentation_end = s.size();
    //TODO: extra_indent_for_blocks = false touched here?
    if(formatter_params.depth == 0)
    {
        results.add(0, "indent too deep");
        return;
    }

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
        //TODO: try futher processing
    }

    //replace IS_CONSISTENT with first detected style
    if(settings.indentation_style == IS_CONSISTENT)
        settings.indentation_style = tabs ? IS_TABS : IS_SPACES;

    if(tabs)
    {
        if(settings.indentation_style == IS_SPACES)
            results.add(0, "tabs found, spaces expected");

        if(s.size() < formatter_params.depth)
            results.add(0, "indent too small");
        else if(s.size() > formatter_params.depth)
            results.add(0, "indent too deep");
    }

    if(spaces)
    {
        if(settings.indentation_style == IS_TABS)
            results.add(0, "spaces found, tabs expected");

        if(settings.indentation_policy == IP_IGNORE)
            return;

        //detect tab_size for IP_CONSISTENT
        if(settings.indentation_policy == IP_CONSISTENT)
        {
            if(s.size() % formatter_params.depth != 0)
            {
                results.add(0, "wrong indent");
                return;
            }
            settings.int_options["indentation_size"] =
                s.size() / formatter_params.depth;
            settings.indentation_policy = IP_BY_SIZE;
            return; //futher checking makes no sence here
        }

        //indentation_size cannot be IGNORE_OPTION at this moment
        int indentation_size = settings.int_options["indentation_size"];

        if(formatter_params.depth * indentation_size < s.size())
            results.add(0, "indent too deep");
        else if(formatter_params.depth * indentation_size > s.size())
            results.add(0, "indent too small");
    }
}

void PseudoFormatter::name_style_check(
    const std::string& s_, int start_, const std::string& type_)
{
    NamingStyle ns = settings.naming_styles[type_];
    bool style_error = false;
    //TODO: check style definition and std_functions are correct
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

void PseudoFormatter::format(
    const std::string& s, const srchilite::FormatterParams* params)
{
    if(params == NULL)
        throw UnilintException("Something strange happened");

    if(element != "normal")
    {
        if(
            formatter_params.indented_operation_expected &&
            settings.ext_bool_options["compulsory_block_braces"] == EB_TRUE &&
            element != "blockbracket")
        {
            results.add(params->start, "block brace expected: { or begin");
            //TODO: next line belongs to other place
            formatter_params.indented_operation_expected = false;
        }
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
        if(
            formatter_params.list_of_names.find(s) ==
            formatter_params.list_of_names.end())
        {
            name_style_check(s, params->start, "variable_naming_style");
        }
        //else name is classname/function, and comments about style were given
    }
    else if(element == "normal")
    {
        //TODO: check, check,, check! that it includes only empty chars now
        if(params->start == 0)
        {
            prefix_check(s);
        }
        else if(
            params->start + s.size() == formatter_params.current_line.size())
        {
            //suffix
            ExtendedBoolean trailing =
                settings.ext_bool_options["forbid_trailing_whitespaces"];
            if(trailing != EB_IGNORE)
                results.add(params->start, "trailing whitespaces");
        }
        else
        {
            //infix
            ExtendedBoolean vertical =
                settings.ext_bool_options["forbid_vertical_alignment"];
            if(vertical != EB_IGNORE && s.size() > 1)
                results.add(params->start, "more than 1 whitespace");
            if(s[0] != ' ')
                results.add(params->start, "unexpected whitespace character");
        }
    }
    else if(element == "keyword_with_following_operation")
    {
        formatter_params.indented_operation_expected = true;
    }
    else if(element == "keyword_with_following_operation_after_braces")
    {
        formatter_params.indented_operation_expected_after_braces = true;
        ExtendedBoolean& space(
            settings.ext_bool_options["space_between_keyword_and_brace"]);

        int keyword_end = params->start + s.size();
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
            results.add(keyword_end, "no space after " + s);
        }
        else if(
            space == EB_FALSE &&
            formatter_params.current_line[keyword_end] == ' ')
        {
            results.add(keyword_end, "space after " + s);
        }
    }
    else if(element == "switch_labels")
    {
        //it means case of default; for possible alignment options
    }
    else if(element == "semicolon")
    {
        formatter_params.semicolon_count++;
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
