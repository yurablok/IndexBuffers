#pragma once
#include "stdafx.h"

enum class Language {
    CPP
};
enum class kw {
    UNDEFINED,
    // Global space
    Include,
    Namespace,
    Const,
    Enum,
    Struct,
    Union,
    // Local space
    CRC32,
    MurMur3,
    Optional,
    NoHeader,
    // Enum's methods
    Count,
    Min,
    Max,
    // Types
    Int8,
    Int16,
    Int32,
    Int64,
    UInt8,
    UInt16,
    UInt32,
    UInt64,
    Float32,
    Float64,
    Bytes,
};
static const std::map<std::string, kw> keyword_to_kw = {
    { "include"  ,  kw::Include     },
    { "namespace",  kw::Namespace   },
    { "enum"     ,  kw::Enum        },
    { "struct"   ,  kw::Struct      },
    { "const"    ,  kw::Const       },
    { "union"    ,  kw::Union       },

    { "crc32"    ,  kw::CRC32       },
    { "mmh3"     ,  kw::MurMur3     },
    { "optional" ,  kw::Optional    },
    { "no_header",  kw::NoHeader    },

    { "count"    ,  kw::Count       },
    { "min"      ,  kw::Min         },
    { "max"      ,  kw::Max         },

    { "int8"     ,  kw::Int8        },
    { "uint8"    ,  kw::UInt8       },
    { "int16"    ,  kw::Int16       },
    { "uint16"   ,  kw::UInt16      },
    { "int32"    ,  kw::Int32       },
    { "uint32"   ,  kw::UInt32      },
    { "int64"    ,  kw::Int64       },
    { "uint64"   ,  kw::UInt64      },
    { "float32"  ,  kw::Float32     },
    { "float64"  ,  kw::Float64     },
    { "bytes"    ,  kw::Bytes       }
};
static const char* kw_to_keyword[] = {
    "UNDEFINED",
    // Global space
    "include",
    "namespace",
    "enum",
    "struct",
    "const",
    "union",
    // Local space
    "crc32",
    "mmh3",
    "optional",
    "no_header",
    // Enum's methods
    "count",
    "min",
    "max",
    // Types
    "int8",
    "uint8",
    "int16",
    "uint16",
    "int32",
    "uint32",
    "int64",
    "uint64",
    "float32",
    "float64",
    "bytes"
};
static kw findKeyword(const std::string& keyword) {
    auto it = keyword_to_kw.find(keyword);
    if (it == keyword_to_kw.end()) {
        return kw::UNDEFINED;
    }
    return it->second;
}
static constexpr bool isBuiltInType(const kw& keyword) {
    switch (keyword) {
    case kw::Int8:
    case kw::UInt8:
    case kw::Int16:
    case kw::UInt16:
    case kw::Int32:
    case kw::UInt32:
    case kw::Int64:
    case kw::UInt64:
    case kw::Float32:
    case kw::Float64:
    case kw::Bytes:
        return true;
    default:
        return false;
    }
    return false;
}
static constexpr bool isScalarType(const kw& keyword) {
    switch (keyword) {
    case kw::Int8:
    case kw::UInt8:
    case kw::Int16:
    case kw::UInt16:
    case kw::Int32:
    case kw::UInt32:
    case kw::Int64:
    case kw::UInt64:
    case kw::Float32:
    case kw::Float64:
        return true;
    default:
        return false;
    }
    return false;
}
//static const std::string tokenOpenScope("{");
//static const std::string tokenCloseScope("}");
//static const std::string tokenOpenArray("[");
//static const std::string tokenCloseArray("]");
//static const std::string tokenEquals("=");
//static const std::string tokenColon(":");
//static const std::string tokenOptional("optional");
//static const std::string tokenBytes("bytes");
//static const std::string tokenConst("const");
static bool isSpecialToken(const std::string& token) {
    if (token.size() == 0) {
        return true;
    }
    if (token.size() == 1) {
        return !(std::isalpha(token[0]) || std::isdigit(token[0]));
    }
    if (token.size() > 1) {
        return findKeyword(token) != kw::UNDEFINED;
    }
    return false;
}

struct token_t {
    std::string str;
    uint32_t line = 0;
    uint32_t pos = 0;
};
using tokens_t = std::deque<token_t>;
using tokens_it = tokens_t::const_iterator;
using lines_t = std::deque<std::string>;

struct AST {
    struct FileMeta;
    struct ParsingMeta {
        FileMeta* fileMeta = nullptr;
        std::deque<std::string> namespace_;
        std::string path;
        lines_t lines;
        tokens_t tokens;
    };

    class ObjectMeta;
    struct ConstMeta {
        std::string name;
        const ObjectMeta* valuePtr = nullptr;
        std::string valueStr;
        uint32_t valueIdx = 0;
        kw valueKw = kw::UNDEFINED;
        kw type = kw::UNDEFINED;
    };
    struct EnumMeta {
        std::string name;
        // first=name, second=value
        std::deque<std::pair<std::string, uint64_t>> valuesVec;
        // key=name, value=valuesVecIdx
        std::unordered_map<std::string, uint32_t> valuesMap;
        kw type = kw::UNDEFINED;
    };
    struct StructFieldMeta {
        const ObjectMeta* arrayPtr = nullptr;
        uint64_t arraySize = 0; // 0 - not fixed, >0 - fixed
        uint32_t arrayIdx = 0;
        kw arrayKw = kw::UNDEFINED;

