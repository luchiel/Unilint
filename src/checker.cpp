#include <fstream>
#include <string>
#include <map>
#include <algorithm>
#include "checker.h"
#include "exception.h"

void Checker::set_language(const std::string& language_)
{
    std::map<std::string, Language> languages_aliases;
    languages_aliases["basic"] = L_BASIC;
    languages_aliases["bas"] = L_BASIC;
    languages_aliases["qb"] = L_BASIC;
    languages_aliases["c"] = L_C;
    languages_aliases["c++"] = L_CPP;
    languages_aliases["cpp"] = L_CPP;
    languages_aliases["cc"] = L_CPP;
    languages_aliases["c#"] = L_CSHARP;
    languages_aliases["cs"] = L_CSHARP;
    languages_aliases["haskell"] = L_HASKELL;
    languages_aliases["hs"] = L_HASKELL;
    languages_aliases["java"] = L_JAVA;
    languages_aliases["pascal"] = L_PASCAL;
    languages_aliases["lpr"] = L_PASCAL;
    languages_aliases["pp"] = L_PASCAL;
    languages_aliases["dpr"] = L_PASCAL;
    languages_aliases["perl"] = L_PERL;
    languages_aliases["pl"] = L_PERL;
    languages_aliases["php"] = L_PHP;
    languages_aliases["python"] = L_PYTHON;
    languages_aliases["py"] = L_PYTHON;
    languages_aliases["ruby"] = L_RUBY;
    languages_aliases["rb"] = L_RUBY;

    std::string lowercase(language_);
    std::transform(
        language_.begin(), language_.end(), lowercase.begin(), ::tolower);

    auto lang_iter = languages_aliases.find(lowercase);
    if(lang_iter == languages_aliases.end())
        throw UnilintException("Unknown language/extension: " + lowercase);

    file_language = lang_iter->second;
}

void Checker::common_check()
{

}

Checker::Checker(
    const std::string& filename_,
    const std::string& language_,
    const Settings& settings_
)
{
    file_to_process.open(filename_.c_str());
    //TODO: does auto detect for language exist in srchighlite?
    if(language_ == "")
    {
        unsigned int dot_position =
            filename_.find_last_of(".", filename_.size() - 2);
        if(dot_position == std::string::npos)
            throw UnilintException(
                "Cannot determine source file language: no extension found. "
                "Use -l directive to specify a language");
        set_language(filename_.substr(dot_position + 1));
    }
    else
    {
        set_language(language_);
    }
}
