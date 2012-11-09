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

void Checker::add_result(const std::string& error)
{
    results.add(token->line_index, token->start, error);
}

void Checker::add_result(int index, const std::string& error)
{
    results.add(token->line_index, index, error);
}

void Checker::set_language(const std::string& language_)
{
    std::map< std::string, std::pair<Language, std::string> > languages_aliases;
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

void Checker::init_token_types()
{
    token_types["brace"] = TT_BRACE;
    token_types["bracket"] = TT_BRACKET;

    token_types["cbracket_open"] = TT_CBRACKET_OPEN;
    token_types["cbracket_close"] = TT_CBRACKET_CLOSE;
    token_types["pasbracket_open"] = TT_PASBRACKET_OPEN;
    token_types["pasbracket_close"] = TT_PASBRACKET_CLOSE;

    token_types["number"] = TT_NUMBER;
    token_types["preproc"] = TT_PREPROC;
    token_types["comment"] = TT_COMMENT;
    token_types["string"] = TT_STRING;
    token_types["specialchar"] = TT_SPECIALCHAR;

    token_types["symbol"] = TT_SYMBOL;
    token_types["semicolon"] = TT_SEMICOLON;

    token_types["type"] = TT_TYPE;
    token_types["classname"] = TT_CLASSNAME;
    token_types["function"] = TT_FUNCTION;
    token_types["identifier"] = TT_IDENTIFIER;

    token_types["keyword"] = TT_KEYWORD;
    token_types["case"] = TT_CASE;
    token_types["of"] = TT_OF;

    token_types["typeblock"] = TT_TYPEBLOCK;
    token_types["varblock"] = TT_VARBLOCK;

    token_types["keyword_declaring_varblock"] = TT_KEYWORD_DECLARING_VARBLOCK;
    token_types["keyword_declaring_func"] = TT_KEYWORD_DECLARING_FUNC;

    token_types["label"] = TT_LABEL;
    token_types["switch_labels"] = TT_SWITCH_LABELS;

    token_types["normal"] = TT_WHITESPACE;

    token_types["keyword_with_following_operation"] = TT_KEYWORD_WITH_FOLLOWING_OPERATION;
    token_types["keyword_with_following_operation_after_braces"] =
        TT_KEYWORD_WITH_FOLLOWING_OPERATION_AFTER_BRACE;
}

Checker::Checker(
    const std::string& filename_,
    const std::string& language_,
    const std::string& settings_
):
    file_to_process(filename_.c_str()),
    settings(settings_),
    empty_lines_counter(0),
    previous_depth(0),
    env(),
    results(),
    tokens(),
    token_types()
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
    init_token_types();
}

Checker::~Checker()
{
    file_to_process.close();
}

PseudoFormatterPtr Checker::new_formatter(const std::string& s_)
{
    return PseudoFormatterPtr(
        new PseudoFormatter(token_types, tokens, env.current_line_index, s_));
}

void Checker::process_file()
{
    srchilite::RegexRuleFactory rule_factory;
    srchilite::LangDefManager lang_def_manager(&rule_factory);
    srchilite::SourceHighlighter highlighter(
        lang_def_manager.getHighlightState(LANG_PATH, file_language.second));
    srchilite::FormatterManager manager(
        PseudoFormatterPtr(new PseudoFormatter(token_types, tokens, env.current_line_index)));

    manager.addFormatter("cbracket_open", new_formatter("cbracket_open"));
    manager.addFormatter("cbracket_close", new_formatter("cbracket_close"));
    manager.addFormatter("pasbracket_open", new_formatter("pasbracket_open"));
    manager.addFormatter("pasbracket_close", new_formatter("pasbracket_close"));

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

    manager.addFormatter("case", new_formatter("case"));
    manager.addFormatter("of", new_formatter("of"));
    manager.addFormatter("typeblock", new_formatter("typeblock"));
    manager.addFormatter("varblock", new_formatter("varblock"));
    manager.addFormatter("keyword_declaring_varblock", new_formatter("keyword_declaring_varblock"));
    manager.addFormatter("keyword_declaring_func", new_formatter("keyword_declaring_func"));

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

    for(env.current_line_index = 0; file_to_process.good(); ++env.current_line_index)
    {
        getline(file_to_process, env.current_line);
        check_line_length();
        check_if_empty();
        lines.push_back(LineInfo(env.current_line, env.current_line.size(), 0, 0, 0));
        if(env.current_line.size() != 0)
        {
            params.start = 0;
            highlighter.highlightParagraph(env.current_line);
        };
    }
    check_newline_at_eof();
    calculate_expected_depth();
    if(settings.indentation_policy != IP_IGNORE)
        check_indentation();
    process_tokens();
}

