#include "stdafx.h"
#include "inbcompiler.h"

std::string toUpper(const std::string &str)
{
    std::string result;
    result.resize(str.size());
    for (uint32_t i = 0; i < str.size(); i++)
        result[i] = static_cast<char>(std::toupper(str[i]));
    return result;
}

static const std::map<std::string, std::string> standard =
{
    { "int8"    , "int8_t"   },
    { "uint8"   , "uint8_t"  },
    { "int16"   , "int16_t"  },
    { "uint16"  , "uint16_t" },
    { "int32"   , "int32_t"  },
    { "uint32"  , "uint32_t" },
    { "int64"   , "int64_t"  },
    { "uint64"  , "uint64_t" },
    { "float32" , "float"    },
    { "float64" , "double"   }
};

void INBCompiler::genCPP(const std::string &out)
{
    std::ofstream file(out);

    std::string filedef(toUpper(out));
    for (uint32_t i = 0; i < filedef.size(); i++)
        if (filedef[i] == '.')
            filedef[i] = '_';

    file << "#ifndef " << filedef << std::endl;
    file << "#define " << filedef << std::endl;
    file << std::endl;
    file << "#include <vector>" << std::endl;
    file << "#include <memory>" << std::endl;
    file << std::endl;
    file << "#ifndef INB_NO_COMPRESSION" << std::endl;
    file << "#   define INB_NO_COMPRESSION    0" << std::endl;
    file << "#endif" << std::endl;
    file << "#ifndef INB_TABLE_COMPRESSION" << std::endl;
    file << "#   define INB_TABLE_COMPRESSION 1" << std::endl;
    file << "#endif" << std::endl;
    file << "#ifndef INB_FULL_COMPRESSION" << std::endl;
    file << "#   define INB_FULL_COMPRESSION  2" << std::endl;
    file << "#endif" << std::endl;
    file << std::endl;
    for (const auto &ns : m_namespaces)
    {
        file << "namespace " << ns << std::endl;
        file << "{" << std::endl;
    }
    file << "namespace _inner_" << std::endl;
    file << "{" << std::endl;
    file << "    struct header" << std::endl;
    file << "    {" << std::endl;
    file << "        uint8_t signature0 = 'i';" << std::endl;
    file << "        uint8_t signature1 = 'b';" << std::endl;
    file << "        uint16_t version    :  3;" << std::endl;
    file << "        uint16_t compression:  2;" << std::endl;
    file << "        uint16_t tableSize  : 11;" << std::endl;
    file << "    };" << std::endl;
    file << "}" << std::endl;
    file << std::endl;
    for (const auto &en : m_enums)
    {
        file << "enum class " << en.first << std::endl;
        file << "{" << std::endl;
        for (auto descr = en.second.begin(); descr != en.second.end();)
        {
            file << "    " << descr->first << " = " << descr->second;
            ++descr;
            if (descr != en.second.end())
                file << ",";
            file << std::endl;
        }
        file << "};" << std::endl;
        file << std::endl;
    }
    for (const auto &st : m_structs)
    {
        file << "class " << st.first << ";" << std::endl;
    }
    file << std::endl;
    for (const auto &st : m_structs)
    {
        file << "class " << st.first << std::endl;
        file << "{" << std::endl;
        file << "public:" << std::endl;
        file << "    void create(const uint32_t reserve = 0)" << std::endl;
        file << "    {" << std::endl;
        file << "        m_buffer.reserve(reserve);" << std::endl;
        file << "        m_buffer.resize(sizeof(_inner_::header) + sizeof(staticData));" << std::endl;
        file << "        _inner_::header *h = reinterpret_cast<_inner_::header*>(&m_buffer[0]);" << std::endl;
        file << "        h->signature0  = 'i';" << std::endl;
        file << "        h->signature1  = 'b';" << std::endl;
        file << "        h->version     = 1;" << std::endl;
        file << "        h->compression = INB_NO_COMPRESSION;" << std::endl;
        file << "        h->tableSize   = 0;" << std::endl;
        file << "    }" << std::endl;
        file << std::endl;
        for (const auto &members : st.second)
        {
            const auto &stType = standard.find(members.second.type);
            if (stType == standard.end())
            {
                std::cout << clr::yellow << "Warning! Unknown type: " << members.second.type << clr::default_ << std::endl;
                continue;
            }
            const std::string &type = stType->second;
            const std::string &name = members.first;
            const bool &isArray = members.second.isArray;
            const bool &isOptional = members.second.isOptional;
            file << "    bool has_" << name << "() const" << std::endl;
            file << "    {" << std::endl;
            file << "        return true;" << std::endl;
            file << "    }" << std::endl;
            file << "    void add_" << name << "()" << std::endl;
            file << "    {}" << std::endl;
            file << "    const " << type << "& get_" << name << "() const" << std::endl;
            file << "    {" << std::endl;
            file << "        const staticData* data = m_from" << std::endl;
            file << "            ? reinterpret_cast<const staticData*>(&m_from[sizeof(_inner_::header)])" << std::endl;
            file << "            : reinterpret_cast<const staticData*>(&m_buffer[sizeof(_inner_::header)]);" << std::endl;
            file << "        return data->" << name << ";" << std::endl;
            file << "    }" << std::endl;
            file << "    void set_" << name << "(const " << type << "& " << name << ")" << std::endl;
            file << "    {" << std::endl;
            file << "        staticData* data = reinterpret_cast<staticData*>(&m_buffer[sizeof(_inner_::header)]);" << std::endl;
            file << "        data->" << name << " = " << name << ";" << std::endl;
            file << "    }" << std::endl;
            file << std::endl;
        }
        file << "    bool from(const void* ptr)" << std::endl;
        file << "    {" << std::endl;
        file << "        if (!ptr)" << std::endl;
        file << "            return false;" << std::endl;
        file << "        const _inner_::header* h = reinterpret_cast<const _inner_::header*>(ptr);" << std::endl;
        file << "        if (h->signature0 != 'i' ||" << std::endl;
        file << "            h->signature1 != 'b' ||" << std::endl;
        file << "            h->version != 1)" << std::endl;
        file << "            return false;" << std::endl;
        file << std::endl;
        file << "        if (!m_buffer.empty())" << std::endl;
        file << "            m_buffer.clear();" << std::endl;
        file << "        m_from = reinterpret_cast<const uint8_t*>(ptr);" << std::endl;
        file << "        return true;" << std::endl;
        file << "    }" << std::endl;
        file << "    void* to()" << std::endl;
        file << "    {" << std::endl;
        file << "        if (m_from)" << std::endl;
        file << "            return nullptr;" << std::endl;
        file << "        else" << std::endl;
        file << "            return m_buffer.data();" << std::endl;
        file << "    }" << std::endl;
        file << "    std::unique_ptr<void*> to(const uint32_t compression)" << std::endl;
        file << "    {" << std::endl;
        file << "        switch (compression)" << std::endl;
        file << "        {" << std::endl;
        file << "        case INB_NO_COMPRESSION:" << std::endl;
        file << "            return nullptr;" << std::endl;
        file << "        case INB_TABLE_COMPRESSION:" << std::endl;
        file << "            return nullptr;" << std::endl;
        file << "        case INB_FULL_COMPRESSION:" << std::endl;
        file << "            return nullptr;" << std::endl;
        file << "        default:" << std::endl;
        file << "            break;" << std::endl;
        file << "        }" << std::endl;
        file << "        return nullptr;" << std::endl;
        file << "    }" << std::endl;
        file << "    uint32_t size() const" << std::endl;
        file << "    {" << std::endl;
        file << "        if (m_from)" << std::endl;
        file << "            return 0;" << std::endl;
        file << "        else" << std::endl;
        file << "            return static_cast<uint32_t>(m_buffer.size());" << std::endl;
        file << "    }" << std::endl;
        file << std::endl;
        file << "private:" << std::endl;
        file << "    #pragma pack(push, 1)" << std::endl;
        file << "    struct staticData" << std::endl;
        file << "    {" << std::endl;
        for (const auto &members : st.second)
        {
            const auto &stType = standard.find(members.second.type);
            if (stType == standard.end())
            {
                continue;
            }
            const std::string &type = stType->second;
            const std::string &name = members.first;
            const bool &isArray = members.second.isArray;
            const bool &isOptional = members.second.isOptional;
            file << "        " << type << " " << name << ";" << std::endl;
        }
        file << "    };" << std::endl;
        file << "    #pragma pack(pop)" << std::endl;
        file << std::endl;
        file << "    const uint8_t* m_from = nullptr;" << std::endl;
        file << "    std::vector<uint8_t> m_buffer;" << std::endl;
        file << "    uint32_t m_tableOffset = 0;" << std::endl;
        file << "};" << std::endl;
        file << std::endl;
    }
    for (uint32_t i = 0; i < m_namespaces.size(); i++)
        file << "}" << std::endl;
    //for (const auto &ns : m_namespaces)
    //{
    //    file << "namespace " << ns << std::endl;
    //    file << "{" << ns << std::endl;
    //}
    file << std::endl;
    file << "#endif // " << filedef << std::endl;
}
