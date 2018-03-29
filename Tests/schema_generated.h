#ifndef SCHEMA_GENERATED_H
#define SCHEMA_GENERATED_H

#include <vector>
#include <memory>

#ifndef INB_NO_COMPRESSION
#   define INB_NO_COMPRESSION    0
#endif
#ifndef INB_TABLE_COMPRESSION
#   define INB_TABLE_COMPRESSION 1
#endif
#ifndef INB_FULL_COMPRESSION
#   define INB_FULL_COMPRESSION  2
#endif

namespace ExtNS
{
namespace IntNS
{
namespace _inner_
{
    struct header
    {
        uint8_t signature0 = 'i';
        uint8_t signature1 = 'b';
        uint16_t version    :  3;
        uint16_t compression:  2;
        uint16_t tableSize  : 11;
    };
}

enum class Color
{
    RED = 0,
    GREEN = 3,
    BLUE = 4
};

class ArrayTest;
class Packet;
class Vec2f;
class Vec3i;

class ArrayTest
{
public:
    void create(const uint32_t reserve = 0)
    {
        m_from = nullptr;
        m_buffer.reserve(reserve);
        m_buffer.resize(sizeof(_inner_::header) + sizeof(m_tableSize) * sizeof(uint32_t) + sizeof(staticData));
        _inner_::header *h = reinterpret_cast<_inner_::header*>(&m_buffer[0]);
        h->signature0  = 'i';
        h->signature1  = 'b';
        h->version     = 1;
        h->compression = INB_NO_COMPRESSION;
        h->tableSize   = m_tableSize;
        if (m_tableSize)
        {
           uint32_t* table = reinterpret_cast<uint32_t*>(
               &m_buffer[0] + sizeof(_inner_::header)+sizeof(staticData));
           for (uint32_t i = 0; i < m_tableSize; i++)
               table[i] = 0;
        }
    }

    enum class ids
    {
        raw,
        arr,
        _size_
    };
    const char* getIdString(const ids& id) const
    {
        switch (id)
        {
        case ids::raw: return "raw"; 
        case ids::arr: return "arr"; 
        default: return "_?unknown_id?_";
        }
    }
    void* getDataById(const ids& id)
    {
        uint8_t* ptr = m_from ? m_from : m_buffer.data();
        const uint32_t* table = reinterpret_cast<const uint32_t*>(
                    &ptr[0] + sizeof(_inner_::header) + sizeof(staticData));
        const uint32_t offset = table[static_cast<uint32_t>(id)];
        if (offset == 0)
            throw std::logic_error("There is no field '"
                + std::string(getIdString(id)) + "' in the packet");
        return reinterpret_cast<void*>(&ptr[0] + offset + sizeof(uint32_t));
    }
    uint32_t getSizeById(const ids& id) const
    {
        const uint8_t* ptr = m_from ? m_from : m_buffer.data();
        const uint32_t* table = reinterpret_cast<const uint32_t*>(
                    &ptr[0] + sizeof(_inner_::header) + sizeof(staticData));
        const uint32_t offset = table[static_cast<uint32_t>(id)];
        if (offset == 0)
            return 0;
        else
            return *reinterpret_cast<const uint32_t*>(&ptr[0] + offset);
    }
    void addDataById(const ids& id, const uint32_t size)
    {
        if (m_from)
            throw std::logic_error("We cannot edit received packets");
        uint32_t* table = reinterpret_cast<uint32_t*>(
            &m_buffer[0] + sizeof(_inner_::header) + sizeof(staticData));
        const uint32_t offset = table[static_cast<uint32_t>(id)];
        if (offset != 0)
            return;
        const uint32_t newOffset = static_cast<uint32_t>(m_buffer.size());
        m_buffer.resize(m_buffer.size() + sizeof(uint32_t) + size);
        table = reinterpret_cast<uint32_t*>(
            &m_buffer[0] + sizeof(_inner_::header) + sizeof(staticData));
        table[static_cast<uint32_t>(id)] = newOffset;
        *reinterpret_cast<uint32_t*>(&m_buffer[0] + newOffset) = size;
    }