void Checker::check_line_length()
{
    int max_length = settings.int_options["maximal_line_length"];
    if(max_length != IGNORE_OPTION && max_length <= env.current_line.size())
    {
        results.add(env.current_line_index, 0, "line too long");
    }
}

void Checker::check_if_empty()
{
    if(env.current_line.size() == 0)
    {
        empty_lines_counter++;
        int max_number = settings.int_options["maximal_number_of_separating_newlines_between_blocks"];
        if(max_number != IGNORE_OPTION && empty_lines_counter > max_number)
            results.add(env.current_line_index, 0, "redundant newline");
    }
    else
        empty_lines_counter = 0;
}

void Checker::check_newline_at_eof()
{
    ExtendedBoolean newline_at_eof =
        settings.ext_bool_options["newline_at_eof"];
    if(newline_at_eof == EB_TRUE && empty_lines_counter == 0)
        results.add(env.current_line_index, 0, "no newline at the end of file");
    else if(newline_at_eof == EB_FALSE && empty_lines_counter != 0)
        results.add(env.current_line_index - 1, 0, "newline at the end of file");
}

void Checker::output_results_to_file(const std::string& results_)
{
    std::ofstream results_stream(results_.c_str());
    results.output_to_stream(results_stream);
    results_stream.close();
}

bool Checker::keyword_equal(const std::string& to)
{
    if(file_language.first == L_PASCAL)
    {
        std::string lowercase(token->line);
        std::transform(token->line.begin(), token->line.end(), lowercase.begin(), ::tolower);
        return lowercase == to;
    }
    return token->line == to;
}

void Checker::nesting_depth_check()
{
    int maximal_nesting_depth = settings.int_options["maximal_nesting_depth"];
    if(
        lines[token->line_index].expected_depth != previous_depth &&
        maximal_nesting_depth != IGNORE_OPTION &&
        lines[token->line_index].expected_depth > maximal_nesting_depth)
    {
        add_result("maximal nesting depth exceeded");
    }
    previous_depth = lines[token->line_index].expected_depth;
}

bool Checker::is_blockbracket()
{
    switch(token->type)
    {
        case TT_CBRACKET_OPEN:
        case TT_CBRACKET_CLOSE:
        case TT_PASBRACKET_OPEN:
        case TT_PASBRACKET_CLOSE:
            return true;
        default:
            return false;
    }
}

void Checker::whitespace_sequence_check()
{
    if(token->start + token->line.size() == lines[token->line_index].size)
    {
        if(settings.ext_bool_options["forbid_trailing_whitespaces"] != EB_IGNORE)
            add_result("trailing whitespaces");
    }
    else if(token->start != 0)
    {
        if(token->line.size() > 1 && settings.ext_bool_options["forbid_vertical_alignment"] != EB_IGNORE)
            add_result("more than one whitespace");
    }
}

void Checker::keyword_and_brace_check()
{
    ExtendedBoolean& space(settings.ext_bool_options["space_between_keyword_and_brace"]);

    int keyword_end = token->start + token->line.size();
    if(keyword_end == lines[token->line_index].size)
        return;

    char next_symbol = lines[token->line_index].line[keyword_end];
    if(space == EB_CONSISTENT)
    {
        space = next_symbol == ' ' ? EB_TRUE : EB_FALSE;
    }
    else if(space == EB_TRUE && next_symbol != ' ')
    {
        add_result(keyword_end, "no space after " + token->line);
    }
    else if(space == EB_FALSE && next_symbol == ' ')
    {
        add_result(keyword_end, "space after " + token->line);
    }
}

