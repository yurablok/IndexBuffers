#ifndef SCHEMA_GENERATED_H
#define SCHEMA_GENERATED_H

#include <vector>
#include <memory>

namespace ExtNS
{
namespace IntNS
{
namespace _inner_
{
    static void copybybytes(const void* src, void* dst, const uint32_t size)
    {
        for (uint32_t i = 0; i < size; ++i)
            reinterpret_cast<uint8_t*>(dst)[i] = reinterpret_cast<const uint8_t*>(src)[i];
    }
    struct header
    {
        uint8_t signature0 = 'i';
        uint8_t signature1 = 'b';
        uint16_t type           ;
    };
    static uint32_t zero = 0;
}

static const uint32_t TestConst = 43344;
enum class Color
{
    RED = 0,
    GREEN = 3,
    BLUE = 4
};
static const char* Color_str(const Color& id)
{
    switch (id)
    {
    case Color::RED: return "RED";
    case Color::GREEN: return "GREEN";
    case Color::BLUE: return "BLUE";
    default: return "_?unknown_Color?_";
    }
}


class OnlyStatic
{
public:
    OnlyStatic(const uint32_t reserve = 0)
    {
        create(reserve);
    }
    OnlyStatic(uint8_t* from_)
    {
        this->from(from_);
    }
    OnlyStatic(const uint8_t* from_)
    {
        this->from(from_);
    }

    enum class ids
    {
        a,
        b,
        c,
        d,
        e,
        f,
        _size_
    };
    static const char* name(const ids& id)
    {
        switch (id)
        {
        case ids::a: return "a"; 
        case ids::b: return "b"; 
        case ids::c: return "c"; 
        case ids::d: return "d"; 
        case ids::e: return "e"; 
        case ids::f: return "f"; 
        default: return "_?unknown_name?_";
        }
    }

    enum class types
    {
        int8, uint8, int8a, uint8a,
        int16, uint16, int16a, uint16a,
        int32, uint32, int32a, uint32a,
        int64, uint64, int64a, uint64a,
        float32, float32a, float64, float64a,
        bytes,
        unknown
    };

    uint32_t has(const ids& id) const
    {
        return caddress(id, cgetTable());
    }
    uint8_t* get(const ids& id)
    {
        Table* table = getTable();
        uint8_t* ptr = to();
        switch (id)
        {
        case ids::a:
            return reinterpret_cast<uint8_t*>(&table->a);
        case ids::b:
            return reinterpret_cast<uint8_t*>(&table->b);
        case ids::c:
            return reinterpret_cast<uint8_t*>(&table->c);
        case ids::d:
            return reinterpret_cast<uint8_t*>(&table->d);
        case ids::e:
            return reinterpret_cast<uint8_t*>(&table->e);
        case ids::f:
            return reinterpret_cast<uint8_t*>(&table->f);
        default:
            return nullptr;
        }
    }
    const uint8_t* cget(const ids& id) const
    {
        const Table* table = cgetTable();
        const uint8_t* ptr = cto();
        switch (id)
        {
        case ids::a:
            return reinterpret_cast<const uint8_t*>(&table->a);
        case ids::b:
            return reinterpret_cast<const uint8_t*>(&table->b);
        case ids::c:
            return reinterpret_cast<const uint8_t*>(&table->c);
        case ids::d:
            return reinterpret_cast<const uint8_t*>(&table->d);
        case ids::e:
            return reinterpret_cast<const uint8_t*>(&table->e);
        case ids::f:
            return reinterpret_cast<const uint8_t*>(&table->f);
        default:
            return nullptr;
        }
    }
    void set(const ids& id, const void* data, const uint32_t size)
    {
        switch (id)
        {
        case ids::a:
            if (data)
                _inner_::copybybytes(data, &getTable()->a, size);
            break;
        case ids::b:
            if (data)
                _inner_::copybybytes(data, &getTable()->b, size);
            break;
        case ids::c:
            if (data)
                _inner_::copybybytes(data, &getTable()->c, size);
            break;
        case ids::d:
            if (data)
                _inner_::copybybytes(data, &getTable()->d, size);
            break;
        case ids::e:
            if (data)
                _inner_::copybybytes(data, &getTable()->e, size);
            break;
        case ids::f:
            if (data)
                _inner_::copybybytes(data, &getTable()->f, size);
            break;
        default:
            break;
        }
    }
    static types type(const ids& id)
    {
        switch (id)
        {
        case ids::a: return types::int8;
        case ids::b: return types::uint16;
        case ids::c: return types::int32;
        case ids::d: return types::uint64;
        case ids::e: return types::float32;
        case ids::f: return types::float64;
        default: return types::unknown;
        }
    }
    uint32_t size(const ids& id) const
    {
        uint8_t* ptr = m_from ? m_from : &m_buffer->data()[0];
        Table *table = reinterpret_cast<Table*>(&ptr[sizeof(_inner_::header)]);
        const uint32_t offset = caddress(id, table);
        return *reinterpret_cast<uint32_t*>(&ptr[offset]);
    }