    uint32_t size_raw() const
    {
        return getSizeById(ids::raw);
    }
    bool has_raw() const
    {
        return getSizeById(ids::raw) != 0;
    }
    void add_raw(const uint32_t size)
    {
        addDataById(ids::raw, size);
    }
    const void* get_raw()
    {
        return getDataById(ids::raw);
}
    void set_raw(const void* raw)
    {
        const uint32_t size = getSizeById(ids::raw);
        if (size == 0)
            throw std::logic_error("Zero size");
        const uint8_t* src = reinterpret_cast<const uint8_t*>(raw);
        uint8_t* dst = reinterpret_cast<uint8_t*>(getDataById(ids::raw));
        for (uint32_t i = 0; i < size; i++)
            dst[i] = src[i];
    }

    uint32_t size_arr() const
    {
        return getSizeById(ids::arr) / sizeof(uint32_t);
    }
    bool has_arr() const
    {
        return getSizeById(ids::arr) != 0;
    }
    void add_arr(const uint32_t size)
    {
        addDataById(ids::arr, size * sizeof(uint32_t));
    }
    const uint32_t* get_arr()
    {
        return reinterpret_cast<uint32_t*>(getDataById(ids::arr));
    }
    const uint32_t& get_arr(const uint32_t index)
    {
        const uint32_t* ptr = reinterpret_cast<uint32_t*>(getDataById(ids::arr));
        if (!ptr)
            throw std::logic_error("Nullptr");
        return ptr[index];
}
    void set_arr(const uint32_t* arr)
    {
        const uint32_t size = getSizeById(ids::arr);
        const uint8_t* src = reinterpret_cast<const uint8_t*>(arr);
        uint8_t* dst = reinterpret_cast<uint8_t*>(getDataById(ids::arr));
        for (uint32_t i = 0; i < size; i++)
            dst[i] = src[i];
    }
    void set_arr(const uint32_t index, const uint32_t& element)
    {
        uint32_t* ptr = reinterpret_cast<uint32_t*>(getDataById(ids::arr));
        if (!ptr)
            throw std::logic_error("Nullptr");
        ptr[index] = element;
    }

    uint32_t size_var() const
    {
        return sizeof(uint16_t);
    }
    bool has_var() const
    {
        return true;
    }
    void add_var()
    {
    }
    const uint16_t& get_var()
    {
        const staticData* data = m_from
            ? reinterpret_cast<const staticData*>(&m_from[sizeof(_inner_::header)])
            : reinterpret_cast<const staticData*>(&m_buffer[sizeof(_inner_::header)]);
        return data->var;
}
    void set_var(const uint16_t& var)
    {
        staticData* data = reinterpret_cast<staticData*>(&m_buffer[sizeof(_inner_::header)]);
        data->var = var;
    }

    bool from(const void* ptr)
    {
        if (!ptr)
            return false;
        const _inner_::header* h = reinterpret_cast<const _inner_::header*>(ptr);
        if (h->signature0 != 'i' ||
            h->signature1 != 'b' ||
            h->version != 1)
            return false;
        if (!m_buffer.empty())
            m_buffer.clear();
        m_from = reinterpret_cast<uint8_t*>(const_cast<void*>(ptr));
        return true;
    }
    void* to()
    {
        if (m_from)
            return nullptr;
        else
            return m_buffer.data();
    }
    std::unique_ptr<void*> to(const uint32_t compression)
    {
        switch (compression)
        {
        case INB_NO_COMPRESSION:
            return nullptr;
        case INB_TABLE_COMPRESSION:
            return nullptr;
        case INB_FULL_COMPRESSION:
            return nullptr;
        default:
            break;
        }
        return nullptr;
    }
    uint32_t size() const
    {
        if (m_from)
            return 0;
        else
            return static_cast<uint32_t>(m_buffer.size());
    }

private:
    #pragma pack(push, 1)
    struct staticData
    {
        uint16_t var;
    };
    #pragma pack(pop)