void Checker::else_check()
{
    ExtendedBoolean& at_newline(settings.ext_bool_options["else_at_newline"]);
    int depth_by_fact = lines[token->line_index].depth_by_fact;
    if(at_newline == EB_CONSISTENT)
    {
        at_newline = token->start == depth_by_fact ? EB_TRUE : EB_FALSE;
    }
    else if(at_newline == EB_TRUE && token->start != depth_by_fact)
    {
        add_result("else is not at new line");
    }
    else if(at_newline == EB_FALSE && token->start == depth_by_fact)
    {
        add_result("else is at new line");
    }
}

void Checker::name_style_check(NameType type)
{
    std::string option_name = (
        type == NT_CLASS ? "class_naming_style" :
        type == NT_FUNCTION ? "function_naming_style" :
            "variable_naming_style");

    NamingStyle ns = settings.naming_styles[option_name];
    bool style_error = false;

    if(ns == NS_CAMEL || ns == NS_PASCAL)
    {
        style_error =
            ns == NS_CAMEL && !std::islower(token->line[0]) ||
            ns == NS_PASCAL && !std::isupper(token->line[0]);

        for(int i = 1; !style_error && i < token->line.size(); ++i)
        {
            style_error = !std::isalpha(token->line[i]) && !std::isdigit(token->line[i]);
        }
    }

    if(ns == NS_UNDERSCORE || ns == NS_CAPS_UNDERSCORE)
    {
        for(int i = 0; !style_error && i < token->line.size(); ++i)
        {
            style_error =
                token->line[i] != '_' &&
                !std::isdigit(token->line[i]) && (
                    ns == NS_UNDERSCORE && !islower(token->line[i]) ||
                    ns == NS_CAPS_UNDERSCORE && !isupper(token->line[i]));
        }
    }

    if(style_error)
    {
        std::string ns_string(
            ns == NS_CAMEL ? "camel" :
            ns == NS_PASCAL ? "pascal" :
            ns == NS_UNDERSCORE ? "underscore" :
                "caps_underscore");
        add_result("name " + token->line + " is not of style " + ns_string);
    }
}

void Checker::brace_check()
{
    char c = token->line[0];

    if(c == '(')
    {
        env.braces_opened++;
    }
    else
    {
        env.braces_opened--;
        assert(env.braces_opened >= 0);

        if(env.braces_opened == 0 && env.indented_operation_expected_after_braces)
        {
            env.indented_operation_expected = true;
            env.indented_operation_expected_after_braces = false;

            if(settings.ext_bool_options["forbid_multiple_expressions_per_line"] == EB_TRUE)
                env.line_closed = true;
        }
    }
}

void Checker::spaces_in_unibrackets_check(int offset)
{
    ExtendedBoolean* opt_ptr = NULL;
    char c = token->line[0];
    char opposite;

    switch(c)
    {
        case '(':
        case ')':
            opt_ptr = &settings.ext_bool_options["spaces_inside_braces"];
            opposite = c == '(' ? ')' : '(';
            break;
        case '[':
        case ']':
            opt_ptr = &settings.ext_bool_options["spaces_inside_brackets"];
            opposite = c == '[' ? ']' : '[';
            break;
        case '{':
        case '}':
            opt_ptr = &settings.ext_bool_options["spaces_inside_curly_braces"];
            opposite = c == '{' ? '}' : '{';
            break;
        default:
            throw UnilintException("Not a bracket symbol");
    }

    ExtendedBoolean& spaces_inside(*opt_ptr);

    char next_symbol = lines[token->line_index].line[token->start + offset];
    //empty brackets are exception
    if(next_symbol == opposite)
        return;

    if(spaces_inside == EB_CONSISTENT)
    {
        spaces_inside = next_symbol == ' ' ? EB_TRUE : EB_FALSE;
    }
    else if(spaces_inside == EB_TRUE && next_symbol != ' ')
    {
        add_result(
            token->start + offset,
            std::string("no space ") + (offset > 0 ? "after " : "before ") + c);
    }
    else if(spaces_inside == EB_FALSE && next_symbol == ' ')
    {
        add_result(
            token->start + offset,
            std::string("unexpected space ") + (offset > 0 ? "after " : "before ") + c);
    }
}

