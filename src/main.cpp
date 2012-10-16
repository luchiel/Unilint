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
            throw ProcessCommandLineException(
                "No arguments found. Use --help");

        if(!strcmp(argv[1], "--help"))
        {
            std::cout << "" << std::endl; //TODO: proper help
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
                    throw ProcessCommandLineException(
                        "Unrecognized argument: " + argument);

                switch(argument[1])
                {
                    //TODO: check if name in range!
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
                        throw ProcessCommandLineException(
                            "Unrecognized argument: " + argument);
                }
            }
            Checker checker(
                file_to_process, language, settings_file);
            checker.process_file();
            checker.output_results_to_file(results_file);
        }
        std::cout << "Job's done." << std::endl;
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

	return 0;
}