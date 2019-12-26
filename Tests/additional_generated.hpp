// This file was generated by IndexBuffers Compiler 0.3-beta.
// https://github.com/yurablok/IndexBuffers
// Do not modify.
#ifndef ADDITIONAL_GENERATED_HPP
#define ADDITIONAL_GENERATED_HPP

#include <cstdint>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include "debug_generated.hpp"

namespace AdditionalNS {

class SomeType { // struct SomeType
public:
    SomeType() {}
    SomeType(void* from_ptr, const uint32_t from_size = 0) {
        from(from_ptr, from_size);
    }
    SomeType(const void* from_ptr, const uint32_t from_size = 0) {
        from(from_ptr, from_size);
    }
    void create(std::shared_ptr<std::vector<uint8_t>> buffer) {
        m_table_offset = 0;
        m_from_ptr = nullptr;
        m_buffer = buffer;
        create(UINT32_MAX);
    }
    void create(const uint32_t reserve = 0) {
        if (reserve != UINT32_MAX) {
            m_table_offset = 0;
            m_from_ptr = nullptr;
            m_buffer.reset();
            m_buffer = std::make_shared<std::vector<uint8_t>>();
            m_buffer->reserve(reserve);
            m_buffer->resize(sizeof(table));
        }
        new(get_table()) table();
    }
    bool from(void* from_ptr, const uint32_t from_size = 0) {
        m_table_offset = 0;
        m_buffer.reset();
        if (from_ptr == nullptr) {
            return false;
        }
        m_from_ptr = reinterpret_cast<uint8_t*>(from_ptr);
        if (from_size > 0) {
            if (from_size < size_min()) {
                return false;
            }
            if (from_size > size_max()) {
                return false;
            }
            m_from_size = from_size;
        }
        else {
            size(1);
        }
        return true;
    }
    bool from(const void* from_ptr, const uint32_t from_size = 0) {
        return from(const_cast<void*>(from_ptr), from_size);
    }
    void from(uint32_t table_offset, const void* from_ptr,
            const uint32_t from_size, std::shared_ptr<std::vector<uint8_t>> buffer) {
        m_table_offset = table_offset;
        m_buffer = buffer;
        m_from_ptr = reinterpret_cast<uint8_t*>(const_cast<void*>(from_ptr));
        m_from_size = from_size;
    }
    void* to() {
        if (m_from_ptr) {
            return m_from_ptr;
        }
        if (m_buffer) {
            return m_buffer->data();
        }
        return nullptr;
    }
    const void* to() const {
        return const_cast<SomeType*>(this)->to();
    }
    uint32_t size(const uint8_t calculate = 0) const {
        if (m_from_ptr) {
            if (calculate > 0) {
                m_from_size = 0;
                if (calculate == 1) {
                    m_from_size += sizeof(table);
                }
                if (has_var()) {
                    m_from_size += size(fields::var);
                }
            }
            return m_from_size;
        }
        if (m_buffer) {
            return static_cast<uint32_t>(m_buffer->size());
        }
        return 0;
    }
    static uint32_t size_min() {
        return 8;
    }
    static uint32_t size_max() {
        return 12;
    }
    
    struct fields { // enum fields
    enum _ : uint32_t {
        var = 0,
        def = 1,
        _SPECIAL_ = 2
    };
    static const char* to_string(const _ enum_value) {
        switch(enum_value) {
        case var: return "var";
        case def: return "def";
        default: break;
        }
        return nullptr;
    }
    template <typename string_t>
    static _ from_string(const string_t& string_value) {
        static const std::unordered_map<string_t, _> map = {
            { "var", var },
            { "def", def }
        };
        const auto it = map.find(string_value);
        if (it == map.end()) {
            return _SPECIAL_;
        }
        return it->second;
    }
    static _ from_string(const char* string_value) {
        return from_string(std::string(string_value));
    }
    static constexpr _ min() {
        return var;
    }
    static constexpr _ max() {
        return def;
    }
    static constexpr uint32_t count() {
        return 2;
    }
    static _ at(const uint32_t index) {
        switch (index) {
        case 0: return var;
        case 1: return def;
        default: break;
        }
        return _SPECIAL_;
    }
    }; // enum fields