    bool has_a() const
    {
        return true;
    }
    uint32_t size_a() const
    {
        return sizeof(int8_t);
    }
    int8_t get_a() const
    {
        int8_t dst;
        _inner_::copybybytes(cget(ids::a), &dst, sizeof(int8_t));
        return dst;
    }
    void set_a(const int8_t& a)
    {
        _inner_::copybybytes(&a, get(ids::a), sizeof(int8_t));
    }

    bool has_b() const
    {
        return true;
    }
    uint32_t size_b() const
    {
        return sizeof(uint16_t);
    }
    uint16_t get_b() const
    {
        uint16_t dst;
        _inner_::copybybytes(cget(ids::b), &dst, sizeof(uint16_t));
        return dst;
    }
    void set_b(const uint16_t& b)
    {
        _inner_::copybybytes(&b, get(ids::b), sizeof(uint16_t));
    }

    bool has_c() const
    {
        return true;
    }
    uint32_t size_c() const
    {
        return sizeof(int32_t);
    }
    int32_t get_c() const
    {
        int32_t dst;
        _inner_::copybybytes(cget(ids::c), &dst, sizeof(int32_t));
        return dst;
    }
    void set_c(const int32_t& c)
    {
        _inner_::copybybytes(&c, get(ids::c), sizeof(int32_t));
    }

    bool has_d() const
    {
        return true;
    }
    uint32_t size_d() const
    {
        return sizeof(uint64_t);
    }
    uint64_t get_d() const
    {
        uint64_t dst;
        _inner_::copybybytes(cget(ids::d), &dst, sizeof(uint64_t));
        return dst;
    }
    void set_d(const uint64_t& d)
    {
        _inner_::copybybytes(&d, get(ids::d), sizeof(uint64_t));
    }

    bool has_e() const
    {
        return true;
    }
    uint32_t size_e() const
    {
        return sizeof(float);
    }
    float get_e() const
    {
        float dst;
        _inner_::copybybytes(cget(ids::e), &dst, sizeof(float));
        return dst;
    }
    void set_e(const float& e)
    {
        _inner_::copybybytes(&e, get(ids::e), sizeof(float));
    }

    bool has_f() const
    {
        return true;
    }
    uint32_t size_f() const
    {
        return sizeof(double);
    }
    double get_f() const
    {
        double dst;
        _inner_::copybybytes(cget(ids::f), &dst, sizeof(double));
        return dst;
    }
    void set_f(const double& f)
    {
        _inner_::copybybytes(&f, get(ids::f), sizeof(double));
    }

    void create(const uint32_t reserve = 0)
    {
        m_from = nullptr;
        m_buffer = std::make_shared<std::vector<uint8_t>>();
        m_buffer->reserve(reserve);
        m_buffer->resize(sizeof(_inner_::header) + sizeof(Table));
        _inner_::header *h = reinterpret_cast<_inner_::header*>(&m_buffer->data()[0]);
        h->signature0  = 'i';
        h->signature1  = 'b';
        m_table = sizeof(_inner_::header);
    }
    bool from(uint8_t* ptr)
    {
        if (!ptr)
            return false;
        const _inner_::header* h = reinterpret_cast<const _inner_::header*>(ptr);
        if (h->signature0 != 'i' ||
            h->signature1 != 'b')
            return false;
        m_buffer.reset();
        m_from = ptr;
        return true;
    }
    bool from(const uint8_t* ptr)
    {
        if (!ptr)
            return false;
        const _inner_::header* h = reinterpret_cast<const _inner_::header*>(ptr);
        if (h->signature0 != 'i' ||
            h->signature1 != 'b')
            return false;
        if (!m_buffer->empty())
            m_buffer->clear();
        m_from = const_cast<uint8_t*>(ptr);
        return true;
    }
    uint8_t* to()
    {
        if (m_from)
            return const_cast<uint8_t*>(m_from);
        else
            return m_buffer->data();
    }
    const uint8_t* cto() const
    {
        if (m_from)
            return m_from;
        else
            return m_buffer->data();
    }
    uint32_t size() const
    {
        if (m_from)
            return 0;
        else
            return static_cast<uint32_t>(m_buffer->size());
    }

private:
    #pragma pack(push, 1)
    struct Table
    {
        int8_t a;
        uint16_t b;
        int32_t c;
        uint64_t d;
        float e;
        double f;
    };
    #pragma pack(pop)

    Table* getTable()
    {
        return reinterpret_cast<Table*>(&to()[m_table]);
    }
    const Table* cgetTable() const
    {
        return reinterpret_cast<const Table*>(&cto()[m_table]);
    }
    uint32_t insert(const uint32_t size)
    {
        if (m_from)
            return 0;
        else
        {
            const uint32_t offset = static_cast<uint32_t>(m_buffer->size());
            m_buffer->resize(m_buffer->size() + size);
            return offset;
        }
    }
    uint32_t& address(const ids& id, Table* table)
    {
        switch (id)
        {
        default: return _inner_::zero;
        }
    }
    uint32_t caddress(const ids& id, const Table* table) const
    {
        switch (id)
        {
        default: return _inner_::zero;
        }
    }

