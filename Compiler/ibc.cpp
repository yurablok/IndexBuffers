#include "stdafx.h"
#include "ibc.h"

namespace {
    void splitStr(const std::string& source, std::deque<std::string>& splitted,
        const char by = ' ') {
        
        if (source.empty()) {
            return;
        }
        std::string buffer;
        for (const char c : source) {
            if (c == by && !buffer.empty()) {
                splitted.emplace_back(std::move(buffer));
            }
            else {
                buffer.push_back(c);
            }
        }
        if (!buffer.empty()) {
            splitted.emplace_back(std::move(buffer));
        }
    }
}

INBCompiler::INBCompiler() {
    //test_next();
}

void INBCompiler::read(const std::string& input, const bool detailed) {
    std::cout << "INPUT: " << input << std::endl;
    m_detailed = detailed;

    ParsingMeta parsingMeta;
    if (!loadFileToLines(input, parsingMeta.lines)) {
        printErrorWrongPath(input);
        exit(0);
    }
    tokenize(parsingMeta.lines, parsingMeta.tokens);
    parsingMeta.path = getRelativeFilePath(input);
    if (parsingMeta.path.empty()) {
        printErrorWrongPath(input);
        exit(0);
    }
    m_processedFilesPaths.insert(parsingMeta.path);
    m_parsingErrorsCount = 0;
    if (!parse(parsingMeta)) {
        exit(0);
    }
    if (m_parsingErrorsCount != 0) {
        std::cout << clr::red << "The parsing was finished with "
            << m_parsingErrorsCount << " error";
        if (m_parsingErrorsCount > 1) {
            std::cout << 's';
        }
        std::cout << '.' << clr::reset << std::endl;
        exit(0);
    }
    calcSchemaHash();
}

void INBCompiler::write(const std::string& output, const Language& lang) {
    switch (lang) {
    case Language::CPP:
        genCPP(output);
        break;
    default:
        std::cout << clr::red << "Error: Unknown language code: "
            << static_cast<uint32_t>(lang) << clr::reset << std::endl;
        exit(0);
        //break;
    }
    std::cout << clr::green << "OUTPUT: " << output << clr::reset << std::endl;
}

void INBCompiler::setStopOnFirstError(const bool stopOnFirstError)
{
    m_stopOnFirstError = stopOnFirstError;
}

// Returns empty string if filePath not exist, or relative path
std::string INBCompiler::getRelativeFilePath(const std::string& filePath) {
#ifdef IBC_USE_STD_FILESYSTEM
    std::filesystem::path p = filePath;
    if (!std::filesystem::is_regular_file(p)) {
        return std::string();
    }
    std::error_code ec;
    return std::filesystem::relative(p, ec).string();
#else
    return path;
#endif // IBC_USE_STD_FILESYSTEM
}

bool INBCompiler::isNameCorrect(const std::string& name) const {
    if (name.empty()) {
        return false;
    }
    if (std::isdigit(name[0])) {
        return false;
    }
    for (const char& c : name) {
        if (!(c == '_' || std::isalnum(c))) {
            return false;
        }
    }
    return true;
}

char INBCompiler::checkScalarTypeMatchingRange(const kw typeKwSrc, const kw typeKwDest) const {
    switch (typeKwDest) {
    case kw::Int8:
        switch (typeKwSrc) {
        case kw::Int8:
            return 'y';
        case kw::Int16:
        case kw::Int32:
        case kw::Int64:
            return 'n';
        default:
            return 'e';
        }
        break;
    case kw::Int16:
        switch (typeKwSrc) {
        case kw::Int8:
        case kw::Int16:
            return 'y';
        case kw::Int32:
        case kw::Int64:
            return 'n';
        case kw::UInt8:
            return 'w';
        default:
            return 'e';
        }
        break;
    case kw::Int32:
        switch (typeKwSrc) {
        case kw::Int8:
        case kw::Int16:
        case kw::Int32:
            return 'y';
        case kw::Int64:
            return 'n';
        case kw::UInt8:
        case kw::UInt16:
            return 'w';
        default:
            return 'e';
        }
        break;
    case kw::Int64:
        switch (typeKwSrc) {
        case kw::Int8:
        case kw::Int16:
        case kw::Int32:
        case kw::Int64:
            return 'y';
        case kw::UInt8:
        case kw::UInt16:
        case kw::UInt32:
            return 'w';
        default:
            return 'e';
        }
        break;
    case kw::UInt8:
        switch (typeKwSrc) {
        case kw::UInt8:
            return 'y';
        case kw::UInt16:
        case kw::UInt32:
        case kw::UInt64:
            return 'n';
        default:
            return 'e';
        }
        break;
    case kw::UInt16:
        switch (typeKwSrc) {
        case kw::UInt8:
        case kw::UInt16:
            return 'y';
        case kw::UInt32:
        case kw::UInt64:
            return 'n';
        default:
            return 'e';
        }
        break;
    case kw::UInt32:
        switch (typeKwSrc) {
        case kw::UInt8:
        case kw::UInt16:
        case kw::UInt32:
            return 'y';
        case kw::UInt64:
            return 'n';
        default:
            return 'e';
        }
        break;
    case kw::UInt64:
        switch (typeKwSrc) {
        case kw::UInt8:
        case kw::UInt16:
        case kw::UInt32:
        case kw::UInt64:
            return 'y';
        default:
            return 'e';
        }
        break;
    case kw::Float32:
        switch (typeKwSrc) {
        case kw::Float32:
            return 'y';
        case kw::Float64:
        case kw::Int8:
        case kw::Int16:
        case kw::Int32:
        case kw::Int64:
        case kw::UInt8:
        case kw::UInt16:
        case kw::UInt32:
        case kw::UInt64:
            return 'w';
        default:
            return 'e';
        }
        break;
    case kw::Float64:
        switch (typeKwSrc) {
        case kw::Float32:
        case kw::Float64:
            return 'y';
        case kw::Int8:
        case kw::Int16:
        case kw::Int32:
        case kw::Int64:
        case kw::UInt8:
        case kw::UInt16:
        case kw::UInt32:
        case kw::UInt64:
            return 'w';
        default:
            return 'e';
        }
        break;
    case kw::Bytes:
    default:
        return 'e';
    }
    return 0;
}

