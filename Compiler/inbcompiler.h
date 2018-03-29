#ifndef INBCOMPILER_H
#define INBCOMPILER_H

#include "stdafx.h"
#include "colored_cout.h"

enum class Language
{
    CPP
};
enum class kw
{
    Namespace,
    Enum,
    Struct,
    Optional,
    Int8,
    Uint8,
    Int16,
    Uint16,
    Int32,
    Uint32,
    Int64,
    Uint64,
    Float32,
    Float64,
    Bytes,
};
static const std::map<std::string, kw> keywords =
{
    { "namespace",  kw::Namespace   },
    { "enum"     ,  kw::Enum        },
    { "struct"   ,  kw::Struct      },
    { "optional" ,  kw::Optional    },
    { "int8"     ,  kw::Int8        },
    { "uint8"    ,  kw::Uint8       },
    { "int16"    ,  kw::Int16       },
    { "uint16"   ,  kw::Uint16      },
    { "int32"    ,  kw::Int32       },
    { "uint32"   ,  kw::Uint32      },
    { "int64"    ,  kw::Int64       },
    { "uint64"   ,  kw::Uint64      },
    { "float32"  ,  kw::Float32     },
    { "float64"  ,  kw::Float64     },
    { "bytes"    ,  kw::Bytes       }
};
static const std::string tokenOpenScope("{");
static const std::string tokenCloseScope("}");
static const std::string tokenArray("[]");
static const std::string tokenEquals("=");
static const std::string tokenColon(":");
static const std::string tokenOptional("optional");
static const std::string tokenBytes("bytes");


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
    using StructDescr = std::pair<std::list<std::pair<std::string,
                                                      StructFieldType>>,
                                  uint32_t>;
    std::map<std::string, StructDescr> m_structs;

    bool m_detailed = false;
};

#endif // INBCOMPILER_H