    uint8_t* m_from = nullptr;
    std::shared_ptr<std::vector<uint8_t>> m_buffer;
    uint32_t m_table = 0;
};

class OnlyOptional
{
public:
    OnlyOptional(const uint32_t reserve = 0)
    {
        create(reserve);
    }
    OnlyOptional(uint8_t* from_)
    {
        this->from(from_);
    }
    OnlyOptional(const uint8_t* from_)
    {
        this->from(from_);
    }

    enum class ids
    {
        a,
        b,
        c,
        d,
        e,
        f,
        _size_
    };
    static const char* name(const ids& id)
    {
        switch (id)
        {
        case ids::a: return "a"; 
        case ids::b: return "b"; 
        case ids::c: return "c"; 
        case ids::d: return "d"; 
        case ids::e: return "e"; 
        case ids::f: return "f"; 
        default: return "_?unknown_name?_";
        }
    }

    enum class types
    {
        int8, uint8, int8a, uint8a,
        int16, uint16, int16a, uint16a,
        int32, uint32, int32a, uint32a,
        int64, uint64, int64a, uint64a,
        float32, float32a, float64, float64a,
        bytes,
        unknown
    };

    uint32_t has(const ids& id) const
    {
        return caddress(id, cgetTable());
    }
    uint8_t* get(const ids& id)
    {
        Table* table = getTable();
        uint8_t* ptr = to();
        switch (id)
        {
        case ids::a:
        case ids::b:
        case ids::c:
        case ids::d:
        case ids::e:
        case ids::f:
            return &ptr[address(id, table) + sizeof(uint32_t)];
        default:
            return nullptr;
        }
    }
    const uint8_t* cget(const ids& id) const
    {
        const Table* table = cgetTable();
        const uint8_t* ptr = cto();
        switch (id)
        {
        case ids::a:
        case ids::b:
        case ids::c:
        case ids::d:
        case ids::e:
        case ids::f:
            return &ptr[caddress(id, table) + sizeof(uint32_t)];
        default:
            return nullptr;
        }
    }
    void set(const ids& id, const void* data, const uint32_t size)
    {
        switch (id)
        {
        default:
        {
            uint32_t offset = has(id);
            if (offset == 0)
            {
                offset = insert(size + sizeof(uint32_t));
                if (!offset)
                    return;
                *reinterpret_cast<uint32_t*>(&m_buffer->data()[offset]) = size;
                address(id, getTable()) = offset;
            }
            if (data)
                _inner_::copybybytes(data, &m_buffer->data()[offset + sizeof(uint32_t)], size);
            break;
        }
        }
    }
    static types type(const ids& id)
    {
        switch (id)
        {
        case ids::a: return types::int8;
        case ids::b: return types::uint16;
        case ids::c: return types::int32;
        case ids::d: return types::uint64;
        case ids::e: return types::float32;
        case ids::f: return types::float64;
        default: return types::unknown;
        }
    }
    uint32_t size(const ids& id) const
    {
        uint8_t* ptr = m_from ? m_from : &m_buffer->data()[0];
        Table *table = reinterpret_cast<Table*>(&ptr[sizeof(_inner_::header)]);
        const uint32_t offset = caddress(id, table);
        return *reinterpret_cast<uint32_t*>(&ptr[offset]);
    }

    bool has_a() const
    {
        return has(ids::a) != 0;
    }
    uint32_t size_a() const
    {
        return size(ids::a);
    }
    int8_t get_a() const
    {
        int8_t dst;
        _inner_::copybybytes(cget(ids::a), &dst, sizeof(int8_t));
        return dst;
    }
    void set_a(const int8_t& a)
    {
        set(ids::a, &a, sizeof(int8_t));
    }

    bool has_b() const
    {
        return has(ids::b) != 0;
    }
    uint32_t size_b() const
    {
        return size(ids::b);
    }
    uint16_t get_b() const
    {
        uint16_t dst;
        _inner_::copybybytes(cget(ids::b), &dst, sizeof(uint16_t));
        return dst;
    }
    void set_b(const uint16_t& b)
    {
        set(ids::b, &b, sizeof(uint16_t));
    }

    bool has_c() const
    {
        return has(ids::c) != 0;
    }
    uint32_t size_c() const
    {
        return size(ids::c);
    }
    int32_t get_c() const
    {
        int32_t dst;
        _inner_::copybybytes(cget(ids::c), &dst, sizeof(int32_t));
        return dst;
    }
    void set_c(const int32_t& c)
    {
        set(ids::c, &c, sizeof(int32_t));
    }