    uint8_t* m_from = nullptr;
    std::vector<uint8_t> m_buffer;
    const uint16_t m_tableSize = 2;
};

class Packet
{
public:
    void create(const uint32_t reserve = 0)
    {
        m_from = nullptr;
        m_buffer.reserve(reserve);
        m_buffer.resize(sizeof(_inner_::header) + sizeof(m_tableSize) * sizeof(uint32_t) + sizeof(staticData));
        _inner_::header *h = reinterpret_cast<_inner_::header*>(&m_buffer[0]);
        h->signature0  = 'i';
        h->signature1  = 'b';
        h->version     = 1;
        h->compression = INB_NO_COMPRESSION;
        h->tableSize   = m_tableSize;
        if (m_tableSize)
        {
           uint32_t* table = reinterpret_cast<uint32_t*>(
               &m_buffer[0] + sizeof(_inner_::header)+sizeof(staticData));
           for (uint32_t i = 0; i < m_tableSize; i++)
               table[i] = 0;
        }
    }

    enum class ids
    {
        vBytes,
        vUint8,
        point2f,
        vArray,
        _size_
    };
    const char* getIdString(const ids& id) const
    {
        switch (id)
        {
        case ids::vBytes: return "vBytes"; 
        case ids::vUint8: return "vUint8"; 
        case ids::point2f: return "point2f"; 
        case ids::vArray: return "vArray"; 
        default: return "_?unknown_id?_";
        }
    }
    void* getDataById(const ids& id)
    {
        uint8_t* ptr = m_from ? m_from : m_buffer.data();
        const uint32_t* table = reinterpret_cast<const uint32_t*>(
                    &ptr[0] + sizeof(_inner_::header) + sizeof(staticData));
        const uint32_t offset = table[static_cast<uint32_t>(id)];
        if (offset == 0)
            throw std::logic_error("There is no field '"
                + std::string(getIdString(id)) + "' in the packet");
        return reinterpret_cast<void*>(&ptr[0] + offset + sizeof(uint32_t));
    }
    uint32_t getSizeById(const ids& id) const
    {
        const uint8_t* ptr = m_from ? m_from : m_buffer.data();
        const uint32_t* table = reinterpret_cast<const uint32_t*>(
                    &ptr[0] + sizeof(_inner_::header) + sizeof(staticData));
        const uint32_t offset = table[static_cast<uint32_t>(id)];
        if (offset == 0)
            return 0;
        else
            return *reinterpret_cast<const uint32_t*>(&ptr[0] + offset);
    }
    void addDataById(const ids& id, const uint32_t size)
    {
        if (m_from)
            throw std::logic_error("We cannot edit received packets");
        uint32_t* table = reinterpret_cast<uint32_t*>(
            &m_buffer[0] + sizeof(_inner_::header) + sizeof(staticData));
        const uint32_t offset = table[static_cast<uint32_t>(id)];
        if (offset != 0)
            return;
        const uint32_t newOffset = static_cast<uint32_t>(m_buffer.size());
        m_buffer.resize(m_buffer.size() + sizeof(uint32_t) + size);
        table = reinterpret_cast<uint32_t*>(
            &m_buffer[0] + sizeof(_inner_::header) + sizeof(staticData));
        table[static_cast<uint32_t>(id)] = newOffset;
        *reinterpret_cast<uint32_t*>(&m_buffer[0] + newOffset) = size;
    }

    uint32_t size_vUint32() const
    {
        return sizeof(uint32_t);
    }
    bool has_vUint32() const
    {
        return true;
    }
    void add_vUint32()
    {
    }
    const uint32_t& get_vUint32()
    {
        const staticData* data = m_from
            ? reinterpret_cast<const staticData*>(&m_from[sizeof(_inner_::header)])
            : reinterpret_cast<const staticData*>(&m_buffer[sizeof(_inner_::header)]);
        return data->vUint32;
}
    void set_vUint32(const uint32_t& vUint32)
    {
        staticData* data = reinterpret_cast<staticData*>(&m_buffer[sizeof(_inner_::header)]);
        data->vUint32 = vUint32;
    }

    uint32_t size_vBytes() const
    {
        return getSizeById(ids::vBytes);
    }
    bool has_vBytes() const
    {
        return getSizeById(ids::vBytes) != 0;
    }
    void add_vBytes(const uint32_t size)
    {
        addDataById(ids::vBytes, size);
    }
    const void* get_vBytes()
    {
        return getDataById(ids::vBytes);
}
    void set_vBytes(const void* vBytes)
    {
        const uint32_t size = getSizeById(ids::vBytes);
        if (size == 0)
            throw std::logic_error("Zero size");
        const uint8_t* src = reinterpret_cast<const uint8_t*>(vBytes);
        uint8_t* dst = reinterpret_cast<uint8_t*>(getDataById(ids::vBytes));
        for (uint32_t i = 0; i < size; i++)
            dst[i] = src[i];
    }