void Checker::unibracket_check()
{
    char c = token->line[0];
    if(c == '(' || c == '[' || c == '{')
    {
        if(token->start != lines[token->line_index].size - 1)
            spaces_in_unibrackets_check(1);
    }
    else
    {
        if(token->start != 0 && token->start != lines[token->line_index].depth_by_fact)
            spaces_in_unibrackets_check(-1);
    }
}

void Checker::blockbracket_check()
{
    if(!is_pascal_functional_begin())
    {
        ExtendedBoolean& block_at_newline(settings.ext_bool_options["start_block_at_newline"]);
        int indentation_end = lines[token->line_index].depth_by_fact;
        if(block_at_newline == EB_CONSISTENT)
        {
            block_at_newline = token->start == indentation_end ? EB_TRUE : EB_FALSE;
        }
        else if(block_at_newline == EB_TRUE && token->start != indentation_end)
        {
            add_result(token->line + " is not at new line");
        }
        else if(block_at_newline == EB_FALSE && token->start == indentation_end)
        {
            add_result(token->line + " is at new line");
        }
    }
}

void Checker::process_tokens()
{
    int line_index = -1;
    for(token = tokens.begin(); token != tokens.end(); ++token)
    {
        if(token->type == TT_WHITESPACE)
        {
            whitespace_sequence_check();
            continue;
        }

        if(token->line_index != line_index)
        {
            nesting_depth_check();
            line_index = token->line_index;
            env.line_closed = false;
        }

        if(env.line_closed && token->type != TT_COMMENT && !is_blockbracket())
        {
            add_result("two or more expressions at one line");
            env.line_closed = false;
        }

        switch(token->type)
        {
            case TT_CBRACKET_OPEN:
            case TT_CBRACKET_CLOSE:
            case TT_BRACE:
            case TT_BRACKET:
                unibracket_check();
        }

        switch(token->type)
        {
            case TT_CBRACKET_OPEN:
            case TT_PASBRACKET_OPEN:
            case TT_KEYWORD_DECLARING_VARBLOCK:
                blockbracket_check();
                break;

            case TT_CLASSNAME:
            case TT_FUNCTION:
            case TT_IDENTIFIER:
            {
                NameType name_type = (
                    token->type == TT_CLASSNAME ? NT_CLASS :
                    token->type == TT_FUNCTION ? NT_FUNCTION :
                        NT_VARIABLE);

                auto name_in_list = env.list_of_names.find(token->line);
                if(name_in_list == env.list_of_names.end())
                {
                    env.list_of_names[token->line] = name_type;
                    name_style_check(name_type);
                }
                else
                {
                    if(name_type == NT_VARIABLE)
                        //TODO: store checked names
                        name_style_check(name_in_list->second);
                    else if(name_in_list->second != name_type)
                    {
                        //constructors in C++ are exceptions
                        //assume C++ function with name == classname is a constructor or destructor
                        //TODO: do check?
                    }
                }
                break;
            }

            case TT_KEYWORD_WITH_FOLLOWING_OPERATION:
            {
                if(keyword_equal("else"))
                {
                    if(previous_is_blockbracket())
                    {
                        else_check();
                    }
                    else if(
                        settings.ext_bool_options["forbid_multiple_expressions_per_line"] == EB_TRUE &&
                        file_language.first == L_PASCAL &&
                        token->start != lines[token->line_index].depth_by_fact)
                    {
                        add_result("two or more expressions at one line");
                        env.line_closed = false;
                    }
                }
                if(settings.ext_bool_options["forbid_multiple_expressions_per_line"] == EB_TRUE)
                    env.line_closed = true;
                env.indented_operation_expected = true;
                break;
            }

            case TT_KEYWORD_WITH_FOLLOWING_OPERATION_AFTER_BRACE:
                keyword_and_brace_check();
                env.indented_operation_expected_after_braces = true; //required for multiple ops
                break;

            case TT_SEMICOLON:
                if(env.braces_opened == 0)
                    if(settings.ext_bool_options["forbid_multiple_expressions_per_line"] == EB_TRUE)
                        env.line_closed = true;
                break;

            case TT_BRACE:
                brace_check();
                break;
        }

        if(env.indented_operation_expected && settings.ext_bool_options["compulsory_block_braces"] == EB_TRUE)
        {
            if(!next_is_blockbracket(false))
                add_result("block brace expected: { or begin");

            env.indented_operation_expected = false;
        }
    }
}

