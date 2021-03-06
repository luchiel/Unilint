#include <cstdio>
#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include "exception.h"
#include "checker.h"

#define DEFAULT_SETTINGS_FILE "default_settings.ini"
#define DEFAULT_RESULTS_FILE "results.txt"

/*
    First argument is a filename
    OR
    --help

    -s="/path/mysettings.settings"
    -r="/path/results.txt"
    -l="cpp"

    tests will be from the outside :P
*/

int main(int argc, char* argv[])
{
    try
    {
        if(argc == 1)
            throw ProcessCommandLineException("No arguments found. Use --help");

        if(!strcmp(argv[1], "--help"))
        {
            std::cout <<
                "usage: unilint <filename> "
                "[-s=<path_to_settings_file>] "
                "[-r=<path_to_results_file>] "
                "[-l=<language>]\n\n"
                "If -s and -r are not specified, "
                "default_settings.ini and results.txt are used.\n"
                "If -l is not specified, unilint tries to "
                "determine language by file extension.\n";
        }
        else
        {
            std::string file_to_process(argv[1]);
            std::string settings_file(DEFAULT_SETTINGS_FILE);
            std::string results_file(DEFAULT_RESULTS_FILE);
            std::string language("");
            for(int i = 2; argc > 2; --argc, ++i)
            {
                std::string argument(argv[i]);
                if(argument[0] != '-' || argument.size() <= 1)
                    throw ProcessCommandLineException("Unrecognized argument: " + argument);

                if(argument.size() <= 3 || argument[2] != '=')
                    throw ProcessCommandLineException("=<value> expected after " + argument.substr(0, 2));

                switch(argument[1])
                {
                    case 's':
                        settings_file = argument.substr(3);
                        break;
                    case 'r':
                        results_file = argument.substr(3);
                        break;
                    case 'l':
                        language = argument.substr(3);
                        break;
                    default:
                        throw ProcessCommandLineException("Unrecognized argument: " + argument.substr(0, 2));
                }
            }
            Checker checker(file_to_process, language, settings_file);
            checker.process_file();
            checker.output_results_to_file(results_file);
        }
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

	return 0;
}
