#include "pseudoformatter_params.h"

void Environment::open_blockbracket()
{
    if(if_depth.size() != 0 && expected_depth == if_depth.top().first)
        if_depth.top().second = true;

    expected_depth++;
    expected_extra_depth++;

    close_on_end.push(0);
    open_if_count_before_blockbracket.push(if_depth.size());
}

void Environment::close_blockbracket()
{
    expected_depth--;
    expected_extra_depth--;

    close_on_end.pop();
    while(
        open_if_count_before_blockbracket.size() != 0 &&
        if_depth.size() > open_if_count_before_blockbracket.top())
    {
        if_depth.pop();
    }
    open_if_count_before_blockbracket.pop();

    close_opened_statements();
}

void Environment::close_opened_statements()
{
    expected_depth -= close_on_end.top();
    close_on_end.top() = 0;
}
