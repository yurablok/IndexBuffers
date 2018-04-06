#include "stdafx.h"
#include "inbcompiler.h"

void splitStr(const std::string &source, std::deque<std::string> &splitted, const char by = ' ')
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

void splitIbs(const std::string &source, std::deque<std::string> &splitted)
{
    if (source.empty())
        return;
    std::string buffer;
    for (const char c : source)
    {
        if (c == ' ' && !buffer.empty())
            splitted.emplace_back(std::move(buffer));
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
    buffer.resize(static_cast<uint32_t>(file.tellg()));
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
    calcSchemaHash();
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
        case kw::Const:
            parseConst(it);
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
        std::cout << "NAMESPACE: " << clr::blue;
        for (const auto &ns : m_namespaces)
            std::cout << ns << ' ';
        std::cout << clr::default_ << std::endl;
    }
}

// <const> <NAME> <:> <TYPE> <=> <VALUE>
void INBCompiler::parseConst(INBCompiler::tokens_it &it)
{
    if (!next(it)) // ++<namespace>
        return;
    m_constants.emplace_back();
    m_constants.back().name = std::move(*it); // <NAME>
    if (!next(it)) // ++<NAME>
        return;
    if (*it != tokenColon) // <:>
        return;
    if (!next(it)) // ++<:>
        return;
    m_constants.back().type = std::move(*it); // <TYPE>
    if (!next(it)) // ++<TYPE>
        return;
    if (*it != tokenEquals) // <=>
        return;
    if (!next(it)) // ++<=>
        return;
    m_constants.back().value = std::move(*it); // <VALUE>
    next(it); // ++<VALUE>
    if (m_detailed)
    {
        std::cout << "CONSTANT: " << clr::blue;
        std::cout << m_constants.back().name << clr::default_ << ':'
                  << m_constants.back().type << '='
                  << m_constants.back().value << std::endl;
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
        std::cout << clr::blue << enumName << clr::default_ << " { ";
        for (const auto &field : fieldsList)
            std::cout << clr::cyan << field.first
                << clr::default_ << '=' << field.second << ' ';
        std::cout << '}' << std::endl;
    }
}

