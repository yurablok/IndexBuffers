#include "stdafx.h"
#include "ast_meta.h"

bool AST::isNamespacesEqual(const std::deque<std::string>& l,
        const std::deque<std::string>& r) {
    if (l.size() != r.size()) {
        return false;
    }
    for (uint32_t i = 0; i < l.size(); ++i) {
        if (l[i] != r[i]) {
            return false;
        }
    }
    return true;
}

void AST::insertObject(const AST::ParsingMeta& meta,
        const std::string& name, std::unique_ptr<AST::ObjectMeta> object) {
    AST::TreeNode* node = &objectsTree;
    for (const auto& ns : meta.namespace_) {
        TreeNode* parent = node;
        auto& ref = node->namespaces[ns];
        if (!ref) {
            ref = std::make_unique<TreeNode>();
        }
        node = ref.get();
        node->parent = parent;
    }
    object->parent = node;
    object->namespace_ = meta.namespace_;

    AST::FileMeta::NamespaceMeta* nsMeta = nullptr;
    if (meta.fileMeta->ns.empty()) {
        meta.fileMeta->ns.emplace_back();
        nsMeta = &meta.fileMeta->ns.back();
        nsMeta->namespace_ = meta.namespace_;
    }
    else if (!AST::isNamespacesEqual(
        meta.fileMeta->ns.back().namespace_, meta.namespace_)) {

        meta.fileMeta->ns.emplace_back();
        nsMeta = &meta.fileMeta->ns.back();
        nsMeta->namespace_ = meta.namespace_;
    }
    else {
        nsMeta = &meta.fileMeta->ns.back();
    }
    nsMeta->objects.push_back(object.get());

    node->objects[name] = std::move(object);
}

AST::ObjectMeta* AST::findObject(
        const std::deque<std::string>& namespace_, const std::string& name) const {
    const AST::TreeNode* node = &objectsTree;
    for (const auto& ns : namespace_) {
        auto nodeIt = node->namespaces.find(ns);
        if (nodeIt == node->namespaces.end()) {
            return nullptr;
        }
        node = nodeIt->second.get();
    }
    auto objectIt = node->objects.find(name);
    if (objectIt == node->objects.end()) {
        return nullptr;
    }
    return objectIt->second.get();
}