void INBCompiler::insertObject(const std::deque<std::string>& namespace_,
    const std::string& name, std::unique_ptr<ObjectMeta> object) {

    TreeNode* node = &m_objectsTree;
    for (const auto& ns : namespace_) {
        TreeNode* parent = node;
        node = &node->namespaces[ns];
        node->parent = parent;
    }
    object->parent = node;
    object->namespace_ = namespace_;
    node->objects[name] = std::move(object);
}

INBCompiler::ObjectMeta* INBCompiler::findObject(
    const std::deque<std::string>& namespace_, const std::string& name) const {

    const TreeNode* node = &m_objectsTree;
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

INBCompiler::ObjectMeta* INBCompiler::findObject(
    const ParsingMeta& meta, tokens_it& it) const {

    std::deque<std::string> ns;
    for (uint32_t i = 0; i < 100; ++i) {
        ns.push_back(it->str);
        if (next(meta, it, true, true) != next_r('y', 'y')) {
            return nullptr;
            //break;
        }
        if (it->str == ".") {
            return nullptr;
        }
        if (it->str != ":") {
            break;
        }
        if (next(meta, it, true, true) != next_r('y', 'y')) {
            return nullptr;
        }
        if (it->str != ":") {
            printErrorWrongToken(meta, it, ":");
            return nullptr;
        }
        if (next(meta, it, true, true) != next_r('y', 'y')) {
            return nullptr;
        }
    }
    std::string name = std::move(ns.back());
    ns.pop_back();
    if (ObjectMeta* ptr = findObject(meta.namespace_, name)) {
        return ptr;
    }
    return findObject(ns, name);
}

bool INBCompiler::findValue(const ParsingMeta& meta, tokens_it& it,
    const ObjectMeta*& resultObjectMeta, kw& resultKw, uint32_t& resultIdx) const {

    resultObjectMeta = nullptr;
    resultKw = kw::UNDEFINED;
    resultIdx = 0;
    const TreeNode* node = &m_objectsTree;
    for (uint32_t i = 0; i < 100; ++i) {
        auto namespaceIt = node->namespaces.find(it->str);
        if (namespaceIt != node->namespaces.end()) {
            node = &namespaceIt->second;
            if (next(meta, it, true, true) != next_r('y', 'y')) {
                return false;
            }
            if (it->str == ":") {
                if (next(meta, it, true, true) != next_r('y', 'y')) {
                    return false;
                }
                if (it->str != ":") {
                    printErrorWrongToken(meta, it, ":");
                    skipLine(meta, it);
                    return false;
                }
                if (next(meta, it, true, true) != next_r('y', 'y')) {
                    return false;
                }
            }
            continue;
        }
        else {
            for (const auto& ns : meta.namespace_) {
                auto nodeIt = node->namespaces.find(ns);
                if (nodeIt == node->namespaces.end()) {
                    break;
                }
                node = &nodeIt->second;
            }
        }
        const auto itAtObject = it;
        const auto objectIt = node->objects.find(it->str);
        if (objectIt != node->objects.end()) {
            const ObjectMeta* objectMeta = objectIt->second.get();
            const next_r nr = next(meta, it, false);
            if (nr == next_r('a', 'n') || it == meta.tokens.end()) {
                if (objectMeta->type() != kw::Const) {
                    printErrorWrongToken(meta, itAtObject, "a const object");
                    return false;
                }
                resultObjectMeta = objectMeta;
                resultKw = kw::Const;
                return true;
            }
            if (nr == next_r('a', 'y')) {
                switch (objectMeta->type()) {
                case kw::Enum:
                    break;
                case kw::Const:
                default:
                    printErrorWrongToken(meta, itAtObject, "an enum object");
                    skipLine(meta, it);
                    return false;
                }
                if (it->str == ":") {
                    if (next(meta, it, true, true) != next_r('y', 'y')) {
                        return false;
                    }
                    if (it->str != ":") {
                        printErrorWrongToken(meta, it, ":");
                        skipLine(meta, it);
                        return false;
                    }
                    if (next(meta, it, true, true) != next_r('y', 'y')) {
                        return false;
                    }
                    const EnumMeta* enumMeta = objectMeta->enumMeta();
                    auto valueIdxIt = enumMeta->valuesMap.find(it->str);
                    if (valueIdxIt == enumMeta->valuesMap.end()) {
                        printErrorWrongToken(meta, it, "an existing value");
                        skipLine(meta, it);
                        return false;
                    }
                    if (next(meta, it, false) == next_r('y', 'y')) {
                        printErrorWrongToken(meta, it);
                        skipLine(meta, it);
                        return false;
                    }
                    resultObjectMeta = objectMeta;
                    resultKw = kw::Enum;
                    resultIdx = valueIdxIt->second;
                    return true;
                }
                else if (it->str == ".") {
                    if (next(meta, it, true, true) != next_r('y', 'y')) {
                        return false;
                    }
                    const kw keyword = findKeyword(it->str);
                    next(meta, it, false);
                    switch (keyword) {
                    case kw::Min:
                    case kw::Max:
                    case kw::Count:
                        resultObjectMeta = objectMeta;
                        resultKw = keyword;
                        resultIdx = 0;
                        return true;
                    default:
                        resultKw = kw::UNDEFINED;
                        return false;
                    }
                }
                else {
                    printErrorWrongToken(meta, it, ": or .");
                    skipLine(meta, it);
                    return false;
                }
            }
        }
        printErrorWrongToken(meta, it, "an existing object");
        skipLine(meta, it);
        return false;
    }
    return false;
}

void INBCompiler::calcSchemaHash() {
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

void INBCompiler::printErrorImpl_ErrorAt(
    const ParsingMeta& meta, const tokens_it& it,
    const uint32_t tokenSize) const {

    if (it == meta.tokens.end() || meta.lines.empty()) {
        std::cout << clr::red << "Error at " << meta.path
            << ':' << meta.lines.size() << ":?:" << std::endl;
        //std::cout << "^ Unexpected end of line. " << clr::reset << std::endl;
    }
    else {
        std::cout << clr::red << "Error at " << meta.path << ':'
            << it->line + 1 << ':' << it->pos + 1 << ':' << std::endl;
        std::cout << meta.lines[it->line] << std::endl;
        for (uint32_t i = 0; i < it->pos; ++i) {
            std::cout << ' ';
        }
        std::cout << '^';
        if (tokenSize == UINT32_MAX || tokenSize == 1) {
            for (uint32_t i = 0; i < it->str.size() - 1; ++i) {
                std::cout << '~';
            }
        }
        else {
            const uint32_t size = tokenSize < 100 ? tokenSize : 100;
            for (uint32_t i = 0; i < size - 1; ++i) {
                std::cout << '~';
            }
        }
        std::cout << std::endl;
    }
}
void INBCompiler::printWarningImpl_WarningAt(
    const ParsingMeta& meta, const tokens_it& it,
    const uint32_t tokenSize) const {

    if (it == meta.tokens.end() || meta.lines.empty()) {
        std::cout << clr::yellow << "Warning at " << meta.path
            << ':' << meta.lines.size() << ":?:" << std::endl;
    }
    else {
        std::cout << clr::yellow << "Warning at " << meta.path << ':'
            << it->line + 1 << ':' << it->pos + 1 << ':' << std::endl;
        std::cout << meta.lines[it->line] << std::endl;
        for (uint32_t i = 0; i < it->pos; ++i) {
            std::cout << ' ';
        }
        std::cout << '^';
        if (tokenSize == UINT32_MAX || tokenSize == 1) {
            for (uint32_t i = 0; i < it->str.size() - 1; ++i) {
                std::cout << '~';
            }
        }
        else {
            const uint32_t size = tokenSize < 100 ? tokenSize : 100;
            for (uint32_t i = 0; i < size - 1; ++i) {
                std::cout << '~';
            }
        }
        std::cout << std::endl;
    }
}
void INBCompiler::printWarningAlreadyParsed(
    const ParsingMeta& meta, const tokens_it& it) const {

    printWarningImpl_WarningAt(meta, it);
    std::cout << "^ The file was already parsed." << clr::reset << std::endl;
}
void INBCompiler::printWarningCustom(
    const ParsingMeta& meta, const tokens_it& it,
    const std::string& message, const uint32_t tokenSize) const {

    printWarningImpl_WarningAt(meta, it, tokenSize);
    std::cout << "^ " << message << clr::reset << std::endl;
}
void INBCompiler::printErrorCustom(
    const ParsingMeta& meta, const tokens_it& it,
    const std::string& message, const uint32_t tokenSize) const {

    printErrorImpl_ErrorAt(meta, it, tokenSize);
    std::cout << "^ " << message << clr::reset << std::endl;
}
void INBCompiler::printErrorWrongPath(const std::string& filePath) const {
    std::cout << clr::red << "Error: \"" << filePath << '"' << std::endl;
    std::cout << "^ Wrong file path." << clr::reset << std::endl;
}
void INBCompiler::printErrorWrongPath(
    const ParsingMeta& meta, const tokens_it& it) const {

    printErrorImpl_ErrorAt(meta, it);
    std::cout << "^ Wrong file path." << clr::reset << std::endl;
}
void INBCompiler::printErrorUnexpectedEndl(
    const ParsingMeta& meta, const tokens_it& it,
    const std::string& explanation) const {

    printErrorImpl_ErrorAt(meta, it);
    std::cout << "^ Unexpected end of line. " << explanation << clr::reset << std::endl;
}
void INBCompiler::printErrorWrongToken(
    const ParsingMeta& meta, const tokens_it& it,
    const std::string& expected) const {

    printErrorImpl_ErrorAt(meta, it);
    std::cout << "^ Wrong token: ";
    if (it->str.size() <= 1) {
        std::cout << '\'' << it->str << "\'.";
    }
    else {
        std::cout << '\"' << it->str << "\".";
    }
    if (expected.size() == 0) {
        std::cout << clr::reset << std::endl;
        return;
    }
    std::cout << " Expected token: ";
    if (expected.size() == 1) {
        std::cout << '\'' << expected << "\'.";
    }
    else if (expected.size() > 1) {
        std::cout << '\"' << expected << "\".";
    }
    std::cout << clr::reset << std::endl;
}
void INBCompiler::printErrorWrongKeywordUsage(
    const ParsingMeta& meta, const tokens_it& it) const {

    printErrorImpl_ErrorAt(meta, it);
    std::cout << "^ Wrong using of keyword: ";
    if (it->str.size() <= 1) {
        std::cout << '\'' << it->str << '\'';
    }
    else {
        std::cout << '\"' << it->str << '\"';
    }
    std::cout << clr::reset << std::endl;
}
void INBCompiler::printErrorNamespaceLimit(
    const ParsingMeta& meta, const tokens_it& it) const {

    printErrorImpl_ErrorAt(meta, it);
    std::cout << "^ Only one namespace definition per file." << clr::reset << std::endl;
}
void INBCompiler::printErrorNameAlreadyInUse(
    const ParsingMeta& meta, const tokens_it& it,
    const ObjectMeta* firstDefinition) const {

    printErrorImpl_ErrorAt(meta, it);
    std::cout << "^ The name \"" << it->str << "\" already in use";
    if (firstDefinition == nullptr) {
        std::cout << '.' << clr::reset << std::endl;
    }
    else {
        std::cout << " at " << firstDefinition->filePath << ':'
            << firstDefinition->fileLine << ':'
            << firstDefinition->filePos << '.' << clr::reset << std::endl;
    }
}
void INBCompiler::printErrorIncorrectName(
    const ParsingMeta& meta, const tokens_it& it) const {

    printErrorImpl_ErrorAt(meta, it);
    std::cout << "^ Incorrect name." << clr::reset << std::endl;
}
void INBCompiler::printErrorWrongDigit(const std::string& path,
    const lines_t& lines, const uint32_t line, const uint32_t posWrong,
    const char wrongDigit) const {

    std::cout << clr::red << "Error at " << path << ':' << line + 1 << ':'
        << posWrong + 1 << ':' << std::endl;
    std::cout << lines[line] << std::endl;
    for (uint32_t i = 0; i < posWrong; ++i) {
        std::cout << ' ';
    }
    std::cout << '^' << std::endl;
    std::cout << "^ Wrong digit: '" << wrongDigit << '\'' << clr::reset << std::endl;
}

