#include <string>
#include <iostream>

class UnilintException: public std::exception
{
private:
    std::string error;
public:
    UnilintException(const std::string& error_) throw():
        exception(), error(error_) {}
    ~UnilintException() throw() {}

    virtual const char* what() const throw() { return error.c_str(); }
};

class ProcessCommandLineException: public UnilintException
{   
public:
    ProcessCommandLineException(const std::string& error_) throw():
        UnilintException(error_) {}
    ~ProcessCommandLineException() throw() {}
};

class LoadSettingsException: public UnilintException
{
public:
    LoadSettingsException(const std::string& error_) throw():
        UnilintException(error_) {}
    ~LoadSettingsException() throw() {}
};
