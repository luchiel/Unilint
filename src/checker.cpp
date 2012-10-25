#include <fstream>
#include <string>
#include <map>
#include <algorithm>
#include <srchilite/langdefmanager.h>
#include <srchilite/regexrulefactory.h>
#include <srchilite/sourcehighlighter.h>
#include <srchilite/formattermanager.h>
#include "checker.h"
#include "exception.h"
#include "pseudoformatter.h"

void Checker::set_language(const std::string& language_)
{
    std::map<std::string, std::pair<Language, std::string>> languages_aliases;
    std::pair<Language, std::string> temp;

    temp = std::make_pair(L_BASIC, "vb.lang");
    languages_aliases["basic"] = temp;
    languages_aliases["bas"] = temp;
    languages_aliases["qb"] = temp;

    languages_aliases["c"] = std::make_pair(L_C, "c.lang");

    temp = std::make_pair(L_CPP, "cpp.lang");
    languages_aliases["c++"] = temp;
    languages_aliases["cpp"] = temp;
    languages_aliases["cc"] = temp;

    temp = std::make_pair(L_CSHARP, "csharp.lang");
    languages_aliases["c#"] = temp;
    languages_aliases["cs"] = temp;

    temp = std::make_pair(L_HASKELL, "haskell.lang");
    languages_aliases["haskell"] = temp;
    languages_aliases["hs"] = temp;

    languages_aliases["java"] = temp = std::make_pair(L_JAVA, "java.lang");

    temp = std::make_pair(L_PASCAL, "pascal.lang");
    languages_aliases["pascal"] = temp;
    languages_aliases["pas"] = temp;
    languages_aliases["lpr"] = temp;
    languages_aliases["pp"] = temp;
    languages_aliases["dpr"] = temp;

    temp = std::make_pair(L_PERL, "perl.lang");
    languages_aliases["perl"] = temp;
    languages_aliases["pl"] = temp;

    languages_aliases["php"] = std::make_pair(L_PHP, "php.lang");

    temp = std::make_pair(L_PYTHON, "python.lang");
    languages_aliases["python"] = temp;
    languages_aliases["py"] = temp;

    temp = std::make_pair(L_RUBY, "ruby.lang");
    languages_aliases["ruby"] = temp;
    languages_aliases["rb"] = temp;

    std::string lowercase(language_);
    std::transform(language_.begin(), language_.end(), lowercase.begin(), ::tolower);

    auto lang_iter = languages_aliases.find(lowercase);
    if(lang_iter == languages_aliases.end())
        throw UnilintException("Unknown language/extension: " + lowercase);

    file_language = lang_iter->second;
}

Checker::Checker(
    const std::string& filename_,
    const std::string& language_,
    const std::string& settings_
):
    file_to_process(filename_.c_str()),
    settings(settings_),
    empty_lines_counter(0),
    formatter_params(),
    results(formatter_params.current_line_index)
{
    if(file_to_process.fail())
        throw UnilintException("File " + filename_ + " not found.");

    if(language_ == "")
    {
        unsigned int dot_position = filename_.find_last_of(".", filename_.size() - 2);
        if(dot_position == std::string::npos)
            throw UnilintException(
                "Cannot determine source file language: no extension found. "
                "Use -l directive to specify language");
        set_language(filename_.substr(dot_position + 1));
    }
    else
    {
        set_language(language_);
    }
}

Checker::~Checker()
{
    file_to_process.close();
}

PseudoFormatterPtr Checker::new_formatter(const std::string& s_)
{
    return PseudoFormatterPtr(new PseudoFormatter(
        results, settings, formatter_params, s_));
}

void Checker::process_file()
{
    srchilite::RegexRuleFactory rule_factory;
    srchilite::LangDefManager lang_def_manager(&rule_factory);
    srchilite::SourceHighlighter highlighter(
        lang_def_manager.getHighlightState(LANG_PATH, file_language.second));
    srchilite::FormatterManager manager(PseudoFormatterPtr(
        new PseudoFormatter(results, settings, formatter_params)));

    manager.addFormatter("cbracket", new_formatter("blockbracket"));
    manager.addFormatter("pasbracket", new_formatter("blockbracket"));

    manager.addFormatter("keyword", new_formatter("keyword"));
    manager.addFormatter("number", new_formatter("number"));
    manager.addFormatter("label", new_formatter("label"));
    manager.addFormatter("preproc", new_formatter("preproc"));
    manager.addFormatter("comment", new_formatter("comment"));
    manager.addFormatter("string", new_formatter("string"));
    manager.addFormatter("specialchar", new_formatter("string"));
    manager.addFormatter("symbol", new_formatter("symbol"));

    manager.addFormatter("type", new_formatter("type"));
    manager.addFormatter("usertype", new_formatter("classname"));
    manager.addFormatter("classname", new_formatter("classname"));
    manager.addFormatter("function", new_formatter("function"));
    manager.addFormatter("identifier", new_formatter("identifier"));

    manager.addFormatter(
        "keyword_with_following_operation", new_formatter("keyword_with_following_operation"));
    manager.addFormatter(
        "keyword_with_following_operation_after_braces",
        new_formatter("keyword_with_following_operation_after_braces"));
    manager.addFormatter("switch_labels", new_formatter("switch_labels"));
    manager.addFormatter("semicolon", new_formatter("semicolon"));
    manager.addFormatter("brace", new_formatter("brace"));
    manager.addFormatter("bracket", new_formatter("bracket"));

    highlighter.setFormatterManager(&manager);

    srchilite::FormatterParams params;
    highlighter.setFormatterParams(&params);

    for(
        formatter_params.current_line_index = 1;
        file_to_process.good();
        ++formatter_params.current_line_index
    )
    {
        getline(file_to_process, formatter_params.current_line);
        check_line_length();
        check_if_empty();
        if(formatter_params.current_line.size() != 0)
        {
            params.start = 0;
            formatter_params.depth_by_fact = 0;
            formatter_params.indentation_end = 0;
            formatter_params.operation_per_line_count = 0;
            formatter_params.perform_indentation_size_check = false;
            highlighter.highlightParagraph(formatter_params.current_line);
        };
    }
    check_newline_at_eof();
}

void Checker::check_line_length()
{
    int max_length = settings.int_options["maximal_line_length"];
    if(max_length != IGNORE_OPTION && max_length <= formatter_params.current_line.size())
    {
        results.add(0, "line too long");
    }
}

void Checker::check_if_empty()
{
    if(formatter_params.current_line.size() == 0)
    {
        empty_lines_counter++;
        int max_number = settings.int_options["maximal_number_of_separating_newlines_between_blocks"];
        if(max_number != IGNORE_OPTION && empty_lines_counter > max_number)
            results.add(0, "redundant newline");
    }
    else
        empty_lines_counter = 0;
}

void Checker::check_newline_at_eof()
{
    ExtendedBoolean newline_at_eof =
        settings.ext_bool_options["newline_at_eof"];
    if(newline_at_eof == EB_TRUE && empty_lines_counter == 0)
        results.add(0, "no newline at the end of file");
    else if(newline_at_eof == EB_FALSE && empty_lines_counter != 0)
        results.add(formatter_params.current_line_index - 1, 0, "newline at the end of file");
}

void Checker::output_results_to_file(const std::string& results_)
{
    std::ofstream results_stream(results_.c_str());
    results.output_to_stream(results_stream);
    results_stream.close();
}
