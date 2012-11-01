#include "pseudoformatter_params.h"

void PseudoFormatterParams::open_blockbracket()
{
    depth++;

    close_on_end.push(0);
    open_if_count_before_blockbracket.push(if_depth.size());
}

void PseudoFormatterParams::close_blockbracket()
{
    depth--;

    close_on_end.pop();
    while(if_depth.size() > open_if_count_before_blockbracket.top())
        if_depth.pop();
    open_if_count_before_blockbracket.pop();

    depth -= close_on_end.top();
    close_on_end.top() = 0;
}

void PseudoFormatterParams::restore_last_if_depth()
{
    close_on_end.top() = if_depth.top() - depth;
    depth = if_depth.top();
    if_depth.pop();
}

void PseudoFormatterParams::open_statement()
{
    depth++;
    close_on_end.top()++;
}

void PseudoFormatterParams::close_opened_statements()
{
    depth -= close_on_end.top();
    close_on_end.top() = 0;
}
