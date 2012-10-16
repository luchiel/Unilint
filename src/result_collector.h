#ifndef RESULT_COLLECTOR_H
#define RESULT_COLLECTOR_H

#include <fstream>
#include <vector>
#include <string>

class ResultCollector
{
private:
    class Record
    {
    private:
        int line;
        int index;
        std::string error;
    public:
        Record(int line_, int index_, const std::string& error_)
            : line(line_), index(index_), error(error_) {}
        void output_to_stream(std::ofstream& results_stream_);
    };
    std::vector<Record> result_records;

public:
    ResultCollector() {}
    void add(int line_, int index_, const std::string& error_);
    void output_to_stream(std::ofstream& results_stream_);
};

#endif
