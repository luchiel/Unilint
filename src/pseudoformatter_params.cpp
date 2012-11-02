#include "pseudoformatter_params.h"

void PseudoFormatterParams::open_blockbracket()
{
    if(if_depth.size() != 0 && depth == if_depth.top().first)
        if_depth.top().second = true;

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

    close_opened_statements();
}

void PseudoFormatterParams::save_if_depth()
{
    if_depth.push(std::make_pair(depth, false));
}

void PseudoFormatterParams::restore_last_if_depth()
{
    close_on_end.top() = if_depth.top().first - depth;
    depth = if_depth.top().first;
    if_had_blockbracket = if_depth.top().second;
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

void PseudoFormatterParams::init_new_line()
{
    line_closed = false;
    depth_by_fact = 0;
    indentation_end = 0;
    operation_per_line_count = 0;
    perform_indentation_size_check = false;
}

void PseudoFormatterParams::create_title()
{
    title_opened.push(false);
}

bool PseudoFormatterParams::try_bind_to_title()
{
    if(language != L_PASCAL)
        return true;

    if(title_opened.size() != 0)
    {
        title_opened.top() = true;
        return true;
    }
    return false;
}

void PseudoFormatterParams::close_title()
{
    title_opened.pop();
}
