#include "stdafx.h"
#include "inbcompiler.h"

void splitStr(const std::string &source, std::list<std::string> &splitted, const char by = ' ')
{
    if (source.empty())
        return;
    std::string buffer;
    for (const char c : source)
    {
        if (c == by && !buffer.empty())
            splitted.emplace_back(std::move(buffer));
        else
            buffer.push_back(c);
    }
    if (!buffer.empty())
        splitted.emplace_back(std::move(buffer));
}

void splitIbs(const std::string &source, std::list<std::string> &splitted)
{
    if (source.empty())
        return;
    std::string buffer;
    for (const char c : source)
    {
        if (c == ' ' && !buffer.empty())
        {
            splitted.emplace_back(std::move(buffer));
            //fast fix
            //if (splitted.back().front() == ' ')
            //    splitted.erase(splitted.begin());
        }
        else if (c == ',' || c == '{' || c == '}' || c == ':')
            splitted.emplace_back(1, c);
        else
            buffer.push_back(c);
    }
    if (!buffer.empty())
        splitted.emplace_back(std::move(buffer));
}


INBCompiler::INBCompiler()
{}

void INBCompiler::read(const std::string &input, const bool detailed)
{
    std::cout << "INPUT: " << input << std::endl;
    m_detailed = detailed;

    std::ifstream file(input, std::ios_base::binary);
    if (!file.is_open())
    {
        std::cout << clr::red << "Error! Cannot open " << input << clr::default_ << std::endl;
        exit(0);
    }

    std::string buffer;

    file.seekg(0, std::ios_base::end);
    buffer.resize(file.tellg());
    file.seekg(0, std::ios_base::beg);
    file.read(&buffer[0], buffer.size());

    m_tokens.clear();
    m_tokens.emplace_back();
    bool isComment = false;
    for (const char c : buffer)
    {
        switch (isComment)
        {
        case true:
            switch (c)
            {
            case '\n':
            case '\r':
                isComment = false;
                break;
            default:
                break;
            }
            break;
        case false:
            switch (c)
            {
            case ' ':
            case '\t':
                if (!m_tokens.back().empty())
                    m_tokens.emplace_back();
                break;
            case '[':
                if (!m_tokens.back().empty())
                    m_tokens.emplace_back();
                m_tokens.back().push_back(c);
                break;
            //case ';':
            case ':':
            case ',':
                if (!m_tokens.back().empty())
                    m_tokens.emplace_back();
                m_tokens.back().push_back(c);
                m_tokens.emplace_back();
                break;
            case '\n':
            case '\r':
                if (!m_tokens.back().empty())
                    m_tokens.emplace_back();
                break;
            case '/':
                isComment = true;
                break;
            default:
                m_tokens.back().push_back(c);
                break;
            }
            break;
        }
    }
    //for (auto it = m_tokens.begin(); it != m_tokens.end(); it++)
    //    std::cout << *it << ' ';
    //std::cout << std::endl;
    parse();
}

void INBCompiler::write(const std::string &output, const Language &lang)
{
    switch (lang)
    {
    case Language::CPP:
        genCPP(output);
        break;
    default:
        std::cout << clr::red << "Error! Unknown language code: "
                  << static_cast<uint32_t>(lang) << clr::default_ << std::endl;
        exit(0);
        //break;
    }
    std::cout << clr::green << "OUTPUT: " << output << clr::default_ << std::endl;
}

void INBCompiler::parse()
{
    tokens_it it = m_tokens.begin();
    tokens_it prev = it;
    while (it != m_tokens.end())
    {
        if (it->empty())
        {
            next(it);
            continue;
        }
        const auto &keyword = keywords.find(*it);
        if (keyword == keywords.end())
        {
            std::cout << clr::red << "Error! Wrong token: " << *it << clr::default_ << std::endl;
            exit(0);
        }
        switch (keyword->second)
        {
        case kw::Namespace:
            parseNamespace(it);
            break;
        case kw::Enum:
            parseEnum(it);
            break;
        case kw::Struct:
            parseStruct(it);
            break;
        default:
            std::cout << clr::red << "Error! Wrong using of keyword: " << *it << clr::default_ << std::endl;
            exit(0);
            //break;
        }
        if (prev == it)
            break;
        prev = it;
    }
}

