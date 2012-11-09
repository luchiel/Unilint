#include "result_collector.h"

void ResultCollector::add(int line_, int index_, const std::string& error_)
{
    result_records.insert(std::make_pair(std::make_pair(line_, index_), error_));
}

void ResultCollector::output_to_stream(std::ofstream& results_stream_)
{
    for(auto i = result_records.begin(); i != result_records.end(); ++i)
        results_stream_ << (i->first.first + 1) << ':' << (i->first.second + 1) << ": " << i->second << "\n";
}
