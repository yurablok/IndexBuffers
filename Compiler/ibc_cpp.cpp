#include "stdafx.h"
#include "ibc.h"
#include "Utf8Ucs2Converter.h"
#include "version.h"

namespace {
    std::string kw_to_domain_specific(const kw keyword) {
        switch (keyword) {
        case kw::Include:   return "include";
        case kw::Namespace: return "namespace";
        case kw::Enum:      return "enum";
        case kw::Struct:    return "struct";
        case kw::Const:     return "const";
        case kw::Union:     return "union";
        case kw::Int8:      return "int8_t";
        case kw::UInt8:     return "uint8_t";
        case kw::Int16:     return "int16_t";
        case kw::UInt16:    return "uint16_t";
        case kw::Int32:     return "int32_t";
        case kw::UInt32:    return "uint32_t";
        case kw::Int64:     return "int64_t";
        case kw::UInt64:    return "uint64_t";
        case kw::Float32:   return "float";
        case kw::Float64:   return "double";
        default:            return std::string();
        }
    }

    static const std::map<std::string, std::string> standard = {
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
        { "bytes"   , "bytes"    }
    };

    struct WritingMeta {
        const std::deque<std::string>* namespacePtr = nullptr;
        std::string spacing;
        kw previous = kw::UNDEFINED;
    };

    void writeConst(WritingMeta& writingMeta, std::ostream& output,
            const AST& ast, const AST::ObjectMeta* objectMeta) {
        const AST::ConstMeta* constMeta = objectMeta->constMeta();
        const std::string type = kw_to_domain_specific(constMeta->type);
        if (writingMeta.previous != kw::Const) {
            output << std::endl;
        }
        output << "static constexpr " << type << " "
            << constMeta->name << " = ";
        if (isScalarType(constMeta->valueKw)) {
            output << constMeta->valueStr << ";";
        }
        else {
            std::string namespace_;
            if (!AST::isNamespacesEqual(
                constMeta->valuePtr->namespace_,
                *writingMeta.namespacePtr)) {

                for (const auto& ns : objectMeta->namespace_) {
                    namespace_ += ns;
                    namespace_ += "::";
                }
            }
            switch (constMeta->valueKw) {
            case kw::Const:
                output << namespace_ << constMeta->valuePtr->constMeta()->name << ";";
                break;
            case kw::Enum: {
                const auto& value = constMeta->valuePtr->enumMeta()->
                    valuesVec[constMeta->valueIdx];
                output << namespace_ << constMeta->valuePtr->enumMeta()->name
                    << "::" << value.first << ";";
                break;
            }
            case kw::Min:
                output << namespace_ << constMeta->valuePtr->enumMeta()->
                        valuesVec.front().second
                    << "; // " << namespace_ << constMeta->valuePtr->enumMeta()->name
                    << ".min";
                break;
            case kw::Max:
                output << namespace_ << constMeta->valuePtr->enumMeta()->
                        valuesVec.back().second
                    << "; // " << namespace_ << constMeta->valuePtr->enumMeta()->name
                    << ".max";
                break;
            case kw::Count:
                output << namespace_ << constMeta->valuePtr->enumMeta()->
                        valuesVec.size()
                    << "; // " << namespace_ << constMeta->valuePtr->enumMeta()->name
                    << ".count";
                break;
            default:
                output << "$ERROR$";
                break;
            }
        }
        output << std::endl;
    }

    void writeEnum(WritingMeta& writingMeta, std::ostream& output,
            const AST& ast, const AST::ObjectMeta* objectMeta) {
        const AST::EnumMeta* enumMeta = objectMeta->enumMeta();
        output << writingMeta.spacing << std::endl;
        output << writingMeta.spacing << "struct " << enumMeta->name
            << " { // enum " << enumMeta->name << std::endl;
        output << writingMeta.spacing << "virtual void __better_enum() = 0;" << std::endl;
        output << writingMeta.spacing << "enum _ : "
            << kw_to_domain_specific(enumMeta->type) << " {" << std::endl;
        for (const auto& value : enumMeta->valuesVec) {
            output << writingMeta.spacing << "    " << value.first
                << " = " << value.second << "," << std::endl;
        }
        output << writingMeta.spacing << "    " << "_SPECIAL_" << " = "
            << enumMeta->valuesVec.back().second + 1 << std::endl;
        output << writingMeta.spacing << "};" << std::endl;
        output << writingMeta.spacing
            << "static const char* to_string(const _ __value__) {" << std::endl;
        output << writingMeta.spacing << "    " << "switch(__value__) {" << std::endl;
        for (const auto& value : enumMeta->valuesVec) {
            output << writingMeta.spacing << "    " << "case " << value.first
                << ": return \"" << value.first << "\";" << std::endl;
        }
        output << writingMeta.spacing << "    " << "default: break;" << std::endl;
        output << writingMeta.spacing << "    " << "}" << std::endl;
        output << writingMeta.spacing << "    " << "return nullptr;" << std::endl;
        output << writingMeta.spacing << "}" << std::endl;
        output << writingMeta.spacing << "template <typename string_t>" << std::endl;
        output << writingMeta.spacing
            << "static _ from_string(const string_t& __value__) {" << std::endl;
        output << writingMeta.spacing << "    "
            << "static const std::unordered_map<string_t, _> map = {" << std::endl;
        for (uint32_t i = 0; i < enumMeta->valuesVec.size(); ++i) {
            const auto& value = enumMeta->valuesVec[i];
            output << writingMeta.spacing << "    " << "    " << "{ \""
                << value.first << "\", " << value.first << " }";
            if (i != enumMeta->valuesVec.size() - 1) {
                output << ",";
            }
            output << std::endl;
        }
        output << writingMeta.spacing << "    " << "};" << std::endl;
        output << writingMeta.spacing << "    "
            << "const auto it = map.find(__value__);" << std::endl;
        output << writingMeta.spacing << "    "
            << "if (it == map.end()) {" << std::endl;
        output << writingMeta.spacing << "    "
            << "    " << "return _SPECIAL_;" << std::endl;
        output << writingMeta.spacing << "    " << "}" << std::endl;
        output << writingMeta.spacing << "    "
            << "return it->second;" << std::endl;
        output << writingMeta.spacing << "}" << std::endl;
        output << writingMeta.spacing << "static _ from_string(const char* __value__) {" << std::endl;
        output << writingMeta.spacing << "    " <<
            "return from_string(std::string(__value__));" << std::endl;
        output << writingMeta.spacing << "}" << std::endl;
        output << writingMeta.spacing << "static constexpr _ min() {" << std::endl;
        output << writingMeta.spacing << "    "
            << "return " << enumMeta->valuesVec.front().first << ";" << std::endl;
        output << writingMeta.spacing << "}" << std::endl;
        output << writingMeta.spacing << "static constexpr _ max() {" << std::endl;
        output << writingMeta.spacing << "    "
            << "return " << enumMeta->valuesVec.back().first << ";" << std::endl;
        output << writingMeta.spacing << "}" << std::endl;
        output << writingMeta.spacing << "static constexpr "
            << kw_to_domain_specific(enumMeta->type) << " count() {" << std::endl;
        output << writingMeta.spacing << "    " << "return "
            << enumMeta->valuesVec.size() << ";" << std::endl;
        output << writingMeta.spacing << "}" << std::endl;
        output << writingMeta.spacing << "static _ at(const "
            << kw_to_domain_specific(enumMeta->type) << " __value__) {" << std::endl;
        output << writingMeta.spacing << "    " << "switch (__value__) {" << std::endl;
        for (uint32_t i = 0; i < enumMeta->valuesVec.size(); ++i) {
            output << writingMeta.spacing << "    " << "case " << i << ": return "
                << enumMeta->valuesVec[i].first << ";" << std::endl;
        }
        output << writingMeta.spacing << "    " << "default: break;" << std::endl;
        output << writingMeta.spacing << "    " << "}" << std::endl;
        output << writingMeta.spacing << "    " << "return _SPECIAL_;" << std::endl;
        output << writingMeta.spacing << "}" << std::endl;
        output << writingMeta.spacing << "}; // enum " << enumMeta->name << std::endl;
    }