    bool has_d() const
    {
        return has(ids::d) != 0;
    }
    uint32_t size_d() const
    {
        return size(ids::d);
    }
    uint64_t get_d() const
    {
        uint64_t dst;
        _inner_::copybybytes(cget(ids::d), &dst, sizeof(uint64_t));
        return dst;
    }
    void set_d(const uint64_t& d)
    {
        set(ids::d, &d, sizeof(uint64_t));
    }

    bool has_e() const
    {
        return has(ids::e) != 0;
    }
    uint32_t size_e() const
    {
        return size(ids::e);
    }
    float get_e() const
    {
        float dst;
        _inner_::copybybytes(cget(ids::e), &dst, sizeof(float));
        return dst;
    }
    void set_e(const float& e)
    {
        set(ids::e, &e, sizeof(float));
    }

    bool has_f() const
    {
        return has(ids::f) != 0;
    }
    uint32_t size_f() const
    {
        return size(ids::f);
    }
    double get_f() const
    {
        double dst;
        _inner_::copybybytes(cget(ids::f), &dst, sizeof(double));
        return dst;
    }
    void set_f(const double& f)
    {
        set(ids::f, &f, sizeof(double));
    }

    void create(const uint32_t reserve = 0)
    {
        m_from = nullptr;
        m_buffer = std::make_shared<std::vector<uint8_t>>();
        m_buffer->reserve(reserve);
        m_buffer->resize(sizeof(_inner_::header) + sizeof(Table));
        _inner_::header *h = reinterpret_cast<_inner_::header*>(&m_buffer->data()[0]);
        h->signature0  = 'i';
        h->signature1  = 'b';
        m_table = sizeof(_inner_::header);
    }
    bool from(uint8_t* ptr)
    {
        if (!ptr)
            return false;
        const _inner_::header* h = reinterpret_cast<const _inner_::header*>(ptr);
        if (h->signature0 != 'i' ||
            h->signature1 != 'b')
            return false;
        m_buffer.reset();
        m_from = ptr;
        return true;
    }
    bool from(const uint8_t* ptr)
    {
        if (!ptr)
            return false;
        const _inner_::header* h = reinterpret_cast<const _inner_::header*>(ptr);
        if (h->signature0 != 'i' ||
            h->signature1 != 'b')
            return false;
        if (!m_buffer->empty())
            m_buffer->clear();
        m_from = const_cast<uint8_t*>(ptr);
        return true;
    }
    uint8_t* to()
    {
        if (m_from)
            return const_cast<uint8_t*>(m_from);
        else
            return m_buffer->data();
    }
    const uint8_t* cto() const
    {
        if (m_from)
            return m_from;
        else
            return m_buffer->data();
    }
    uint32_t size() const
    {
        if (m_from)
            return 0;
        else
            return static_cast<uint32_t>(m_buffer->size());
    }

private:
    #pragma pack(push, 1)
    struct Table
    {
        uint32_t __a = 0;
        uint32_t __b = 0;
        uint32_t __c = 0;
        uint32_t __d = 0;
        uint32_t __e = 0;
        uint32_t __f = 0;
    };
    #pragma pack(pop)

    Table* getTable()
    {
        return reinterpret_cast<Table*>(&to()[m_table]);
    }
    const Table* cgetTable() const
    {
        return reinterpret_cast<const Table*>(&cto()[m_table]);
    }
    uint32_t insert(const uint32_t size)
    {
        if (m_from)
            return 0;
        else
        {
            const uint32_t offset = static_cast<uint32_t>(m_buffer->size());
            m_buffer->resize(m_buffer->size() + size);
            return offset;
        }
    }
    uint32_t& address(const ids& id, Table* table)
    {
        switch (id)
        {
        case ids::a: return table->__a;
        case ids::b: return table->__b;
        case ids::c: return table->__c;
        case ids::d: return table->__d;
        case ids::e: return table->__e;
        case ids::f: return table->__f;
        default: return _inner_::zero;
        }
    }
    uint32_t caddress(const ids& id, const Table* table) const
    {
        switch (id)
        {
        case ids::a: return table->__a;
        case ids::b: return table->__b;
        case ids::c: return table->__c;
        case ids::d: return table->__d;
        case ids::e: return table->__e;
        case ids::f: return table->__f;
        default: return _inner_::zero;
        }
    }

    uint8_t* m_from = nullptr;
    std::shared_ptr<std::vector<uint8_t>> m_buffer;
    uint32_t m_table = 0;
};

class Vec3f
{
public:
    Vec3f(const uint32_t reserve = 0)
    {
        create(reserve);
    }
    Vec3f(uint8_t* from_)
    {
        this->from(from_);
    }
    Vec3f(const uint8_t* from_)
    {
        this->from(from_);
    }

    enum class ids
    {
        x,
        y,
        z,
        _size_
    };
    static const char* name(const ids& id)
    {
        switch (id)
        {
        case ids::x: return "x"; 
        case ids::y: return "y"; 
        case ids::z: return "z"; 
        default: return "_?unknown_name?_";
        }
    }

