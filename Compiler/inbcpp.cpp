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
    { "float64" , "double"   },
    { "bytes"   , "bytes"  }
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
        const uint32_t optionalCount = st.second.second;
        const uint32_t staticCount = st.second.first.size() - optionalCount;
        file << "class " << st.first << std::endl;
        file << "{" << std::endl;
        file << "public:" << std::endl;
        file << "    void create(const uint32_t reserve = 0)" << std::endl;
        file << "    {" << std::endl;
        file << "        m_from = nullptr;" << std::endl;
        file << "        m_buffer.reserve(reserve);" << std::endl;
        file << "        m_buffer.resize(sizeof(_inner_::header) + sizeof(m_tableSize) * sizeof(uint32_t)";
        if (staticCount)
            file << " + sizeof(staticData));" << std::endl;
        else
            file << ");" << std::endl;
        file << "        _inner_::header *h = reinterpret_cast<_inner_::header*>(&m_buffer[0]);" << std::endl;
        file << "        h->signature0  = 'i';" << std::endl;
        file << "        h->signature1  = 'b';" << std::endl;
        file << "        h->version     = 1;" << std::endl;
        file << "        h->compression = INB_NO_COMPRESSION;" << std::endl;
        file << "        h->tableSize   = m_tableSize;" << std::endl;
        file << "        if (m_tableSize)" << std::endl;
        file << "        {" << std::endl;
        file << "           uint32_t* table = reinterpret_cast<uint32_t*>(" << std::endl;
        file << "               &m_buffer[0] + sizeof(_inner_::header)";
        if (staticCount)
            file << "+sizeof(staticData));" << std::endl;
        else
            file << ");" << std::endl;
        file << "           for (uint32_t i = 0; i < m_tableSize; i++)" << std::endl;
        file << "               table[i] = 0;" << std::endl;
        file << "        }" << std::endl;
        file << "    }" << std::endl;
        file << std::endl;
        if (optionalCount)
        {
            file << "    enum class ids" << std::endl;
            file << "    {" << std::endl;
            for (const auto &member : st.second.first)
            {
                if (!member.second.isOptional)
                    continue;
                file << "        " << member.first << "," << std::endl;
            }
            file << "        _size_" << std::endl;
            file << "    };" << std::endl;
            file << "    const char* getIdString(const ids& id) const" << std::endl;
            file << "    {" << std::endl;
            file << "        switch (id)" << std::endl;
            file << "        {" << std::endl;
            for (const auto &member : st.second.first)
            {
                if (!member.second.isOptional)
                    continue;
                file << "        case ids::" << member.first << ": return \"" << member.first << "\"; " << std::endl;
            }
            file << "        default: return \"_?unknown_id?_\";" << std::endl;
            file << "        }" << std::endl;
            file << "    }" << std::endl;
            file << "    void* getDataById(const ids& id)" << std::endl;
            file << "    {" << std::endl;
            file << "        uint8_t* ptr = m_from ? m_from : m_buffer.data();" << std::endl;
            file << "        const uint32_t* table = reinterpret_cast<const uint32_t*>(" << std::endl;
            file << "                    &ptr[0] + sizeof(_inner_::header)";
            if (staticCount)
                file << " + sizeof(staticData)";
            file << ");" << std::endl;
            file << "        const uint32_t offset = table[static_cast<uint32_t>(id)];" << std::endl;
            file << "        if (offset == 0)" << std::endl;
            file << "            throw std::logic_error(\"There is no field '\"" << std::endl;
            file << "                + std::string(getIdString(id)) + \"' in the packet\");" << std::endl;
            file << "        return reinterpret_cast<void*>(&ptr[0] + offset + sizeof(uint32_t));" << std::endl;
            file << "    }" << std::endl;
            file << "    uint32_t getSizeById(const ids& id) const" << std::endl;
            file << "    {" << std::endl;
            file << "        const uint8_t* ptr = m_from ? m_from : m_buffer.data();" << std::endl;
            file << "        const uint32_t* table = reinterpret_cast<const uint32_t*>(" << std::endl;
            file << "                    &ptr[0] + sizeof(_inner_::header)";
            if (staticCount)
                file << " + sizeof(staticData)";
            file << ");" << std::endl;
            file << "        const uint32_t offset = table[static_cast<uint32_t>(id)];" << std::endl;
            file << "        if (offset == 0)" << std::endl;
            file << "            return 0;" << std::endl;
            file << "        else" << std::endl;
            file << "            return *reinterpret_cast<const uint32_t*>(&ptr[0] + offset);" << std::endl;
            file << "    }" << std::endl;
            file << "    void addDataById(const ids& id, const uint32_t size)" << std::endl;
            file << "    {" << std::endl;
            file << "        if (m_from)" << std::endl;
            file << "            throw std::logic_error(\"We cannot edit received packets\");" << std::endl;
            file << "        uint32_t* table = reinterpret_cast<uint32_t*>(" << std::endl;
            file << "            &m_buffer[0] + sizeof(_inner_::header)";
            if (staticCount)
                file << " + sizeof(staticData)";
            file << ");" << std::endl;
            file << "        const uint32_t offset = table[static_cast<uint32_t>(id)];" << std::endl;
            file << "        if (offset != 0)" << std::endl;
            file << "            return;" << std::endl;
            file << "        const uint32_t newOffset = static_cast<uint32_t>(m_buffer.size());" << std::endl;
            file << "        m_buffer.resize(m_buffer.size() + sizeof(uint32_t) + size);" << std::endl;
            file << "        table = reinterpret_cast<uint32_t*>(" << std::endl;
            file << "            &m_buffer[0] + sizeof(_inner_::header)";
            if (staticCount)
                file << " + sizeof(staticData)";
            file << ");" << std::endl;
            file << "        table[static_cast<uint32_t>(id)] = newOffset;" << std::endl;
            file << "        *reinterpret_cast<uint32_t*>(&m_buffer[0] + newOffset) = size;" << std::endl;
            file << "    }" << std::endl;
            file << std::endl;
        }
        for (const auto &member : st.second.first)
        {
            const auto &stType = standard.find(member.second.type);
            if (stType == standard.end())
            {
                std::cout << clr::yellow << "Warning! Unknown type: " << member.second.type << clr::default_ << std::endl;
                continue;
            }
            const std::string &type = stType->second;
            const std::string &name = member.first;
            const bool &isArray = member.second.isArray;
            const bool &isOptional = member.second.isOptional;

            file << "    uint32_t size_" << name << "() const" << std::endl;
            file << "    {" << std::endl;
            if (isOptional)
            {
                if (type == tokenBytes)
                    file << "        return getSizeById(ids::" << name << ");" << std::endl;
                else
                    file << "        return getSizeById(ids::" << name << ") / sizeof(" << type << ");" << std::endl;
            }
            else
            {
                file << "        return sizeof(" << type << ");" << std::endl;
            }
            file << "    }" << std::endl;
            file << "    bool has_" << name << "() const" << std::endl;
            file << "    {" << std::endl;
            if (isOptional)
            {
                file << "        return getSizeById(ids::" << name << ") != 0;" << std::endl;
            }
            else
            {
                file << "        return true;" << std::endl;
            }
            file << "    }" << std::endl;
            if (isOptional)
            {
                if (isArray)
                {
                    file << "    void add_" << name << "(const uint32_t size)" << std::endl;
                    file << "    {" << std::endl;
                    if (type == tokenBytes)
                        file << "        addDataById(ids::" << name << ", size);" << std::endl;
                    else
                        file << "        addDataById(ids::" << name << ", size * sizeof(" << type << "));" << std::endl;
                }
                else
                {
                    file << "    void add_" << name << "()" << std::endl;
                    file << "    {" << std::endl;
                    file << "        addDataById(ids::" << name << ", sizeof(" << type << "));" << std::endl;
                }
            }
            else
            {
                file << "    void add_" << name << "()" << std::endl;
                file << "    {" << std::endl;
            }
            file << "    }" << std::endl;
            if (isOptional)
            {
                if (isArray)
                {
                    if (type == tokenBytes)
                    {
                        file << "    const void* get_" << name << "()" << std::endl;
                        file << "    {" << std::endl;
                        file << "        return getDataById(ids::" << name << ");" << std::endl;
                    }
                    else
                    {
                        file << "    const " << type << "* get_" << name << "()" << std::endl;
                        file << "    {" << std::endl;
                        file << "        return reinterpret_cast<" << type << "*>(getDataById(ids::" << name << "));" << std::endl;
                        file << "    }" << std::endl;
                        file << "    const " << type << "& get_" << name << "(const uint32_t index)" << std::endl;
                        file << "    {" << std::endl;
                        file << "        const " << type << "* ptr = reinterpret_cast<" << type << "*>(getDataById(ids::" << name << "));" << std::endl;
                        file << "        if (!ptr)" << std::endl;
                        file << "            throw std::logic_error(\"Nullptr\");" << std::endl;
                        file << "        return ptr[index];" << std::endl;
                    }
                }
                else
                {
                    file << "    const " << type << "& get_" << name << "()" << std::endl;
                    file << "    {" << std::endl;
                    file << "        return *reinterpret_cast<" << type << "*>(getDataById(ids::" << name << "));" << std::endl;
                }
            }
            else
            {
                file << "    const " << type << "& get_" << name << "()" << std::endl;
                file << "    {" << std::endl;
                file << "        const staticData* data = m_from" << std::endl;
                file << "            ? reinterpret_cast<const staticData*>(&m_from[sizeof(_inner_::header)])" << std::endl;
                file << "            : reinterpret_cast<const staticData*>(&m_buffer[sizeof(_inner_::header)]);" << std::endl;
                file << "        return data->" << name << ";" << std::endl;
            }
            file << "}" << std::endl;
            if (isOptional)
            {
                if (isArray)
                {
                    if (type == tokenBytes)
                    {
                        file << "    void set_" << name << "(const void* " << name << ")" << std::endl;
                        file << "    {" << std::endl;
                        file << "        const uint32_t size = getSizeById(ids::" << name << ");" << std::endl;
                        file << "        if (size == 0)" << std::endl;
                        file << "            throw std::logic_error(\"Zero size\");" << std::endl;
                        file << "        const uint8_t* src = reinterpret_cast<const uint8_t*>(" << name << ");" << std::endl;
                        file << "        uint8_t* dst = reinterpret_cast<uint8_t*>(getDataById(ids::" << name << "));" << std::endl;
                        file << "        for (uint32_t i = 0; i < size; i++)" << std::endl;
                        file << "            dst[i] = src[i];" << std::endl;
                    }
                    else
                    {
                        file << "    void set_" << name << "(const " << type << "* " << name << ")" << std::endl;
                        file << "    {" << std::endl;
                        file << "        const uint32_t size = getSizeById(ids::" << name << ");" << std::endl;
                        file << "        const uint8_t* src = reinterpret_cast<const uint8_t*>(" << name << ");" << std::endl;
                        file << "        uint8_t* dst = reinterpret_cast<uint8_t*>(getDataById(ids::" << name << "));" << std::endl;
                        file << "        for (uint32_t i = 0; i < size; i++)" << std::endl;
                        file << "            dst[i] = src[i];" << std::endl;
                        file << "    }" << std::endl;
                        file << "    void set_" << name << "(const uint32_t index, const " << type << "& element)" << std::endl;
                        file << "    {" << std::endl;
                        file << "        " << type << "* ptr = reinterpret_cast<" << type << "*>(getDataById(ids::" << name << "));" << std::endl;
                        file << "        if (!ptr)" << std::endl;
                        file << "            throw std::logic_error(\"Nullptr\");" << std::endl;
                        file << "        ptr[index] = element;" << std::endl;
                    }
                }
                else
                {
                    file << "    void set_" << name << "(const " << type << "& " << name << ")" << std::endl;
                    file << "    {" << std::endl;
                    file << "        *reinterpret_cast<" << type << "*>(getDataById(ids::" << name << ")) = " << name << ";" << std::endl;
                }
            }
            else
            {
                file << "    void set_" << name << "(const " << type << "& " << name << ")" << std::endl;
                file << "    {" << std::endl;
                file << "        staticData* data = reinterpret_cast<staticData*>(&m_buffer[sizeof(_inner_::header)]);" << std::endl;
                file << "        data->" << name << " = " << name << ";" << std::endl;
            }
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
        file << "        if (!m_buffer.empty())" << std::endl;
        file << "            m_buffer.clear();" << std::endl;
        file << "        m_from = reinterpret_cast<uint8_t*>(const_cast<void*>(ptr));" << std::endl;
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
        if (staticCount)
        {
            file << "    #pragma pack(push, 1)" << std::endl;
            file << "    struct staticData" << std::endl;
            file << "    {" << std::endl;
            for (const auto &member : st.second.first)
            {
                const auto &stType = standard.find(member.second.type);
                if (stType == standard.end())
                    continue;
                //const bool &isArray = member.second.isArray;
                const bool &isOptional = member.second.isOptional;
                if (isOptional)
                    continue;
                const std::string &type = stType->second;
                const std::string &name = member.first;
                file << "        " << type << " " << name << ";" << std::endl;
            }
            file << "    };" << std::endl;
            file << "    #pragma pack(pop)" << std::endl;
            file << std::endl;
        }
        file << "    uint8_t* m_from = nullptr;" << std::endl;
        file << "    std::vector<uint8_t> m_buffer;" << std::endl;
        file << "    const uint16_t m_tableSize = " << optionalCount << ";" << std::endl;
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