        const ObjectMeta* typePtr = nullptr;
        kw typeKw = kw::UNDEFINED;

        std::string name;

        const ObjectMeta* valuePtr = nullptr;
        std::string valueStr;
        uint32_t valueIdx = 0;
        kw valueKw = kw::UNDEFINED;

        bool isBuiltIn = false;
        bool isOptional = false;
        bool isArray = false;
    };
    struct StructMeta {
        std::string name;
        std::set<std::string> friends;
        std::deque<StructFieldMeta*> fieldsVec;
        std::unordered_map<std::string, std::unique_ptr<StructFieldMeta>> fieldsMap;
        uint32_t optionalCount = 0;
        kw attribute = kw::UNDEFINED;
    };
    struct UnionFieldMeta {
        const ObjectMeta* arrayPtr = nullptr;
        uint64_t arraySize = 0; // 0 - not fixed, >0 - fixed
        uint32_t arrayIdx = 0;
        kw arrayKw = kw::UNDEFINED;

        const ObjectMeta* typePtr = nullptr;
        kw typeKw = kw::UNDEFINED;

        std::string name;

        bool isBuiltIn = false;
        bool isArray = false;
    };
    struct UnionMeta {
        std::string name;
        //std::deque<UnionFieldMeta*> fieldsVec;
        std::unordered_map<std::string, std::unique_ptr<UnionFieldMeta>> fieldsMap;
    };

    struct TreeNode;
    class ObjectMeta {
    public:
        ObjectMeta(
            const std::string& filePath_ = std::string(),
            const uint32_t fileLine_ = 0,
            const uint32_t filePos_ = 0)
            : filePath(filePath_)
            , fileLine(fileLine_)
            , filePos(filePos_) {}
        ~ObjectMeta() { clear(); }

        void clear() {
            switch (m_type) {
            case kw::Const:
                data.constMeta.reset();
                break;
            case kw::Enum:
                data.enumMeta.reset();
                break;
            case kw::Struct:
                data.structMeta.reset();
                break;
            case kw::Union:
                data.unionMeta.reset();
                break;
            default:
                break;
            }
        }
        kw type() const {
            return m_type;
        }
        void putConstMeta(std::unique_ptr<ConstMeta> constMeta) {
            clear();
            m_type = kw::Const;
            data.constMeta = std::move(constMeta);
        }
        void putEnumMeta(std::unique_ptr<EnumMeta> enumMeta) {
            clear();
            m_type = kw::Enum;
            data.enumMeta = std::move(enumMeta);
        }
        void putStructMeta(std::unique_ptr<StructMeta> structMeta) {
            clear();
            m_type = kw::Struct;
            data.structMeta = std::move(structMeta);
        }
        void putUnionMeta(std::unique_ptr<UnionMeta> unionMeta) {
            clear();
            m_type = kw::Union;
            data.unionMeta = std::move(unionMeta);
        }
        const ConstMeta* constMeta() const {
            return data.constMeta.get();
        }
        const EnumMeta* enumMeta() const {
            return data.enumMeta.get();
        }
        const StructMeta* structMeta() const {
            return data.structMeta.get();
        }
        const UnionMeta* unionMeta() const {
            return data.unionMeta.get();
        }

        std::deque<std::string> namespace_;
        const TreeNode* parent = nullptr;
        std::string filePath;
        uint32_t fileLine;
        uint32_t filePos;

    private:
        kw m_type = kw::UNDEFINED;
        union data_t {
            data_t() { memset(this, 0, sizeof(data_t)); }
            ~data_t() {}
            std::unique_ptr<ConstMeta> constMeta;
            std::unique_ptr<EnumMeta> enumMeta;
            std::unique_ptr<StructMeta> structMeta;
            std::unique_ptr<UnionMeta> unionMeta;
        } data;
    };

    struct TreeNode {
        std::unordered_map<std::string, TreeNode> namespaces;
        std::unordered_map<std::string, std::unique_ptr<ObjectMeta>> objects;
        const TreeNode* parent = nullptr;
    };
    TreeNode objectsTree;
    struct FileMeta {
        std::string name;
        struct NamespaceMeta {
            std::deque<std::string> namespace_;
            std::deque<ObjectMeta*> objects;
        };
        std::deque<NamespaceMeta> ns;
    };
    std::deque<FileMeta> filesMeta;

    void calcSchemaHash();
    static bool isNamespacesEqual(const std::deque<std::string>& l,
        const std::deque<std::string>& r);
    void insertObject(const ParsingMeta& meta,
        const std::string& name, std::unique_ptr<ObjectMeta> object);
    ObjectMeta* findObject(const std::deque<std::string>& namespace_,
        const std::string& name) const;
};
