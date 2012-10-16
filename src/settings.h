#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <map>
#include <boost/property_tree/ptree.hpp>

enum ExtendedBoolean
{
    EB_IGNORE,
    EB_CONSISTENT,
    EB_FALSE,
    EB_TRUE,
};

enum IndentStyle
{
    IS_IGNORE,
    IS_CONSISTENT,
    IS_TABS,
    IS_SPACES,
};

enum IndentPolicy
{
    IP_IGNORE,
    IP_CONSISTENT,
    IP_BY_SIZE,
};

enum NamingStyle
{
    NS_IGNORE,
    NS_CONSISTENT,
    NS_CAMEL,
    NS_PASCAL,
    NS_UNDERSCORE,
    NS_CAPS_UNDERSCORE,
};

const int IGNORE_OPTION = -1;

class Settings
{
private:
    boost::property_tree::ptree settings;

    void read_int_option(
        const std::string& section_, const std::string& option_);
    void read_eb_options(
        const std::string& section_, const std::string& option_);
    void read_sb_options(
        const std::string& section_, const std::string& option_);
    void read_ns_options(
        const std::string& section_, const std::string& option_);
    void throw_invalid_value_exception(
        const std::string& section_,
        const std::string& option_,
        const std::string& value_
    );
public:
    Settings(const std::string& settings_file_name_);

    std::map<std::string, NamingStyle> naming_styles;
    std::map<std::string, ExtendedBoolean> ext_bool_options;
    std::map<std::string, int> int_options;

    IndentStyle indentation_style;
    IndentPolicy indentation_policy;
};

#endif
