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

class Packet;
class Vec2f;
class Vec3i;

class Packet
{
public:
    void create(const uint32_t reserve = 0)
    {
        m_buffer.reserve(reserve);
        m_buffer.resize(sizeof(_inner_::header) + sizeof(staticData));
        _inner_::header *h = reinterpret_cast<_inner_::header*>(&m_buffer[0]);
        h->signature0  = 'i';
        h->signature1  = 'b';
        h->version     = 1;
        h->compression = INB_NO_COMPRESSION;
        h->tableSize   = 0;
    }

    bool has_vUint32() const
    {
        return true;
    }
    void add_vUint32()
    {}
    const uint32_t& get_vUint32() const
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

    bool has_vUint8() const
    {
        return true;
    }
    void add_vUint8()
    {}
    const uint8_t& get_vUint8() const
    {
        const staticData* data = m_from
            ? reinterpret_cast<const staticData*>(&m_from[sizeof(_inner_::header)])
            : reinterpret_cast<const staticData*>(&m_buffer[sizeof(_inner_::header)]);
        return data->vUint8;
    }
    void set_vUint8(const uint8_t& vUint8)
    {
        staticData* data = reinterpret_cast<staticData*>(&m_buffer[sizeof(_inner_::header)]);
        data->vUint8 = vUint8;
    }

    bool has_vArray() const
    {
        return true;
    }
    void add_vArray()
    {}
    const int64_t& get_vArray() const
    {
        const staticData* data = m_from
            ? reinterpret_cast<const staticData*>(&m_from[sizeof(_inner_::header)])
            : reinterpret_cast<const staticData*>(&m_buffer[sizeof(_inner_::header)]);
        return data->vArray;
    }
    void set_vArray(const int64_t& vArray)
    {
        staticData* data = reinterpret_cast<staticData*>(&m_buffer[sizeof(_inner_::header)]);
        data->vArray = vArray;
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
        m_from = reinterpret_cast<const uint8_t*>(ptr);
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
        uint8_t vUint8;
        int64_t vArray;
    };
    #pragma pack(pop)

    const uint8_t* m_from = nullptr;
    std::vector<uint8_t> m_buffer;
    uint32_t m_tableOffset = 0;
};

class Vec2f
{
public:
    void create(const uint32_t reserve = 0)
    {
        m_buffer.reserve(reserve);
        m_buffer.resize(sizeof(_inner_::header) + sizeof(staticData));
        _inner_::header *h = reinterpret_cast<_inner_::header*>(&m_buffer[0]);
        h->signature0  = 'i';
        h->signature1  = 'b';
        h->version     = 1;
        h->compression = INB_NO_COMPRESSION;
        h->tableSize   = 0;
    }

    bool has_x() const
    {
        return true;
    }
    void add_x()
    {}
    const float& get_x() const
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

    bool has_y() const
    {
        return true;
    }
    void add_y()
    {}
    const float& get_y() const
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
        m_from = reinterpret_cast<const uint8_t*>(ptr);
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

    const uint8_t* m_from = nullptr;
    std::vector<uint8_t> m_buffer;
    uint32_t m_tableOffset = 0;
};

class Vec3i
{
public:
    void create(const uint32_t reserve = 0)
    {
        m_buffer.reserve(reserve);
        m_buffer.resize(sizeof(_inner_::header) + sizeof(staticData));
        _inner_::header *h = reinterpret_cast<_inner_::header*>(&m_buffer[0]);
        h->signature0  = 'i';
        h->signature1  = 'b';
        h->version     = 1;
        h->compression = INB_NO_COMPRESSION;
        h->tableSize   = 0;
    }

    bool has_x() const
    {
        return true;
    }
    void add_x()
    {}
    const int32_t& get_x() const
    {
        const staticData* data = m_from
            ? reinterpret_cast<const staticData*>(&m_from[sizeof(_inner_::header)])
            : reinterpret_cast<const staticData*>(&m_buffer[sizeof(_inner_::header)]);
        return data->x;
    }
    void set_x(const int32_t& x)
    {
        staticData* data = reinterpret_cast<staticData*>(&m_buffer[sizeof(_inner_::header)]);
        data->x = x;
    }

    bool has_y() const
    {
        return true;
    }
    void add_y()
    {}
    const int32_t& get_y() const
    {
        const staticData* data = m_from
            ? reinterpret_cast<const staticData*>(&m_from[sizeof(_inner_::header)])
            : reinterpret_cast<const staticData*>(&m_buffer[sizeof(_inner_::header)]);
        return data->y;
    }
    void set_y(const int32_t& y)
    {
        staticData* data = reinterpret_cast<staticData*>(&m_buffer[sizeof(_inner_::header)]);
        data->y = y;
    }

    bool has_z() const
    {
        return true;
    }
    void add_z()
    {}
    const int32_t& get_z() const
    {
        const staticData* data = m_from
            ? reinterpret_cast<const staticData*>(&m_from[sizeof(_inner_::header)])
            : reinterpret_cast<const staticData*>(&m_buffer[sizeof(_inner_::header)]);
        return data->z;
    }
    void set_z(const int32_t& z)
    {
        staticData* data = reinterpret_cast<staticData*>(&m_buffer[sizeof(_inner_::header)]);
        data->z = z;
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
        m_from = reinterpret_cast<const uint8_t*>(ptr);
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
        int32_t x;
        int32_t y;
        int32_t z;
    };
    #pragma pack(pop)

    const uint8_t* m_from = nullptr;
    std::vector<uint8_t> m_buffer;
    uint32_t m_tableOffset = 0;
};

}
}

#endif // SCHEMA_GENERATED_H