void AST::FieldMeta::calcSizeMinMax(uint64_t& min, uint64_t& max,
        const uint8_t offsetSize) const {
    uint64_t arraySizeTemp = 0;
    if (isArray) {
        if (isScalarType(arrayKw)) {
            arraySizeTemp = arraySize;
        }
        else switch (arrayKw) {
        case kw::Const:
            if (!arrayPtr->constMeta()->valueStr.empty()) {
                arraySizeTemp = std::stoull(arrayPtr->constMeta()->valueStr);
            }
            else if (arrayPtr->constMeta()->valuePtr) {
                switch (arrayPtr->constMeta()->valueKw) {
                case kw::Const:
                    std::cout << clr::red << "Error: Const of Const is not implemented yet!"
                        << clr::reset << std::endl;
                    arraySizeTemp = 0;
                    break;
                case kw::Enum:
                    arraySizeTemp = arrayPtr->constMeta()->valuePtr->
                        enumMeta()->valuesVec[
                            arrayPtr->constMeta()->valueIdx].second;
                    break;
                case kw::Min:
                    arraySizeTemp = arrayPtr->constMeta()->valuePtr->
                        enumMeta()->valuesVec.front().second;
                    break;
                case kw::Max:
                    arraySizeTemp = arrayPtr->constMeta()->valuePtr->
                        enumMeta()->valuesVec.back().second;
                    break;
                case kw::Count:
                    arraySizeTemp = arrayPtr->constMeta()->valuePtr->
                        enumMeta()->valuesVec.size();
                    break;
                default:
                    arraySizeTemp = 0; // warning?
                    break;
                }
            }
            break;
        case kw::Enum:
            arraySizeTemp = arrayPtr->enumMeta()->valuesVec[arrayIdx].second;
            break;
        case kw::Min:
            arraySizeTemp = arrayPtr->enumMeta()->valuesVec.front().second;
            break;
        case kw::Max:
            arraySizeTemp = arrayPtr->enumMeta()->valuesVec.back().second;
            break;
        case kw::Count:
            arraySizeTemp = arrayPtr->enumMeta()->valuesVec.size();
            break;
        default:
            arraySizeTemp = 0; // when typeKw == kw::Bytes
            break;
        }
    }
    // 0. fixed scalar               sizeof(type)
    // 1. fixed struct               for fields
    // 2. fixed array of scalars     sizeof(type) * size
    // 3. fixed array of structs     struct.calc * size
    // 4. optional scalar            sizeof(uint32_t) + {max}sizeof(type)
    // 5. optional struct            sizeof(uint32_t) + {max}struct.calc
    // 6. optional array of scalars  {min}sizeof(uint32_t) & {max}UINT64_MAX
    // 7. optional array of structs  {min}sizeof(uint32_t) & {max}UINT64_MAX
    if (typeKw == kw::Bytes) {
        if (isOptional) { // 6
            min += offsetSize;
            max = UINT64_MAX;
        }
        else { // 2
            min += sizeof(uint8_t) * arraySizeTemp;
            if (max != UINT64_MAX) {
                max += sizeof(uint8_t) * arraySizeTemp;
            }
        }
    }
    else if (isOptional) {
        if (isArray) {
            if (isScalar) { // 6
                min += offsetSize;
                max = UINT64_MAX;
            }
            else { // 7
                min += offsetSize;
                max = UINT64_MAX;
            }
        }
        else {
            if (isScalar) { // 4
                min += offsetSize;
                if (max != UINT64_MAX) {
                    max += offsetSize;
                    if (typeKw == kw::Enum) {
                        max += getScalarSize(typePtr->enumMeta()->type);
                    }
                    else {
                        max += getScalarSize(typeKw);
                    }
                }
            }
            else { // 5
                min += offsetSize;
                if (max != UINT64_MAX) {
                    max += offsetSize;
                    uint64_t minUnused = 0;
                    switch (typeKw) {
                    case kw::Struct:
                        typePtr->structMeta()->calcSizeMinMax(minUnused, max, false);
                        break;
                    case kw::Union:
                        typePtr->unionMeta()->calcSizeMinMax(minUnused, max, false);
                        break;
                    default:
                        break;
                    }
                }
            }
        }
    }
    else {
        if (isArray) {
            if (isScalar) { // 2
                uint8_t scalarSize = 0;
                if (typeKw == kw::Enum) {
                    scalarSize = getScalarSize(typePtr->enumMeta()->type);
                }
                else {
                    scalarSize = getScalarSize(typeKw);
                }
                min += scalarSize * arraySizeTemp;
                if (max != UINT64_MAX) {
                    max += scalarSize * arraySizeTemp;
                }
            }
            else { // 3
                uint64_t minTemp = 0;
                uint64_t maxTemp = 0;
                switch (typeKw) {
                case kw::Struct:
                    typePtr->structMeta()->calcSizeMinMax(minTemp, maxTemp, false);
                    break;
                case kw::Union:
                    typePtr->unionMeta()->calcSizeMinMax(minTemp, maxTemp, false);
                    break;
                default:
                    break;
                }
                min += minTemp * arraySizeTemp;
                if (max != UINT64_MAX && maxTemp != UINT64_MAX) {
                    max += maxTemp * arraySizeTemp;
                }
            }
        }
        else {
            if (isScalar) { // 0
                uint8_t scalarSize = 0;
                if (typeKw == kw::Enum) {
                    scalarSize = getScalarSize(typePtr->enumMeta()->type);
                }
                else {
                    scalarSize = getScalarSize(typeKw);
                }
                min += scalarSize;
                if (max != UINT64_MAX) {
                    max += scalarSize;
                }
            }
            else { // 1
                switch (typeKw) {
                case kw::Struct:
                    typePtr->structMeta()->calcSizeMinMax(min, max, false);
                    break;
                case kw::Union:
                    typePtr->unionMeta()->calcSizeMinMax(min, max, false);
                    break;
                default:
                    break;
                }
            }
        }
    }
}

