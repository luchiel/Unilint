#include <cstdlib>
#include "pseudoformatter.h"
#include "exception.h"

void PseudoFormatter::cbracket_check(const std::string& s_, const int start_)
{
    if(s_ == "{")
        depth++;
    else
        depth--;
    //TODO: not only place to change depth
    int maximal_nesting_depth = settings.int_options["maximal_nesting_depth"];
    if(maximal_nesting_depth != IGNORE_OPTION && depth > maximal_nesting_depth)
        results.add(current_line_index, 0, "maximal nesting depth exceeded");
}

void PseudoFormatter::prefix_check(const std::string& s, const int start_)
{
    //TODO: extra_indent_for_blocks = false touched here?
    if(depth == 0)
    {
        results.add(current_line_index, 0, "indent too deep");
        return;
    }
    //TODO: forbid 0-1 and 1-0 pairing
    //TODO: forbid tabs && any policy
    if(
        settings.indentation_style == IS_IGNORE ||
        settings.indentation_policy == IP_IGNORE)
    {
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
            results.add(
                current_line_index, start_ + i,
                "strange whitespace character");
            return;
        }
    }
    if(tabs && spaces)
    {
        results.add(current_line_index, 0, "tabs and spaces mixed");
        return;
        //TODO: try futher processing
    }

    //replace IS_CONSISTENT with first detected style
    if(settings.indentation_style == IS_CONSISTENT)
        settings.indentation_style = tabs ? IS_TABS : IS_SPACES;

    if(tabs)
    {
        if(settings.indentation_style == IS_SPACES)
            results.add(
                current_line_index, 0, "tabs found, spaces expected");

        if(s.size() < depth)
            results.add(current_line_index, 0, "indent too small");
        else if(s.size() > depth)
            results.add(current_line_index, 0, "indent too deep");
    }

    if(spaces)
    {
        if(settings.indentation_style == IS_TABS)
            results.add(
                current_line_index, 0, "spaces found, tabs expected");

        //detect tab_size for IP_CONSISTENT
        if(settings.indentation_policy == IP_CONSISTENT)
        {
            if(s.size() % depth != 0)
            {
                results.add(current_line_index, 0, "wrong indent");
                return;
            }
            settings.int_options["indentation_size"] = s.size() / depth;
            settings.indentation_policy = IP_BY_SIZE;
            return; //futher checking makes no sence here
        }
        //indentation_size cannot be IGNORE_OPTION at this moment
        int indentation_size = settings.int_options["indentation_size"];

        if(depth * indentation_size < s.size())
            results.add(current_line_index, 0, "indent too deep");
        else if(depth * indentation_size > s.size())
            results.add(current_line_index, 0, "indent too small");
    }
}

void PseudoFormatter::name_style_check(
    const std::string& s_, const int start_, const std::string& type_)
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
        results.add(
            current_line_index, start_, "name " + s_ + " is not of style " +
            ns_string);
    }
}

void PseudoFormatter::format(
    const std::string& s, const srchilite::FormatterParams* params)
{
    if(params == NULL)
        throw UnilintException("Something strange happened");

    if(element == "cbracket")
        cbracket_check(s, params->start);
    else if(element == "classname")
        name_style_check(s, params->start, "class_naming_style");
    else if(element == "function")
        name_style_check(s, params->start, "function_naming_style");
    else if(element == "variable")
        name_style_check(s, params->start, "variable_naming_style");
    else if(element == "normal")
    {
        //TODO: check, check,, check! that it includes only empty chars now
        if(params->start == 0)
        {
            prefix_check(s, params->start);
        }
        else if(params->start + s.size() == current_line.size())
        {
            //suffix
            ExtendedBoolean trailing =
                settings.ext_bool_options["forbid_trailing_whitespaces"];
            if(trailing != EB_IGNORE)
                results.add(
                    current_line_index, params->start, "trailing whitespaces");
        }
        else
        {
            //infix
            //TODO: check for unexpected whitespace symbols like \v, \t
            ExtendedBoolean vertical =
                settings.ext_bool_options["forbid_vertical_alignment"];
            if(vertical != EB_IGNORE && s.size() > 1)
                results.add(
                    current_line_index, params->start, "more than 1 whitespace");
        }
    }
    std::cout << element << ": " << s;
    std::cout << ", start: " << params->start;
    std::cout << std::endl;
}