    uint32_t size_vUint8() const
    {
        return getSizeById(ids::vUint8) / sizeof(uint8_t);
    }
    bool has_vUint8() const
    {
        return getSizeById(ids::vUint8) != 0;
    }
    void add_vUint8()
    {
        addDataById(ids::vUint8, sizeof(uint8_t));
    }
    const uint8_t& get_vUint8()
    {
        return *reinterpret_cast<uint8_t*>(getDataById(ids::vUint8));
}
    void set_vUint8(const uint8_t& vUint8)
    {
        *reinterpret_cast<uint8_t*>(getDataById(ids::vUint8)) = vUint8;
    }

    uint32_t size_vArray() const
    {
        return getSizeById(ids::vArray) / sizeof(int64_t);
    }
    bool has_vArray() const
    {
        return getSizeById(ids::vArray) != 0;
    }
    void add_vArray(const uint32_t size)
    {
        addDataById(ids::vArray, size * sizeof(int64_t));
    }
    const int64_t* get_vArray()
    {
        return reinterpret_cast<int64_t*>(getDataById(ids::vArray));
    }
    const int64_t& get_vArray(const uint32_t index)
    {
        const int64_t* ptr = reinterpret_cast<int64_t*>(getDataById(ids::vArray));
        if (!ptr)
            throw std::logic_error("Nullptr");
        return ptr[index];
}
    void set_vArray(const int64_t* vArray)
    {
        const uint32_t size = getSizeById(ids::vArray);
        const uint8_t* src = reinterpret_cast<const uint8_t*>(vArray);
        uint8_t* dst = reinterpret_cast<uint8_t*>(getDataById(ids::vArray));
        for (uint32_t i = 0; i < size; i++)
            dst[i] = src[i];
    }
    void set_vArray(const uint32_t index, const int64_t& element)
    {
        int64_t* ptr = reinterpret_cast<int64_t*>(getDataById(ids::vArray));
        if (!ptr)
            throw std::logic_error("Nullptr");
        ptr[index] = element;
    }

    bool from(const void* ptr)
    {
        if (!ptr)
            return false;
        const _inner_::header* h = reinterpret_cast<const _inner_::header*>(ptr);
        if (h->signature0 != 'i' ||
            h->signature1 != 'b' ||
            h->version != 1)
            return false;
        if (!m_buffer.empty())
            m_buffer.clear();
        m_from = reinterpret_cast<uint8_t*>(const_cast<void*>(ptr));
        return true;
    }
    void* to()
    {
        if (m_from)
            return nullptr;
        else
            return m_buffer.data();
    }
    std::unique_ptr<void*> to(const uint32_t compression)
    {
        switch (compression)
        {
        case INB_NO_COMPRESSION:
            return nullptr;
        case INB_TABLE_COMPRESSION:
            return nullptr;
        case INB_FULL_COMPRESSION:
            return nullptr;
        default:
            break;
        }
        return nullptr;
    }
    uint32_t size() const
    {
        if (m_from)
            return 0;
        else
            return static_cast<uint32_t>(m_buffer.size());
    }

private:
    #pragma pack(push, 1)
    struct staticData
    {
        uint32_t vUint32;
    };
    #pragma pack(pop)

    uint8_t* m_from = nullptr;
    std::vector<uint8_t> m_buffer;
    const uint16_t m_tableSize = 4;
};

class Vec2f
{
public:
    void create(const uint32_t reserve = 0)
    {
        m_from = nullptr;
        m_buffer.reserve(reserve);
        m_buffer.resize(sizeof(_inner_::header) + sizeof(m_tableSize) * sizeof(uint32_t) + sizeof(staticData));
        _inner_::header *h = reinterpret_cast<_inner_::header*>(&m_buffer[0]);
        h->signature0  = 'i';
        h->signature1  = 'b';
        h->version     = 1;
        h->compression = INB_NO_COMPRESSION;
        h->tableSize   = m_tableSize;
        if (m_tableSize)
        {
           uint32_t* table = reinterpret_cast<uint32_t*>(
               &m_buffer[0] + sizeof(_inner_::header)+sizeof(staticData));
           for (uint32_t i = 0; i < m_tableSize; i++)
               table[i] = 0;
        }
    }