// <namespace> <EXTERNAL.INTERNAL>
void INBCompiler::parseNamespace(tokens_it &it)
{
    if (!next(it)) // ++<namespace>
        return;
    m_namespaces.clear();
    splitStr(*it, m_namespaces, '.'); // <EXTERNAL.INTERNAL>
    next(it); // ++<EXTERNAL.INTERNAL>
    if (m_detailed)
    {
        std::cout << "NAMESPACE: ";
        for (const auto &ns : m_namespaces)
            std::cout << ns << ' ';
        std::cout << std::endl;
    }
}

// <enum> <NAME> <{> [<FIELD> <=> <NUMBER>] <}>
void INBCompiler::parseEnum(tokens_it &it)
{
    if (!next(it)) // ++<enum>
        return;
    std::string enumName = std::move(*it); // <NAME>
    if (!next(it)) // ++<NAME>
        return;
    if (*it != tokenOpenScope) // <{>
        return;
    auto &fieldsList = m_enums[enumName];
    next(it); // ++<{>
    uint32_t fieldsIndex = 0;
    while (*it != tokenCloseScope)
    {
        if (*it == tokenEquals)
        {
            if (!next(it))
                return;
            if (fieldsList.empty())
                return;
            fieldsIndex = std::stoul(*it);
            if (fieldsList.back().second > fieldsIndex)
                return;
            fieldsList.back().second = fieldsIndex;
            fieldsIndex++;
            if (!next(it))
                return;
            continue;
        }
        fieldsList.emplace_back(std::move(*it), fieldsIndex);
        fieldsIndex++;
        if (!next(it))
            return;
    }
    next(it);
    if (m_detailed)
    {
        std::cout << "ENUM: ";
        std::cout << enumName << " { ";
        for (const auto &field : fieldsList)
            std::cout << field.first << '=' << field.second << ' ';
        std::cout << '}' << std::endl;
    }
}

// <struct> <NAME> <{> [<FIELD> <:> <TYPE> <[]> <optional>] <}>
void INBCompiler::parseStruct(tokens_it &it)
{
    if (!next(it)) // ++<struct>
        return;
    std::string structName = std::move(*it); // <NAME>
    if (!next(it)) // ++<NAME>
        return;
    if (*it != tokenOpenScope) // <{>
        return;
    next(it); // ++<{>
    auto &fieldsList = m_structs[structName].first;
    auto &optionalCount = m_structs[structName].second;
    optionalCount = 0;
    bool isExpectType = false;
    while (*it != tokenCloseScope)
    {
        if (*it == tokenArray)
        {
            if (isExpectType)
                return;
            if (fieldsList.empty())
                return;
            fieldsList.back().second.isArray = true;
            if (!fieldsList.back().second.isOptional)
                optionalCount++;
            fieldsList.back().second.isOptional = true;
            if (!next(it))
                return;
        }
        else if (*it == tokenOptional)
        {
            if (isExpectType)
                return;
            if (fieldsList.empty())
                return;
            if (!fieldsList.back().second.isOptional)
                optionalCount++;
            fieldsList.back().second.isOptional = true;
            if (!next(it))
                return;
        }
        else if (*it == tokenColon)
        {
            if (!isExpectType)
                return;
            if (!next(it))
                return;
            if (fieldsList.empty())
                return;
            fieldsList.back().second.type = std::move(*it);
            if (fieldsList.back().second.type == tokenBytes)
            {
                fieldsList.back().second.isArray = true;
                if (!fieldsList.back().second.isOptional)
                    optionalCount++;
                fieldsList.back().second.isOptional = true;
            }
            if (!next(it))
                return;
            isExpectType = false;
        }
        else
        {
            fieldsList.emplace_back(std::move(*it), StructFieldType());
            if (!next(it))
                return;
            isExpectType = true;
        }
        //if (!next(it))
        //    return;
    }
    next(it);
    if (m_detailed)
    {
        std::cout << "STRUCT: ";
        std::cout << structName << " { ";
        for (const auto &field : fieldsList)
        {
            std::cout << field.first << ':' << field.second.type;
            if (field.second.isArray)
                std::cout << ".arr";
            if (field.second.isOptional)
                std::cout << ".opt";
            std::cout << ' ';
        }
        std::cout << '}' << std::endl;
    }
}

bool INBCompiler::next(tokens_it &it) const
{
    if (it == m_tokens.end())
        return false;
    ++it;
    return true;
}
