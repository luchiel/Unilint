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

    void prefix_check(const std::string& s);
    void brace_check(const std::string& s_, int start_);
    void spaces_in_braces_check(int start_, int offset_);
    void blockbracket_check(const std::string& s_, int start_);
    void name_style_check(
        const std::string& s_, int start_, const std::string& type_);

public:
    //ToDo: inheritance tree, so no ifs in format()
    PseudoFormatter(
        ResultCollector& results_,
        Settings& settings_,
        PseudoFormatterParams& formatter_params_,
        std::string element_ = "normal"
    ):
        results(results_), settings(settings_),
        formatter_params(formatter_params_), element(element_) {}

    virtual void format(
        const std::string& s, const srchilite::FormatterParams* params = 0);
};

typedef boost::shared_ptr<PseudoFormatter> PseudoFormatterPtr;

#endif