    void writeStruct_create(WritingMeta& writingMeta, std::ostream& output,
            const AST& ast, const AST::StructMeta* structMeta) {
        //output << "    " << "void create(void* external_ptr, const "
        //    << structMeta->offsetTypeStr << " external_size) {" << std::endl;
        //output << "    " << "    " << "m_table_offset = 0;" << std::endl;
        //output << "    " << "    " << "m_from_ptr = reinterpret_cast<uint8_t*>(external_ptr);" << std::endl;
        //output << "    " << "    " << "m_from_size = 0;" << std::endl;
        //output << "    " << "    " << "m_buffer.reset();" << std::endl;
        //output << "    " << "    " << "create(UINT32_MAX);" << std::endl;
        //output << "    " << "}" << std::endl;
        output << "    " << "void create(std::shared_ptr<std::vector<uint8_t>> buffer) {" << std::endl;
        output << "    " << "    " << "m_from_ptr = nullptr;" << std::endl;
        output << "    " << "    " << "m_buffer = buffer;" << std::endl;
        output << "    " << "    " << "m_buffer->clear();" << std::endl;
        if (structMeta->withHeader) {
            output << "    " << "    " << "m_table_offset = sizeof(header);" << std::endl;
        }
        else {
            output << "    " << "    " << "m_table_offset = 0;" << std::endl;
        }
        output << "    " << "    " << "create(0);" << std::endl;
        output << "    " << "}" << std::endl;
        output << "    " << "void create(const uint32_t reserve = 0) {" << std::endl;
        output << "    " << "    " << "if (reserve != UINT32_MAX) {" << std::endl;
        output << "    " << "    " << "    " << "m_from_ptr = nullptr;" << std::endl;
        output << "    " << "    " << "    " << "m_buffer.reset();" << std::endl;
        output << "    " << "    " << "    " << "m_buffer = std::make_shared<"
            "std::vector<uint8_t>>();" << std::endl;
        output << "    " << "    " << "    " << "m_buffer->reserve(reserve);" << std::endl;
        if (structMeta->withHeader) {
            output << "    " << "    " << "    " << "m_buffer->resize(sizeof(header) + sizeof(table));" << std::endl;
            output << "    " << "    " << "    " << "m_table_offset = sizeof(header);" << std::endl;
            output << "    " << "    " << "    " << "new(base_ptr()) header();" << std::endl;
        }
        else {
            output << "    " << "    " << "    " << "m_buffer->resize(sizeof(table));" << std::endl;
            output << "    " << "    " << "    " << "m_table_offset = 0;" << std::endl;
        }
        output << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "new(get_table()) table();" << std::endl;
        for (const auto& field : structMeta->fieldsVec) {
            if (field->isOptional || field->isBuiltIn
                || field->isArray || field->typeKw == kw::Enum) {
                continue;
            }
            output << "    " << "    " << "get_" << field->name << "().create(UINT32_MAX);" << std::endl;
        }
        output << "    " << "}" << std::endl;

    }
    void writeStruct_from_to(WritingMeta& writingMeta, std::ostream& output,
            const AST& ast, const AST::StructMeta* structMeta) {
        output << "    " << "bool from(void* from_ptr, const "
            << structMeta->offsetTypeStr << " from_size = 0) {" << std::endl;
        output << "    " << "    " << "m_buffer.reset();" << std::endl;
        //TODO: m_is_read_only
        //output << "    " << "    " << "//TODO: m_is_read_only" << std::endl;
        output << "    " << "    " << "if (from_ptr == nullptr) {" << std::endl;
        output << "    " << "    " << "    " << "return false;" << std::endl;
        output << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "m_from_ptr = reinterpret_cast<uint8_t*>(from_ptr);" << std::endl;
        if (structMeta->withHeader) {
            output << "    " << "    " << "m_table_offset = sizeof(header);" << std::endl;
        }
        else {
            output << "    " << "    " << "m_table_offset = 0;" << std::endl;
        }
        output << "    " << "    " << "if (from_size > 0) {" << std::endl;
        output << "    " << "    " << "    " << "if (from_size < size_min()) {" << std::endl;
        output << "    " << "    " << "    " << "    " << "return false;" << std::endl;
        output << "    " << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "    " << "if (from_size > size_max()) {" << std::endl;
        output << "    " << "    " << "    " << "    " << "return false;" << std::endl;
        output << "    " << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "    " << "m_from_size = from_size;" << std::endl;
        output << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "else {" << std::endl;
        output << "    " << "    " << "    " << "size(3);" << std::endl;
        output << "    " << "    " << "}" << std::endl;
        if (structMeta->withHeader) {
            output << "    " << "    " << "auto h = reinterpret_cast<const header*>(base_ptr());" << std::endl;
            output << "    " << "    " << "if (h->___i != 'i' || h->___b != 'b' || h->___s != 's') {" << std::endl;
            output << "    " << "    " << "    " << "return false;" << std::endl;
            output << "    " << "    " << "}" << std::endl;
            output << "    " << "    " << "if (h->___sh != " << structMeta->schemaHash << ") {" << std::endl;
            output << "    " << "    " << "    " << "return false;" << std::endl;
            output << "    " << "    " << "}" << std::endl;
        }
        output << "    " << "    " << "return true;" << std::endl;
        output << "    " << "}" << std::endl;
        output << "    " << "bool from(const void* from_ptr, const "
            << structMeta->offsetTypeStr << " from_size = 0) {" << std::endl;
        output << "    " << "    " << "return from(const_cast<void*>(from_ptr), from_size);" << std::endl;
        output << "    " << "}" << std::endl;
        output << "    " << "void from(" << structMeta->offsetTypeStr
            << " table_offset, const void* from_ptr," << std::endl;
        output << "    " << "    " << "    " << "const " << structMeta->offsetTypeStr
            << " from_size, std::shared_ptr<std::vector<uint8_t>> buffer) {" << std::endl;
        output << "    " << "    " << "m_table_offset = table_offset;" << std::endl;
        output << "    " << "    " << "m_buffer = buffer;" << std::endl;
        output << "    " << "    " << "m_from_ptr = reinterpret_cast<uint8_t*>("
            "const_cast<void*>(from_ptr));" << std::endl;
        output << "    " << "    " << "m_from_size = from_size;" << std::endl;
        output << "    " << "}" << std::endl;
        output << "    " << "void* to() {" << std::endl;
        output << "    " << "    " << "if (m_from_ptr) {" << std::endl;
        output << "    " << "    " << "    " << "return m_from_ptr;" << std::endl;
        output << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "if (m_buffer) {" << std::endl;
        output << "    " << "    " << "    " << "return m_buffer->data();" << std::endl;
        output << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "return nullptr;" << std::endl;
        output << "    " << "}" << std::endl;
        output << "    " << "const void* to() const {" << std::endl;
        output << "    " << "    " << "return const_cast<" << structMeta->name
            << "*>(this)->to();" << std::endl;
        output << "    " << "}" << std::endl;
    }
    void writeStruct_size(WritingMeta& writingMeta, std::ostream& output,
            const AST& ast, const AST::StructMeta* structMeta) {
        // calculate:
        //  0 - nothing
        //  1 - data, fixed in struct
        //  2 - table + data, optional in struct/union
        //  3 - header + table + data, from root
        output << "    " << structMeta->offsetTypeStr << " size(const uint8_t calculate = 0) const {" << std::endl;
        output << "    " << "    " << "if (m_from_ptr) {" << std::endl;
        output << "    " << "    " << "    " << "if (calculate > 0) {" << std::endl;
        output << "    " << "    " << "    " << "    " << "m_from_size = 0;" << std::endl;
        output << "    " << "    " << "    " << "    " << "if (calculate > 1) {" << std::endl;
        output << "    " << "    " << "    " << "    " << "    " << "m_from_size += sizeof(table);" << std::endl;
        output << "    " << "    " << "    " << "    " << "}" << std::endl;
        if (structMeta->withHeader) {
            output << "    " << "    " << "    " << "    " << "if (calculate > 2) {" << std::endl;
            output << "    " << "    " << "    " << "    " << "    " << "m_from_size += sizeof(header);" << std::endl;
            output << "    " << "    " << "    " << "    " << "}" << std::endl;
        }
        for (const auto& field : structMeta->fieldsVec) {
            // 0. fixed scalar               ---
            // 1. fixed struct               get_x().size(1)
            // 2. fixed array of scalars     ---
            // 3. fixed array of structs     for size_x(): get_x(i).size(1)
            // 4. optional scalar            size_x()
            // 5. optional struct            has_x() get_x().size(2)
            // 6. optional array of scalars  sizeof(offset) + size_x() * sizeof(typeof(x))
            // 7. optional array of structs  sizeof(offset) + for size_x(): get_x(i).size(2)
            if (field->typeKw == kw::Bytes) {
                if (!field->isOptional) {
                    continue;
                }
                output << "    " << "    " << "    " << "    "
                    << "m_from_size += size_" << field->name
                    << "() + sizeof(" << kw_to_domain_specific(field->arraySizeKw) << ");" << std::endl;
            }
            else if (field->isOptional) {
                output << "    " << "    " << "    " << "    "
                    << "if (has_" << field->name << "()) {" << std::endl;
                if (field->isArray) {
                    if (field->isScalar) { // 6
                        output << "    " << "    " << "    " << "    " << "    "
                            << "m_from_size += size_" << field->name << "() * sizeof("
                            << kw_to_domain_specific(field->typeKw) << ")";
                        if (field->arrayKw == kw::UNDEFINED) {
                            output << " + sizeof("
                                << kw_to_domain_specific(field->arraySizeKw) << ")";
                        }
                        output << ";" << std::endl;
                    }
                    else { // 7
                        output << "    " << "    " << "    " << "    " << "    "
                            << "for (" << structMeta->offsetTypeStr << " i = 0, s = size_"
                            << field->name << "(); i < s; ++i) {" << std::endl;
                        output << "    " << "    " << "    " << "    " << "    "
                            << "    " << "m_from_size += get_" << field->name
                            << "(i).size(2);" << std::endl;
                        output << "    " << "    " << "    " << "    " << "    "
                            << "}";
                        if (field->arrayKw == kw::UNDEFINED) {
                            output << " m_from_size += sizeof("
                                << kw_to_domain_specific(field->arraySizeKw) << ");";
                        }
                        output << std::endl;
                    }
                }
                else {
                    if (field->isScalar) { // 4
                        output << "    " << "    " << "    " << "    " << "    "
                            << "m_from_size += size(fields::" << field->name << ");" << std::endl;
                    }
                    else { // 5
                        output << "    " << "    " << "    " << "    " << "    "
                            << "m_from_size += get_" << field->name
                            << "().size(2);" << std::endl;
                    }
                }
                output << "    " << "    " << "    " << "    "
                    << "}" << std::endl;
            }
            else {
                if (field->isArray) {
                    if (field->isScalar) { // 2
                    }
                    else { // 3
                        output << "    " << "    " << "    " << "    "
                            << "for (" << structMeta->offsetTypeStr << " i = 0, s = size_"
                            << field->name << "(); i < s; ++i) {" << std::endl;
                        output << "    " << "    " << "    " << "    "
                            << "    " << "m_from_size += get_" << field->name
                            << "(i).size(1);" << std::endl;
                        output << "    " << "    " << "    " << "    " << "}" << std::endl;
                    }
                }
                else {
                    if (field->isScalar) { // 0
                    }
                    else { // 1
                        output << "    " << "    " << "    " << "    "
                            << "m_from_size += get_" << field->name
                            << "().size(1);" << std::endl;
                    }
                }
            }
        }
        output << "    " << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "    " << "return m_from_size;" << std::endl;
        output << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "if (m_buffer) {" << std::endl;
        output << "    " << "    " << "    "
            << "return static_cast<" << structMeta->offsetTypeStr << ">(m_buffer->size());" << std::endl;
        output << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "return 0;" << std::endl;
        output << "    " << "}" << std::endl;
        uint64_t sizeMin = 0;
        uint64_t sizeMax = 0;
        structMeta->calcSizeMinMax(sizeMin, sizeMax);
        if (sizeMax == UINT64_MAX) {
            switch (structMeta->offsetType) {
            case kw::UInt8: sizeMax = UINT8_MAX; break;
            case kw::UInt16: sizeMax = UINT16_MAX; break;
            case kw::UInt32: sizeMax = UINT32_MAX; break;
            default: break;
            }
        }
        output << "    " << "static " << structMeta->offsetTypeStr << " size_min() {" << std::endl;
        output << "    " << "    " << "return " << sizeMin << ";" << std::endl;
        output << "    " << "}" << std::endl;
        output << "    " << "static " << structMeta->offsetTypeStr << " size_max() {" << std::endl;
        output << "    " << "    " << "return " << sizeMax << ";" << std::endl;
        output << "    " << "}" << std::endl;
    }
    void writeStruct_offset(WritingMeta& writingMeta, std::ostream& output,
            const AST& ast, const AST::StructMeta* structMeta) {
        output << "    " << structMeta->offsetTypeStr << " offset(const fields::_ field) const {" << std::endl;
        output << "    " << "    " << "static const table o{};" << std::endl;
        output << "    " << "    " << "const table* t = get_table();" << std::endl;
        output << "    " << "    " << "switch (field) {" << std::endl;
        for (const auto& field : structMeta->fieldsVec) {
            output << "    " << "    " << "case fields::" << field->name << ": return ";
            if (field->isOptional) {
                output << "t->__" << field->name << ";";
            }
            else {
                output << "m_table_offset + static_cast<" << structMeta->offsetTypeStr << ">(" << std::endl;
                output << "    " << "    " << "    "
                    << "reinterpret_cast<uintptr_t>(&o" << "." << field->name
                    << ") - " << "reinterpret_cast<uintptr_t>(&o" << "));";
            }
            output << std::endl;
        }
        output << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "return 0;" << std::endl;
        output << "    " << "}" << std::endl;
    }
    void writeStruct_has(WritingMeta& writingMeta, std::ostream& output,
            const AST& ast, const AST::StructMeta* structMeta) {
        output << "    " << "bool has(const fields::_ field) const {" << std::endl;
        output << "    " << "    " << "switch (field) {" << std::endl;
        for (const auto& field : structMeta->fieldsVec) {
            output << "    " << "    " << "case fields::" << field->name << ": ";
            if (field->isOptional) {
                output << "return offset(field) > 0;";
            }
            else {
                output << "return true;";
            }
            output << std::endl;
        }
        output << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "return false;" << std::endl;
        output << "    " << "}" << std::endl;
    }
    void writeStruct_get(WritingMeta& writingMeta, std::ostream& output,
            const AST& ast, const AST::StructMeta* structMeta) {
        output << "    " << "void* get(const fields::_ field) {" << std::endl;
        output << "    " << "    " << "switch (field) {" << std::endl;
        for (const auto& field : structMeta->fieldsVec) {
            output << "    " << "    " << "case fields::" << field->name
                << ": return reinterpret_cast<uint8_t*>(" << std::endl;
            output << "    " << "    " << "    " << "base_ptr()) + offset(field)";
            if (field->isArray && field->arrayKw == kw::UNDEFINED) {
                output << " + sizeof(" << kw_to_domain_specific(field->arraySizeKw) << ")";
            }
            output << ";" << std::endl;
        }
        output << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "return nullptr;" << std::endl;
        output << "    " << "}" << std::endl;
        output << "    " << "const void* get(const fields::_ field) const {" << std::endl;
        output << "    " << "    " << "return const_cast<"
            << structMeta->name << "*>(this)->get(field);" << std::endl;
        output << "    " << "}" << std::endl;
    }
    void writeStruct_set(WritingMeta& writingMeta, std::ostream& output,
            const AST& ast, const AST::StructMeta* structMeta) {
        output << "    " << "void set(const fields::_ field, const void* data, const "
            << structMeta->offsetTypeStr << " size) {" << std::endl;
        output << "    " << "    " << "if (!has(field)) {" << std::endl;
        output << "    " << "    " << "    " << "if (m_from_ptr) {" << std::endl;
        output << "    " << "    " << "    " << "    " << "return;" << std::endl;
        output << "    " << "    " << "    " << "}" << std::endl;
        if (structMeta->optionalCount > 0) {
            output << "    " << "    " << "    " << "switch (field) {" << std::endl;
            for (const auto& field : structMeta->fieldsVec) {
                if (!field->isOptional) {
                    continue;
                }
                output << "    " << "    " << "    " << "case fields::"
                    << field->name << ":" << std::endl;
                output << "    " << "    " << "    " << "    "
                    << "get_table()->__" << field->name << " = static_cast<"
                    << structMeta->offsetTypeStr << ">(m_buffer->size());" << std::endl;
                // 4. optional scalar            sizeof(typeof(x))
                // 5. optional struct            sizeof(x::table)
                // 6. optional array of scalars  sizeof(typeof(x)) * size + sizeof(offset)
                // 7. optional array of structs  sizeof(x::table) * size + sizeof(offset)
                std::string size;
                if (field->isArray) {
                    if (field->arrayKw != kw::UNDEFINED) {
                        if (isScalarType(field->arrayKw)) {
                            size = std::to_string(field->arraySize);
                        }
                        else {
                            for (const auto& ns : field->arrayPtr->namespace_) {
                                size += ns;
                                size += "::";
                            }
                            switch (field->arrayKw) {
                            case kw::Const:
                                size += field->arrayPtr->constMeta()->name;
                                break;
                            case kw::Enum:
                                size += field->arrayPtr->enumMeta()->name;
                                size += "::";
                                size += field->arrayPtr->enumMeta()->valuesVec[
                                    field->arrayIdx].first;
                                break;
                            case kw::Min:
                                size += field->arrayPtr->enumMeta()->name;
                                size += "::min()";
                                break;
                            case kw::Max:
                                size += field->arrayPtr->enumMeta()->name;
                                size += "::max()";
                                break;
                            case kw::Count:
                                size += field->arrayPtr->enumMeta()->name;
                                size += "::count()";
                                break;
                            default:
                                size += "%ERROR%";
                                break;
                            }
                        }
                    }
                    else {
                        size = "size + sizeof(";
                        size += kw_to_domain_specific(field->arraySizeKw);
                        size += ")";
                    }
                }
                output << "    " << "    " << "    " << "    "
                    << "m_buffer->resize(m_buffer->size()" << std::endl;
                output << "    " << "    " << "    " << "    " << "    " << "+ ";
                if (field->typeKw == kw::Bytes) {
                    output << "sizeof(" << kw_to_domain_specific(kw::UInt8) << ") * " << size;
                }
                else {
                    if (field->isArray) {
                        if (field->isScalar) { // 6
                            output << "sizeof("
                                << kw_to_domain_specific(field->typeKw) << ") * " << size;
                        }
                        else { // 7
                            output << "sizeof(";
                            for (const auto& ns : field->typePtr->namespace_) {
                                output << ns << "::";
                            }
                            switch (field->typeKw) {
                            case kw::Enum:
                                output << kw_to_domain_specific(field->typePtr->enumMeta()->type);
                                break;
                            case kw::Struct:
                                output << field->typePtr->structMeta()->name << "::table";
                                break;
                            case kw::Union:
                                output << field->typePtr->unionMeta()->name << "::table";
                                break;
                            default:
                                output << "%ERROR%";
                                break;
                            }
                            output << ") * " << size;
                        }
                    }
                    else {
                        if (field->isScalar) { // 4
                            output << "sizeof("
                                << kw_to_domain_specific(field->typeKw) << ")";
                        }
                        else { // 5
                            output << "sizeof(";
                            for (const auto& ns : field->typePtr->namespace_) {
                                output << ns << "::";
                            }
                            switch (field->typeKw) {
                            case kw::Enum:
                                output << kw_to_domain_specific(field->typePtr->enumMeta()->type);
                                break;
                            case kw::Struct:
                                output << field->typePtr->structMeta()->name << "::table";
                                break;
                            case kw::Union:
                                output << field->typePtr->unionMeta()->name << "::table";
                                break;
                            default:
                                output << "%ERROR%";
                                break;
                            }
                            output << ")";
                        }
                    }
                }
                output << ");" << std::endl;
                if (field->isArray && field->arrayKw == kw::UNDEFINED) {
                    output << "    " << "    " << "    " << "    "
                        << "*reinterpret_cast<" << structMeta->offsetTypeStr
                        << "*>((reinterpret_cast<uint8_t*>(" << std::endl;
                    output << "    " << "    " << "    " << "    " << "    "
                        << "base_ptr()) + get_table()->__" << field->name << ")) = size;" << std::endl;
                }
                if (!field->isBuiltIn) {
                    if (field->isArray) {
                        output << "    " << "    " << "    " << "    "
                            << "for (" << structMeta->offsetTypeStr
                            << " i = 0; i < size; ++i) {" << std::endl;
                        output << "    " << "    " << "    " << "    " << "    "
                            << "get_" << field->name << "(i).create(UINT32_MAX);" << std::endl;
                        output << "    " << "    " << "    " << "    "
                            << "}" << std::endl;
                    }
                    else {
                        output << "    " << "    " << "    " << "    "
                            << "get_" << field->name << "().create(UINT32_MAX);" << std::endl;
                    }
                }
                output << "    " << "    " << "    " << "    "
                    << "break;" << std::endl;
            }
            output << "    " << "    " << "    " << "default: return;" << std::endl;
            output << "    " << "    " << "    " << "}" << std::endl;
        }
        output << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "if (data) {" << std::endl;
        output << "    " << "    " << "    " << "std::copy(reinterpret_cast<const uint8_t*>(data)," << std::endl;
        output << "    " << "    " << "    " << "    " << "reinterpret_cast<const uint8_t*>(data) + size," << std::endl;
        output << "    " << "    " << "    " << "    " << "reinterpret_cast<uint8_t*>(get(field)));" << std::endl;
        output << "    " << "    " << "}" << std::endl;
        output << "    " << "}" << std::endl;
    }
    void writeStruct_size_field(WritingMeta& writingMeta, std::ostream& output,
            const AST& ast, const AST::StructMeta* structMeta) {
        output << "    " << structMeta->offsetTypeStr << " size(const fields::_ field) const {" << std::endl;
        output << "    " << "    " << "if (!has(field)) {" << std::endl;
        output << "    " << "    " << "    " << "return 0;" << std::endl;
        output << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "switch (field) {" << std::endl;
        for (const auto& field : structMeta->fieldsVec) {
            output << "    " << "    " << "case fields::" << field->name << ":" << std::endl;
            output << "    " << "    " << "    " << "return ";
            if (field->isArray) {
                if (field->arrayKw == kw::UNDEFINED) {
                    output << "*(reinterpret_cast<const "
                        << kw_to_domain_specific(field->arraySizeKw)
                        << "*>(get(field)) - 1)";
                }
                else if (isScalarType(field->arrayKw)) {
                    output << field->arraySize;
                }
                else {
                    for (const auto& ns : field->arrayPtr->namespace_) {
                        output << ns << "::";
                    }
                    switch (field->arrayKw) {
                    case kw::Const:
                        output << field->arrayPtr->constMeta()->name;
                        break;
                    case kw::Enum:
                        output << field->arrayPtr->enumMeta()->name << "::"
                            << field->arrayPtr->enumMeta()->valuesVec[
                                field->arrayIdx].first;
                        break;
                    case kw::Min:
                        output << field->arrayPtr->enumMeta()->name << "::min()";
                        break;
                    case kw::Max:
                        output << field->arrayPtr->enumMeta()->name << "::max()";
                        break;
                    case kw::Count:
                        output << field->arrayPtr->enumMeta()->name << "::count()";
                        break;
                    default:
                        output << "%ERROR%";
                        break;
                    }
                }
            }
            else if (field->isBuiltIn) {
                output << "sizeof(" << kw_to_domain_specific(field->typeKw) << ")";
            }
            else {
                output << "sizeof(";
                for (const auto& ns : field->typePtr->namespace_) {
                    output << ns << "::";
                }
                switch (field->typeKw) {
                case kw::Enum:
                    output << field->typePtr->enumMeta()->name << "::_";
                    break;
                case kw::Struct:
                    output << field->typePtr->structMeta()->name << "::table";
                    break;
                case kw::Union:
                    output << field->typePtr->unionMeta()->name << "::table";
                    break;
                default:
                    output << "%ERROR%";
                    break;
                }
                output << ")";
            }
            output << ";" << std::endl;
        }
        output << "    " << "    " << "default:" << std::endl;
        output << "    " << "    " << "    " << "return 0;" << std::endl;
        output << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "return 0;" << std::endl;
        output << "    " << "}" << std::endl;
    }
    void writeStruct_has_get_set(WritingMeta& writingMeta, std::ostream& output,
            const AST& ast, const AST::StructMeta* structMeta) {
        for (const auto& field : structMeta->fieldsVec) {
            output << "    " << "bool has_" << field->name << "() const {" << std::endl;
            output << "    " << "    " << "return has(fields::" << field->name << ");" << std::endl;
            output << "    " << "}" << std::endl;
            std::string type;
            if (field->isBuiltIn) {
                if (field->typeKw == kw::Bytes) {
                    type = kw_to_domain_specific(kw::UInt8);
                }
                else {
                    type = kw_to_domain_specific(field->typeKw);
                }
            }
            else {
                for (const auto& ns : field->typePtr->namespace_) {
                    type += ns;
                    type += "::";
                }
                switch (field->typeKw) {
                case kw::Enum:
                    type += field->typePtr->enumMeta()->name;
                    type += "::_";
                    break;
                case kw::Struct:
                    type += field->typePtr->enumMeta()->name;
                    break;
                case kw::Union:
                    type += field->typePtr->enumMeta()->name;
                    break;
                default:
                    type = "%ERROR%";
                    break;
                }
            }
            output << "    " << type;
            if (field->isArray || !field->isBuiltIn) {
                if (field->typeKw != kw::Enum) {
                    output << "&";
                }
            }
            output << " get_" << field->name << "(";
            if (field->isArray) {
                output << "const " << structMeta->offsetTypeStr << " index";
            }
            output << ") {" << std::endl;
            if (!field->isBuiltIn && field->typeKw != kw::Enum) {
                output << "    " << "    " << "static thread_local " << type
                    << " " << field->name << ";" << std::endl;
                output << "    " << "    " << "const auto " << field->name << "_t = ";
                if (field->isArray) {
                    output << "&";
                }
                output << "reinterpret_cast<" << type << "::table*>(get(fields::"
                    << field->name << "))";
                if (field->isArray) {
                    output << "[index]";
                }
                output << ";" << std::endl;
                output << "    " << "    " << "const " << structMeta->offsetTypeStr
                    << " table_offset = static_cast<" << structMeta->offsetTypeStr
                    << ">(" << std::endl;
                output << "    " << "    " << "    " << "reinterpret_cast<const uint8_t*>("
                    << field->name << "_t)" << std::endl;
                output << "    " << "    " << "    "
                    << "- reinterpret_cast<const uint8_t*>(base_ptr()));" << std::endl;
                output << "    " << "    " << field->name
                    << ".from(table_offset, m_from_ptr, 0, m_buffer);" << std::endl;
                output << "    " << "    " << "return " << field->name << ";" << std::endl;
            }
            else {
                output << "    " << "    " << "return ";
                if (!field->isArray) {
                    output << "*";
                }
                output << "reinterpret_cast<" << type << "*>(get(fields::"
                    << field->name << "))";
                if (field->isArray) {
                    output << "[index]";
                }
                output << ";" << std::endl;
            }
            output << "    " << "}" << std::endl;
            output << "    " << "const " << type << "& get_" << field->name << "(";
            if (field->isArray) {
                output << "const " << structMeta->offsetTypeStr << " index";
            }
            output << ") const {" << std::endl;
            output << "    " << "    " << "return const_cast<" << structMeta->name
                << "*>(this)->get_" << field->name << "(";
            if (field->isArray) {
                output << "index";
            }
            output << ");" << std::endl;
            output << "    " << "}" << std::endl;
            if (field->typeKw == kw::Bytes && field->arrayKw == kw::UNDEFINED) {
                output << "    " << "template <typename bytes_t>" << std::endl;
                output << "    " << "bytes_t get_" << field->name << "() const {" << std::endl;
                output << "    " << "    " << "bytes_t bytes;" << std::endl;
                output << "    " << "    " << "bytes.resize(size_" << field->name
                    << "());" << std::endl;
                output << "    " << "    " << "std::copy(reinterpret_cast<"
                    "const uint8_t*>(get(fields::" << field->name << "))," << std::endl;
                output << "    " << "    " << "    " << "reinterpret_cast<const uint8_t*>("
                    "get(fields::" << field->name << ")) + size_" << field->name << "()," << std::endl;
                output << "    " << "    " << "    " << "reinterpret_cast<uint8_t*>("
                    "&bytes[0]));" << std::endl;
                //std::back_inserter(bytes));
                output << "    " << "    " << "return bytes;" << std::endl;
                output << "    " << "}" << std::endl;

                output << "    " << "template <typename bytes_t>" << std::endl;
                output << "    " << "void set_" << field->name << "(const bytes_t& bytes) {" << std::endl;
                output << "    " << "    " << "set(fields::" << field->name
                    << ", bytes.data(), static_cast<" << structMeta->offsetTypeStr
                    << ">(bytes.size()));" << std::endl;
                output << "    " << "}" << std::endl;
            }
            if (field->isArray) {
                if (field->arrayKw == kw::UNDEFINED) {
                    output << "    " << "void set_" << field->name
                        << "(const " << structMeta->offsetTypeStr << " size) {" << std::endl;
                    output << "    " << "    " << "set(fields::" << field->name
                        << ", nullptr, size);" << std::endl;
                    output << "    " << "}" << std::endl;
                }
                else if (field->isOptional) {
                    output << "    " << "void set_" << field->name << "() {" << std::endl;
                    output << "    " << "    " << "set(fields::" << field->name
                        << ", nullptr, ";
                    if (isScalarType(field->arrayKw)) {
                        output << field->arraySize;
                    }
                    else {
                        for (const auto& ns : field->arrayPtr->namespace_) {
                            output << ns << "::";
                        }
                        switch (field->arrayKw) {
                        case kw::Const:
                            output << field->arrayPtr->constMeta()->name;
                            break;
                        case kw::Enum:
                            output << field->arrayPtr->enumMeta()->name << "::"
                                << field->arrayPtr->enumMeta()->valuesVec[
                                    field->arrayIdx].first;
                            break;
                        case kw::Min:
                            output << field->arrayPtr->enumMeta()->name << "::min()";
                            break;
                        case kw::Max:
                            output << field->arrayPtr->enumMeta()->name << "::max()";
                            break;
                        case kw::Count:
                            output << field->arrayPtr->enumMeta()->name << "::count()";
                            break;
                        default:
                            output << "%ERROR%";
                            break;
                        }
                    }
                    output << "); " << std::endl;
                    output << "    " << "}" << std::endl;
                }
                output << "    " << structMeta->offsetTypeStr << " size_" << field->name
                    << "() const {" << std::endl;
                output << "    " << "    " << "return size(fields::" << field->name << ");" << std::endl;
                output << "    " << "}" << std::endl;
            }
            else {
                if (field->isBuiltIn) {
                    output << "    " << "void set_" << field->name << "(const " << type
                        << " value) {" << std::endl;
                    output << "    " << "    " << "set(fields::" << field->name
                        << ", &value, sizeof(" << type << "));" << std::endl;
                    output << "    " << "}" << std::endl;
                }
                else if (field->typeKw == kw::Enum) {
                    output << "    " << "void set_" << field->name << "(const ";
                    for (const auto& ns : field->typePtr->namespace_) {
                        output << ns << "::";
                    }
                    output << field->typePtr->enumMeta()->name << "::_ value) {" << std::endl;
                    output << "    " << "    " << "set(fields::" << field->name
                        << ", &value, sizeof(" << type << "));" << std::endl;
                    output << "    " << "}" << std::endl;
                }
                else {
                    output << "    " << "void set_" << field->name << "() {" << std::endl;
                    output << "    " << "    " << "set(fields::" << field->name
                        << ", nullptr, sizeof(" << type << "::table));" << std::endl;
                    output << "    " << "}" << std::endl;
                }
            }
        }
    }
    void writeStruct_table(WritingMeta& writingMeta, std::ostream& output,
            const AST& ast, const AST::StructMeta* structMeta) {
        output << "    " << "#pragma pack(1)" << std::endl;
        if (structMeta->withHeader) {
            output << "    " << "struct header {" << std::endl;
            output << "    " << "    " << "int8_t ___i = 'i';" << std::endl;
            output << "    " << "    " << "int8_t ___b = 'b';" << std::endl;
            output << "    " << "    " << "int8_t ___s = 's';" << std::endl;
            output << "    " << "    " << "uint8_t ___v = (3 << 4) | 0;" << std::endl;
            output << "    " << "    " << "uint32_t ___sh = " << structMeta->schemaHash << ";" << std::endl;
            output << "    " << "};" << std::endl;
        }
        output << "    " << "struct table {" << std::endl;
        for (const auto& field : structMeta->fieldsVec) {
            output << "    " << "    ";
            if (field->isOptional) {
                output << structMeta->offsetTypeStr << " __";
            }
            else if (field->isBuiltIn) {
                if (field->typeKw == kw::Bytes) {
                    output << kw_to_domain_specific(kw::UInt8) << " ";
                }
                else {
                    output << kw_to_domain_specific(field->typeKw) << " ";
                }
            }
            else if (field->typeKw == kw::Enum) {
                for (const auto& ns : field->typePtr->namespace_) {
                    output << ns << "::";
                }
                output << field->typePtr->enumMeta()->name << "::_ ";
            }
            else if (field->typeKw == kw::Struct) {
                for (const auto& ns : field->typePtr->namespace_) {
                    output << ns << "::";
                }
                output << field->typePtr->structMeta()->name << "::table ";
            }
            else if (field->typeKw == kw::Union) {
                for (const auto& ns : field->typePtr->namespace_) {
                    output << ns << "::";
                }
                output << field->typePtr->unionMeta()->name << "::table ";
            }
            else {
                output << "%ERROR%";
            }

            output << field->name;

            if (!field->isOptional) {
                if (field->isArray && field->arrayKw != kw::UNDEFINED) {
                    output << "[";
                    if (isScalarType(field->arrayKw)) {
                        output << field->arraySize;
                    }
                    else {
                        for (const auto& ns : field->arrayPtr->namespace_) {
                            output << ns << "::";
                        }
                        switch (field->arrayKw) {
                        case kw::Const:
                            output << field->arrayPtr->constMeta()->name;
                            break;
                        case kw::Enum:
                            output << field->arrayPtr->enumMeta()->name << "::"
                                << field->arrayPtr->enumMeta()->valuesVec[
                                    field->arrayIdx].first;
                            break;
                        case kw::Min:
                            output << field->arrayPtr->enumMeta()->name << "::min()";
                            break;
                        case kw::Max:
                            output << field->arrayPtr->enumMeta()->name << "::max()";
                            break;
                        case kw::Count:
                            output << field->arrayPtr->enumMeta()->name << "::count()";
                            break;
                        default:
                            output << "%ERROR%";
                            break;
                        }
                    }
                    output << "]";
                }
                else if (!field->valueStr.empty()) {
                    output << " = " << field->valueStr;
                }
                else if (field->valuePtr) {
                    output << " = ";
                    for (const auto& ns : field->valuePtr->namespace_) {
                        output << ns << "::";
                    }
                    switch (field->valueKw) {
                    case kw::Const:
                        output << field->valuePtr->constMeta()->name;
                        break;
                    case kw::Enum:
                        output << field->valuePtr->enumMeta()->name << "::"
                            << field->valuePtr->enumMeta()->valuesVec[
                                field->valueIdx].first;
                        break;
                    case kw::Min:
                        output << field->valuePtr->enumMeta()->name << "::min()";
                        break;
                    case kw::Max:
                        output << field->valuePtr->enumMeta()->name << "::max()";
                        break;
                    case kw::Count:
                        output << field->valuePtr->enumMeta()->name << "::count()";
                        break;
                    default:
                        output << "%ERROR%";
                        break;
                    }
                }
            }
            else {
                output << " = 0";
            }
            output << ";" << std::endl;
        }
        output << "    " << "};" << std::endl;
        output << "    " << "#pragma pack()" << std::endl;
    }
    void writeStruct_staff(WritingMeta& writingMeta, std::ostream& output,
            const AST& ast, const AST::StructMeta* structMeta) {
        output << "    " << "table* get_table() {" << std::endl;
        #ifdef _DEBUG
            output << "    " << "    " << "#ifdef _DEBUG" << std::endl;
            output << "    " << "    " << "m_table = reinterpret_cast<table*>(" << std::endl;
            output << "    " << "    " << "    " << "reinterpret_cast<uint8_t*>(base_ptr()) "
                "+ m_table_offset);" << std::endl;
            output << "    " << "    " << "#endif // _DEBUG" << std::endl;
        #endif // _DEBUG
        output << "    " << "    " << "return reinterpret_cast<table*>(" << std::endl;
        output << "    " << "    " << "    " << "reinterpret_cast<uint8_t*>(base_ptr()) "
            "+ m_table_offset);" << std::endl;
        output << "    " << "}" << std::endl;
        output << "    " << "const table* get_table() const {" << std::endl;
        output << "    " << "    " << "return const_cast<" << structMeta->name
            << "*>(this)->get_table();" << std::endl;
        output << "    " << "}" << std::endl;
        output << "    " << "void* base_ptr() {" << std::endl;
        output << "    " << "    " << "if (m_buffer) {" << std::endl;
        output << "    " << "    " << "    " << "if (m_buffer->empty()) {" << std::endl;
        output << "    " << "    " << "    " << "    " << "return nullptr;" << std::endl;
        output << "    " << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "    " << "return m_buffer->data();" << std::endl;
        output << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "return m_from_ptr;" << std::endl;
        output << "    " << "}" << std::endl;
        output << "    " << "const void* base_ptr() const {" << std::endl;
        output << "    " << "    " << "return const_cast<"
            << structMeta->name << "*>(this)->base_ptr();" << std::endl;
        output << "    " << "}" << std::endl;
        #ifdef _DEBUG
            output << "    " << "#ifdef _DEBUG" << std::endl;
            output << "    " << "table* m_table = nullptr;" << std::endl;
            output << "    " << "#endif // _DEBUG" << std::endl;
        #endif // _DEBUG
        output << "    " << "std::shared_ptr<std::vector<uint8_t>> m_buffer;" << std::endl;
        output << "    " << "uint8_t* m_from_ptr = nullptr;" << std::endl;
        output << "    " << "mutable " << structMeta->offsetTypeStr << " m_from_size = 0;" << std::endl;
        output << "    " << structMeta->offsetTypeStr << " m_table_offset = 0;" << std::endl;
    }
    void writeStruct(WritingMeta& writingMeta, std::ostream& output,
            const AST& ast, const AST::ObjectMeta* objectMeta) {
        const AST::StructMeta* structMeta = objectMeta->structMeta();
        if (structMeta->fieldsVec.empty()) {
            return;
        }
        structMeta->offsetTypeStr = kw_to_domain_specific(structMeta->offsetType);
        output << std::endl;
        output << "class " << structMeta->name << " { // struct " << structMeta->name << std::endl;
        output << "public:" << std::endl;
        output << "    " << structMeta->name << "() {}" << std::endl;
        //output << "    " << structMeta->name << "(const uint32_t reserve = 0) {" << std::endl;
        //output << "    " << "    " << "create(reserve);" << std::endl;
        //output << "    " << "}" << std::endl;
        output << "    " << structMeta->name << "(void* from_ptr, const "
            << structMeta->offsetTypeStr << " from_size = 0) {" << std::endl;
        output << "    " << "    " << "from(from_ptr, from_size);" << std::endl;
        output << "    " << "}" << std::endl;
        output << "    " << structMeta->name << "(const void* from_ptr, const "
            << structMeta->offsetTypeStr << " from_size = 0) {" << std::endl;
        output << "    " << "    " << "from(from_ptr, from_size);" << std::endl;
        output << "    " << "}" << std::endl;
        writeStruct_create(writingMeta, output, ast, structMeta);
        writeStruct_from_to(writingMeta, output, ast, structMeta);
        writeStruct_size(writingMeta, output, ast, structMeta);
        {
            writingMeta.spacing = "    ";
            std::unique_ptr<AST::EnumMeta> enumField = std::make_unique<AST::EnumMeta>();
            enumField->name = "fields";
            enumField->type = structMeta->offsetType;
            for (uint32_t i = 0; i < structMeta->fieldsVec.size(); ++i) {
                const auto& field = structMeta->fieldsVec[i];
                enumField->valuesVec.emplace_back(field->name, i);
            }
            std::unique_ptr<AST::ObjectMeta> objectField = std::make_unique<AST::ObjectMeta>();
            objectField->putEnumMeta(std::move(enumField));
            writeEnum(writingMeta, output, ast, objectField.get());
        }
        output << std::endl;
        writeStruct_offset(writingMeta, output, ast, structMeta);
        writeStruct_has(writingMeta, output, ast, structMeta);
        writeStruct_get(writingMeta, output, ast, structMeta);
        writeStruct_set(writingMeta, output, ast, structMeta);
        writeStruct_size_field(writingMeta, output, ast, structMeta);
        //output << "    static types type(const ids& id)" << std::endl;
        //file << "    {" << std::endl;
        //file << "        switch (id)" << std::endl;
        //file << "        {" << std::endl;
        //for (uint32_t i = 0; i < st.fields.size(); i++)
        //{
        //    const auto& member = st.fields[i];
        //    file << "        case ids::" << member.name << ": return types::" << member.type;
        //    if (member.isArray && member.type != tokenBytes)
        //        file << "a";
        //    file << ";" << std::endl;
        //}
        //file << "        default: return types::unknown;" << std::endl;
        //file << "        }" << std::endl;
        //file << "    }" << std::endl;
        //file << std::endl;
        output << std::endl;
        writeStruct_has_get_set(writingMeta, output, ast, structMeta);
        output << std::endl;
        writeStruct_table(writingMeta, output, ast, structMeta);
        output << "private:" << std::endl;
        writeStruct_staff(writingMeta, output, ast, structMeta);
        output << "}; // struct " << structMeta->name << std::endl;
    }

