#ifndef INBCOMPILER_H
#define INBCOMPILER_H

#include "stdafx.h"
#include "colored_cout.h"

class INBCompiler
{
public:
    INBCompiler();

    void read(const std::string &input);
    void write(const std::string &output);

private:

    using tokens_t = std::list<std::string>;
    using tokens_it = tokens_t::iterator;
    
    void parse();
    void parseNamespace(tokens_it &it);
    void parseEnum(tokens_it &it);
    void parseStruct(tokens_it &it);
    bool next(tokens_it &it) const;
    
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
};

#endif // INBCOMPILER_H
