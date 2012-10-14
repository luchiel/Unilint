#ifndef CHECKER_H
#define CHECKER_H

#include <fstream>
#include <string>
/*
#include "srchilite/langdefmanager.h"
#include "srchilite/regexrulefactory.h"
#include "srchilite/sourcehighlighter.h"
#include "srchilite/formattermanager.h"
#include "infoformatter.h"
*/
#include "settings.h"

#define LANG_PATH "langs"

enum Language
{
    L_BASIC,
    L_C,
    L_CPP,
    L_CSHARP,
    L_HASKELL,
    L_JAVA,
    L_PASCAL,
    L_PERL,
    L_PHP,
    L_PYTHON,
    L_RUBY,
};

class Checker
{
private:
    std::ifstream file_to_process;
    Language file_language;
    void common_check();
    void set_language(const std::string& language_);
public:
    Checker(
        const std::string& filename_,
        const std::string& language_,
        const Settings& settings_
    );
    ~Checker() { file_to_process.close(); }
};

#endif
