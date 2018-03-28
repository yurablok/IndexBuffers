#ifndef INBCOMPILER_H
#define INBCOMPILER_H

#include "stdafx.h"
#include "colored_cout.h"

enum class Language
{
    CPP
};

class INBCompiler
{
public:
    INBCompiler();

    void read(const std::string &input, const bool detailed = false);
    void write(const std::string &output, const Language &lang);

private:

    using tokens_t = std::list<std::string>;
    using tokens_it = tokens_t::iterator;
    
    void parse();
    void parseNamespace(tokens_it &it);
    void parseEnum(tokens_it &it);
    void parseStruct(tokens_it &it);
    bool next(tokens_it &it) const;
    void genCPP(const std::string &out);

    tokens_t m_tokens;
    
    std::list<std::string> m_namespaces;

    using EnumDescr = std::list<std::pair<std::string, uint32_t>>;
    std::map<std::string, EnumDescr> m_enums;

    struct StructFieldType
    {
        std::string type;
        bool isOptional = false;
        bool isArray = false;
    };
    using StructDescr = std::list<std::pair<std::string, StructFieldType>>;
    std::map<std::string, StructDescr> m_structs;

    bool m_detailed = false;
};

#endif // INBCOMPILER_H