// <struct> <NAME> <{> [<FIELD> <:> <TYPE> <[]> <optional>] <}>
void INBCompiler::parseStruct(tokens_it &it)
{
    if (!next(it)) // ++<struct>
        return;
    std::string structNameTemp = std::move(*it); // <NAME>
    if (!next(it)) // ++<NAME>
        return;
    if (*it != tokenOpenScope) // <{>
        return;
    next(it); // ++<{>
    m_structs.emplace_back();
    
    m_structs.back().name = std::move(structNameTemp);
    const std::string &structName = m_structs.back().name;

    auto &fieldsList = m_structs.back().fields;
    auto &optionalCount = m_structs.back().optionalCount;
    
    optionalCount = 0;
    bool isExpectType = false;
    bool isExpectOptional = false;
    while (*it != tokenCloseScope)
    {
        if (*it == tokenArray)
        {
            if (isExpectType)
                return;
            if (fieldsList.empty())
                return;
            fieldsList.back().isArray = true;
            if (!fieldsList.back().isOptional)
                optionalCount++;
            fieldsList.back().isOptional = true;
            if (!next(it))
                return;
        }
        else if (*it == tokenOptional)
        {
            if (isExpectType)
                return;
            if (fieldsList.empty())
                return;
            if (!fieldsList.back().isOptional)
                optionalCount++;
            fieldsList.back().isOptional = true;
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
            fieldsList.back().type = std::move(*it);
            if (isBuiltInType(fieldsList.back().type))
            {
                fieldsList.back().isBuiltIn = true;
                if (fieldsList.back().type == tokenBytes)
                {
                    fieldsList.back().isArray = true;
                    if (!fieldsList.back().isOptional)
                        optionalCount++;
                    fieldsList.back().isOptional = true;
                }
            }
            else
            {
                if (!fieldsList.back().isOptional)
                    optionalCount++;
                fieldsList.back().isOptional = true;
                fieldsList.back().isBuiltIn = false;
                isExpectOptional = true;

                auto fs = std::find_if(m_structs.begin(), m_structs.end(),
                    [&](StructDescr &sd) { return sd.name == fieldsList.back().type; });
                if (fs == m_structs.end())
                {
                    std::cout << clr::red << "Wrong structs order, must be (first)'"
                              << fieldsList.back().type << "'+(second)'"
                              << structName << "'" << clr::default_ << std::endl;
                    exit(0);
                }
                fs->friends.insert(structName);
            }
            if (!next(it))
                return;
            isExpectType = false;
        }
        else
        {
            if (isExpectOptional)
            {
                isExpectOptional = false;
                if (!fieldsList.back().isArray)
                {
                    std::cout << clr::red << "Custom types ("
                              << fieldsList.back().name << ":"
                              << fieldsList.back().type
                              << ") should be specified as optional "
                              << clr::default_ << std::endl;
                    exit(0);
                }
            }
            fieldsList.emplace_back();
            fieldsList.back().name = std::move(*it);
            if (!next(it))
                return;
            isExpectType = true;
        }
    }
    if (isExpectOptional)
    {
        isExpectOptional = false;
        if (!fieldsList.back().isArray && !fieldsList.back().isOptional)
        {
            std::cout << clr::red << "Custom types ("
                      << fieldsList.back().name << ":"
                      << fieldsList.back().type
                      << ") should be specified as optional "
                      << clr::default_ << std::endl;
            exit(0);
        }
    }
    next(it);
    if (m_detailed)
    {
        std::cout << "STRUCT: ";
        std::cout << clr::blue << structName << clr::default_ << " { ";
        for (const auto &field : fieldsList)
        {
            if (field.isBuiltIn)
                std::cout << clr::cyan;
            else
                std::cout << clr::blue;
            std::cout << field.name << clr::default_ << ':' << field.type;
            if (field.isBuiltIn)
                std::cout << ".b-in";
            if (field.isArray)
                std::cout << ".arr";
            if (field.isOptional)
                std::cout << ".opt";
            std::cout << ' ';
        }
        std::cout << '}' << std::endl;
    }
    std::sort(fieldsList.begin(), fieldsList.end(),
        [](const StructFieldType& a, const StructFieldType& b)
        {
            return a.isOptional < b.isOptional;
        });
}

bool INBCompiler::next(tokens_it &it) const
{
    if (it == m_tokens.end())
        return false;
    ++it;
    return true;
}

void INBCompiler::calcSchemaHash()
{
    uint32_t mm3 = 0;
    for (const auto &ns : m_namespaces) {
        mm3 = MurmurHash3_x86_32(ns.data(), ns.size(), mm3);
    }
    //for (const auto &ct : m_constants) {
    //}
    for (const auto &en : m_enums) {
        mm3 = MurmurHash3_x86_32(en.first.data(), en.first.size(), mm3);
        for (const auto &f : en.second) {
            mm3 = MurmurHash3_x86_32(f.first.data(), f.first.size(), mm3);
            mm3 = MurmurHash3_x86_32(&f.second, sizeof(f.second), mm3);
        }
    }
    for (const auto &st : m_structs) {
        mm3 = MurmurHash3_x86_32(st.name.data(), st.name.size(), mm3);
        for (const auto &f : st.fields) {
            mm3 = MurmurHash3_x86_32(f.name.data(), f.name.size(), mm3);
            mm3 = MurmurHash3_x86_32(f.type.data(), f.type.size(), mm3);
            mm3 = MurmurHash3_x86_32(&f.isArray, sizeof(f.isArray), mm3);
            mm3 = MurmurHash3_x86_32(&f.isBuiltIn, sizeof(f.isBuiltIn), mm3);
            mm3 = MurmurHash3_x86_32(&f.isOptional, sizeof(f.isOptional), mm3);
        }
    }
    m_schemaHash = mm3; //(mm3 << 16) ^ (mm3 & 0xffff);
}