    enum class types
    {
        int8, uint8, int8a, uint8a,
        int16, uint16, int16a, uint16a,
        int32, uint32, int32a, uint32a,
        int64, uint64, int64a, uint64a,
        float32, float32a, float64, float64a,
        bytes,
        unknown
    };

    uint32_t has(const ids& id) const
    {
        return caddress(id, cgetTable());
    }
    uint8_t* get(const ids& id)
    {
        Table* table = getTable();
        uint8_t* ptr = to();
        switch (id)
        {
        case ids::x:
            return reinterpret_cast<uint8_t*>(&table->x);
        case ids::y:
            return reinterpret_cast<uint8_t*>(&table->y);
        case ids::z:
            return reinterpret_cast<uint8_t*>(&table->z);
        default:
            return nullptr;
        }
    }
    const uint8_t* cget(const ids& id) const
    {
        const Table* table = cgetTable();
        const uint8_t* ptr = cto();
        switch (id)
        {
        case ids::x:
            return reinterpret_cast<const uint8_t*>(&table->x);
        case ids::y:
            return reinterpret_cast<const uint8_t*>(&table->y);
        case ids::z:
            return reinterpret_cast<const uint8_t*>(&table->z);
        default:
            return nullptr;
        }
    }
    void set(const ids& id, const void* data, const uint32_t size)
    {
        switch (id)
        {
        case ids::x:
            if (data)
                _inner_::copybybytes(data, &getTable()->x, size);
            break;
        case ids::y:
            if (data)
                _inner_::copybybytes(data, &getTable()->y, size);
            break;
        case ids::z:
            if (data)
                _inner_::copybybytes(data, &getTable()->z, size);
            break;
        default:
            break;
        }
    }
    static types type(const ids& id)
    {
        switch (id)
        {
        case ids::x: return types::float32;
        case ids::y: return types::float32;
        case ids::z: return types::float32;
        default: return types::unknown;
        }
    }
    uint32_t size(const ids& id) const
    {
        uint8_t* ptr = m_from ? m_from : &m_buffer->data()[0];
        Table *table = reinterpret_cast<Table*>(&ptr[sizeof(_inner_::header)]);
        const uint32_t offset = caddress(id, table);
        return *reinterpret_cast<uint32_t*>(&ptr[offset]);
    }

    bool has_x() const
    {
        return true;
    }
    uint32_t size_x() const
    {
        return sizeof(float);
    }
    float get_x() const
    {
        float dst;
        _inner_::copybybytes(cget(ids::x), &dst, sizeof(float));
        return dst;
    }
    void set_x(const float& x)
    {
        _inner_::copybybytes(&x, get(ids::x), sizeof(float));
    }

    bool has_y() const
    {
        return true;
    }
    uint32_t size_y() const
    {
        return sizeof(float);
    }
    float get_y() const
    {
        float dst;
        _inner_::copybybytes(cget(ids::y), &dst, sizeof(float));
        return dst;
    }
    void set_y(const float& y)
    {
        _inner_::copybybytes(&y, get(ids::y), sizeof(float));
    }

    bool has_z() const
    {
        return true;
    }
    uint32_t size_z() const
    {
        return sizeof(float);
    }
    float get_z() const
    {
        float dst;
        _inner_::copybybytes(cget(ids::z), &dst, sizeof(float));
        return dst;
    }
    void set_z(const float& z)
    {
        _inner_::copybybytes(&z, get(ids::z), sizeof(float));
    }

    void create(const uint32_t reserve = 0)
    {
        m_from = nullptr;
        m_buffer = std::make_shared<std::vector<uint8_t>>();
        m_buffer->reserve(reserve);
        m_buffer->resize(sizeof(_inner_::header) + sizeof(Table));
        _inner_::header *h = reinterpret_cast<_inner_::header*>(&m_buffer->data()[0]);
        h->signature0  = 'i';
        h->signature1  = 'b';
        m_table = sizeof(_inner_::header);
    }
    bool from(uint8_t* ptr)
    {
        if (!ptr)
            return false;
        const _inner_::header* h = reinterpret_cast<const _inner_::header*>(ptr);
        if (h->signature0 != 'i' ||
            h->signature1 != 'b')
            return false;
        m_buffer.reset();
        m_from = ptr;
        return true;
    }
    bool from(const uint8_t* ptr)
    {
        if (!ptr)
            return false;
        const _inner_::header* h = reinterpret_cast<const _inner_::header*>(ptr);
        if (h->signature0 != 'i' ||
            h->signature1 != 'b')
            return false;
        if (!m_buffer->empty())
            m_buffer->clear();
        m_from = const_cast<uint8_t*>(ptr);
        return true;
    }
    uint8_t* to()
    {
        if (m_from)
            return const_cast<uint8_t*>(m_from);
        else
            return m_buffer->data();
    }
    const uint8_t* cto() const
    {
        if (m_from)
            return m_from;
        else
            return m_buffer->data();
    }
    uint32_t size() const
    {
        if (m_from)
            return 0;
        else
            return static_cast<uint32_t>(m_buffer->size());
    }

