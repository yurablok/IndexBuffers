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
        node = &node->namespaces[ns];
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
        node = &nodeIt->second;
    }
    auto objectIt = node->objects.find(name);
    if (objectIt == node->objects.end()) {
        return nullptr;
    }
    return objectIt->second.get();
}

void AST::calcSchemaHash() {
    // uint32_t mm3 = 0;
    // for (const auto &ns : m_namespaces) {
    //     mm3 = MurmurHash3_x86_32(ns.data(), ns.size(), mm3);
    // }
    // //for (const auto &ct : m_constants) {
    // //}
    // for (const auto &en : m_enums) {
    //     mm3 = MurmurHash3_x86_32(en.first.data(), en.first.size(), mm3);
    //     for (const auto &f : en.second) {
    //         mm3 = MurmurHash3_x86_32(f.first.data(), f.first.size(), mm3);
    //         mm3 = MurmurHash3_x86_32(&f.second, sizeof(f.second), mm3);
    //     }
    // }
    // for (const auto &st : m_structs) {
    //     mm3 = MurmurHash3_x86_32(st.name.data(), st.name.size(), mm3);
    //     for (const auto &f : st.fields) {
    //         mm3 = MurmurHash3_x86_32(f.name.data(), f.name.size(), mm3);
    //         mm3 = MurmurHash3_x86_32(f.type.data(), f.type.size(), mm3);
    //         mm3 = MurmurHash3_x86_32(&f.isArray, sizeof(f.isArray), mm3);
    //         mm3 = MurmurHash3_x86_32(&f.isBuiltIn, sizeof(f.isBuiltIn), mm3);
    //         mm3 = MurmurHash3_x86_32(&f.isOptional, sizeof(f.isOptional), mm3);
    //     }
    // }
    // m_schemaHash = mm3; //(mm3 << 16) ^ (mm3 & 0xffff);
}
