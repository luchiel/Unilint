#include <cstdlib>
#include "pseudoformatter.h"
#include "exception.h"

void PseudoFormatter::format(const std::string& s, const srchilite::FormatterParams* params)
{
    if(params == NULL)
        throw UnilintException("Something strange happened");

    TokenType type = token_types[element];
    tokens.push_back(Token(s, line_index, params->start, type));
}