    friend class Arrays;
private:
    #pragma pack(push, 1)
    struct Table
    {
        float x;
        float y;
        float z;
    };
    #pragma pack(pop)

    Table* getTable()
    {
        return reinterpret_cast<Table*>(&to()[m_table]);
    }
    const Table* cgetTable() const
    {
        return reinterpret_cast<const Table*>(&cto()[m_table]);
    }
    uint32_t insert(const uint32_t size)
    {
        if (m_from)
            return 0;
        else
        {
            const uint32_t offset = static_cast<uint32_t>(m_buffer->size());
            m_buffer->resize(m_buffer->size() + size);
            return offset;
        }
    }
    uint32_t& address(const ids& id, Table* table)
    {
        switch (id)
        {
        default: return _inner_::zero;
        }
    }
    uint32_t caddress(const ids& id, const Table* table) const
    {
        switch (id)
        {
        default: return _inner_::zero;
        }
    }

    uint8_t* m_from = nullptr;
    std::shared_ptr<std::vector<uint8_t>> m_buffer;
    uint32_t m_table = 0;
};

class Arrays
{
public:
    Arrays(const uint32_t reserve = 0)
    {
        create(reserve);
    }
    Arrays(uint8_t* from_)
    {
        this->from(from_);
    }
    Arrays(const uint8_t* from_)
    {
        this->from(from_);
    }

    enum class ids
    {
        b,
        m,
        v,
        _size_
    };
    static const char* name(const ids& id)
    {
        switch (id)
        {
        case ids::b: return "b"; 
        case ids::m: return "m"; 
        case ids::v: return "v"; 
        default: return "_?unknown_name?_";
        }
    }

    enum class types
    {
        int8, uint8, int8a, uint8a,
        int16, uint16, int16a, uint16a,
        int32, uint32, int32a, uint32a,
        int64, uint64, int64a, uint64a,
        float32, float32a, float64, float64a,
        bytes,
        Vec3f, Vec3fa,
        unknown
    };

    uint32_t has(const ids& id) const
    {
        return caddress(id, cgetTable());
    }
    uint8_t* get(const ids& id)
    {
        Table* table = getTable();
        uint8_t* ptr = to();
        switch (id)
        {
        case ids::b:
        case ids::m:
        case ids::v:
            return &ptr[address(id, table) + sizeof(uint32_t)];
        default:
            return nullptr;
        }
    }
    const uint8_t* cget(const ids& id) const
    {
        const Table* table = cgetTable();
        const uint8_t* ptr = cto();
        switch (id)
        {
        case ids::b:
        case ids::m:
        case ids::v:
            return &ptr[caddress(id, table) + sizeof(uint32_t)];
        default:
            return nullptr;
        }
    }
    void set(const ids& id, const void* data, const uint32_t size)
    {
        switch (id)
        {
        default:
        {
            uint32_t offset = has(id);
            if (offset == 0)
            {
                offset = insert(size + sizeof(uint32_t));
                if (!offset)
                    return;
                *reinterpret_cast<uint32_t*>(&m_buffer->data()[offset]) = size;
                address(id, getTable()) = offset;
            }
            if (data)
                _inner_::copybybytes(data, &m_buffer->data()[offset + sizeof(uint32_t)], size);
            break;
        }
        }
    }
    static types type(const ids& id)
    {
        switch (id)
        {
        case ids::b: return types::bytes;
        case ids::m: return types::int16a;
        case ids::v: return types::Vec3fa;
        default: return types::unknown;
        }
    }
    uint32_t size(const ids& id) const
    {
        uint8_t* ptr = m_from ? m_from : &m_buffer->data()[0];
        Table *table = reinterpret_cast<Table*>(&ptr[sizeof(_inner_::header)]);
        const uint32_t offset = caddress(id, table);
        return *reinterpret_cast<uint32_t*>(&ptr[offset]);
    }

    bool has_b() const
    {
        return has(ids::b) != 0;
    }
    uint32_t size_b() const
    {
        return size(ids::b);
    }
    const uint8_t* get_b()
    {
        return reinterpret_cast<const uint8_t*>(get(ids::b));
    }
    void set_b(const void* data, const uint32_t numberOfBytes)
    {
        set(ids::b, data, numberOfBytes);
    }

    bool has_m() const
    {
        return has(ids::m) != 0;
    }
    uint32_t size_m() const
    {
        return size(ids::m) / sizeof(int16_t);
    }
    const int16_t* get_m()
    {
        return reinterpret_cast<int16_t*>(get(ids::m));
    }
    const int16_t& get_m(const uint32_t index) const
    {
        const int16_t* ptr = reinterpret_cast<const int16_t*>(cget(ids::m));
        if (!ptr)
            throw std::logic_error("Nullptr");
        return ptr[index];
    }
    void set_m(const int16_t* data, const uint32_t numberOfElements)
    {
        set(ids::m, data, numberOfElements * sizeof(int16_t));
    }
    void set_m(const uint32_t index, const int16_t& element)
    {
        int16_t* ptr = reinterpret_cast<int16_t*>(get(ids::m));
        if (!ptr)
            throw std::logic_error("Nullptr");
        _inner_::copybybytes(&element, &ptr[index], sizeof(int16_t));
    }