void AST::StructMeta::calcSizeMinMax(uint64_t& min, uint64_t& max, bool isRoot) const {
    uint8_t offsetSize = 0;
    switch (offsetType) {
    case kw::UInt8: offsetSize = sizeof(uint8_t); break;
    case kw::UInt16: offsetSize = sizeof(uint16_t); break;
    case kw::UInt32: offsetSize = sizeof(uint32_t); break;
    case kw::UInt64: offsetSize = sizeof(uint64_t); break;
    default: break;
    }
    if (withHeader && isRoot) {
        min += 8; // sizeof(header)
        if (max != UINT64_MAX) {
            max += 8;
        }
    }
    for (const auto& field : fieldsVec) {
        field->calcSizeMinMax(min, max, offsetSize);
    }
}

void AST::UnionMeta::calcSizeMinMax(uint64_t& min, uint64_t& max, bool isRoot) const {
    uint8_t offsetSize = 0;
    switch (offsetType) {
    case kw::UInt8: offsetSize = sizeof(uint8_t); break;
    case kw::UInt16: offsetSize = sizeof(uint16_t); break;
    case kw::UInt32: offsetSize = sizeof(uint32_t); break;
    case kw::UInt64: offsetSize = sizeof(uint64_t); break;
    default: break;
    }
    uint64_t minRes = 0;
    uint64_t maxRes = 0;
    for (const auto& field : fieldsVec) {
        uint64_t minTemp = 0;
        uint64_t maxTemp = 0;
        field->calcSizeMinMax(minTemp, maxTemp, offsetSize);
        minRes = std::max(minRes, minTemp);
        maxRes = std::max(maxRes, maxTemp);
    }
    if (withHeader && isRoot) {
        min += 8; // sizeof(header)
        if (max != UINT64_MAX) {
            max += 8;
        }
    }
    min += offsetSize * 2; // sizeof(table), table { fields variant; offsetType offset; }
    if (max != UINT64_MAX) {
        if (maxRes == UINT64_MAX) {
            max = UINT64_MAX;
        }
        else {
            max += maxRes;
        }
    }
}