    uint32_t size_x() const
    {
        return sizeof(float);
    }
    bool has_x() const
    {
        return true;
    }
    void add_x()
    {
    }
    const float& get_x()
    {
        const staticData* data = m_from
            ? reinterpret_cast<const staticData*>(&m_from[sizeof(_inner_::header)])
            : reinterpret_cast<const staticData*>(&m_buffer[sizeof(_inner_::header)]);
        return data->x;
}
    void set_x(const float& x)
    {
        staticData* data = reinterpret_cast<staticData*>(&m_buffer[sizeof(_inner_::header)]);
        data->x = x;
    }

    uint32_t size_y() const
    {
        return sizeof(float);
    }
    bool has_y() const
    {
        return true;
    }
    void add_y()
    {
    }
    const float& get_y()
    {
        const staticData* data = m_from
            ? reinterpret_cast<const staticData*>(&m_from[sizeof(_inner_::header)])
            : reinterpret_cast<const staticData*>(&m_buffer[sizeof(_inner_::header)]);
        return data->y;
}
    void set_y(const float& y)
    {
        staticData* data = reinterpret_cast<staticData*>(&m_buffer[sizeof(_inner_::header)]);
        data->y = y;
    }

    bool from(const void* ptr)
    {
        if (!ptr)
            return false;
        const _inner_::header* h = reinterpret_cast<const _inner_::header*>(ptr);
        if (h->signature0 != 'i' ||
            h->signature1 != 'b' ||
            h->version != 1)
            return false;
        if (!m_buffer.empty())
            m_buffer.clear();
        m_from = reinterpret_cast<uint8_t*>(const_cast<void*>(ptr));
        return true;
    }
    void* to()
    {
        if (m_from)
            return nullptr;
        else
            return m_buffer.data();
    }
    std::unique_ptr<void*> to(const uint32_t compression)
    {
        switch (compression)
        {
        case INB_NO_COMPRESSION:
            return nullptr;
        case INB_TABLE_COMPRESSION:
            return nullptr;
        case INB_FULL_COMPRESSION:
            return nullptr;
        default:
            break;
        }
        return nullptr;
    }
    uint32_t size() const
    {
        if (m_from)
            return 0;
        else
            return static_cast<uint32_t>(m_buffer.size());
    }

private:
    #pragma pack(push, 1)
    struct staticData
    {
        float x;
        float y;
    };
    #pragma pack(pop)

    uint8_t* m_from = nullptr;
    std::vector<uint8_t> m_buffer;
    const uint16_t m_tableSize = 0;
};

class Vec3i
{
public:
    void create(const uint32_t reserve = 0)
    {
        m_from = nullptr;
        m_buffer.reserve(reserve);
        m_buffer.resize(sizeof(_inner_::header) + sizeof(m_tableSize) * sizeof(uint32_t));
        _inner_::header *h = reinterpret_cast<_inner_::header*>(&m_buffer[0]);
        h->signature0  = 'i';
        h->signature1  = 'b';
        h->version     = 1;
        h->compression = INB_NO_COMPRESSION;
        h->tableSize   = m_tableSize;
        if (m_tableSize)
        {
           uint32_t* table = reinterpret_cast<uint32_t*>(
               &m_buffer[0] + sizeof(_inner_::header));
           for (uint32_t i = 0; i < m_tableSize; i++)
               table[i] = 0;
        }
    }