    bool has_v() const
    {
        return has(ids::v) != 0;
    }
    uint32_t size_v() const
    {
        return size(ids::v) / sizeof(Vec3f::Table);
    }
    Vec3f& get_v(const uint32_t index)
    {
        if (index >= custom.v.size())
            throw std::logic_error("Nullref");
        return custom.v[index];
    }
    void set_v(const uint32_t numberOfElements)
    {
        if (!custom.v.empty())
            throw std::logic_error("Already setted");
        set(ids::v, nullptr, numberOfElements * sizeof(Vec3f::Table));
        custom.v.resize(numberOfElements);
        Table *table = getTable();
        for (uint32_t i = 0; i < numberOfElements; ++i)
        {
            custom.v[i].m_table = address(ids::v, table) + sizeof(Vec3f::Table) * i + sizeof(uint32_t);
            custom.v[i].m_buffer = m_buffer;
        }
    }

    void create(const uint32_t reserve = 0)
    {
        m_from = nullptr;
        m_buffer = std::make_shared<std::vector<uint8_t>>();
        m_buffer->reserve(reserve);
        m_buffer->resize(sizeof(_inner_::header) + sizeof(Table));
        _inner_::header *h = reinterpret_cast<_inner_::header*>(&m_buffer->data()[0]);
        h->signature0  = 'i';
        h->signature1  = 'b';
        m_table = sizeof(_inner_::header);
    }
    bool from(uint8_t* ptr)
    {
        if (!ptr)
            return false;
        const _inner_::header* h = reinterpret_cast<const _inner_::header*>(ptr);
        if (h->signature0 != 'i' ||
            h->signature1 != 'b')
            return false;
        m_buffer.reset();
        m_from = ptr;
        if (getTable()->__v)
        {
            const uint32_t number = size(ids::v) / sizeof(Vec3f::Table);
            custom.v.reserve(number);
            for (uint32_t i = 0; i < number; ++i)
            {
                custom.v.emplace_back();
                custom.v.back().from(ptr);
                auto table = reinterpret_cast<const Vec3f::Table*>(ptr + getTable()->__v + sizeof(uint32_t) + sizeof(Vec3f::Table) * i);
                custom.v.back().m_table = reinterpret_cast<const uint8_t*>(table) - ptr;
            }
        }
        return true;
    }
    bool from(const uint8_t* ptr)
    {
        if (!ptr)
            return false;
        const _inner_::header* h = reinterpret_cast<const _inner_::header*>(ptr);
        if (h->signature0 != 'i' ||
            h->signature1 != 'b')
            return false;
        if (!m_buffer->empty())
            m_buffer->clear();
        m_from = const_cast<uint8_t*>(ptr);
        if (getTable()->__v)
        {
            const uint32_t number = size(ids::v) / sizeof(Vec3f::Table);
            custom.v.reserve(number);
            for (uint32_t i = 0; i < number; ++i)
            {
                custom.v.emplace_back();
                custom.v.back().from(ptr);
                auto table = reinterpret_cast<const Vec3f::Table*>(ptr + getTable()->__v + sizeof(uint32_t) + sizeof(Vec3f::Table) * i);
                custom.v.back().m_table = reinterpret_cast<const uint8_t*>(table) - ptr;
            }
        }
        return true;
    }
    uint8_t* to()
    {
        if (m_from)
            return const_cast<uint8_t*>(m_from);
        else
            return m_buffer->data();
    }
    const uint8_t* cto() const
    {
        if (m_from)
            return m_from;
        else
            return m_buffer->data();
    }
    uint32_t size() const
    {
        if (m_from)
            return 0;
        else
            return static_cast<uint32_t>(m_buffer->size());
    }

private:
    #pragma pack(push, 1)
    struct Table
    {
        uint32_t __b = 0;
        uint32_t __m = 0;
        uint32_t __v = 0;
    };
    #pragma pack(pop)

    Table* getTable()
    {
        return reinterpret_cast<Table*>(&to()[m_table]);
    }
    const Table* cgetTable() const
    {
        return reinterpret_cast<const Table*>(&cto()[m_table]);
    }
    uint32_t insert(const uint32_t size)
    {
        if (m_from)
            return 0;
        else
        {
            const uint32_t offset = static_cast<uint32_t>(m_buffer->size());
            m_buffer->resize(m_buffer->size() + size);
            return offset;
        }
    }
    uint32_t& address(const ids& id, Table* table)
    {
        switch (id)
        {
        case ids::b: return table->__b;
        case ids::m: return table->__m;
        case ids::v: return table->__v;
        default: return _inner_::zero;
        }
    }
    uint32_t caddress(const ids& id, const Table* table) const
    {
        switch (id)
        {
        case ids::b: return table->__b;
        case ids::m: return table->__m;
        case ids::v: return table->__v;
        default: return _inner_::zero;
        }
    }