    void writeUnion_create(WritingMeta& writingMeta, std::ostream& output,
            const AST& ast, const AST::UnionMeta* unionMeta) {
        //output << "    " << "void create(void* external_ptr, const "
        //    << unionMeta->offsetTypeStr << " external_size) {" << std::endl;
        //output << "    " << "    " << "m_table_offset = 0;" << std::endl;
        //output << "    " << "    " << "m_from_ptr = reinterpret_cast<uint8_t*>(external_ptr);" << std::endl;
        //output << "    " << "    " << "m_from_size = 0;" << std::endl;
        //output << "    " << "    " << "m_buffer.reset();" << std::endl;
        //output << "    " << "    " << "create(UINT32_MAX);" << std::endl;
        //output << "    " << "}" << std::endl;
        output << "    " << "void create(std::shared_ptr<std::vector<uint8_t>> buffer) {" << std::endl;
        output << "    " << "    " << "m_from_ptr = nullptr;" << std::endl;
        output << "    " << "    " << "m_buffer = buffer;" << std::endl;
        output << "    " << "    " << "m_buffer->clear();" << std::endl;
        if (unionMeta->withHeader) {
            output << "    " << "    " << "m_table_offset = sizeof(header);" << std::endl;
        }
        else {
            output << "    " << "    " << "m_table_offset = 0;" << std::endl;
        }
        output << "    " << "    " << "create(0);" << std::endl;
        output << "    " << "}" << std::endl;
        output << "    " << "void create(const uint32_t reserve = 0) {" << std::endl;
        output << "    " << "    " << "if (reserve != UINT32_MAX) {" << std::endl;
        output << "    " << "    " << "    " << "m_from_ptr = nullptr;" << std::endl;
        output << "    " << "    " << "    " << "m_buffer.reset();" << std::endl;
        output << "    " << "    " << "    " << "m_buffer = std::make_shared<"
            "std::vector<uint8_t>>();" << std::endl;
        output << "    " << "    " << "    " << "m_buffer->reserve(reserve);" << std::endl;
        if (unionMeta->withHeader) {
            output << "    " << "    " << "    " << "m_buffer->resize(sizeof(header) + sizeof(table));" << std::endl;
            output << "    " << "    " << "    " << "m_table_offset = sizeof(header);" << std::endl;
            output << "    " << "    " << "    " << "new(base_ptr()) header();" << std::endl;
        }
        else {
            output << "    " << "    " << "    " << "m_buffer->resize(sizeof(table));" << std::endl;
            output << "    " << "    " << "    " << "m_table_offset = 0;" << std::endl;
        }
        output << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "new(get_table()) table();" << std::endl;
        output << "    " << "}" << std::endl;
    }
    void writeUnion_from_to(WritingMeta& writingMeta, std::ostream& output,
            const AST& ast, const AST::UnionMeta* unionMeta) {
        output << "    " << "bool from(void* from_ptr, const "
            << unionMeta->offsetTypeStr << " from_size = 0) {" << std::endl;
        output << "    " << "    " << "m_buffer.reset();" << std::endl;
        //TODO: m_is_read_only
        //output << "    " << "    " << "//TODO: m_is_read_only" << std::endl;
        output << "    " << "    " << "if (from_ptr == nullptr) {" << std::endl;
        output << "    " << "    " << "    " << "return false;" << std::endl;
        output << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "m_from_ptr = reinterpret_cast<uint8_t*>(from_ptr);" << std::endl;
        if (unionMeta->withHeader) {
            output << "    " << "    " << "m_table_offset = sizeof(header);" << std::endl;
        }
        else {
            output << "    " << "    " << "m_table_offset = 0;" << std::endl;
        }
        output << "    " << "    " << "if (from_size > 0) {" << std::endl;
        output << "    " << "    " << "    " << "if (from_size < size_min()) {" << std::endl;
        output << "    " << "    " << "    " << "    " << "return false;" << std::endl;
        output << "    " << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "    " << "if (from_size > size_max()) {" << std::endl;
        output << "    " << "    " << "    " << "    " << "return false;" << std::endl;
        output << "    " << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "    " << "m_from_size = from_size;" << std::endl;
        output << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "else {" << std::endl;
        output << "    " << "    " << "    " << "size(3);" << std::endl;
        output << "    " << "    " << "}" << std::endl;
        if (unionMeta->withHeader) {
            output << "    " << "    " << "auto h = reinterpret_cast<const header*>(base_ptr());" << std::endl;
            output << "    " << "    " << "if (h->___i != 'i' || h->___b != 'b' || h->___s != 's') {" << std::endl;
            output << "    " << "    " << "    " << "return false;" << std::endl;
            output << "    " << "    " << "}" << std::endl;
            output << "    " << "    " << "if (h->___sh != " << unionMeta->schemaHash << ") {" << std::endl;
            output << "    " << "    " << "    " << "return false;" << std::endl;
            output << "    " << "    " << "}" << std::endl;
        }
        output << "    " << "    " << "return true;" << std::endl;
        output << "    " << "}" << std::endl;
        output << "    " << "bool from(const void* from_ptr, const "
            << unionMeta->offsetTypeStr << " from_size = 0) {" << std::endl;
        output << "    " << "    " << "return from(const_cast<void*>(from_ptr), from_size);" << std::endl;
        output << "    " << "}" << std::endl;
        output << "    " << "void from(" << unionMeta->offsetTypeStr
            << " table_offset, const void* from_ptr," << std::endl;
        output << "    " << "    " << "    " << "const " << unionMeta->offsetTypeStr
            << " from_size, std::shared_ptr<std::vector<uint8_t>> buffer) {" << std::endl;
        output << "    " << "    " << "m_table_offset = table_offset;" << std::endl;
        output << "    " << "    " << "m_buffer = buffer;" << std::endl;
        output << "    " << "    " << "m_from_ptr = reinterpret_cast<uint8_t*>("
            "const_cast<void*>(from_ptr));" << std::endl;
        output << "    " << "    " << "m_from_size = from_size;" << std::endl;
        output << "    " << "}" << std::endl;
        output << "    " << "void* to() {" << std::endl;
        output << "    " << "    " << "if (m_from_ptr) {" << std::endl;
        output << "    " << "    " << "    " << "return m_from_ptr;" << std::endl;
        output << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "if (m_buffer) {" << std::endl;
        output << "    " << "    " << "    " << "return m_buffer->data();" << std::endl;
        output << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "return nullptr;" << std::endl;
        output << "    " << "}" << std::endl;
        output << "    " << "const void* to() const {" << std::endl;
        output << "    " << "    " << "return const_cast<" << unionMeta->name
            << "*>(this)->to();" << std::endl;
        output << "    " << "}" << std::endl;
    }
    void writeUnion_size(WritingMeta& writingMeta, std::ostream& output,
            const AST& ast, const AST::UnionMeta* unionMeta) {
        // calculate:
        //  0 - nothing
        //  1 - data, fixed in struct
        //  2 - table + data, optional in struct/union
        //  3 - header + table + data, from root
        output << "    " << unionMeta->offsetTypeStr << " size(const uint8_t calculate = 0) const {" << std::endl;
        output << "    " << "    " << "if (m_from_ptr) {" << std::endl;
        output << "    " << "    " << "    " << "if (calculate > 0) {" << std::endl;
        output << "    " << "    " << "    " << "    " << "m_from_size = 0;" << std::endl;
        output << "    " << "    " << "    " << "    " << "if (calculate > 1) {" << std::endl;
        output << "    " << "    " << "    " << "    " << "    " << "m_from_size += sizeof(table);" << std::endl;
        output << "    " << "    " << "    " << "    " << "}" << std::endl;
        if (unionMeta->withHeader) {
            output << "    " << "    " << "    " << "    " << "if (calculate > 2) {" << std::endl;
            output << "    " << "    " << "    " << "    " << "    " << "m_from_size += sizeof(header);" << std::endl;
            output << "    " << "    " << "    " << "    " << "}" << std::endl;
        }
        output << "    " << "    " << "    " << "    " << "switch (get_table()->variant) {" << std::endl;
        for (const auto& field : unionMeta->fieldsVec) {
            output << "    " << "    " << "    " << "    "
                << "case fields::" << field->name << ":" << std::endl;
            // 4. optional scalar            size_x()
            // 5. optional struct            get_x().size(2)
            // 6. optional array of scalars  sizeof(offset) + size_x() * sizeof(typeof(x))
            // 7. optional array of structs  sizeof(offset) + for size_x(): get_x(i).size(2)
            if (field->typeKw == kw::Bytes) {
                output << "    " << "    " << "    " << "    " << "    "
                    << "m_from_size += size_" << field->name
                    << "() + sizeof(" << kw_to_domain_specific(field->arraySizeKw) << ");" << std::endl;
            }
            else if (field->isArray) {
                if (field->isScalar) { // 6
                    output << "    " << "    " << "    " << "    " << "    "
                        << "m_from_size += size_" << field->name << "() * sizeof("
                        << kw_to_domain_specific(field->typeKw) << ")";
                    if (field->arrayKw == kw::UNDEFINED) {
                        output << " + sizeof("
                            << kw_to_domain_specific(field->arraySizeKw) << ")";
                    }
                    output << ";" << std::endl;
                }
                else { // 7
                    output << "    " << "    " << "    " << "    " << "    "
                        << "for (" << unionMeta->offsetTypeStr << " i = 0, s = size_"
                        << field->name << "(); i < s; ++i) {" << std::endl;
                    output << "    " << "    " << "    " << "    " << "    "
                        << "    " << "m_from_size += get_" << field->name
                        << "(i).size(2);" << std::endl;
                    output << "    " << "    " << "    " << "    " << "    " << "}";
                    if (field->arrayKw == kw::UNDEFINED) {
                        output << " m_from_size += sizeof("
                            << kw_to_domain_specific(field->arraySizeKw) << ");";
                    }
                    output << std::endl;
                }
            }
            else {
                if (field->isScalar) { // 4
                    output << "    " << "    " << "    " << "    " << "    "
                        << "m_from_size += size(fields::" << field->name << ");" << std::endl;
                }
                else { // 5
                    output << "    " << "    " << "    " << "    " << "    "
                        << "m_from_size += get_" << field->name
                        << "().size(2);" << std::endl;
                }
            }
            output << "    " << "    " << "    " << "    " << "    "
                << "break;" << std::endl;
        }
        output << "    " << "    " << "    " << "    " << "default: break;" << std::endl;
        output << "    " << "    " << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "    " << "return m_from_size;" << std::endl;
        output << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "if (m_buffer) {" << std::endl;
        output << "    " << "    " << "    "
            << "return static_cast<" << unionMeta->offsetTypeStr << ">(m_buffer->size());" << std::endl;
        output << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "return 0;" << std::endl;
        output << "    " << "}" << std::endl;
        uint64_t sizeMin = 0;
        uint64_t sizeMax = 0;
        unionMeta->calcSizeMinMax(sizeMin, sizeMax);
        if (sizeMax == UINT64_MAX) {
            switch (unionMeta->offsetType) {
            case kw::UInt8: sizeMax = UINT8_MAX; break;
            case kw::UInt16: sizeMax = UINT16_MAX; break;
            case kw::UInt32: sizeMax = UINT32_MAX; break;
            default: break;
            }
        }
        output << "    " << "static " << unionMeta->offsetTypeStr << " size_min() {" << std::endl;
        output << "    " << "    " << "return " << sizeMin << ";" << std::endl;
        output << "    " << "}" << std::endl;
        output << "    " << "static " << unionMeta->offsetTypeStr << " size_max() {" << std::endl;
        output << "    " << "    " << "return " << sizeMax << ";" << std::endl;
        output << "    " << "}" << std::endl;
    }
    void writeUnion_offset(WritingMeta& writingMeta, std::ostream& output,
            const AST& ast, const AST::UnionMeta* unionMeta) {
        output << "    " << unionMeta->offsetTypeStr << " offset() const {" << std::endl;
        output << "    " << "    " << "return get_table()->offset;" << std::endl;
        output << "    " << "}" << std::endl;
    }
    void writeUnion_has(WritingMeta& writingMeta, std::ostream& output,
            const AST& ast, const AST::UnionMeta* unionMeta) {
        output << "    " << "fields::_ variant() const {" << std::endl;
        output << "    " << "    " << "return get_table()->variant;" << std::endl;
        output << "    " << "}" << std::endl;
        output << "    " << "bool has(const fields::_ field) const {" << std::endl;
        output << "    " << "    " << "return get_table()->variant == field;" << std::endl;
        output << "    " << "}" << std::endl;
    }
    void writeUnion_get(WritingMeta& writingMeta, std::ostream& output,
            const AST& ast, const AST::UnionMeta* unionMeta) {
        output << "    " << "void* get(const fields::_ field) {" << std::endl;
        output << "    " << "    " << "switch (get_table()->variant) {" << std::endl;
        for (const auto& field : unionMeta->fieldsVec) {
            output << "    " << "    " << "case fields::" << field->name
                << ": return reinterpret_cast<uint8_t*>(" << std::endl;
            output << "    " << "    "
                << "    " << "base_ptr()) + offset()";
            if (field->isArray && field->arrayKw == kw::UNDEFINED) {
                output << " + sizeof(" << kw_to_domain_specific(field->arraySizeKw) << ")";
            }
            output << ";" << std::endl;
        }
        output << "    " << "    " << "default: break;" << std::endl;
        output << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "return nullptr;" << std::endl;
        output << "    " << "}" << std::endl;
        output << "    " << "const void* get(const fields::_ field) const {" << std::endl;
        output << "    " << "    " << "return const_cast<"
            << unionMeta->name << "*>(this)->get(field);" << std::endl;
        output << "    " << "}" << std::endl;
    }
    void writeUnion_set(WritingMeta& writingMeta, std::ostream& output,
            const AST& ast, const AST::UnionMeta* unionMeta) {
        output << "    " << "void set(const fields::_ field, const void* data, const "
            << unionMeta->offsetTypeStr << " size) {" << std::endl;
        output << "    " << "    " << "if (get_table()->variant == fields::_SPECIAL_) {" << std::endl;
        output << "    " << "    " << "    " << "if (m_from_ptr) {" << std::endl;
        output << "    " << "    " << "    " << "    " << "return;" << std::endl;
        output << "    " << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "    " << "get_table()->variant = field;" << std::endl;
        output << "    " << "    " << "    " << "get_table()->offset = "
            "static_cast<" << unionMeta->offsetTypeStr << ">(m_buffer->size());" << std::endl;
        output << "    " << "    " << "    " << "switch (field) {" << std::endl;
        for (const auto& field : unionMeta->fieldsVec) {
            output << "    " << "    " << "    " << "case fields::" << field->name
                << ":" << std::endl;
            output << "    " << "    " << "    " << "    " << "m_buffer->resize(m_buffer->size()";
            if (field->isArray) {
                if (field->arrayKw == kw::UNDEFINED) {
                    output << " + sizeof(" << kw_to_domain_specific(field->arraySizeKw)
                        << ")";
                }
                output << std::endl;
                output << "    " << "    " << "    " << "    " << "    "
                    << "+ size * sizeof(";
                if (field->isBuiltIn || field->typeKw == kw::Bytes) {
                    if (field->typeKw == kw::Bytes) {
                        output << kw_to_domain_specific(kw::UInt8);
                    }
                    else {
                        output << kw_to_domain_specific(field->typeKw);
                    }
                }
                else {
                    for (const auto& ns : field->typePtr->namespace_) {
                        output << ns << "::";
                    }
                    switch (field->typeKw) {
                    case kw::Enum:
                        output << kw_to_domain_specific(field->typePtr->enumMeta()->type);
                        break;
                    case kw::Struct:
                        output << field->typePtr->structMeta()->name << "::table";
                        break;
                    case kw::Union:
                        output << field->typePtr->unionMeta()->name << "::table";
                        break;
                    default:
                        output << "%ERROR%";
                        break;
                    }
                }
                output << "));" << std::endl;
                output << "    " << "    " << "    " << "    "
                    << "*reinterpret_cast<" << unionMeta->offsetTypeStr
                    << "*>((reinterpret_cast<uint8_t*>(" << std::endl;
                output << "    " << "    " << "    " << "    "
                    << "    " << "base_ptr()) + get_table()->offset)) = size;" << std::endl;
            }
            else {
                output << " + sizeof(";
                if (field->isBuiltIn || field->typeKw == kw::Bytes) {
                    if (field->typeKw == kw::Bytes) {
                        output << kw_to_domain_specific(kw::UInt8);
                    }
                    else {
                        output << kw_to_domain_specific(field->typeKw);
                    }
                }
                else {
                    for (const auto& ns : field->typePtr->namespace_) {
                        output << ns << "::";
                    }
                    switch (field->typeKw) {
                    case kw::Enum:
                        output << field->typePtr->enumMeta()->name << "::_";
                        break;
                    case kw::Struct:
                        output << field->typePtr->structMeta()->name << "::table";
                        break;
                    case kw::Union:
                        output << field->typePtr->unionMeta()->name << "::table";
                        break;
                    default:
                        output << "%ERROR%";
                        break;
                    }
                }
                output << "));" << std::endl;
            }
            if (!field->isBuiltIn && field->typeKw != kw::Enum) {
                if (field->isArray) {
                    output << "    " << "    " << "    " << "    "
                        << "for (" << kw_to_domain_specific(field->arraySizeKw)
                        << " i = 0; i < size; ++i) {" << std::endl;
                    output << "    " << "    " << "    " << "    " << "    "
                        << "get_" << field->name << "(i).create(UINT32_MAX);" << std::endl;
                    output << "    " << "    " << "    " << "    "
                        << "}" << std::endl;
                }
                else {
                    output << "    " << "    " << "    " << "    "
                        << "get_" << field->name << "().create(UINT32_MAX);" << std::endl;
                }
            }
            output << "    " << "    " << "    " << "    " << "break;" << std::endl;
        }
        output << "    " << "    " << "    " << "default: return;" << std::endl;
        output << "    " << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "if (data) {" << std::endl;
        output << "    " << "    " << "    " << "std::copy(reinterpret_cast<const uint8_t*>(data)," << std::endl;
        output << "    " << "    " << "    " << "    " << "reinterpret_cast<const uint8_t*>(data) + size," << std::endl;
        output << "    " << "    " << "    " << "    " << "reinterpret_cast<uint8_t*>(get(field)));" << std::endl;
        output << "    " << "    " << "}" << std::endl;
        output << "    " << "}" << std::endl;
    }
    void writeUnion_size_field(WritingMeta& writingMeta, std::ostream& output,
            const AST& ast, const AST::UnionMeta* unionMeta) {
        output << "    " << unionMeta->offsetTypeStr << " size(const fields::_ field) const {" << std::endl;
        output << "    " << "    " << "if (!has(field)) {" << std::endl;
        output << "    " << "    " << "    " << "return 0;" << std::endl;
        output << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "switch (field) {" << std::endl;
        for (const auto& field : unionMeta->fieldsVec) {
            output << "    " << "    " << "case fields::" << field->name << ":" << std::endl;
            output << "    " << "    " << "    " << "return ";
            if (field->isArray) {
                if (field->arrayKw == kw::UNDEFINED) {
                    output << "*(reinterpret_cast<const "
                        << kw_to_domain_specific(field->arraySizeKw)
                        << "*>(get(field)) - 1)";
                }
                else if (isScalarType(field->arrayKw)) {
                    output << field->arraySize;
                }
                else {
                    for (const auto& ns : field->arrayPtr->namespace_) {
                        output << ns << "::";
                    }
                    switch (field->arrayKw) {
                    case kw::Const:
                        output << field->arrayPtr->constMeta()->name;
                        break;
                    case kw::Enum:
                        output << field->arrayPtr->enumMeta()->name << "::"
                            << field->arrayPtr->enumMeta()->valuesVec[
                                field->arrayIdx].first;
                        break;
                    case kw::Min:
                        output << field->arrayPtr->enumMeta()->name << "::min()";
                        break;
                    case kw::Max:
                        output << field->arrayPtr->enumMeta()->name << "::max()";
                        break;
                    case kw::Count:
                        output << field->arrayPtr->enumMeta()->name << "::count()";
                        break;
                    default:
                        output << "%ERROR%";
                        break;
                    }
                }
            }
            else if (field->isBuiltIn) {
                output << "sizeof(" << kw_to_domain_specific(field->typeKw) << ")";
            }
            else switch (field->typeKw) {
            case kw::Enum:
                output << "sizeof(";
                for (const auto& ns : field->typePtr->namespace_) {
                    output << ns << "::";
                }
                output << field->typePtr->enumMeta()->name << "::_)";
                break;
            case kw::Struct:
                output << "sizeof(";
                for (const auto& ns : field->typePtr->namespace_) {
                    output << ns << "::";
                }
                output << field->typePtr->structMeta()->name << "::table)";
                break;
            default:
                output << "%ERROR%";
                break;
            }
            output << ";" << std::endl;
        }
        output << "    " << "    " << "default:" << std::endl;
        output << "    " << "    " << "    " << "return 0;" << std::endl;
        output << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "return 0;" << std::endl;
        output << "    " << "}" << std::endl;
    }
    void writeUnion_has_get_set(WritingMeta& writingMeta, std::ostream& output,
            const AST& ast, const AST::UnionMeta* unionMeta) {
        for (const auto& field : unionMeta->fieldsVec) {
            output << "    " << "bool has_" << field->name << "() const {" << std::endl;
            output << "    " << "    " << "return has(fields::" << field->name << ");" << std::endl;
            output << "    " << "}" << std::endl;
            std::string type;
            if (field->isBuiltIn) {
                if (field->typeKw == kw::Bytes) {
                    type = kw_to_domain_specific(kw::UInt8);
                }
                else {
                    type = kw_to_domain_specific(field->typeKw);
                }
            }
            else {
                for (const auto& ns : field->typePtr->namespace_) {
                    type += ns;
                    type += "::";
                }
                switch (field->typeKw) {
                case kw::Enum:
                    type += field->typePtr->enumMeta()->name;
                    type += "::_";
                    break;
                case kw::Struct:
                    type += field->typePtr->enumMeta()->name;
                    break;
                case kw::Union:
                    type += field->typePtr->enumMeta()->name;
                    break;
                default:
                    type = "%ERROR%";
                    break;
                }
            }
            output << "    " << type;
            if (field->isArray || !field->isBuiltIn) {
                if (field->typeKw != kw::Enum) {
                    output << "&";
                }
            }
            output << " get_" << field->name << "(";
            if (field->isArray) {
                output << "const " << unionMeta->offsetTypeStr << " index";
            }
            output << ") {" << std::endl;
            if (!field->isBuiltIn && field->typeKw != kw::Enum) {
                output << "    " << "    " << "static thread_local " << type
                    << " " << field->name << ";" << std::endl;
                output << "    " << "    " << "const auto " << field->name << "_t = ";
                if (field->isArray) {
                    output << "&";
                }
                output << "reinterpret_cast<" << type << "::table*>(get(fields::"
                    << field->name << "))";
                if (field->isArray) {
                    output << "[index]";
                }
                output << ";" << std::endl;
                output << "    " << "    " << "const " << unionMeta->offsetTypeStr
                    << " table_offset = static_cast<" << unionMeta->offsetTypeStr
                    << ">(" << std::endl;
                output << "    " << "    " << "    " << "reinterpret_cast<const uint8_t*>("
                    << field->name << "_t)" << std::endl;
                output << "    " << "    " << "    "
                    << "- reinterpret_cast<const uint8_t*>(base_ptr()));" << std::endl;
                output << "    " << "    " << field->name
                    << ".from(table_offset, m_from_ptr, 0, m_buffer);" << std::endl;
                output << "    " << "    " << "return " << field->name << ";" << std::endl;
            }
            else {
                output << "    " << "    " << "return ";
                if (!field->isArray) {
                    output << "*";
                }
                output << "reinterpret_cast<" << type << "*>(get(fields::"
                    << field->name << "))";
                if (field->isArray) {
                    output << "[index]";
                }
                output << ";" << std::endl;
            }
            output << "    " << "}" << std::endl;
            output << "    " << "const " << type << "& get_" << field->name << "(";
            if (field->isArray) {
                output << "const " << unionMeta->offsetTypeStr << " index";
            }
            output << ") const {" << std::endl;
            output << "    " << "    " << "return const_cast<" << unionMeta->name
                << "*>(this)->get_" << field->name << "(";
            if (field->isArray) {
                output << "index";
            }
            output << ");" << std::endl;
            output << "    " << "}" << std::endl;
            if (field->typeKw == kw::Bytes && field->arrayKw == kw::UNDEFINED) {
                output << "    " << "template <typename bytes_t>" << std::endl;
                output << "    " << "bytes_t get_" << field->name << "() const {" << std::endl;
                output << "    " << "    " << "bytes_t bytes;" << std::endl;
                output << "    " << "    " << "bytes.resize(size_" << field->name
                    << "());" << std::endl;
                output << "    " << "    " << "std::copy(reinterpret_cast<"
                    "const uint8_t*>(get(fields::" << field->name << "))," << std::endl;
                output << "    " << "    " << "    " << "reinterpret_cast<const uint8_t*>("
                    "get(fields::" << field->name << ")) + size_" << field->name << "()," << std::endl;
                output << "    " << "    " << "    " << "reinterpret_cast<uint8_t*>("
                    "&bytes[0]));" << std::endl;
                //std::back_inserter(bytes));
                output << "    " << "    " << "return bytes;" << std::endl;
                output << "    " << "}" << std::endl;

                output << "    " << "template <typename bytes_t>" << std::endl;
                output << "    " << "void set_" << field->name << "(const bytes_t& bytes) {" << std::endl;
                output << "    " << "    " << "set(fields::" << field->name
                    << ", bytes.data(), bytes.size());" << std::endl;
                output << "    " << "}" << std::endl;
            }
            if (field->isArray) {
                if (field->arrayKw == kw::UNDEFINED) {
                    output << "    " << "void set_" << field->name
                        << "(const " << unionMeta->offsetTypeStr << " size) {" << std::endl;
                    output << "    " << "    " << "set(fields::" << field->name
                        << ", nullptr, size);" << std::endl;
                    output << "    " << "}" << std::endl;
                }
                else if (field->isOptional) {
                    output << "    " << "void set_" << field->name << "() {" << std::endl;
                    output << "    " << "    " << "set(fields::" << field->name
                        << ", nullptr, ";
                    if (isScalarType(field->arrayKw)) {
                        output << field->arraySize;
                    }
                    else {
                        for (const auto& ns : field->arrayPtr->namespace_) {
                            output << ns << "::";
                        }
                        switch (field->arrayKw) {
                        case kw::Const:
                            output << field->arrayPtr->constMeta()->name;
                            break;
                        case kw::Enum:
                            output << field->arrayPtr->enumMeta()->name << "::"
                                << field->arrayPtr->enumMeta()->valuesVec[
                                    field->arrayIdx].first;
                            break;
                        case kw::Min:
                            output << field->arrayPtr->enumMeta()->name << "::min()";
                            break;
                        case kw::Max:
                            output << field->arrayPtr->enumMeta()->name << "::max()";
                            break;
                        case kw::Count:
                            output << field->arrayPtr->enumMeta()->name << "::count()";
                            break;
                        default:
                            output << "%ERROR%";
                            break;
                        }
                    }
                    output << "); " << std::endl;
                    output << "    " << "}" << std::endl;
                }
                output << "    " << unionMeta->offsetTypeStr << " size_" << field->name
                    << "() const {" << std::endl;
                output << "    " << "    " << "return size(fields::" << field->name << ");" << std::endl;
                output << "    " << "}" << std::endl;
            }
            else {
                if (field->isBuiltIn) {
                    output << "    " << "void set_" << field->name << "(const " << type
                        << " value) {" << std::endl;
                    output << "    " << "    " << "set(fields::" << field->name
                        << ", &value, sizeof(" << type << "));" << std::endl;
                    output << "    " << "}" << std::endl;
                }
                else if (field->typeKw == kw::Enum) {
                    output << "    " << "void set_" << field->name << "(const ";
                    for (const auto& ns : field->typePtr->namespace_) {
                        output << ns << "::";
                    }
                    output << field->typePtr->enumMeta()->name << "::_ value) {" << std::endl;
                    output << "    " << "    " << "set(fields::" << field->name
                        << ", &value, sizeof(" << type << "));" << std::endl;
                    output << "    " << "}" << std::endl;
                }
                else {
                    output << "    " << "void set_" << field->name << "() {" << std::endl;
                    output << "    " << "    " << "set(fields::" << field->name
                        << ", nullptr, sizeof(" << type << "::table));" << std::endl;
                    output << "    " << "}" << std::endl;
                }
            }
        }
    }
    void writeUnion_table(WritingMeta& writingMeta, std::ostream& output,
            const AST& ast, const AST::UnionMeta* unionMeta) {
        output << "    " << "#pragma pack(1)" << std::endl;
        if (unionMeta->withHeader) {
            output << "    " << "struct header {" << std::endl;
            output << "    " << "    " << "int8_t ___i = 'i';" << std::endl;
            output << "    " << "    " << "int8_t ___b = 'b';" << std::endl;
            output << "    " << "    " << "int8_t ___s = 's';" << std::endl;
            output << "    " << "    " << "uint8_t ___v = (3 << 4) | 0;" << std::endl;
            output << "    " << "    " << "uint32_t ___sh = " << unionMeta->schemaHash << ";" << std::endl;
            output << "    " << "};" << std::endl;
        }
        output << "    " << "struct table {" << std::endl;
        output << "    " << "    " << "fields::_ variant = fields::_SPECIAL_;" << std::endl;
        output << "    " << "    " << unionMeta->offsetTypeStr << " offset = 0;" << std::endl;
        output << "    " << "};" << std::endl;
        output << "    " << "#pragma pack()" << std::endl;
    }
    void writeUnion_staff(WritingMeta& writingMeta, std::ostream& output,
            const AST& ast, const AST::UnionMeta* unionMeta) {
        output << "    " << "table* get_table() {" << std::endl;
        #ifdef _DEBUG
            output << "    " << "    " << "#ifdef _DEBUG" << std::endl;
            output << "    " << "    " << "m_table = reinterpret_cast<table*>(" << std::endl;
            output << "    " << "    " << "    " << "reinterpret_cast<uint8_t*>(base_ptr()) "
                "+ m_table_offset);" << std::endl;
            output << "    " << "    " << "#endif // _DEBUG" << std::endl;
        #endif // _DEBUG
        output << "    " << "    " << "return reinterpret_cast<table*>(" << std::endl;
        output << "    " << "    " << "    " << "reinterpret_cast<uint8_t*>(base_ptr()) "
            "+ m_table_offset);" << std::endl;
        output << "    " << "}" << std::endl;
        output << "    " << "const table* get_table() const {" << std::endl;
        output << "    " << "    " << "return const_cast<" << unionMeta->name
            << "*>(this)->get_table();" << std::endl;
        output << "    " << "}" << std::endl;
        output << "    " << "void* base_ptr() {" << std::endl;
        output << "    " << "    " << "if (m_buffer) {" << std::endl;
        output << "    " << "    " << "    " << "if (m_buffer->empty()) {" << std::endl;
        output << "    " << "    " << "    " << "    " << "return nullptr;" << std::endl;
        output << "    " << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "    " << "return m_buffer->data();" << std::endl;
        output << "    " << "    " << "}" << std::endl;
        output << "    " << "    " << "return m_from_ptr;" << std::endl;
        output << "    " << "}" << std::endl;
        output << "    " << "const void* base_ptr() const {" << std::endl;
        output << "    " << "    " << "return const_cast<"
            << unionMeta->name << "*>(this)->base_ptr();" << std::endl;
        output << "    " << "}" << std::endl;
        #ifdef _DEBUG
            output << "    " << "#ifdef _DEBUG" << std::endl;
            output << "    " << "table* m_table = nullptr;" << std::endl;
            output << "    " << "#endif // _DEBUG" << std::endl;
        #endif // _DEBUG
        output << "    " << "std::shared_ptr<std::vector<uint8_t>> m_buffer;" << std::endl;
        output << "    " << "uint8_t* m_from_ptr = nullptr;" << std::endl;
        output << "    " << "mutable " << unionMeta->offsetTypeStr << " m_from_size = 0;" << std::endl;
        output << "    " << unionMeta->offsetTypeStr << " m_table_offset = 0;" << std::endl;
    }
    void writeUnion(WritingMeta& writingMeta, std::ostream& output,
            const AST& ast, const AST::ObjectMeta* objectMeta) {
        const AST::UnionMeta* unionMeta = objectMeta->unionMeta();
        if (unionMeta->fieldsVec.empty()) {
            return;
        }
        unionMeta->offsetTypeStr = kw_to_domain_specific(unionMeta->offsetType);
        output << std::endl;
        output << "class " << unionMeta->name << " { // union " << unionMeta->name << std::endl;
        output << "public:" << std::endl;
        output << "    " << unionMeta->name << "() {}" << std::endl;
        //output << "    " << unionMeta->name << "(const uint32_t reserve = 0) {" << std::endl;
        //output << "    " << "    " << "create(reserve);" << std::endl;
        //output << "    " << "}" << std::endl;
        output << "    " << unionMeta->name << "(void* from_ptr, const "
            << unionMeta->offsetTypeStr << " from_size = 0) {" << std::endl;
        output << "    " << "    " << "from(from_ptr, from_size);" << std::endl;
        output << "    " << "}" << std::endl;
        output << "    " << unionMeta->name << "(const void* from_ptr, const "
            << unionMeta->offsetTypeStr << " from_size = 0) {" << std::endl;
        output << "    " << "    " << "from(from_ptr, from_size);" << std::endl;
        output << "    " << "}" << std::endl;
        writeUnion_create(writingMeta, output, ast, unionMeta);
        writeUnion_from_to(writingMeta, output, ast, unionMeta);
        writeUnion_size(writingMeta, output, ast, unionMeta);
        {
            writingMeta.spacing = "    ";
            std::unique_ptr<AST::EnumMeta> enumField = std::make_unique<AST::EnumMeta>();
            enumField->name = "fields";
            enumField->type = unionMeta->offsetType;
            for (uint32_t i = 0; i < unionMeta->fieldsVec.size(); ++i) {
                const auto& field = unionMeta->fieldsVec[i];
                enumField->valuesVec.emplace_back(field->name, i);
            }
            std::unique_ptr<AST::ObjectMeta> objectField = std::make_unique<AST::ObjectMeta>();
            objectField->putEnumMeta(std::move(enumField));
            writeEnum(writingMeta, output, ast, objectField.get());
        }
        output << std::endl;
        writeUnion_offset(writingMeta, output, ast, unionMeta);
        writeUnion_has(writingMeta, output, ast, unionMeta);
        writeUnion_get(writingMeta, output, ast, unionMeta);
        writeUnion_set(writingMeta, output, ast, unionMeta);
        writeUnion_size_field(writingMeta, output, ast, unionMeta);
        output << std::endl;
        writeUnion_has_get_set(writingMeta, output, ast, unionMeta);
        output << std::endl;
        writeUnion_table(writingMeta, output, ast, unionMeta);
        output << "private:" << std::endl;
        writeUnion_staff(writingMeta, output, ast, unionMeta);
        output << "}; // union " << unionMeta->name << std::endl;
    }

}

