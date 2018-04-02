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
    //file << "#include <cstring>" << std::endl;
    file << std::endl;
    for (const auto &ns : m_namespaces)
    {
        file << "namespace " << ns << std::endl;
        file << "{" << std::endl;
    }
    file << "namespace _inner_" << std::endl;
    file << "{" << std::endl;
    file << "    static void copybybytes(const void* src, void* dst, const uint32_t size)" << std::endl;
    file << "    {" << std::endl;
    file << "        for (uint32_t i = 0; i < size; ++i)" << std::endl;
    file << "            reinterpret_cast<uint8_t*>(dst)[i] = reinterpret_cast<const uint8_t*>(src)[i];" << std::endl;
    file << "    }" << std::endl;
    file << "    struct header" << std::endl;
    file << "    {" << std::endl;
    file << "        uint8_t signature0 = 'i';" << std::endl;
    file << "        uint8_t signature1 = 'b';" << std::endl;
    file << "        uint16_t type           ;" << std::endl;
    file << "    };" << std::endl;
    file << "    static uint32_t zero = 0;" << std::endl;
    file << "}" << std::endl;
    file << std::endl;
    // === =============================== ===
    // ==               enums               ==
    // === =============================== ===
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
        file << "static const char* " << en.first << "_str(const " << en.first << "& id)" << std::endl;
        file << "{" << std::endl;
        file << "    switch (id)" << std::endl;
        file << "    {" << std::endl;
        for (auto descr = en.second.begin(); descr != en.second.end(); ++descr)
        {
            file << "    case " << en.first << "::" << descr->first << ": return \"" << descr->first << "\";" << std::endl;
        }

        file << "    default: return \"_?unknown_" << en.first << "?_\";" << std::endl;
        file << "    }" << std::endl;
        file << "}" << std::endl;
        file << std::endl;
    }
    file << std::endl;
    // === =============================== ===
    // ==              classes              ==
    // === =============================== ===
    //for (const auto &st : m_structs)
    //{
    //    file << "class " << st.name << ";" << std::endl;
    //}
    //file << std::endl;
    for (const auto &st : m_structs)
    {
        const uint32_t optionalCount = st.optionalCount;
        const uint32_t staticCount = st.fields.size() - optionalCount;
        uint32_t builtInCount = 0;
        file << "class " << st.name << std::endl;
        file << "{" << std::endl;
        file << "public:" << std::endl;
        file << "    " << st.name << "(const uint32_t reserve = 0)" << std::endl;
        file << "    {" << std::endl;
        file << "        create(reserve);" << std::endl;
        file << "    }" << std::endl;
        file << "    " << st.name << "(uint8_t *from_)" << std::endl;
        file << "    {" << std::endl;
        file << "        this->from(from_);" << std::endl;
        file << "    }" << std::endl;
        file << std::endl;
        // === =============================== ===
        // ==             ids staff             ==
        // === =============================== ===
        file << "    enum class ids" << std::endl;
        file << "    {" << std::endl;
        for (const auto &member : st.fields)
        {
            if (member.isBuiltIn)
                builtInCount++;
            file << "        " << member.name << "," << std::endl;
        }
        file << "        _size_" << std::endl;
        file << "    };" << std::endl;
        file << "    static const char* name(const ids& id)" << std::endl;
        file << "    {" << std::endl;
        file << "        switch (id)" << std::endl;
        file << "        {" << std::endl;
        for (const auto &member : st.fields)
        {
            file << "        case ids::" << member.name << ": return \"" << member.name << "\"; " << std::endl;
        }
        file << "        default: return \"_?unknown_name?_\";" << std::endl;
        file << "        }" << std::endl;
        file << "    }" << std::endl;
        file << std::endl;
        file << "    enum class types" << std::endl;
        file << "    {" << std::endl;
        file << "        int8, uint8, int8a, uint8a," << std::endl;
        file << "        int16, uint16, int16a, uint16a," << std::endl;
        file << "        int32, uint32, int32a, uint32a," << std::endl;
        file << "        int64, uint64, int64a, uint64a," << std::endl;
        file << "        float32, float32a, float64, float64a," << std::endl;
        file << "        bytes";
        for (uint32_t i = 0; i < st.fields.size(); i++)
        {
            const auto &member = st.fields[i];
            if (member.isBuiltIn)
                continue;
            file << "," << std::endl << "        " << member.type << ", " << member.type << "a";
        }
        file << "," << std::endl << "        unknown" << std::endl;
        file << "    };" << std::endl;
        file << std::endl;
        // === =============================== ===
        // ==       main has,get,set,size       ==
        // === =============================== ===
        file << "    uint32_t has(const ids& id)" << std::endl;
        file << "    {" << std::endl;
        file << "        return address(id, getTable());" << std::endl;
        file << "    }" << std::endl;
        file << "    uint8_t* get(const ids& id)" << std::endl;
        file << "    {" << std::endl;
        file << "        Table* table = getTable();" << std::endl;
        file << "        uint8_t* ptr = to();" << std::endl;
        file << "        switch (id)" << std::endl;
        file << "        {" << std::endl;
        for (uint32_t i = 0; i < st.fields.size(); i++)
        {
            const auto &member = st.fields[i];
            //if (standard.find(member.second.type) == standard.end())
            //    continue;
            if (i < staticCount)
            {
                file << "        case ids::" << member.name << ":" << std::endl;
                file << "            return reinterpret_cast<uint8_t*>(&table->" << member.name << ");" << std::endl;
            }
            else
            {
                file << "        case ids::" << member.name << ":" << std::endl;
            }
        }
        if (optionalCount)
        {
            file << "            return &ptr[address(id, table) + sizeof(uint32_t)];" << std::endl;
        }
        file << "        default:" << std::endl;
        file << "            return nullptr;" << std::endl;
        file << "        }" << std::endl;
        file << "    }" << std::endl;
        file << "    void set(const ids& id, const void* data, const uint32_t size)" << std::endl;
        file << "    {" << std::endl;
        file << "        switch (id)" << std::endl;
        file << "        {" << std::endl;
        for (uint32_t i = 0; i < st.fields.size(); i++)
        {
            const auto &member = st.fields[i];
            //if (standard.find(member.second.type) == standard.end())
            //    continue;
            if (i < staticCount)
            {
                file << "        case ids::" << member.name << ":" << std::endl;
                file << "            if (data)" << std::endl;
                file << "                _inner_::copybybytes(data, &getTable()->" << member.name << ", size);" << std::endl;
                file << "            break;" << std::endl;
            }
            else
            {
                break;
            }
        }
        file << "        default:" << std::endl;
        if (optionalCount)
        {
            file << "        {" << std::endl;
            file << "            uint32_t offset = has(id);" << std::endl;
            file << "            if (offset == 0)" << std::endl;
            file << "            {" << std::endl;
            file << "                offset = insert(size + sizeof(uint32_t));" << std::endl;
            file << "                if (!offset)" << std::endl;
            file << "                    return;" << std::endl;
            file << "                *reinterpret_cast<uint32_t*>(&m_buffer->data()[offset]) = size;" << std::endl;
            file << "                address(id, getTable()) = offset;" << std::endl;
            file << "            }" << std::endl;
            file << "            if (data)" << std::endl;
            file << "                _inner_::copybybytes(data, &m_buffer->data()[offset + sizeof(uint32_t)], size);" << std::endl;
            file << "            break;" << std::endl;
            file << "        }" << std::endl;
        }
        else
        {
            file << "            break;" << std::endl;
        }
        file << "        }" << std::endl;
        file << "    }" << std::endl;
        file << "    static types type(const ids& id)" << std::endl;
        file << "    {" << std::endl;
        file << "        switch (id)" << std::endl;
        file << "        {" << std::endl;
        for (uint32_t i = 0; i < st.fields.size(); i++)
        {
            const auto &member = st.fields[i];
            file << "        case ids::" << member.name << ": return types::" << member.type;
            if (member.isArray && member.type != tokenBytes)
                file << "a";
            file << ";" << std::endl;
        }
        file << "        default: return types::unknown;" << std::endl;
        file << "        }" << std::endl;
        file << "    }" << std::endl;
        file << "    uint32_t size(const ids& id)" << std::endl;
        file << "    {" << std::endl;
        file << "        uint8_t* ptr = m_from ? m_from : &m_buffer->data()[0];" << std::endl;
        file << "        Table *table = reinterpret_cast<Table*>(&ptr[sizeof(_inner_::header)]);" << std::endl;
        file << "        const uint32_t offset = address(id, table);" << std::endl;
        file << "        return *reinterpret_cast<uint32_t*>(&ptr[offset]);" << std::endl;
        file << "    }" << std::endl;
        file << std::endl;
        // === =============================== ===
        // ==           fields staff            ==
        // === =============================== ===
        for (const auto &member : st.fields)
        {
            const std::string *type;
            const auto &stType = standard.find(member.type);
            if (stType == standard.end())
                type = &member.type;
            else
                type = &stType->second;
            const std::string &name = member.name;
            const bool &isArray = member.isArray;
            const bool &isOptional = member.isOptional;
            const bool &isBuiltIn = member.isBuiltIn;
            // === =============================== ===
            // ==                has                ==
            // === =============================== ===
            file << "    bool has_" << name << "()" << std::endl;
            file << "    {" << std::endl;
            if (isOptional)
            {
                file << "        return has(ids::" << name << ") != 0;" << std::endl;
            }
            else
            {
                file << "        return true;" << std::endl;
            }
            file << "    }" << std::endl;
            // === =============================== ===
            // ==               size                ==
            // === =============================== ===
            file << "    uint32_t size_" << name << "()" << std::endl;
            file << "    {" << std::endl;
            if (isBuiltIn)
            {
                if (isOptional)
                {
                    if (*type == tokenBytes)
                    {
                        file << "        return size(ids::" << name << ");" << std::endl;
                    }
                    else if (isArray)
                    {
                        file << "        return size(ids::" << name << ") / sizeof(" << *type << ");" << std::endl;
                    }
                    else
                    {
                        file << "        return size(ids::" << name << ");" << std::endl;
                    }
                }
                else
                {
                    file << "        return sizeof(" << *type << ");" << std::endl;
                }
            }
            else
            {
                if (isArray)
                {
                    file << "        return size(ids::" << name << ") / sizeof(" << *type << "::Table);" << std::endl;
                }
                else
                {
                    file << "        return size(ids::" << name << ") / sizeof(" << *type << "::Table);" << std::endl;
                }
            }
            file << "    }" << std::endl;
            // === =============================== ===
            // ==                get                ==
            // === =============================== ===
            if (isBuiltIn)
            {
                if (isOptional)
                {
                    if (*type == tokenBytes)
                    {
                        file << "    const uint8_t* get_" << name << "()" << std::endl;
                        file << "    {" << std::endl;
                        file << "        return reinterpret_cast<const uint8_t*>(get(ids::" << name << "));" << std::endl;
                    }
                    else if (isArray)
                    {
                        file << "    const " << *type << "* get_" << name << "()" << std::endl;
                        file << "    {" << std::endl;
                        file << "        return reinterpret_cast<" << *type << "*>(get(ids::" << name << "));" << std::endl;
                        file << "    }" << std::endl;
                        file << "    const " << *type << "& get_" << name << "(const uint32_t index)" << std::endl;
                        file << "    {" << std::endl;
                        file << "        const " << *type << "* ptr = reinterpret_cast<" << *type << "*>(get(ids::" << name << "));" << std::endl;
                        file << "        if (!ptr)" << std::endl;
                        file << "            throw std::logic_error(\"Nullptr\");" << std::endl;
                        file << "        return ptr[index];" << std::endl;
                    }
                    else
                    {
                        file << "    " << *type << " get_" << name << "()" << std::endl;
                        file << "    {" << std::endl;
                        file << "        " << *type << " dst;" << std::endl;
                        file << "        _inner_::copybybytes(get(ids::" << name << "), &dst, sizeof(" << *type << "));" << std::endl;
                        file << "        return dst;" << std::endl;
                    }
                }
                else
                {
                    file << "    " << *type << " get_" << name << "()" << std::endl;
                    file << "    {" << std::endl;
                    file << "        " << *type << " dst;" << std::endl;
                    file << "        _inner_::copybybytes(get(ids::" << name << "), &dst, sizeof(" << *type << "));" << std::endl;
                    file << "        return dst;" << std::endl;
                }
            }
            else
            {
                if (isArray)
                {
                    file << "    " << *type << "& get_" << name << "(const uint32_t index)" << std::endl;
                    file << "    {" << std::endl;
                    file << "        if (index >= custom." << name << ".size())" << std::endl;
                    file << "            throw std::logic_error(\"Nullref\");" << std::endl;
                    file << "        return custom." << name << "[index];" << std::endl;
                }
                else
                {
                    file << "    " << *type << "& get_" << name << "()" << std::endl;
                    file << "    {" << std::endl;
                    file << "        if (custom." << name << ".empty())" << std::endl;
                    file << "            throw std::logic_error(\"Nullref\");" << std::endl;
                    file << "        return custom." << name << "[0];" << std::endl;
                }
            }
            file << "    }" << std::endl;
            // === =============================== ===
            // ==                set                ==
            // === =============================== ===
            if (isBuiltIn)
            {
                if (isOptional)
                {
                    if (*type == tokenBytes)
                    {
                        file << "    void set_" << name << "(const void* data, const uint32_t numberOfBytes)" << std::endl;
                        file << "    {" << std::endl;
                        file << "        set(ids::" << name << ", data, numberOfBytes);" << std::endl;
                    }
                    else if (isArray)
                    {
                        file << "    void set_" << name << "(const " << *type << "* data, const uint32_t numberOfElements)" << std::endl;
                        file << "    {" << std::endl;
                        file << "        set(ids::" << name << ", data, numberOfElements * sizeof(" << *type << "));" << std::endl;
                        file << "    }" << std::endl;
                        file << "    void set_" << name << "(const uint32_t index, const " << *type << "& element)" << std::endl;
                        file << "    {" << std::endl;
                        file << "        " << *type << "* ptr = reinterpret_cast<" << *type << "*>(get(ids::" << name << "));" << std::endl;
                        file << "        if (!ptr)" << std::endl;
                        file << "            throw std::logic_error(\"Nullptr\");" << std::endl;
                        file << "        _inner_::copybybytes(&element, &ptr[index], sizeof(" << *type << "));" << std::endl;
                    }
                    else
                    {
                        file << "    void set_" << name << "(const " << *type << "& " << name << ")" << std::endl;
                        file << "    {" << std::endl;
                        file << "        set(ids::" << name << ", &" << name << ", sizeof(" << *type << "));" << std::endl;
                    }
                }
                else
                {
                    file << "    void set_" << name << "(const " << *type << "& " << name << ")" << std::endl;
                    file << "    {" << std::endl;
                    file << "        _inner_::copybybytes(&" << name << ", get(ids::" << name << "), sizeof(" << *type << "));" << std::endl;
                }
            }
            else
            {
                if (isArray)
                {
                    file << "    void set_" << name << "(const uint32_t numberOfElements)" << std::endl;
                    file << "    {" << std::endl;
                    file << "        if (!custom." << name << ".empty())" << std::endl;
                    file << "            throw std::logic_error(\"Already setted\");" << std::endl;
                    file << "        set(ids::" << name << ", nullptr, numberOfElements * sizeof(" << *type << "::Table));" << std::endl;
                    file << "        custom." << name << ".resize(numberOfElements);" << std::endl;
                    file << "        Table *table = getTable();" << std::endl;
                    file << "        for (uint32_t i = 0; i < numberOfElements; ++i)" << std::endl;
                    file << "        {" << std::endl;
                    file << "            custom." << name << "[i].m_table = address(ids::" << name << ", table) + sizeof(" << *type << "::Table) * i + sizeof(uint32_t);" << std::endl;
                    file << "            custom." << name << "[i].m_buffer = m_buffer;" << std::endl;
                    file << "        }" << std::endl;
                }
                else
                {
                    file << "    void set_" << name << "()" << std::endl;
                    file << "    {" << std::endl;
                    file << "        if (!custom." << name << ".empty())" << std::endl;
                    file << "            throw std::logic_error(\"Already setted\");" << std::endl;
                    file << "        set(ids::" << name << ", nullptr, sizeof(" << *type << "::Table));" << std::endl;
                    file << "        custom." << name << ".resize(1);" << std::endl;
                    file << "        Table *table = getTable();" << std::endl;
                    file << "        custom." << name << "[0].m_table = address(ids::" << name << ", table) + sizeof(uint32_t);" << std::endl;
                    file << "        custom." << name << "[0].m_buffer = m_buffer;" << std::endl;
                }
            }
            file << "    }" << std::endl;
            file << std::endl;
        }
        // === =============================== ===
        // ==           packet's staff          ==
        // === =============================== ===
        file << "    void create(const uint32_t reserve = 0)" << std::endl;
        file << "    {" << std::endl;
        file << "        m_from = nullptr;" << std::endl;
        file << "        m_buffer = std::make_shared<std::vector<uint8_t>>();" << std::endl;
        file << "        m_buffer->reserve(reserve);" << std::endl;
        file << "        m_buffer->resize(sizeof(_inner_::header) + sizeof(Table));" << std::endl;
        file << "        _inner_::header *h = reinterpret_cast<_inner_::header*>(&m_buffer->data()[0]);" << std::endl;
        file << "        h->signature0  = 'i';" << std::endl;
        file << "        h->signature1  = 'b';" << std::endl;
        file << "        m_table = sizeof(_inner_::header);" << std::endl;
        file << "    }" << std::endl;
        file << "    bool from(uint8_t* ptr)" << std::endl;
        file << "    {" << std::endl;
        file << "        if (!ptr)" << std::endl;
        file << "            return false;" << std::endl;
        file << "        const _inner_::header* h = reinterpret_cast<const _inner_::header*>(ptr);" << std::endl;
        file << "        if (h->signature0 != 'i' ||" << std::endl;
        file << "            h->signature1 != 'b')" << std::endl;
        file << "            return false;" << std::endl;
        file << "        if (!m_buffer->empty())" << std::endl;
        file << "            m_buffer->clear();" << std::endl;
        file << "        m_from = ptr;" << std::endl;
        for (const auto &member : st.fields)
        {
            const bool &isBuiltIn = member.isBuiltIn;
            if (isBuiltIn)
                continue;
            const std::string &name = member.name;
            const bool &isOptional = member.isOptional;
            const std::string &type = member.type;

            file << "        if (getTable()->__" << name << ")" << std::endl;
            file << "        {" << std::endl;
            file << "            const uint32_t number = size(ids::" << name << ") / sizeof(" << type << "::Table);" << std::endl;
            file << "            custom." << name << ".reserve(number);" << std::endl;
            file << "            for (uint32_t i = 0; i < number; ++i)" << std::endl;
            file << "            {" << std::endl;
            file << "                custom." << name << ".emplace_back();" << std::endl;
            file << "                custom." << name << ".back().from(ptr);" << std::endl;
            file << "                auto table = reinterpret_cast<" << type << "::Table*>(ptr + getTable()->__" << name << " + sizeof(uint32_t) + sizeof(" << type << "::Table) * i);" << std::endl;
            file << "                custom." << name << ".back().m_table = reinterpret_cast<uint8_t*>(table) - ptr;" << std::endl;
            file << "            }" << std::endl;
            file << "        }" << std::endl;
        }
        file << "        return true;" << std::endl;
        file << "    }" << std::endl;
        file << "    uint8_t* to()" << std::endl;
        file << "    {" << std::endl;
        file << "        if (m_from)" << std::endl;
        file << "            return m_from;" << std::endl;
        file << "        else" << std::endl;
        file << "            return m_buffer->data();" << std::endl;
        file << "    }" << std::endl;
        file << "    uint32_t size() const" << std::endl;
        file << "    {" << std::endl;
        file << "        if (m_from)" << std::endl;
        file << "            return 0;" << std::endl;
        file << "        else" << std::endl;
        file << "            return static_cast<uint32_t>(m_buffer->size());" << std::endl;
        file << "    }" << std::endl;
        file << std::endl;
        //file << "private:" << std::endl;
        file << "    #pragma pack(push, 1)" << std::endl;
        file << "    struct Table" << std::endl;
        file << "    {" << std::endl;
        for (const auto &member : st.fields)
        {
            const std::string &name = member.name;
            const bool &isOptional = member.isOptional;
            const std::string *type;
            const auto &stType = standard.find(member.type);
            if (stType == standard.end())
                type = &member.type;
            else
                type = &stType->second;
            if (isOptional)
                file << "        uint32_t __" << name << " = 0;" << std::endl;
            else
                file << "        " << *type << " " << name << ";" << std::endl;
        }
        file << "    };" << std::endl;
        file << "    #pragma pack(pop)" << std::endl;
        file << std::endl;
        file << "    Table* getTable()" << std::endl;
        file << "    {" << std::endl;
        file << "        return reinterpret_cast<Table*>(&to()[m_table]);" << std::endl;
        file << "    }" << std::endl;
        file << "    uint32_t insert(const uint32_t size)" << std::endl;
        file << "    {" << std::endl;
        file << "        if (m_from)" << std::endl;
        file << "            return 0;" << std::endl;
        file << "        else" << std::endl;
        file << "        {" << std::endl;
        file << "            const uint32_t offset = static_cast<uint32_t>(m_buffer->size());" << std::endl;
        file << "            m_buffer->resize(m_buffer->size() + size);" << std::endl;
        file << "            return offset;" << std::endl;
        file << "        }" << std::endl;
        file << "    }" << std::endl;
        file << "    uint32_t& address(const ids& id, Table* table)" << std::endl;
        file << "    {" << std::endl;
        file << "        switch (id)" << std::endl;
        file << "        {" << std::endl;
        for (const auto &member : st.fields)
        {
            const std::string &name = member.name;
            const bool &isOptional = member.isOptional;
            //const std::string *type;
            //const auto &stType = standard.find(member.second.type);
            //if (stType == standard.end())
            //    type = &member.second.type;
            //else
            //    type = &stType->second;
            if (!isOptional)
                continue;
            file << "        case ids::" << name << ": return table->__" << name << ";" << std::endl;
        }
        file << "        default: return _inner_::zero;" << std::endl;
        file << "        }" << std::endl;
        file << "    }" << std::endl;
        file << std::endl;
        if (st.fields.size() - builtInCount)
        {
            file << "    struct" << std::endl;
            file << "    {" << std::endl;
            for (const auto &member : st.fields)
            {
                const bool &isBuiltIn = member.isBuiltIn;
                if (isBuiltIn)
                    continue;
                const std::string &name = member.name;
                const std::string &type = member.type;
                file << "        std::vector<" << type << "> " << name << ";" << std::endl;
            }
            file << "    } custom;" << std::endl;
            file << std::endl;
        }
        file << "    uint8_t* m_from = nullptr;" << std::endl;
        file << "    std::shared_ptr<std::vector<uint8_t>> m_buffer;" << std::endl;
        file << "    uint32_t m_table = 0;" << std::endl;
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
