#ifndef PSEUDOFORMATTER_H
#define PSEUDOFORMATTER_H

#include <fstream>
#include <string>
#include <boost/shared_ptr.hpp>
#include <srchilite/formatter.h>
#include <srchilite/formatterparams.h>
#include "settings.h"
#include "result_collector.h"

class PseudoFormatter: public srchilite::Formatter
{
private:
    ResultCollector& results;
    Settings& settings;
    int& depth;
    int& current_line_index;
    const std::string& current_line;
    std::string element;

    void prefix_check(const std::string& s, const int start_);
    void cbracket_check(const std::string& s_, const int start_);
    void name_style_check(
        const std::string& s_, const int start_, const std::string& type_);

public:
    PseudoFormatter(
        ResultCollector& results_,
        Settings& settings_,
        int& depth_,
        int& current_line_index_,
        const std::string& current_line_,
        std::string element_ = "normal"
    ):
        results(results_), settings(settings_), depth(depth_),
        current_line_index(current_line_index_), current_line(current_line_),
        element(element_) {}

    virtual void format(
        const std::string& s, const srchilite::FormatterParams* params = 0);
};

typedef boost::shared_ptr<PseudoFormatter> PseudoFormatterPtr;

#endif
