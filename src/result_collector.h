#ifndef RESULT_COLLECTOR_H
#define RESULT_COLLECTOR_H

#include <fstream>
#include <map>
#include <string>

class ResultCollector
{
private:
    std::multimap<std::pair<int, int>, std::string> result_records;

public:
    ResultCollector() {}
    void add(int line_, int index_, const std::string& error_);
    void output_to_stream(std::ofstream& results_stream_);
};

#endif