void INBCompiler::genCPP(const AST& ast, const std::string& outputSuffix) {
    WritingMeta writingMeta;
    for (const auto& fileMeta : ast.filesMeta) {
        std::string outputPath = fileMeta.name;
        outputPath.resize(outputPath.rfind('.'));
        outputPath += outputSuffix;
        outputPath += ".hpp";
#ifdef _WIN32
        std::ofstream output(utf::utf8ToUcs2(outputPath));
#else
        std::ofstream output(outputPath);
#endif
        if (output.is_open()) {
            std::cout << "OUTPUT: " << outputPath << std::endl;
        }
        else {
            std::cout << clr::red << "Error: Can't open file \""
                << outputPath << "\" for writing" << clr::reset << std::endl;
            continue;
        }
        std::string strDefineGuard;
        strDefineGuard.reserve(outputPath.size());
        for (const char c : outputPath) {
            switch (c) {
            case ':':
                continue;
            case '/':
            case '\\':
            case '.':
                if (!strDefineGuard.empty() && strDefineGuard.back() == '_') {
                    continue;
                }
                strDefineGuard.push_back('_');
                break;
            default:
                strDefineGuard.push_back(std::toupper(c));
                break;
            }
        }
        output << "// This file was generated by IndexBuffers Compiler " << g_version << "." << std::endl;
        output << "// https://github.com/yurablok/IndexBuffers" << std::endl;
        output << "// Do not modify." << std::endl;
        output << "#ifndef " << strDefineGuard << std::endl;
        output << "#define " << strDefineGuard << std::endl;

        output << std::endl;
        output << "#include <cstdint>" << std::endl;
        output << "#include <memory>" << std::endl;
        output << "#include <vector>" << std::endl;
        output << "#include <string>" << std::endl;
        output << "#include <unordered_map>" << std::endl;
        for (const auto& include_ : fileMeta.includes) {
            std::string temp = include_;
            temp.resize(temp.rfind('.'));
            temp += outputSuffix;
            temp += ".hpp";
            output << "#include \"" << temp << "\"" << std::endl;
        }

        for (const auto& namespaceMeta : fileMeta.ns) {
            output << std::endl;
            for (const auto& namespacePart : namespaceMeta.namespace_) {
                output << "namespace " << namespacePart << " {" << std::endl;
            }
            writingMeta.namespacePtr = &namespaceMeta.namespace_;

            for (const auto& object : namespaceMeta.objects) {
                switch (object->type()) {
                case kw::Const:
                    writeConst(writingMeta, output, ast, object);
                    break;
                case kw::Enum:
                    writingMeta.spacing.clear();
                    writeEnum(writingMeta, output, ast, object);
                    break;
                case kw::Struct:
                    writeStruct(writingMeta, output, ast, object);
                    break;
                case kw::Union:
                    writeUnion(writingMeta, output, ast, object);
                    break;
                default:
                    std::cout << clr::yellow << "Warning: Unexpected object type ("
                        << kw_to_keyword[static_cast<uint8_t>(object->type())]
                        << clr::reset << std::endl;
                    break;
                }
                writingMeta.previous = object->type();
            }

            output << std::endl;
            for (const auto& namespacePart : namespaceMeta.namespace_) {
                output << "} // " << namespacePart << std::endl;
            }
        }
        output << std::endl;
        output << "#endif // " << strDefineGuard << std::endl;
    }

    /*
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
        // ==           packet's staff          ==
        // === =============================== ===
        file << "    static uint32_t header()" << std::endl;
        file << "    {" << std::endl;
        file << "        uint32_t res;" << std::endl;
        file << "        _inner_::header *h = reinterpret_cast<_inner_::header*>(&res);" << std::endl;
        file << "        h->signature0 = 'i';" << std::endl;
        file << "        h->signature1 = 'b';" << std::endl;
        file << "        h->type = " << std::to_string(classId) << ";" << std::endl;
        file << "        return res;" << std::endl;
        file << "    }" << std::endl;
        file << "    void create(const uint32_t reserve = 0)" << std::endl;
        file << "    {" << std::endl;
        file << "        m_from = nullptr;" << std::endl;
        file << "        m_buffer = std::make_shared<std::vector<uint8_t>>();" << std::endl;
        file << "        m_buffer->reserve(reserve);" << std::endl;
        file << "        m_buffer->resize(sizeof(_inner_::header) + sizeof(Table));" << std::endl;
        file << "        _inner_::header *h = reinterpret_cast<_inner_::header*>(&m_buffer->data()[0]);" << std::endl;
        file << "        h->signature0  = 'i';" << std::endl;
        file << "        h->signature1  = 'b';" << std::endl;
        file << "        h->type = " << std::to_string(classId) << ";" << std::endl;
        file << "        m_table = sizeof(_inner_::header);" << std::endl;
        file << "    }" << std::endl;
        file << "    void create(std::shared_ptr<std::vector<uint8_t>> externalBuffer)" << std::endl;
        file << "    {" << std::endl;
        file << "        m_from = nullptr;" << std::endl;
        file << "        m_buffer = externalBuffer;" << std::endl;
        file << "        m_buffer->resize(sizeof(_inner_::header) + sizeof(Table));" << std::endl;
        file << "        _inner_::header *h = reinterpret_cast<_inner_::header*>(&m_buffer->data()[0]);" << std::endl;
        file << "        h->signature0  = 'i';" << std::endl;
        file << "        h->signature1  = 'b';" << std::endl;
        file << "        h->type = " << std::to_string(classId) << ";" << std::endl;
        file << "        m_table = sizeof(_inner_::header);" << std::endl;
        file << "    }" << std::endl;
        file << "    bool from(void* ptr)" << std::endl;
        file << "    {" << std::endl;
        file << "        if (!ptr)" << std::endl;
        file << "            return false;" << std::endl;
        file << "        const _inner_::header* h = reinterpret_cast<const _inner_::header*>(ptr);" << std::endl;
        file << "        if (h->signature0 != 'i' ||" << std::endl;
        file << "            h->signature1 != 'b' ||" << std::endl;
        file << "            h->type != " << std::to_string(classId) << ")" << std::endl;
        file << "            return false;" << std::endl;
        file << "        m_buffer.reset();" << std::endl;
        file << "        m_from = reinterpret_cast<uint8_t*>(ptr);" << std::endl;
    */
}
