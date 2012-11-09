#ifndef PSEUDOFORMATTER_H
#define PSEUDOFORMATTER_H

#include <map>
#include <list>
#include <string>
#include <boost/shared_ptr.hpp>
#include <srchilite/formatter.h>
#include <srchilite/formatterparams.h>
#include "token.h"

class PseudoFormatter: public srchilite::Formatter
{
private:
    std::map<std::string, TokenType>& token_types;
    std::list<Token>& tokens;
    int& line_index;
    std::string element;

public:
    PseudoFormatter(
        std::map<std::string, TokenType>& token_types_,
        std::list<Token>& tokens_,
        int& line_index_,
        std::string element_ = "normal"
    ):
        token_types(token_types_),
        tokens(tokens_),
        line_index(line_index_),
        element(element_)
    {}
    virtual void format(const std::string& s, const srchilite::FormatterParams* params = 0);
};

typedef boost::shared_ptr<PseudoFormatter> PseudoFormatterPtr;

#endif
