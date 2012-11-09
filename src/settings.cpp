#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include "settings.h"
#include "exception.h"

void Settings::throw_invalid_value_exception(
    const std::string& section_,
    const std::string& option_,
    const std::string& value_
)
{
    throw LoadSettingsException(
        "Invalid value '" + value_ + "'' of " + section_ + "." + option_);
}

void Settings::read_int_option(const std::string& section_, const std::string& option_)
{
    int value(settings.get<int>(section_ + '.' + option_, IGNORE_OPTION));
    if(value < 0 && value != IGNORE_OPTION)
        throw_invalid_value_exception(
            section_,
            option_,
            settings.get<std::string>(section_ + '.' + option_, ""));
    int_options[option_] = value;
}

void Settings::read_eb_options(const std::string& section_, const std::string& option_)
{
    std::string value(settings.get<std::string>(section_ + "." + option_, ""));

    if(value == "")
        ext_bool_options[option_] = EB_IGNORE;
    else if(value == "consistent")
        ext_bool_options[option_] = EB_CONSISTENT;
    else if(value == "false")
        ext_bool_options[option_] = EB_FALSE;
    else if(value == "true")
        ext_bool_options[option_] = EB_TRUE;
    else
        throw_invalid_value_exception(value, section_, option_);
}

void Settings::read_sb_options(const std::string& section_, const std::string& option_)
{
    //simple boolean is stored as extended, but can be either true or ignored
    std::string value(settings.get<std::string>(section_ + "." + option_, ""));

    if(value == "" || value == "false")
        ext_bool_options[option_] = EB_IGNORE;
    else if(value == "true")
        ext_bool_options[option_] = EB_TRUE;
    else
        throw_invalid_value_exception(value, section_, option_);
}


void Settings::read_ns_options(const std::string& section_, const std::string& option_)
{
    std::string value(settings.get<std::string>(section_ + "." + option_, ""));

    if(value == "")
        naming_styles[option_] = NS_IGNORE;
    else if(value == "consistent")
        naming_styles[option_] = NS_CONSISTENT;
    else if(value == "camel")
        naming_styles[option_] = NS_CAMEL;
    else if(value == "pascal")
        naming_styles[option_] = NS_PASCAL;
    else if(value == "underscore")
        naming_styles[option_] = NS_UNDERSCORE;
    else if(value == "caps_underscore")
        naming_styles[option_] = NS_CAPS_UNDERSCORE;
    else
        throw_invalid_value_exception(value, section_, option_);
}

Settings::Settings(const std::string& settings_file_name_)
{
    boost::property_tree::ini_parser::read_ini(settings_file_name_, settings);

    std::string value(
        settings.get<std::string>("indentation.indentation_style", ""));
    if(value == "")
        indentation_style = IS_IGNORE;
    else if(value == "consistent")
        indentation_style = IS_CONSISTENT;
    else if(value == "tabs")
        indentation_style = IS_TABS;
    else if(value == "spaces")
        indentation_style = IS_SPACES;
    else
        throw_invalid_value_exception(value, "indentation", "indentation_style");

    value = settings.get<std::string>("indentation.indentation_policy", "");
    if(value == "")
        indentation_policy = IP_IGNORE;
    else if(value == "consistent")
        indentation_policy = IP_CONSISTENT;
    else if(value == "by_size")
        indentation_policy = IP_BY_SIZE;
    else
        throw_invalid_value_exception(value, "indentation", "indentation_policy");

    read_int_option("indentation", "indentation_size");
    if(indentation_policy == IP_BY_SIZE && int_options["indentation_size"] == IGNORE_OPTION)
        throw LoadSettingsException(
            "indentation_size option must be set when indentation_policy = by_size");

    if(indentation_style == IS_IGNORE && indentation_policy != IP_IGNORE)
        throw LoadSettingsException(
            "indentation_policy cannot be checked when indentation_style is ignored");

    if(indentation_style == IS_TABS)
    {
        indentation_policy = IP_BY_SIZE;
        int_options["indentation_size"] = 1;
    }

    read_eb_options("indentation", "extra_indent_for_blocks");

    std::string section("limits");
    read_int_option(section, "maximal_line_length");
    read_int_option(section, "maximal_nesting_depth");
    read_int_option(section, "maximal_number_of_separating_newlines_between_blocks");

    section = "spaces_and_newlines";
    read_eb_options(section, "spaces_inside_braces");
    read_eb_options(section, "spaces_inside_curly_braces");
    read_eb_options(section, "spaces_inside_brackets");
    read_eb_options(section, "space_between_keyword_and_brace");
    read_eb_options(section, "start_block_at_newline");
    read_eb_options(section, "else_at_newline");
    read_eb_options(section, "newline_at_eof");
    if(ext_bool_options["newline_at_eof"] == EB_CONSISTENT)
        throw_invalid_value_exception(section, "newline_at_eof", "consistent");

    section = "other";
    read_sb_options(section, "compulsory_block_braces");
    read_sb_options(section, "forbid_vertical_alignment");
    read_sb_options(section, "forbid_multiple_expressions_per_line");
    read_sb_options(section, "forbid_trailing_whitespaces");
    read_sb_options(section, "check_encoding");
    read_sb_options(section, "check_newline_consistency");

    section = "naming_styles";
    read_ns_options(section, "class_naming_style");
    read_ns_options(section, "function_naming_style");
    read_ns_options(section, "variable_naming_style");
}
