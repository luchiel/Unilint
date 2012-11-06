#ifndef PSEUDOFORMATTER_H
#define PSEUDOFORMATTER_H

#include <fstream>
#include <map>
#include <string>
#include <boost/shared_ptr.hpp>
#include <srchilite/formatter.h>
#include <srchilite/formatterparams.h>
#include "settings.h"
#include "result_collector.h"
#include "pseudoformatter_params.h"

class PseudoFormatter: public srchilite::Formatter
{
private:
    ResultCollector& results;
    Settings& settings;
    PseudoFormatterParams& formatter_params;

    std::string element;

    void prefix_preprocessing(const std::string& s);
    void brace_check(const std::string& s, int start);
    void unibracket_check(char c, int start);
    void spaces_in_unibrackets_check(char c, int start, int offset);
    void blockbracket_check(const std::string& s, int start);
    void name_style_check(const std::string& s, int start, const std::string& type);
    void indent_error_check(int expected_depth, int scale, int start);
    void whitespace_sequence_check(const std::string& s, int start);
    void token_check(const std::string& s, int start);
    void nesting_depth_check(int start);
    void keyword_and_brace_check(const std::string& s, int start);
    void else_check(const std::string s, int start);
    bool is_opening_blockbracket(const std::string& s);
    bool keyword_equal(const std::string& s, const std::string& to);

public:
    //ToDo: inheritance tree, so no ifs in format()
    PseudoFormatter(
        ResultCollector& results_,
        Settings& settings_,
        PseudoFormatterParams& formatter_params_,
        std::string element_ = "normal"
    ):
        results(results_),
        settings(settings_),
        formatter_params(formatter_params_),
        element(element_)
    {}

    virtual void format(const std::string& s, const srchilite::FormatterParams* params = 0);
};

typedef boost::shared_ptr<PseudoFormatter> PseudoFormatterPtr;

#endif