void Checker::prefix_preprocessing()
{
    lines[token->line_index].can_check_indent = false;

    bool tabs = false;
    bool spaces = false;
    for(unsigned int i = 0; i < token->line.size(); ++i)
    {
        tabs = tabs || (token->line[i] == '\t');
        spaces = spaces || (token->line[i] == ' ');
        if(!tabs && !spaces)
        {
            add_result(i, "strange whitespace character");
            return;
        }
    }

    if(tabs && spaces)
    {
        add_result("tabs and spaces mixed");
        return;
    }

    if(settings.indentation_style == IS_CONSISTENT)
    {
        settings.indentation_style = tabs ? IS_TABS : IS_SPACES;
        if(tabs)
        {
            settings.indentation_policy = IP_BY_SIZE;
            settings.int_options["indentation_size"] = 1;
        }
    }

    if(tabs && settings.indentation_style == IS_SPACES)
    {
        add_result("tabs found, spaces expected");
        //indentation_size can be undefined
        //so tabs cannot be translated to spaces
        return;
    }

    if(spaces)
    {
        if(settings.indentation_style == IS_TABS)
        {
            add_result("spaces found, tabs expected");
            //indentation_size is not stored for style = tabs
            //so spaces cannot be translated to tabs
            return;
        }

        if(settings.indentation_policy == IP_IGNORE)
            return;

        //TODO: try calc this from expected_depth
        if(settings.indentation_policy == IP_CONSISTENT)
        {
            settings.indentation_policy = IP_BY_SIZE;
            settings.int_options["indentation_size"] = token->line.size();
        }
    }

    lines[token->line_index].can_check_indent = true;
}

void Checker::check_indentation()
{
    int scale = settings.int_options["indentation_size"];

    int j = 0;
    for(auto i = lines.begin(); i != lines.end(); ++i, ++j)
    {
        // std::cout
        //     << (j + 1) << " "
        //     << i->expected_depth << " "
        //     << i->expected_extra_depth << " "
        //     << i->depth_by_fact << std::endl;

        assert(i->expected_depth >= 0);
        assert(i->expected_extra_depth >= 0);

        if(!i->can_check_indent)
            continue;

        int depth = i->expected_depth;

        ExtendedBoolean& extra_indent(settings.ext_bool_options["extra_indent_for_blocks"]);
        if(extra_indent == EB_CONSISTENT && i->expected_extra_depth != 0)
        {
            //false is more likely
            extra_indent = i->depth_by_fact == (i->expected_extra_depth + depth) * scale ?
                EB_TRUE : EB_FALSE;
        }

        if(extra_indent == EB_TRUE)
            depth += i->expected_extra_depth;

        if(i->depth_by_fact < depth * scale)
            results.add(j, 0, "indent too small");
        else if(i->depth_by_fact > depth * scale)
            results.add(j, 0, "indent too deep");
    }
}

bool Checker::next_is_blockbracket(bool include_current)
{
    auto find_block(token);
    if(!include_current)
        find_block++;
    for(; find_block != tokens.end(); ++find_block)
    {
        switch(find_block->type)
        {
            case TT_WHITESPACE:
            case TT_COMMENT:
                //both are non-valid tokens
                break;

            case TT_CBRACKET_OPEN:
            case TT_PASBRACKET_OPEN:
                return true;

            default:
                return false;
        }
    }
    return false;
}

bool Checker::previous_is_blockbracket()
{
    auto find_block(token);
    find_block--;
    for(; find_block != tokens.begin(); --find_block)
    {
        switch(find_block->type)
        {
            case TT_WHITESPACE:
            case TT_COMMENT:
                //both are non-valid tokens
                break;

            case TT_CBRACKET_CLOSE:
            case TT_PASBRACKET_CLOSE:
                return true;

            default:
                return false;
        }
    }
    return false;
}