uint32_t AST::FieldMeta::calcHash() const {
    uint32_t hash = 0;
    uint32_t value = 0;

    switch (arrayKw) {
    case kw::Const: {
        const auto ptr = arrayPtr->constMeta()->valuePtr;
        switch (arrayPtr->constMeta()->valueKw) {
        case kw::Const:
            hash = MurmurHash3_x86_32(ptr->constMeta()->name.data(),
                static_cast<uint32_t>(ptr->constMeta()->name.size()), hash);
            break;
        case kw::Enum:
            hash = MurmurHash3_x86_32(&ptr->enumMeta()->valuesVec[arrayIdx].second,
                sizeof(ptr->enumMeta()->valuesVec[arrayIdx].second), hash);
            break;
        case kw::Min:
            hash = MurmurHash3_x86_32(&ptr->enumMeta()->valuesVec.front().second,
                sizeof(ptr->enumMeta()->valuesVec.front().second), hash);
            break;
        case kw::Max:
            hash = MurmurHash3_x86_32(&ptr->enumMeta()->valuesVec.back().second,
                sizeof(ptr->enumMeta()->valuesVec.back().second), hash);
            break;
        case kw::Count:
            value = static_cast<uint32_t>(ptr->enumMeta()->valuesVec.size());
            hash = MurmurHash3_x86_32(&value, sizeof(value), hash);
            break;
        default:
            break;
        }
        break;
    }
    case kw::Enum:
        hash = MurmurHash3_x86_32(&arrayPtr->enumMeta()->valuesVec[arrayIdx].second,
            sizeof(arrayPtr->enumMeta()->valuesVec[arrayIdx].second), hash);
        break;
    case kw::Min:
        hash = MurmurHash3_x86_32(&arrayPtr->enumMeta()->valuesVec.front().second,
            sizeof(arrayPtr->enumMeta()->valuesVec.front().second), hash);
        break;
    case kw::Max:
        hash = MurmurHash3_x86_32(&arrayPtr->enumMeta()->valuesVec.back().second,
            sizeof(arrayPtr->enumMeta()->valuesVec.back().second), hash);
        break;
    case kw::Count:
        value = static_cast<uint32_t>(arrayPtr->enumMeta()->valuesVec.size());
        hash = MurmurHash3_x86_32(&value, sizeof(value), hash);
        break;
    default:
        hash = MurmurHash3_x86_32(&arraySize, sizeof(arraySize), hash);
        break;
    }
    
    switch (typeKw) {
    case kw::Enum:
        for (const auto& v : typePtr->enumMeta()->valuesVec) {
            hash = MurmurHash3_x86_32(&v.second, sizeof(v.second), hash);
        }
        break;
    case kw::Struct:
        value = typePtr->structMeta()->calcHash();
        hash = MurmurHash3_x86_32(&value, sizeof(value), hash);
        break;
    case kw::Union:
        value = typePtr->unionMeta()->calcHash();
        hash = MurmurHash3_x86_32(&value, sizeof(value), hash);
        break;
    default:
        hash = MurmurHash3_x86_32(&typeKw, sizeof(typeKw), hash);
        break;
    }

    hash = MurmurHash3_x86_32(name.data(), static_cast<uint32_t>(name.size()), hash);

    switch (valueKw) {
    case kw::Const: {
        const auto ptr = valuePtr->constMeta()->valuePtr;
        switch (valuePtr->constMeta()->valueKw) {
        case kw::Const:
            hash = MurmurHash3_x86_32(ptr->constMeta()->name.data(),
                static_cast<uint32_t>(ptr->constMeta()->name.size()), hash);
            break;
        case kw::Enum:
            hash = MurmurHash3_x86_32(&ptr->enumMeta()->valuesVec[valueIdx].second,
                sizeof(ptr->enumMeta()->valuesVec[valueIdx].second), hash);
            break;
        case kw::Min:
            hash = MurmurHash3_x86_32(&ptr->enumMeta()->valuesVec.front().second,
                sizeof(ptr->enumMeta()->valuesVec.front().second), hash);
            break;
        case kw::Max:
            hash = MurmurHash3_x86_32(&ptr->enumMeta()->valuesVec.back().second,
                sizeof(ptr->enumMeta()->valuesVec.back().second), hash);
            break;
        case kw::Count:
            value = static_cast<uint32_t>(ptr->enumMeta()->valuesVec.size());
            hash = MurmurHash3_x86_32(&value, sizeof(value), hash);
            break;
        default:
            break;
        }
        break;
    }
    case kw::Enum:
        hash = MurmurHash3_x86_32(&valuePtr->enumMeta()->valuesVec[valueIdx].second,
            sizeof(valuePtr->enumMeta()->valuesVec[valueIdx].second), hash);
        break;
    case kw::Min:
        hash = MurmurHash3_x86_32(&valuePtr->enumMeta()->valuesVec.front().second,
            sizeof(valuePtr->enumMeta()->valuesVec.front().second), hash);
        break;
    case kw::Max:
        hash = MurmurHash3_x86_32(&valuePtr->enumMeta()->valuesVec.back().second,
            sizeof(valuePtr->enumMeta()->valuesVec.back().second), hash);
        break;
    case kw::Count:
        value = static_cast<uint32_t>(valuePtr->enumMeta()->valuesVec.size());
        hash = MurmurHash3_x86_32(&value, sizeof(value), hash);
        break;
    default:
        break;
    }

    hash = MurmurHash3_x86_32(&isOptional, sizeof(bool), hash);
    hash = MurmurHash3_x86_32(&isBuiltIn, sizeof(bool), hash);
    hash = MurmurHash3_x86_32(&isArray, sizeof(bool), hash);
    hash = MurmurHash3_x86_32(&isScalar, sizeof(bool), hash);
    return hash;
}

uint32_t AST::StructMeta::calcHash() const {
    uint32_t hash = 0;
    uint32_t value = 0;
    for (const auto& field : fieldsVec) {
        value = field->calcHash();
        hash = MurmurHash3_x86_32(&value, sizeof(value), hash);
    }
    return hash;
}

uint32_t AST::UnionMeta::calcHash() const {
    uint32_t hash = 0;
    uint32_t value = 0;
    for (const auto& field : fieldsVec) {
        value = field->calcHash();
        hash += value; // maybe wrong
    }
    return hash;
}