    enum class ids
    {
        x,
        y,
        z,
        _size_
    };
    const char* getIdString(const ids& id) const
    {
        switch (id)
        {
        case ids::x: return "x"; 
        case ids::y: return "y"; 
        case ids::z: return "z"; 
        default: return "_?unknown_id?_";
        }
    }
    void* getDataById(const ids& id)
    {
        uint8_t* ptr = m_from ? m_from : m_buffer.data();
        const uint32_t* table = reinterpret_cast<const uint32_t*>(
                    &ptr[0] + sizeof(_inner_::header));
        const uint32_t offset = table[static_cast<uint32_t>(id)];
        if (offset == 0)
            throw std::logic_error("There is no field '"
                + std::string(getIdString(id)) + "' in the packet");
        return reinterpret_cast<void*>(&ptr[0] + offset + sizeof(uint32_t));
    }
    uint32_t getSizeById(const ids& id) const
    {
        const uint8_t* ptr = m_from ? m_from : m_buffer.data();
        const uint32_t* table = reinterpret_cast<const uint32_t*>(
                    &ptr[0] + sizeof(_inner_::header));
        const uint32_t offset = table[static_cast<uint32_t>(id)];
        if (offset == 0)
            return 0;
        else
            return *reinterpret_cast<const uint32_t*>(&ptr[0] + offset);
    }
    void addDataById(const ids& id, const uint32_t size)
    {
        if (m_from)
            throw std::logic_error("We cannot edit received packets");
        uint32_t* table = reinterpret_cast<uint32_t*>(
            &m_buffer[0] + sizeof(_inner_::header));
        const uint32_t offset = table[static_cast<uint32_t>(id)];
        if (offset != 0)
            return;
        const uint32_t newOffset = static_cast<uint32_t>(m_buffer.size());
        m_buffer.resize(m_buffer.size() + sizeof(uint32_t) + size);
        table = reinterpret_cast<uint32_t*>(
            &m_buffer[0] + sizeof(_inner_::header));
        table[static_cast<uint32_t>(id)] = newOffset;
        *reinterpret_cast<uint32_t*>(&m_buffer[0] + newOffset) = size;
    }

    uint32_t size_x() const
    {
        return getSizeById(ids::x) / sizeof(int32_t);
    }
    bool has_x() const
    {
        return getSizeById(ids::x) != 0;
    }
    void add_x()
    {
        addDataById(ids::x, sizeof(int32_t));
    }
    const int32_t& get_x()
    {
        return *reinterpret_cast<int32_t*>(getDataById(ids::x));
}
    void set_x(const int32_t& x)
    {
        *reinterpret_cast<int32_t*>(getDataById(ids::x)) = x;
    }

    uint32_t size_y() const
    {
        return getSizeById(ids::y) / sizeof(int32_t);
    }
    bool has_y() const
    {
        return getSizeById(ids::y) != 0;
    }
    void add_y()
    {
        addDataById(ids::y, sizeof(int32_t));
    }
    const int32_t& get_y()
    {
        return *reinterpret_cast<int32_t*>(getDataById(ids::y));
}
    void set_y(const int32_t& y)
    {
        *reinterpret_cast<int32_t*>(getDataById(ids::y)) = y;
    }

    uint32_t size_z() const
    {
        return getSizeById(ids::z) / sizeof(int32_t);
    }
    bool has_z() const
    {
        return getSizeById(ids::z) != 0;
    }
    void add_z()
    {
        addDataById(ids::z, sizeof(int32_t));
    }
    const int32_t& get_z()
    {
        return *reinterpret_cast<int32_t*>(getDataById(ids::z));
}
    void set_z(const int32_t& z)
    {
        *reinterpret_cast<int32_t*>(getDataById(ids::z)) = z;
    }

    bool from(const void* ptr)
    {
        if (!ptr)
            return false;
        const _inner_::header* h = reinterpret_cast<const _inner_::header*>(ptr);
        if (h->signature0 != 'i' ||
            h->signature1 != 'b' ||
            h->version != 1)
            return false;
        if (!m_buffer.empty())
            m_buffer.clear();
        m_from = reinterpret_cast<uint8_t*>(const_cast<void*>(ptr));
        return true;
    }
    void* to()
    {
        if (m_from)
            return nullptr;
        else
            return m_buffer.data();
    }
    std::unique_ptr<void*> to(const uint32_t compression)
    {
        switch (compression)
        {
        case INB_NO_COMPRESSION:
            return nullptr;
        case INB_TABLE_COMPRESSION:
            return nullptr;
        case INB_FULL_COMPRESSION:
            return nullptr;
        default:
            break;
        }
        return nullptr;
    }
    uint32_t size() const
    {
        if (m_from)
            return 0;
        else
            return static_cast<uint32_t>(m_buffer.size());
    }

private:
    uint8_t* m_from = nullptr;
    std::vector<uint8_t> m_buffer;
    const uint16_t m_tableSize = 3;
};

}
}

#endif // SCHEMA_GENERATED_H
