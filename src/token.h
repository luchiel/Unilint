#ifndef TOKEN_H
#define TOKEN_H

enum TokenType
{
    TT_KEYWORD,

    TT_CBRACKET_OPEN,
    TT_CBRACKET_CLOSE,
    TT_PASBRACKET_OPEN,
    TT_PASBRACKET_CLOSE,

    TT_NUMBER,
    TT_PREPROC,
    TT_COMMENT,
    TT_STRING,
    TT_SPECIALCHAR,
    TT_SYMBOL,

    TT_TYPE,
    TT_USERTYPE,
    TT_CLASSNAME,
    TT_FUNCTION,
    TT_IDENTIFIER,

    TT_CASE,
    TT_OF,

    TT_TYPEBLOCK,
    TT_VARBLOCK,

    TT_KEYWORD_DECLARING_VARBLOCK,
    TT_KEYWORD_DECLARING_FUNC,

    TT_LABEL,
    TT_SWITCH_LABELS,
    TT_SEMICOLON,
    TT_BRACE,
    TT_BRACKET,
    TT_WHITESPACE,
    TT_KEYWORD_WITH_FOLLOWING_OPERATION,
    TT_KEYWORD_WITH_FOLLOWING_OPERATION_AFTER_BRACE,
};

class Token
{
public:
    std::string line;
    int line_index;
    int start;
    TokenType type;

    Token(std::string line_, int line_index_, int start_, TokenType type_):
        line(line_), line_index(line_index_), start(start_), type(type_)
    {}
};

#endif
