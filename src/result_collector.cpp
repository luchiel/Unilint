#include "result_collector.h"

void ResultCollector::Record::output_to_stream(std::ofstream& results_stream_)
{
    results_stream_ << line << ':' << index + 1 << ": " << error << "\n";
}

void ResultCollector::add(int index_, const std::string& error_)
{
    add(current_line_index, index_, error_);
}

void ResultCollector::add(
    int line_, int index_, const std::string& error_)
{
    result_records.push_back(Record(line_, index_, error_));
}

void ResultCollector::output_to_stream(std::ofstream& results_stream_)
{
    for(auto i = result_records.begin(); i != result_records.end(); ++i)
        i->output_to_stream(results_stream_);
}