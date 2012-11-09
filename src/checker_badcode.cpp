#include <fstream>
#include <string>
#include <map>
#include <algorithm>
#include "checker.h"
#include "exception.h"
#include "pseudoformatter.h"

void Checker::check_if_declaration_block_is_over()
{
    if(
        env.section.top() != S_CODE && (
            env.declaration_depth.size() == 0 ||
            env.declaration_depth.top() < lines[token->line_index].expected_depth))
    {
        switch(token->type)
        {
            case TT_KEYWORD_DECLARING_VARBLOCK:
            case TT_KEYWORD_DECLARING_FUNC:
            case TT_VARBLOCK:
            case TT_TYPEBLOCK:
            case TT_PASBRACKET_OPEN:
            case TT_KEYWORD:
            case TT_LABEL:
            case TT_KEYWORD_WITH_FOLLOWING_OPERATION:
            case TT_KEYWORD_WITH_FOLLOWING_OPERATION_AFTER_BRACE:
                env.section.top() = S_CODE;
                break;
        }
    }
};

bool Checker::try_bind_to_title()
{
    if(file_language.first != L_PASCAL)
        return true;

    if(env.section.top() == S_CODE)
    {
        switch(token->type)
        {
            case TT_KEYWORD_DECLARING_VARBLOCK:
            case TT_KEYWORD_DECLARING_FUNC:
            case TT_VARBLOCK:
            case TT_TYPEBLOCK:
            case TT_PASBRACKET_OPEN:
                // if(env.title_opened_at_depth.size() != 0)
                // {
                //     if(env.title_opened_at_depth.top() == -1)
                //         env.title_opened_at_depth.top() = lines[token->line_index].expected_depth;
                //     return env.title_opened_at_depth.top() == lines[token->line_index].expected_depth;
                // }
                break;
        }
    }
    return false;
}

void Checker::try_add_indent()
{
    if(!next_is_blockbracket(true))
    {
        env.expected_depth++;
        env.close_on_end.top()++;
    }
    env.indented_operation_expected = false;
}

void Checker::calculate_expected_depth()
{
    env.expected_depth = 0;
    env.expected_extra_depth = 0;
    token = tokens.begin();
    while(token != tokens.end())
    {
        int line_index = token->line_index;

        if(env.indented_operation_expected)
            try_add_indent();

        lines[line_index].expected_depth = env.expected_depth;
        lines[line_index].expected_extra_depth = env.expected_extra_depth;

        for(; token->line_index == line_index && token != tokens.end(); ++token)
        {
            if(env.indented_operation_expected)
                try_add_indent();

            check_if_declaration_block_is_over();
            //bool bound = try_bind_to_title();

            switch(token->type)
            {
                case TT_WHITESPACE:
                    if(token->start == 0 && token->line.size() != lines[line_index].size)
                    {
                        lines[line_index].depth_by_fact = token->line.size();
                        prefix_preprocessing();
                    }
                    break;

                case TT_VARBLOCK:
                case TT_TYPEBLOCK:
                    if(env.braces_opened == 0)
                        env.section.top() = token->type == TT_VARBLOCK ? S_VAR : S_TYPE;

                    //for next
                    env.expected_depth++;
                    env.expected_extra_depth++;
                    //for current
                    if(token->start == lines[line_index].depth_by_fact)
                        lines[line_index].expected_extra_depth++;
                    break;

                case TT_IDENTIFIER:
                    if(env.section.top() == S_TYPE)
                        env.list_of_names[token->line] = NT_CLASS;

                    if(env.section.top() != S_CODE) //pascal declaration block
                    {
                        int e_d = lines[line_index].expected_depth;
                        if(env.declaration_depth.size() == 0  || env.declaration_depth.top() != e_d)
                        {
                            env.declaration_depth.push(e_d);
                        }
                    }
                    break;

                case TT_SEMICOLON:
                    if(env.section.top() != S_CODE)
                    {
                        int e_d = lines[line_index].expected_depth;
                        if(env.declaration_depth.size() != 0 && env.declaration_depth.top() == e_d)
                        {
                            env.declaration_depth.pop();
                        }
                    }
                    if(env.braces_opened == 0)
                        env.close_opened_statements();
                    break;

                case TT_CBRACKET_OPEN:
                case TT_PASBRACKET_OPEN:
                case TT_KEYWORD_DECLARING_VARBLOCK:
                    env.section.push(token->type == TT_KEYWORD_DECLARING_VARBLOCK ? S_VAR : S_CODE);

                    //for current
                    if(token->start == lines[line_index].depth_by_fact)
                        lines[line_index].expected_extra_depth++;

                    env.open_blockbracket();
                    break;

                case TT_CBRACKET_CLOSE:
                case TT_PASBRACKET_CLOSE:
                    env.section.pop();
                    //for current
                    if(token->start == lines[line_index].depth_by_fact)
                        lines[line_index].expected_depth--;

                    env.close_blockbracket();
                    break;

                case TT_CASE:
                    env.case_unmatched = true;
                    break;

                case TT_OF:
                    if(env.case_unmatched)
                    {
                        env.case_unmatched = false;
                        env.section.push(S_CODE);
                        env.open_blockbracket();
                    }
                    break;

                case TT_FUNCTION:
                    break;

                case TT_KEYWORD_DECLARING_FUNC:
                    //env.create_title();
                    env.section.top() = S_CODE;
                    break;

                case TT_LABEL:
                case TT_SWITCH_LABELS:
                    break;

                case TT_BRACE:
                    if(token->line[0] == '(')
                    {
                        env.expected_depth++;
                        env.braces_opened++;
                    }
                    else
                    {
                        env.expected_depth--;
                        env.braces_opened--;
                    }
                    if(env.braces_opened == 0 && env.indented_operation_expected_after_braces)
                    {
                        env.indented_operation_expected_after_braces = false;
                        env.indented_operation_expected = true;
                    }
                    break;

                case TT_KEYWORD_WITH_FOLLOWING_OPERATION:
                    env.indented_operation_expected = true;
                    if(keyword_equal("else"))
                    {
                        env.close_opened_statements();

                        env.close_on_end.top() =
                            env.if_depth.top().first - lines[token->line_index].expected_depth;
                        lines[token->line_index].expected_depth = env.if_depth.top().first;
                        env.if_depth.pop();

                        env.expected_depth = lines[token->line_index].expected_depth;
                    }
                    break;

                case TT_KEYWORD_WITH_FOLLOWING_OPERATION_AFTER_BRACE:
                    env.indented_operation_expected_after_braces = true;
                    break;
            }

            if(keyword_equal("if"))
            {
                env.if_depth.push(std::make_pair(lines[token->line_index].expected_depth, false));
            }
        }
    }
}

bool Checker::is_pascal_functional_begin()
{
    return
        token->type == TT_PASBRACKET_OPEN && (
            try_bind_to_title() ||
            lines[token->line_index].expected_depth == 0);
}