    uint32_t offset(const fields::_ field) const {
        static const table o{};
        const table* t = get_table();
        switch (field) {
        case fields::var: return t->__var;
        case fields::def: return m_table_offset + static_cast<uint32_t>(
            reinterpret_cast<uintptr_t>(&o.def) - reinterpret_cast<uintptr_t>(&o));
        }
        return 0;
    }
    bool has(const fields::_ field) const {
        switch (field) {
        case fields::var: return offset(field) > 0;
        case fields::def: return true;
        }
        return false;
    }
    void* get(const fields::_ field) {
        switch (field) {
        case fields::var: return reinterpret_cast<uint8_t*>(
            base_ptr()) + offset(field);
        case fields::def: return reinterpret_cast<uint8_t*>(
            base_ptr()) + offset(field);
        }
        return nullptr;
    }
    const void* get(const fields::_ field) const {
        return const_cast<SomeType*>(this)->get(field);
    }
    void set(const fields::_ field, const void* data, const uint32_t size) {
        if (!has(field)) {
            if (m_from_ptr) {
                return;
            }
            switch (field) {
            case fields::var:
                get_table()->__var = static_cast<uint32_t>(m_buffer->size());
                m_buffer->resize(m_buffer->size()
                    + sizeof(uint32_t));
                break;
            default: return;
            }
        }
        if (data) {
            std::copy(reinterpret_cast<const uint8_t*>(data),
                reinterpret_cast<const uint8_t*>(data) + size,
                reinterpret_cast<uint8_t*>(get(field)));
        }
    }
    uint32_t size(const fields::_ field) const {
        if (!has(field)) {
            return 0;
        }
        switch (field) {
        case fields::var:
            return sizeof(uint32_t);
        case fields::def:
            return sizeof(int32_t);
        default:
            return 0;
        }
        return 0;
    }

    bool has_var() const {
        return has(fields::var);
    }
    uint32_t get_var() {
        return *reinterpret_cast<uint32_t*>(get(fields::var));
    }
    const uint32_t& get_var() const {
        return const_cast<SomeType*>(this)->get_var();
    }
    void set_var(const uint32_t value) {
        set(fields::var, &value, sizeof(uint32_t));
    }
    bool has_def() const {
        return has(fields::def);
    }
    int32_t get_def() {
        return *reinterpret_cast<int32_t*>(get(fields::def));
    }
    const int32_t& get_def() const {
        return const_cast<SomeType*>(this)->get_def();
    }
    void set_def(const int32_t value) {
        set(fields::def, &value, sizeof(int32_t));
    }

    #pragma pack(1)
    struct table {
        uint32_t __var = 0;
        int32_t def = -123;
    };
    #pragma pack()
private:
    table* get_table() {
        #ifdef _DEBUG
        m_table = reinterpret_cast<table*>(
            reinterpret_cast<uint8_t*>(base_ptr()) + m_table_offset);
        #endif // _DEBUG
        return reinterpret_cast<table*>(
            reinterpret_cast<uint8_t*>(base_ptr()) + m_table_offset);
    }
    const table* get_table() const {
        return const_cast<SomeType*>(this)->get_table();
    }
    void* base_ptr() {
        if (m_buffer) {
            if (m_buffer->empty()) {
                return nullptr;
            }
            return m_buffer->data();
        }
        return m_from_ptr;
    }
    const void* base_ptr() const {
        return const_cast<SomeType*>(this)->base_ptr();
    }
    #ifdef _DEBUG
    table* m_table = nullptr;
    #endif // _DEBUG
    std::shared_ptr<std::vector<uint8_t>> m_buffer;
    uint8_t* m_from_ptr = nullptr;
    mutable uint32_t m_from_size = 0;
    uint32_t m_table_offset = 0;
}; // struct SomeType

struct TestEnum2 { // enum TestEnum2
enum _ : uint32_t {
    one = 0,
    two = 2,
    three = 4,
    _SPECIAL_ = 5
};
static const char* to_string(const _ enum_value) {
    switch(enum_value) {
    case one: return "one";
    case two: return "two";
    case three: return "three";
    default: break;
    }
    return nullptr;
}
template <typename string_t>
static _ from_string(const string_t& string_value) {
    static const std::unordered_map<string_t, _> map = {
        { "one", one },
        { "two", two },
        { "three", three }
    };
    const auto it = map.find(string_value);
    if (it == map.end()) {
        return _SPECIAL_;
    }
    return it->second;
}
static _ from_string(const char* string_value) {
    return from_string(std::string(string_value));
}
static constexpr _ min() {
    return one;
}
static constexpr _ max() {
    return three;
}
static constexpr uint32_t count() {
    return 3;
}
static _ at(const uint32_t index) {
    switch (index) {
    case 0: return one;
    case 1: return two;
    case 2: return three;
    default: break;
    }
    return _SPECIAL_;
}
}; // enum TestEnum2

} // AdditionalNS

#endif // ADDITIONAL_GENERATED_HPP