    struct
    {
        std::vector<Vec3f> v;
    } custom;

    uint8_t* m_from = nullptr;
    std::shared_ptr<std::vector<uint8_t>> m_buffer;
    uint32_t m_table = 0;
};

class Complex
{
public:
    Complex(const uint32_t reserve = 0)
    {
        create(reserve);
    }
    Complex(uint8_t* from_)
    {
        this->from(from_);
    }
    Complex(const uint8_t* from_)
    {
        this->from(from_);
    }

    enum class ids
    {
        _size_
    };
    static const char* name(const ids& id)
    {
        switch (id)
        {
        default: return "_?unknown_name?_";
        }
    }

    enum class types
    {
        int8, uint8, int8a, uint8a,
        int16, uint16, int16a, uint16a,
        int32, uint32, int32a, uint32a,
        int64, uint64, int64a, uint64a,
        float32, float32a, float64, float64a,
        bytes,
        unknown
    };

    uint32_t has(const ids& id) const
    {
        return caddress(id, cgetTable());
    }
    uint8_t* get(const ids& id)
    {
        Table* table = getTable();
        uint8_t* ptr = to();
        switch (id)
        {
        default:
            return nullptr;
        }
    }
    const uint8_t* cget(const ids& id) const
    {
        const Table* table = cgetTable();
        const uint8_t* ptr = cto();
        switch (id)
        {
        default:
            return nullptr;
        }
    }
    void set(const ids& id, const void* data, const uint32_t size)
    {
        switch (id)
        {
        default:
            break;
        }
    }
    static types type(const ids& id)
    {
        switch (id)
        {
        default: return types::unknown;
        }
    }
    uint32_t size(const ids& id) const
    {
        uint8_t* ptr = m_from ? m_from : &m_buffer->data()[0];
        Table *table = reinterpret_cast<Table*>(&ptr[sizeof(_inner_::header)]);
        const uint32_t offset = caddress(id, table);
        return *reinterpret_cast<uint32_t*>(&ptr[offset]);
    }

    void create(const uint32_t reserve = 0)
    {
        m_from = nullptr;
        m_buffer = std::make_shared<std::vector<uint8_t>>();
        m_buffer->reserve(reserve);
        m_buffer->resize(sizeof(_inner_::header) + sizeof(Table));
        _inner_::header *h = reinterpret_cast<_inner_::header*>(&m_buffer->data()[0]);
        h->signature0  = 'i';
        h->signature1  = 'b';
        m_table = sizeof(_inner_::header);
    }
    bool from(uint8_t* ptr)
    {
        if (!ptr)
            return false;
        const _inner_::header* h = reinterpret_cast<const _inner_::header*>(ptr);
        if (h->signature0 != 'i' ||
            h->signature1 != 'b')
            return false;
        m_buffer.reset();
        m_from = ptr;
        return true;
    }
    bool from(const uint8_t* ptr)
    {
        if (!ptr)
            return false;
        const _inner_::header* h = reinterpret_cast<const _inner_::header*>(ptr);
        if (h->signature0 != 'i' ||
            h->signature1 != 'b')
            return false;
        if (!m_buffer->empty())
            m_buffer->clear();
        m_from = const_cast<uint8_t*>(ptr);
        return true;
    }
    uint8_t* to()
    {
        if (m_from)
            return const_cast<uint8_t*>(m_from);
        else
            return m_buffer->data();
    }
    const uint8_t* cto() const
    {
        if (m_from)
            return m_from;
        else
            return m_buffer->data();
    }
    uint32_t size() const
    {
        if (m_from)
            return 0;
        else
            return static_cast<uint32_t>(m_buffer->size());
    }

private:
    #pragma pack(push, 1)
    struct Table
    {
    };
    #pragma pack(pop)

    Table* getTable()
    {
        return reinterpret_cast<Table*>(&to()[m_table]);
    }
    const Table* cgetTable() const
    {
        return reinterpret_cast<const Table*>(&cto()[m_table]);
    }
    uint32_t insert(const uint32_t size)
    {
        if (m_from)
            return 0;
        else
        {
            const uint32_t offset = static_cast<uint32_t>(m_buffer->size());
            m_buffer->resize(m_buffer->size() + size);
            return offset;
        }
    }
    uint32_t& address(const ids& id, Table* table)
    {
        switch (id)
        {
        default: return _inner_::zero;
        }
    }
    uint32_t caddress(const ids& id, const Table* table) const
    {
        switch (id)
        {
        default: return _inner_::zero;
        }
    }

    uint8_t* m_from = nullptr;
    std::shared_ptr<std::vector<uint8_t>> m_buffer;
    uint32_t m_table = 0;
};

}
}

#endif // SCHEMA_GENERATED_H
