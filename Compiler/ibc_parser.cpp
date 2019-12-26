#include "stdafx.h"
#include "ibc.h"

namespace {
    //constexpr std::string_view strEndl("\n"); //C++17
    static const std::string strEndl("\n");
}

bool INBCompiler::loadFileToLines(const std::string& filename, lines_t& lines) {
    lines.clear();
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << clr::red << "Error: Cannot open " << filename << clr::reset << std::endl;
        return false;
    }
    std::string buffer;
    while (!file.eof()) {
        std::getline(file, buffer);
        lines.push_back(std::move(buffer));
    }
    return true;
}

void INBCompiler::tokenize(const lines_t& lines, tokens_t& tokens) const {
    tokens.clear();
    tokens.emplace_back();
    bool isString = false;
    for (uint32_t lineIt = 0; lineIt < lines.size(); ++lineIt) {
        const auto& line = lines[lineIt];
        isString = false;
        for (uint32_t charIt = 0; charIt < line.size(); ++charIt) {
            const char c = line[charIt];
            switch (isString) {
            case true:
                switch (c) {
                case '"':
                    if (charIt > 0 && line[charIt - 1] == '\\') {
                        if (tokens.back().str.empty()) {
                            tokens.back().line = lineIt;
                            tokens.back().pos = charIt;
                        }
                        tokens.back().str.push_back(c);
                    }
                    else {
                        isString = false;
                        if (!tokens.back().str.empty()) {
                            tokens.emplace_back();
                        }
                        if (tokens.back().str.empty()) {
                            tokens.back().line = lineIt;
                            tokens.back().pos = charIt;
                        }
                        tokens.back().str.push_back('"');
                        tokens.emplace_back();
                    }
                    break;
                default:
                    if (tokens.back().str.empty()) {
                        tokens.back().line = lineIt;
                        tokens.back().pos = charIt;
                    }
                    tokens.back().str.push_back(c);
                    break;
                }
                break;
            case false:
                switch (c) {
                case ' ':
                case '\t':
                    if (!tokens.back().str.empty()) {
                        tokens.emplace_back();
                    }
                    break;
                case '"':
                    isString = true;
                    if (!tokens.back().str.empty()) {
                        tokens.emplace_back();
                    }
                    if (tokens.back().str.empty()) {
                        tokens.back().line = lineIt;
                        tokens.back().pos = charIt;
                    }
                    tokens.back().str.push_back('"');
                    tokens.emplace_back();
                    break;
                case '~':
                case '`':
                case '!':
                case '@':
                case '#':
                case '$':
                case ';':
                case '%':
                case '^':
                case '&':
                case '?':
                case '*':
                case '(':
                case ')':
                case '-':
                case '+':
                case '{':
                case '}':
                case '\\':
                case '|':
                case '\'':

                case '[':
                case ']':
                case ':':
                case ',':
                case '.':
                case '=':
                    if (!tokens.back().str.empty()) {
                        tokens.emplace_back();
                    }
                    if (tokens.back().str.empty()) {
                        tokens.back().line = lineIt;
                        tokens.back().pos = charIt;
                    }
                    tokens.back().str.push_back(c);
                    tokens.emplace_back();
                    break;
                case '/':
                    if (tokens.back().str.empty()) {
                        tokens.back().str = strEndl;
                        tokens.back().line = lineIt;
                        tokens.back().pos = charIt;
                        tokens.emplace_back();
                    }
                    else if (tokens.back().str != strEndl) {
                        tokens.emplace_back();
                        tokens.back().str = strEndl;
                        tokens.back().line = lineIt;
                        tokens.back().pos = charIt;
                        tokens.emplace_back();
                    }
                    charIt = UINT32_MAX - 1;
                    break;
                default:
                    if (tokens.back().str.empty()) {
                        tokens.back().line = lineIt;
                        tokens.back().pos = charIt;
                    }
                    tokens.back().str.push_back(c);
                    break;
                }
                break;
            }
        }
        if (tokens.back().str.empty()) {
            tokens.back().str = strEndl;
            tokens.back().line = lineIt;
            tokens.back().pos = static_cast<uint32_t>(line.size());
            tokens.emplace_back();
        }
        else if (tokens.back().str != strEndl) {
            tokens.emplace_back();
            tokens.back().str = strEndl;
            tokens.back().line = lineIt;
            tokens.back().pos = static_cast<uint32_t>(line.size());
            tokens.emplace_back();
        }
    }

    //for (auto it = tokens.begin(); it != tokens.end(); ++it) {
    //    if (it->str == strEndl) {
    //        std::cout << std::endl;
    //        continue;
    //    }
    //    std::cout << '"' << it->str << "\":" << it->line + 1 << '.' << it->pos + 1 << ' ';
    //}
    //std::cout << std::endl;
    //system("pause");
}

bool INBCompiler::parse(AST::ParsingMeta& meta) {
    tokens_it it = meta.tokens.cbegin();
    if (it != meta.tokens.cend() && it->str == strEndl) {
        next(meta, it, false);
    }
    tokens_it prev = it;
    while (it != meta.tokens.cend()) {
        const kw keyword = findKeyword(it->str);
        switch (keyword) {
        case kw::Include:
            if (!parseInclude(meta, it)) {
                ++m_parsingErrorsCount;
                if (m_stopOnFirstError) {
                    return false;
                }
            }
            break;
        case kw::Namespace:
            if (!parseNamespace(meta, it)) {
                ++m_parsingErrorsCount;
                if (m_stopOnFirstError) {
                    return false;
                }
            }
            break;
        case kw::Const:
            if (!parseConst(meta, it)) {
                ++m_parsingErrorsCount;
                if (m_stopOnFirstError) {
                    return false;
                }
            }
            break;
        case kw::Enum:
            if (!parseEnum(meta, it)) {
                ++m_parsingErrorsCount;
                if (m_stopOnFirstError) {
                    return false;
                }
            }
            break;
        case kw::Struct:
            if (!parseStruct(meta, it)) {
                ++m_parsingErrorsCount;
                if (m_stopOnFirstError) {
                    return false;
                }
            }
            break;
        case kw::Union:
            if (!parseUnion(meta, it)) {
                ++m_parsingErrorsCount;
                if (m_stopOnFirstError) {
                    return false;
                }
            }
            break;
        case kw::UNDEFINED:
            printErrorWrongToken(meta, it);
            ++m_parsingErrorsCount;
            if (m_stopOnFirstError) {
                return false;
            }
            break;
        default:
            printErrorWrongKeywordUsage(meta, it);
            ++m_parsingErrorsCount;
            if (m_stopOnFirstError) {
                return false;
            }
            break;
        }

        if (it == meta.tokens.cend()) {
            return true;
        }
        if (prev == it) {
            if (next(meta, it, false) == next_r('n')) {
                return true;
            }
        }
        prev = it;
    }
    std::cout << clr::red << "Undefined parsing error." << clr::reset << std::endl;
    return false;
}

// (<+> | <->)? <\d>+
bool INBCompiler::parseValueInt(AST::ParsingMeta& meta, tokens_it& it,
    std::string& valueStr, const bool quiet) const {

    valueStr.clear();
    if (it == meta.tokens.cend()) {
        return false;
    }
    if (it->str.empty() || it->str == strEndl) {
        printErrorUnexpectedEndl(meta, it);
        return false;
    }

    if (it->str == "+") {
        if (next(meta, it, true, true) != next_r('y', 'y')) { // <+> <?>
            return false;
        }
    }
    else if (it->str == "-") {
        valueStr.push_back('-');
        if (next(meta, it, true, true) != next_r('y', 'y')) { // <-> <?>
            return false;
        }
    }
    bool prevIsApostrophe = false;
    const tokens_it itFirst = it;
    while (true) {
        if (it->str.size() == 0) {
            if (next(meta, it, true, true) != next_r('y', 'y')) {
                return false;
            }
        }
        for (uint32_t i = 0; i < it->str.size(); ++i) {
            switch (it->str[i]) {
            case '\'':
                if (prevIsApostrophe == true || it == itFirst) {
                    if (!quiet) {
                        printErrorWrongDigit(meta.path, meta.lines, it->line,
                            it->pos + i, it->str[i]);
                    }
                    return false;
                }
                prevIsApostrophe = true;
                break;
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                valueStr.push_back(it->str[i]);
                prevIsApostrophe = false;
                break;
            default:
                if (!quiet) {
                    printErrorWrongDigit(meta.path, meta.lines, it->line,
                        it->pos + i, it->str[i]);
                }
                return false;
            }
        }
        if (prevIsApostrophe) {
            if (next(meta, it, true, true) != next_r('y', 'y')) {
                return false;
            }
        }
        else {
            if (next(meta, it, false) == next_r('y', 'y')) {
                continue;
            }
            else {
                return true;
            }
        }
    }
    return false;
}

// <\d>+
// <0>  (<b> | <B>) (<0> | <1>)+
// <0>  (<x> | <X>) (<\d> | <[a-fA-F]>)+
bool INBCompiler::parseValueUInt(AST::ParsingMeta& meta, tokens_it& it,
    std::string& valueStr, uint8_t& numeralSystem, const bool quiet) const {

    valueStr.clear();
    if (it == meta.tokens.cend()) {
        return false;
    }
    if (it->str.empty() || it->str == strEndl) {
        printErrorUnexpectedEndl(meta, it);
        return false;
    }
    numeralSystem = 10;
    bool prevIsApostrophe = false;
    bool allowedB = false;
    bool modeBin = false;
    bool allowedX = false;
    bool modeHex = false;
    while (true) {
        if (it->str.size() == 0) {
            if (next(meta, it, true, true) != next_r('y', 'y')) {
                return false;
            }
        }
        for (uint32_t i = 0; i < it->str.size(); ++i) {
            char c = it->str[i];
            switch (c) {
            case '\'':
                if (prevIsApostrophe == true) {
                    if (!quiet) {
                        printErrorWrongDigit(meta.path, meta.lines, it->line,
                            it->pos + i, c);
                    }
                    return false;
                }
                prevIsApostrophe = true;
                break;
            case 'b': case 'B':
                if (allowedB == false) {
                    if (!quiet) {
                        printErrorWrongDigit(meta.path, meta.lines, it->line,
                            it->pos + i, c);
                    }
                    return false;
                }
                if (modeHex == false && modeBin == true) {
                    if (!quiet) {
                        printErrorWrongDigit(meta.path, meta.lines, it->line,
                            it->pos + i, c);
                    }
                    return false;
                }
                if (modeHex == false) {
                    if (valueStr != "0") {
                        if (!quiet) {
                            printErrorWrongDigit(meta.path, meta.lines, it->line,
                                it->pos + i, c);
                        }
                        return false;
                    }
                    modeBin = true;
                    numeralSystem = 2;
                }
                if (modeHex == true) { // hex
                    valueStr.push_back('B');
                }
                else { // modeHex == false // bin
                    valueStr.push_back('b');
                }
                prevIsApostrophe = false;
                break;
            case 'x': case 'X':
                if (allowedX == false) {
                    if (!quiet) {
                        printErrorWrongDigit(meta.path, meta.lines, it->line,
                            it->pos + i, c);
                    }
                    return false;
                }
                if (modeHex == true) {
                    if (!quiet) {
                        printErrorWrongDigit(meta.path, meta.lines, it->line,
                            it->pos + i, c);
                    }
                    return false;
                }
                if (valueStr != "0") {
                    if (!quiet) {
                        printErrorWrongDigit(meta.path, meta.lines, it->line,
                            it->pos + i, c);
                    }
                    return false;
                }
                modeHex = true;
                numeralSystem = 16;
                valueStr.push_back('x');
                prevIsApostrophe = false;
                break;
            case 'a': case 'c': case 'd': case 'e': case 'f':
                if (modeHex == false) {
                    if (!quiet) {
                        printErrorWrongDigit(meta.path, meta.lines, it->line,
                            it->pos + i, c);
                    }
                    return false;
                }
                c -= 'a' - 'A';
            case 'A': case 'C': case 'D': case 'E': case 'F':
                if (modeHex == false) {
                    if (!quiet) {
                        printErrorWrongDigit(meta.path, meta.lines, it->line,
                            it->pos + i, c);
                    }
                    return false;
                }
                valueStr.push_back(c);
                prevIsApostrophe = false;
                break;
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                if (modeBin == false) {
                    allowedB = true;
                }
                else { // modeBin == true
                    if (c > '1') {
                        if (!quiet) {
                            printErrorWrongDigit(meta.path, meta.lines, it->line,
                                it->pos + i, c);
                        }
                        return false;
                    }
                }
                if (modeHex == false) {
                    allowedX = true;
                }
                valueStr.push_back(c);
                prevIsApostrophe = false;
                break;
            case ']':
                if (quiet) {
                    return true;
                }
                // don't break
            default:
                if (!quiet) {
                    printErrorWrongDigit(meta.path, meta.lines, it->line,
                        it->pos + i, c);
                }
                return false;
            }
        }
        if (prevIsApostrophe) {
            if (next(meta, it, true, true) != next_r('y', 'y')) {
                return false;
            }
        }
        else {
            if (next(meta, it, false) == next_r('y', 'y')) {
                continue;
            }
            else {
                return true;
            }
        }
    }
    return false;
}

// (<+> | <->)? <\d>+ (<.> <\d>+)?
// (<+> | <->)? <\d>+ <.> <\d>+ <e> (<+> | <->) <\d>+
bool INBCompiler::parseValueFloat(AST::ParsingMeta& meta, tokens_it& it,
    std::string& valueStr, const bool quiet) const {

    valueStr.clear();
    if (it == meta.tokens.cend()) {
        return false;
    }
    if (it->str.empty() || it->str == strEndl) {
        printErrorUnexpectedEndl(meta, it);
        return false;
    }
    bool prevIsApostrophe = false;
    bool allowedSign = true;
    bool allowedDot = false;
    bool foundDot = false;
    bool allowedE = false;
    bool foundE = false;
    while (true) {
        if (it->str.size() == 0) {
            if (next(meta,it, true, true) != next_r('y', 'y')) {
                return false;
            }
        }
        for (uint32_t i = 0; i < it->str.size(); ++i) {
            const char c = it->str[i];
            switch (c) {
            case '\'':
                if (prevIsApostrophe == true) {
                    if (!quiet) {
                        printErrorWrongDigit(meta.path, meta.lines, it->line,
                            it->pos + i, c);
                    }
                    return false;
                }
                prevIsApostrophe = true;
                break;
            case '+':
            case '-':
                if (allowedSign == false) {
                    if (!quiet) {
                        printErrorWrongDigit(meta.path, meta.lines, it->line,
                            it->pos + i, c);
                    }
                    return false;
                }
                allowedSign = false;
                if (c == '-') {
                    valueStr.push_back('-');
                }
                else if (foundE == true) {
                    valueStr.push_back('+');
                }
                prevIsApostrophe = true;
                break;
            case '.':
                if (allowedDot == false) {
                    if (!quiet) {
                        printErrorWrongDigit(meta.path, meta.lines, it->line,
                            it->pos + i, c);
                    }
                    return false;
                }
                allowedDot = false;
                foundDot = true;
                valueStr.push_back('.');
                prevIsApostrophe = true;
                break;
            case 'e': case 'E':
                if (allowedE == false) {
                    if (!quiet) {
                        printErrorWrongDigit(meta.path, meta.lines, it->line,
                            it->pos + i, c);
                    }
                    return false;
                }
                allowedE = false;
                foundE = true;
                allowedSign = true;
                valueStr.push_back('e');
                prevIsApostrophe = true;
                break;
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
                if (foundDot == false) {
                    allowedDot = true;
                }
                else { // foundDot == true
                    if (foundE == false) {
                        allowedE = true;
                    }
                    else { // foundE == true
                        if (allowedSign == true) {
                            if (!quiet) {
                                printErrorWrongDigit(meta.path, meta.lines, it->line,
                                    it->pos + i, c);
                            }
                            return false;
                        }
                    }
                }
                valueStr.push_back(c);
                prevIsApostrophe = false;
                break;
            default:
                if (!quiet) {
                    printErrorWrongDigit(meta.path, meta.lines, it->line,
                        it->pos + i, c);
                }
                return false;
            }
        }
        if (prevIsApostrophe) {
            if (next(meta, it, true, true) != next_r('y', 'y')) {
                return false;
            }
        }
        else {
            if (next(meta, it, false) == next_r('y', 'y')) {
                continue;
            }
            else {
                return true;
            }
        }
    }
    return false;
}

bool INBCompiler::parseValue(AST::ParsingMeta& meta, tokens_it& it,
    const kw& expectedValueType, std::string& valueStr,
    const bool quiet) const {

    tokens_it itBegin = it;
    switch (expectedValueType) {
    case kw::Int8:
    case kw::Int16:
    case kw::Int32:
    case kw::Int64:
        if (!parseValueInt(meta, it, valueStr, quiet)) {
            return false;
        }

        switch (expectedValueType) {
        case kw::Int8:
            try {
                const auto temp = std::stoi(valueStr);
                static_assert(sizeof(temp) > sizeof(int8_t));
                if (temp < INT8_MIN || temp > INT8_MAX) {
                    throw std::logic_error(nullptr);
                }
            }
            catch (...) {
                printErrorCustom(meta, itBegin, "The value " + valueStr
                    + " not in range [" + std::string("INT8_MIN..INT8_MAX") + "]",
                    it->pos - itBegin->pos);
                return false;
            }
            break;
        case kw::Int16:
            try {
                const auto temp = std::stoi(valueStr);
                static_assert(sizeof(temp) > sizeof(int16_t));
                if (temp < INT16_MIN || temp > INT16_MAX) {
                    throw std::logic_error(nullptr);
                }
            }
            catch (...) {
                printErrorCustom(meta, itBegin, "The value " + valueStr
                    + " not in range [" + std::string("INT8_MIN..INT8_MAX") + "]",
                    it->pos - itBegin->pos);
                return false;
            }
            break;
        case kw::Int32:
            try {
                const auto temp = std::stoi(valueStr);
                static_assert(sizeof(temp) == sizeof(int32_t));
            }
            catch (...) {
                printErrorCustom(meta, itBegin, "The value " + valueStr
                    + " not in range [" + std::string("INT32_MIN..INT32_MAX") + "]",
                    it->pos - itBegin->pos);
                return false;
            }
            break;
        case kw::Int64:
            try {
                const auto temp = std::stoll(valueStr);
                static_assert(sizeof(temp) == sizeof(int64_t));
            }
            catch (...) {
                printErrorCustom(meta, itBegin, "The value " + valueStr
                    + " not in range [" + std::string("INT64_MIN..INT64_MAX") + "]",
                    it->pos - itBegin->pos);
                return false;
            }
            break;
        }
        break;
    case kw::UInt8:
    case kw::UInt16:
    case kw::UInt32:
    case kw::UInt64: {
        uint8_t numeralSystem = 0;
        if (!parseValueUInt(meta, it, valueStr, numeralSystem, quiet)) {
            return false;
        }

        switch (expectedValueType) {
        case kw::UInt8:
            switch (numeralSystem) {
            case 10:
                try {
                    const auto temp = std::stoul(valueStr);
                    static_assert(sizeof(temp) > sizeof(uint8_t));
                    if (temp > UINT8_MAX) {
                        throw std::logic_error(nullptr);
                    }
                }
                catch (...) {
                    printErrorCustom(meta, itBegin, "The value " + valueStr
                        + " not in range [" + std::string("0..UINT8_MAX") + "]",
                        it->pos - itBegin->pos);
                    return false;
                }
                break;
            case 2:
                if (valueStr.size() > 2 + sizeof(uint8_t) * 8) {
                    printErrorCustom(meta, itBegin, "The value " + valueStr
                        + " cannot be stored in " + std::string("1 byte"),
                        it->pos - itBegin->pos);
                    return false;
                }
                break;
            case 16:
                if (valueStr.size() > 2 + sizeof(uint8_t) * 2) {
                    printErrorCustom(meta, itBegin, "The value " + valueStr
                        + " cannot be stored in " + std::string("1 byte"),
                        it->pos - itBegin->pos);
                    return false;
                }
                break;
            }
            break;
        case kw::UInt16:
            switch (numeralSystem) {
            case 10:
                try {
                    const auto temp = std::stoul(valueStr);
                    static_assert(sizeof(temp) > sizeof(uint16_t));
                    if (temp > UINT16_MAX) {
                        throw std::logic_error(nullptr);
                    }
                }
                catch (...) {
                    printErrorCustom(meta, itBegin, "The value " + valueStr
                        + " not in range [" + std::string("0..UINT16_MAX") + "]",
                        it->pos - itBegin->pos);
                    return false;
                }
                break;
            case 2:
                if (valueStr.size() > 2 + sizeof(uint16_t) * 8) {
                    printErrorCustom(meta, itBegin, "The value " + valueStr
                        + " cannot be stored in " + std::string("2 bytes"),
                        it->pos - itBegin->pos);
                    return false;
                }
                break;
            case 16:
                if (valueStr.size() > 2 + sizeof(uint16_t) * 2) {
                    printErrorCustom(meta, itBegin, "The value " + valueStr
                        + " cannot be stored in " + std::string("2 bytes"),
                        it->pos - itBegin->pos);
                    return false;
                }
                break;
            }
            break;
        case kw::UInt32:
            switch (numeralSystem) {
            case 10:
                try {
                    const auto temp = std::stoul(valueStr);
                    static_assert(sizeof(temp) == sizeof(uint32_t));
                }
                catch (...) {
                    printErrorCustom(meta, itBegin, "The value " + valueStr
                        + " not in range [" + std::string("0..UINT32_MAX") + "]",
                        it->pos - itBegin->pos);
                    return false;
                }
                break;
            case 2:
                if (valueStr.size() > 2 + sizeof(uint32_t) * 8) {
                    printErrorCustom(meta, itBegin, "The value " + valueStr
                        + " cannot be stored in " + std::string("4 bytes"),
                        it->pos - itBegin->pos);
                    return false;
                }
                break;
            case 16:
                if (valueStr.size() > 2 + sizeof(uint32_t) * 2) {
                    printErrorCustom(meta, itBegin, "The value " + valueStr
                        + " cannot be stored in " + std::string("4 bytes"),
                        it->pos - itBegin->pos);
                    return false;
                }
                break;
            }
            break;
        case kw::UInt64:
            switch (numeralSystem) {
            case 10:
                try {
                    const auto temp = std::stoull(valueStr);
                    static_assert(sizeof(temp) == sizeof(uint64_t));
                }
                catch (...) {
                    printErrorCustom(meta, itBegin, "The value " + valueStr
                        + " not in range [" + std::string("0..UINT64_MAX") + "]",
                        it->pos - itBegin->pos);
                    return false;
                }
                break;
            case 2:
                if (valueStr.size() > 2 + sizeof(uint64_t) * 8) {
                    printErrorCustom(meta, itBegin, "The value " + valueStr
                        + " cannot be stored in " + std::string("8 bytes"),
                        it->pos - itBegin->pos);
                    return false;
                }
                break;
            case 16:
                if (valueStr.size() > 2 + sizeof(uint64_t) * 2) {
                    printErrorCustom(meta, itBegin, "The value " + valueStr
                        + " cannot be stored in " + std::string("8 bytes"),
                        it->pos - itBegin->pos);
                    return false;
                }
                break;
            }
            break;
        }
        break;
    }
    case kw::Float32:
    case kw::Float64:
        if (!parseValueFloat(meta, it, valueStr, quiet)) {
            return false;
        }
        break;
    default:
        return false;
    }
    return true;
}

// <include> <"> <filepath> <">
bool INBCompiler::parseInclude(AST::ParsingMeta& meta, tokens_it& it) {
    if (next(meta, it, true, true) != next_r('y', 'y')) { // <include> <?>
        return false;
    }
    if (it->str != "\"") { // <include> <">
        printErrorWrongToken(meta, it, "\"");
        skipLine(meta, it);
        return false;
    }
    if (next(meta, it, true, true) != next_r('y', 'y')) { // <include> <"> <?>
        return false;
    }
    std::string includePath = getRelativeFilePath(it->str);
    const tokens_it itPath = it;
    if (next(meta, it, true, true) != next_r('y', 'y')) { // <include> <"> <filepath> <?>
        return false;
    }
    if (it->str != "\"") { // <include> <"> <filepath> <">
        printErrorWrongToken(meta, it, "\"");
        skipLine(meta, it);
        return false;
    }
    if (next(meta, it, false) == next_r('y', 'y')) {
        printErrorWrongToken(meta, it, "end of line");
        skipLine(meta, it);
        return false;
    }

    if (includePath.empty()) {
        printErrorWrongPath(meta, itPath);
        return false;
    }

    if (m_processedFilesPaths.count(includePath) == 0) {
        if (m_detailed) {
            std::cout << "INCLUDE: " << clr::blue << includePath << clr::reset << std::endl;
        }
        AST::ParsingMeta includeMeta;
        if (!loadFileToLines(includePath, includeMeta.lines)) {
            printErrorWrongPath(includePath);
            return false;
        }
        tokenize(includeMeta.lines, includeMeta.tokens);
        m_processedFilesPaths.insert(includePath);
        includeMeta.path = includePath;
        m_ast.filesMeta.emplace_back();
        m_ast.filesMeta.back().name = includeMeta.path;
        meta.fileMeta->includes.push_back(includeMeta.path);
        for (char& c : meta.fileMeta->includes.back()) {
            if (c == '\\') {
                c = '/';
            }
        }
        includeMeta.fileMeta = &m_ast.filesMeta.back();
        if (!parse(includeMeta)) {
            return false;
        }
    }
    else {
        printWarningAlreadyParsed(meta, itPath);
    }
    return true;
}

// <namespace> <EXTERNAL::INTERNAL>
bool INBCompiler::parseNamespace(AST::ParsingMeta& meta, tokens_it& it) {
    if (!meta.namespace_.empty()) {
        printErrorNamespaceLimit(meta, it);
        skipLine(meta, it);
        return false;
    }

    for (uint32_t i = 0; i < 100; ++i) {
        if (next(meta, it, true, true) != next_r('y', 'y')) {
            return false;
        }
        if (findKeyword(it->str) != kw::UNDEFINED) {
            printErrorWrongKeywordUsage(meta, it);
            return false;
        }
        if (isSpecialToken(it->str)) {
            printErrorWrongToken(meta, it);
            return false;
        }
        meta.namespace_.push_back(it->str);
        if (next(meta, it, false) != next_r('y', 'y')) {
            if (m_detailed) {
                std::cout << "NAMESPACE: " << clr::blue;
                for (uint32_t i = 0; i < meta.namespace_.size(); ++i) {
                    if (i > 0) {
                        std::cout << "::";
                    }
                    std::cout << meta.namespace_[i];
                }
                std::cout << clr::reset << std::endl;
            }
            return true;
        }
        if (it->str != ":") {
            printErrorWrongToken(meta, it);
            return false;
        }
        if (next(meta, it, true, true) != next_r('y', 'y')) {
            return false;
        }
        if (it->str != ":") {
            printErrorWrongToken(meta, it, ":");
            return false;
        }
    }
    printErrorCustom(meta, it, "Too many namespace levels (>100).");
    return false;
}

// <const> <TYPE> <NAME> <=> <VALUE>
bool INBCompiler::parseConst(AST::ParsingMeta& meta, tokens_it& it) {
    const std::string& line = meta.lines[it->line];
    if (next(meta, it, true, true) != next_r('y', 'y')) { // <const> <?>
        return false;
    }
    kw constType = findKeyword(it->str);
    if (!isScalarType(constType)) { // <const> <TYPE>
        printErrorCustom(meta, it, "The type of the constant must be numeric.");
        skipLine(meta, it);
        return false;
    }
    const std::string& typeStr = it->str;
    if (next(meta, it, true, true) != next_r('y', 'y')) { // <const> <TYPE> <?>
        return false;
    }
    if (!isNameCorrect(it->str)) {
        printErrorIncorrectName(meta, it);
        skipLine(meta, it);
        return false;
    }
    if (findKeyword(it->str) != kw::UNDEFINED) { // <const> <TYPE> <?>
        printErrorWrongKeywordUsage(meta, it);
        skipLine(meta, it);
        return false;
    }
    const AST::ObjectMeta* checkObject = m_ast.findObject(meta.namespace_, it->str);
    if (checkObject != nullptr) { // <const> <TYPE> <NAME>
        printErrorNameAlreadyInUse(meta, it, checkObject);
        skipLine(meta, it);
        return false;
    }
    tokens_it itConstName = it;
    if (next(meta, it, true, true) != next_r('y', 'y')) { // <const> <TYPE> <NAME> <?>
        return false;
    }
    if (it->str != "=") { // <const> <TYPE> <NAME> <=>
        printErrorWrongToken(meta, it, "=");
        skipLine(meta, it);
        return false;
    }
    if (next(meta, it, true, true) != next_r('y', 'y')) { // <const> <TYPE> <NAME> <=> <?>
        return false;
    }
    const tokens_it itAtValue = it;
    std::unique_ptr<AST::ConstMeta> constMeta = std::make_unique<AST::ConstMeta>();
    constMeta->type = constType;
    constMeta->name = itConstName->str;

    const AST::ObjectMeta* resultObjectMeta = nullptr;
    kw resultKw = kw::UNDEFINED;
    uint32_t resultIdx = 0;
    if (parseValue(meta, it, constType, constMeta->valueStr, true)) {
        constMeta->valueKw = constType;
    }
    else if (findValue(meta, it, resultObjectMeta, resultKw, resultIdx)) {
        switch (resultKw) {
        case kw::Const:
            switch (checkScalarTypeMatchingRange(
                resultObjectMeta->constMeta()->type, constType)) {
            case 'y':
                constMeta->valueKw = resultKw;
                constMeta->valuePtr = resultObjectMeta;
                constMeta->valueIdx = resultIdx;
                break;
            case 'n':
                printErrorCustom(meta, itAtValue,
                    "The value is out of range allowed by the type.");
                it = itAtValue;
                skipLine(meta, it);
                return false;
            case 'w':
                printWarningCustom(meta, itAtValue,
                    "Type mismatch.");
                constMeta->valueKw = resultKw;
                constMeta->valuePtr = resultObjectMeta;
                constMeta->valueIdx = resultIdx;
                break;
            case 'e':
            default:
                printErrorCustom(meta, itAtValue,
                    "Type mismatch.");
                it = itAtValue;
                skipLine(meta, it);
                return false;
            }
            break;
        case kw::Enum:
            switch (checkScalarTypeMatchingRange(
                resultObjectMeta->enumMeta()->type, constType)) {
            case 'y':
                constMeta->valueKw = resultKw;
                constMeta->valuePtr = resultObjectMeta;
                constMeta->valueIdx = resultIdx;
                break;
            case 'n':
                printErrorCustom(meta, itAtValue,
                    "The value is out of range allowed by the type.");
                it = itAtValue;
                skipLine(meta, it);
                return false;
            case 'w':
                printWarningCustom(meta, itAtValue,
                    "Type mismatch.");
                constMeta->valueKw = resultKw;
                constMeta->valuePtr = resultObjectMeta;
                constMeta->valueIdx = resultIdx;
                break;
            case 'e':
            default:
                printErrorCustom(meta, itAtValue,
                    "Type mismatch.");
                it = itAtValue;
                skipLine(meta, it);
                return false;
            }
            break;
        case kw::Min:
        case kw::Max:
        case kw::Count:
            constMeta->valueKw = resultKw;
            constMeta->valuePtr = resultObjectMeta;
            constMeta->valueIdx = resultIdx;
            break;
        default:
            printErrorCustom(meta, itAtValue, "Can't parse value.");
            it = itAtValue;
            skipLine(meta, it);
            return false;
        }
    }
    else {
        printErrorCustom(meta, itAtValue, "Can't parse value.");
        it = itAtValue;
        skipLine(meta, it);
        return false;
    }

    if (m_detailed) {
        std::cout << "CONSTANT: " << typeStr << " ";
        for (uint32_t i = 0; i < meta.namespace_.size(); ++i) {
            std::cout << meta.namespace_[i] << "::";
        }
        std::cout << clr::blue << constMeta->name << clr::reset << " = " << clr::cyan;
        if (!constMeta->valueStr.empty()) {
            std::cout << constMeta->valueStr;
        }
        else switch (constMeta->valueKw) {
        case kw::Const:
            std::cout << constMeta->valuePtr->constMeta()->name;
            break;
        case kw::Enum:
            std::cout
                << constMeta->valuePtr->enumMeta()->valuesVec[constMeta->valueIdx].first << "("
                << constMeta->valuePtr->enumMeta()->valuesVec[constMeta->valueIdx].second << ")";
            break;
        case kw::Min:
            std::cout
                << constMeta->valuePtr->enumMeta()->name << ".min("
                << constMeta->valuePtr->enumMeta()->valuesVec.front().second << ")";
            break;
        case kw::Max:
            std::cout
                << constMeta->valuePtr->enumMeta()->name << ".max("
                << constMeta->valuePtr->enumMeta()->valuesVec.back().second << ")";
            break;
        case kw::Count:
            std::cout
                << constMeta->valuePtr->enumMeta()->name << ".count("
                << constMeta->valuePtr->enumMeta()->valuesVec.size() << ")";
            break;
        default:
            break;
        }
        std::cout << clr::reset << std::endl;
    }
    std::unique_ptr<AST::ObjectMeta> objectMeta = std::make_unique<AST::ObjectMeta>(
        meta.path, itConstName->line, itConstName->pos);
    objectMeta->putConstMeta(std::move(constMeta));
    m_ast.insertObject(meta, itConstName->str, std::move(objectMeta));
    return true;
}

// <enum> <NAME> (<:> <TYPE>)? <{> (<FIELD> (<=> <NUMBER>)?)+ <}>
bool INBCompiler::parseEnum(AST::ParsingMeta& meta, tokens_it& it) {
    if (next(meta, it, true, true) != next_r('y', 'y')) { // <enum> <?>
        return false;
    }
    if (findKeyword(it->str) != kw::UNDEFINED) { // <enum> <NAME>
        printErrorWrongKeywordUsage(meta, it);
        return false;
    }
    if (!isNameCorrect(it->str)) {
        printErrorIncorrectName(meta, it);
        skipLine(meta, it);
        return false;
    }
    const AST::ObjectMeta* checkObject = m_ast.findObject(meta.namespace_, it->str);
    if (checkObject != nullptr) { // <enum> <NAME>
        printErrorNameAlreadyInUse(meta, it, checkObject);
        skipLine(meta, it);
        return false;
    }
    std::unique_ptr<AST::EnumMeta> enumMeta = std::make_unique<AST::EnumMeta>();
    enumMeta->name = it->str; // <enum> <NAME>
    const tokens_it itAtEnumName = it;
    if (next(meta, it, true) != next_r('y')) { // <enum> <NAME> <?>
        return false;
    }
    uint64_t valuesLimit = 0;
    if (it->str == ":") { // <enum> <NAME> <:>
        if (next(meta, it, true, true) != next_r('y', 'y')) { // <enum> <NAME> <:> <?>
            return false;
        }
        enumMeta->type = findKeyword(it->str);
        switch (enumMeta->type) {
        case kw::UInt8:
            valuesLimit = UINT8_MAX;
            break;
        case kw::UInt16:
            valuesLimit = UINT16_MAX;
            break;
        case kw::UInt32:
            valuesLimit = UINT32_MAX;
            break;
        case kw::UInt64:
            valuesLimit = UINT64_MAX;
            break;
        case kw::Include:
        case kw::Namespace:
        case kw::Enum:
        case kw::Struct:
        case kw::Const:
        case kw::Union:
        case kw::CRC32:
        case kw::MurMur3:
        case kw::Optional:
        case kw::NoHeader:
        case kw::Count:
        case kw::Min:
        case kw::Max:
            printErrorWrongKeywordUsage(meta, it);
            return false;
        case kw::Int8:
        case kw::Int16:
        case kw::Int32:
        case kw::Int64:
        case kw::Float32:
        case kw::Float64:
        case kw::Bytes:
        case kw::UNDEFINED:
        default:
            printErrorWrongToken(meta, it, "uint8 | uint16 | uint32 | uint64");
            return false;
        }
        if (next(meta, it, true) != next_r('y')) { // <enum> <NAME> <:> <TYPE> <?>
            return false;
        }
    }
    else {
        enumMeta->type = kw::UInt32;
        valuesLimit = UINT32_MAX;
    }
    if (it->str != "{") { // <enum> <NAME> (<:> <TYPE>)? <{>
        printErrorWrongToken(meta, it, "{");
        return false;
    }
    if (next(meta, it, true) != next_r('y')) { // ... <{> <?>
        return false;
    }
    bool result = true;
    uint64_t value = 0;
    std::set<std::string> names;
    while (true) {
        if (it == meta.tokens.cend()) {
            printErrorUnexpectedEndl(meta, it);
            return false;
        }
        else if (it->str == "}") {
            next(meta, it, false);
            break;
        }
        else if (findKeyword(it->str) != kw::UNDEFINED) {
            printErrorWrongKeywordUsage(meta, it);
            skipLine(meta, it);
            result = false;
            continue;
        }
        else if (!isNameCorrect(it->str)) {
            printErrorIncorrectName(meta, it);
            skipLine(meta, it);
            result = false;
            continue;
        }
        else if (names.count(it->str) > 0) {
            printErrorNameAlreadyInUse(meta, it);
            skipLine(meta, it);
            result = false;
            continue;
        }

        const tokens_it itAtName = it;
        enumMeta->valuesVec.emplace_back(it->str, value);
        enumMeta->valuesMap[it->str] = static_cast<uint32_t>(enumMeta->valuesVec.size() - 1);
        names.insert(it->str);

        const auto res = next(meta, it, true);
        if (res.token == 'n') {
            return false;
        }
        if (res.at_line == 'y') {
            if (it->str != "=") {
                printErrorWrongToken(meta, it, "=");
                skipLine(meta, it);
                result = false;
                continue;
            }
            if (next(meta, it, true, true) != next_r('y', 'y')) {
                result = false;
                continue;
            }
            std::string valueStr;
            uint8_t numeralSystem = 0;
            const tokens_it itAtValue = it;
            if (!parseValueUInt(meta, it, valueStr, numeralSystem)) {
                skipLine(meta, it);
                result = false;
                continue;
            }
            //if (numeralSystem != 'D') {
            //    printErrorCustom(meta, itAtValue, "Must be in decimal format.");
            //    result = false;
            //    continue;
            //}
            try {
                const uint64_t valuePrev = value;
                value = std::stoull(valueStr, nullptr, numeralSystem);
                enumMeta->valuesVec.back().second = value;
                if (value < valuePrev) {
                    printErrorCustom(meta, itAtValue,
                        "Enum value " + std::to_string(value)
                        + " must be >= than " + std::to_string(valuePrev) + ".");
                    ++value;
                    result = false;
                    continue;
                }
                if (value > valuesLimit) {
                    printErrorCustom(meta, itAtValue,
                        "Enum value " + std::to_string(value)
                        + " must be < than " + std::to_string(valuesLimit) + ".");
                    ++value;
                    result = false;
                    continue;
                }
            }
            catch (const std::exception& exc) {
                printErrorCustom(meta, itAtValue, exc.what());
                result = false;
                continue;
            }
        }
        else if (value > valuesLimit) {
            printErrorCustom(meta, itAtName, "Enum value " + std::to_string(value)
                + " must be < than " + std::to_string(valuesLimit) + ".");
            result = false;
            continue;
        }
        ++value;
    }

    if (result && m_detailed) {
        std::cout << "ENUM: ";
        for (uint32_t i = 0; i < meta.namespace_.size(); ++i) {
            std::cout << meta.namespace_[i] << "::";
        }
        std::cout << clr::blue << enumMeta->name << clr::reset << "{ ";
        for (const auto& value : enumMeta->valuesVec) {
            std::cout << clr::cyan << value.first
                << clr::reset << '=' << value.second << ' ';
        }
        std::cout << '}' << std::endl;
    }

    std::unique_ptr<AST::ObjectMeta> objectMeta = std::make_unique<AST::ObjectMeta>(
        meta.path, itAtEnumName->line, itAtEnumName->pos);
    objectMeta->putEnumMeta(std::move(enumMeta));
    m_ast.insertObject(meta, itAtEnumName->str, std::move(objectMeta));
    return result;
}


// <struct> <NAME> (<:> <ATTRIBUTE>)? <{> (
//   <optional>? (<NAMESPACE><:><:>)* <TYPE> <NAME> (
//     ( <[> (<NAMESPACE><:><:>)* <CONSTANT>
//       []
//       [namespace::object::value]
//       [namespace::object.special]
//       = namespace::object::value
//       = namespace::object.special
//     ......
bool INBCompiler::parseStruct(AST::ParsingMeta& meta, tokens_it& it) {
    if (next(meta, it, true, true) != next_r('y', 'y')) { // <struct> <?>
        return false;
    }
    if (!isNameCorrect(it->str)) { // <struct> <?>
        printErrorIncorrectName(meta, it);
        skipLine(meta, it);
        return false;
    }
    if (findKeyword(it->str) != kw::UNDEFINED) { // <struct> <?>
        printErrorWrongKeywordUsage(meta, it);
        skipLine(meta, it);
        return false;
    }
    const AST::ObjectMeta* checkObject = m_ast.findObject(meta.namespace_, it->str);
    if (checkObject != nullptr) { // <struct> <?>
        printErrorNameAlreadyInUse(meta, it, checkObject);
        skipLine(meta, it);
        return false;
    }
    const tokens_it itAtStructName = it; // <struct> <NAME>
    std::unique_ptr<AST::StructMeta> structMeta = std::make_unique<AST::StructMeta>();
    structMeta->name = it->str;
    if (next(meta, it, true) != next_r('y')) { // <struct> <NAME> <?>
        return false;
    }
    if (it->str == ":") { // <struct> <NAME> <:>
        if (next(meta, it, true) != next_r('y')) {
            return false;
        }
        for (uint8_t i = 0; i < 10; ++i) {
            // <struct> <NAME> <:> <?>
            if (it->str == "{") {
                break;
            }
            const kw attribute = findKeyword(it->str);
            switch (attribute) {
            case kw::MurMur3:
            case kw::CRC32:
            case kw::NoHeader:
                structMeta->attribute = attribute;
                break;
            case kw::UInt8:
            case kw::UInt16:
            case kw::UInt32:
            case kw::UInt64:
                structMeta->offsetType = attribute;
                break;
            default:
                printErrorWrongToken(meta, it, "uint8\", \"uint16\", \"uint32\", "
                    "\"uint64\", \"crc32\", \"mmh3\" or \"no_header");
                skipLine(meta, it);
                return false;
            }
            if (next(meta, it, true) != next_r('y')) {
                return false;
            }
        }
    }
    if (it->str != "{") { // ... <{>
        printErrorWrongToken(meta, it);
        skipLine(meta, it);
        return false;
    }
    if (next(meta, it, true) != next_r('y')) { // ... <{> <?>
        return false;
    }
    if (m_detailed) {
        std::cout << "STRUCT: ";
        for (const auto& ns : meta.namespace_) {
            std::cout << ns << "::";
        }
        std::cout << clr::blue << structMeta->name << clr::reset;
        if (structMeta->attribute != kw::UNDEFINED) {
            std::cout << " " << kw_to_keyword[static_cast<uint8_t>(structMeta->attribute)];
        }
        std::cout << std::endl;
    }
    uint32_t errorsCount = 0;
    std::unique_ptr<AST::ObjectMeta> objectMeta = std::make_unique<AST::ObjectMeta>(
        meta.path, itAtStructName->line, itAtStructName->pos);
    while (true) {
        if (it == meta.tokens.end()) {
            return false;
        }
        if (it->str == "}") {
            next(meta, it, false);
            break;
        }
        ++errorsCount;
        std::unique_ptr<AST::FieldMeta> fieldPtr = std::make_unique<AST::FieldMeta>();
        AST::FieldMeta* field = fieldPtr.get();
        kw keyword = kw::UNDEFINED;
        keyword = findKeyword(it->str);
        if (keyword == kw::Optional) {
            field->isOptional = true;
            //field->isByOffset = true;
            auto itTemp = it;
            if (next(meta, it, true, true) != next_r('y', 'y')) {
                it = itTemp;
                skipLine(meta, it);
                continue;
            }
            keyword = findKeyword(it->str);
        }
        if (isBuiltInType(keyword)) {
            field->isBuiltIn = true;
            field->isScalar = true;
            field->typeKw = keyword;
            auto itTemp = it;
            if (next(meta, it, true, true) != next_r('y', 'y')) {
                it = itTemp;
                skipLine(meta, it);
                continue;
            }
            if (field->typeKw == kw::Bytes) {
                field->isArray = true;
                field->isOptional = true;
                field->isScalar = false;
            }
        }
        else {
            //field->isByOffset = true;
            auto itTypeBegin = it;
            field->typePtr = findObject(meta, it);
            auto itTypeEnd = it;
            if (itTypeEnd != meta.tokens.end() && itTypeEnd->line != itTypeBegin->line) {
                continue;
            }
            if (field->typePtr == nullptr) {
                printErrorCustom(meta, itTypeBegin, "Unknown type.", static_cast<uint32_t>(
                    itTypeEnd->pos + itTypeEnd->str.size() - itTypeBegin->pos));
                it = itTypeBegin;
                skipLine(meta, it);
                continue;
            }
            field->typeKw = field->typePtr->type();
            if (field->typeKw == kw::Enum) {
                field->isScalar = true;
            }
        }
        field->name = it->str;
        if (!isNameCorrect(field->name)) {
            printErrorIncorrectName(meta, it);
            skipLine(meta, it);
            continue;
        }
        if (structMeta->fieldsMap.count(field->name) > 0) {
            printErrorNameAlreadyInUse(meta, it);
            skipLine(meta, it);
            continue;
        }

        structMeta->fieldsVec.push_back(field);
        structMeta->fieldsMap[field->name] = std::move(fieldPtr);

        if (next(meta, it, true) == next_r('y', 'y')) {
            if (it->str == "=") {
                switch (field->typeKw) {
                case kw::Struct:
                    printErrorCustom(meta, it, "A default value for a struct object.");
                    skipLine(meta, it);
                    continue;
                case kw::Union:
                    printErrorCustom(meta, it, "A default value for an union object.");
                    skipLine(meta, it);
                    continue;
                default:
                    break;
                }
                if (field->isOptional) {
                    printErrorCustom(meta, it, "A default value with the 'optional' attribute.");
                    skipLine(meta, it);
                    continue;
                }
                if (next(meta, it, true, true) != next_r('y', 'y')) { // <=> <?>
                    continue;
                }
                const tokens_it itAtValue = it;
                if (field->isBuiltIn) {
                    const AST::ObjectMeta* resultObjectMeta = nullptr;
                    kw resultKw = kw::UNDEFINED;
                    uint32_t resultIdx = 0;
                    if (parseValue(meta, it, field->typeKw, field->valueStr, true)) {
                    }
                    else if (findValue(meta, it, resultObjectMeta, resultKw, resultIdx)) {
                        switch (resultKw) {
                        case kw::Const:
                            switch (checkScalarTypeMatchingRange(
                                resultObjectMeta->constMeta()->type, field->typeKw)) {
                            case 'y':
                                field->valueKw = resultKw;
                                field->valuePtr = resultObjectMeta;
                                field->valueIdx = resultIdx;
                                break;
                            case 'n':
                                printErrorCustom(meta, itAtValue,
                                    "The value is out of range allowed by the type.");
                                it = itAtValue;
                                skipLine(meta, it);
                                continue;
                            case 'w':
                                printWarningCustom(meta, itAtValue,
                                    "Type mismatch.");
                                field->valueKw = resultKw;
                                field->valuePtr = resultObjectMeta;
                                field->valueIdx = resultIdx;
                                break;
                            case 'e':
                            default:
                                printErrorCustom(meta, itAtValue,
                                    "Type mismatch.");
                                it = itAtValue;
                                skipLine(meta, it);
                                continue;
                            }
                            break;
                        case kw::Enum:
                            switch (checkScalarTypeMatchingRange(
                                resultObjectMeta->enumMeta()->type, field->typeKw)) {
                            case 'y':
                                field->valueKw = resultKw;
                                field->valuePtr = resultObjectMeta;
                                field->valueIdx = resultIdx;
                                break;
                            case 'n':
                                printErrorCustom(meta, itAtValue,
                                    "The value is out of range allowed by the type.");
                                it = itAtValue;
                                skipLine(meta, it);
                                continue;
                            case 'w':
                                printWarningCustom(meta, itAtValue,
                                    "Type mismatch.");
                                field->valueKw = resultKw;
                                field->valuePtr = resultObjectMeta;
                                field->valueIdx = resultIdx;
                                break;
                            case 'e':
                            default:
                                printErrorCustom(meta, itAtValue,
                                    "Type mismatch.");
                                it = itAtValue;
                                skipLine(meta, it);
                                continue;
                            }
                            break;
                        case kw::Min:
                        case kw::Max:
                        case kw::Count:
                            field->valueKw = resultKw;
                            field->valuePtr = resultObjectMeta;
                            field->valueIdx = resultIdx;
                            break;
                        default:
                            printErrorCustom(meta, itAtValue, "Can't parse value.");
                            it = itAtValue;
                            skipLine(meta, it);
                            continue;
                        }
                    }
                    else {
                        printErrorCustom(meta, itAtValue, "Can't parse value.");
                        it = itAtValue;
                        skipLine(meta, it);
                        continue;
                    }
                }
                else {
                    const AST::ObjectMeta* resultObjectMeta = nullptr;
                    kw resultKw = kw::UNDEFINED;
                    uint32_t resultIdx = 0;
                    if (!findValue(meta, it, resultObjectMeta, resultKw, resultIdx)) {
                        it = itAtValue;
                        skipLine(meta, it);
                        continue;
                    }
                    switch (resultKw) {
                    case kw::Enum: {
                        if (field->typePtr != resultObjectMeta) {
                            printErrorCustom(meta, itAtValue, "Enum type mismatch.");
                                //static_cast<uint32_t>(it->pos + it->str.size() - itAtValue->pos));
                            it = itAtValue;
                            skipLine(meta, it);
                            continue;
                        }
                        field->valueKw = kw::Enum;
                        field->valuePtr = field->typePtr;
                        field->valueIdx = resultIdx;
                        break;
                    }
                    default:
                        printErrorCustom(meta, itAtValue, "Expected an enum value.");
                            //static_cast<uint32_t>(it->pos + it->str.size() - itAtValue->pos));
                        it = itAtValue;
                        skipLine(meta, it);
                        continue;
                    }
                }
            }
            else if (it->str == "[") {
                field->isArray = true;
                //field->isByOffset = true;
                //if (field->isOptional) {
                //    printWarningCustom(meta, it, "Arrays are optional by default.");
                //}
                if (next(meta, it, true, true) != next_r('y', 'y')) { // <[> <?>
                    continue;
                }
                if (it->str == "]") {
                    if (next(meta, it, false) == next_r('y', 'y')) {
                        printErrorWrongToken(meta, it, "end of line");
                        skipLine(meta, it);
                        continue;
                    }
                    field->isOptional = true;
                }
                else {
                    const tokens_it itAtValue = it;
                    const AST::ObjectMeta* arrayPtr = nullptr;
                    kw arrayKw = kw::UNDEFINED;
                    uint32_t arrayIdx = 0;
                    std::string arrayStr;
                    if (parseValue(meta, it, kw::UInt64, arrayStr, true)) {
                        field->arraySize = std::stoull(arrayStr);
                        field->arrayKw = kw::UInt64;
                    }
                    else {
                        it = itAtValue;
                        arrayPtr = findObject(meta, it);
                        if (arrayPtr) {
                            switch (arrayPtr->type()) {
                            case kw::Const:
                                if (arrayPtr->constMeta()->type != kw::UInt8
                                    && arrayPtr->constMeta()->type != kw::UInt16
                                    && arrayPtr->constMeta()->type != kw::UInt32
                                    && arrayPtr->constMeta()->type != kw::UInt64) {
                                    printErrorCustom(meta, itAtValue, "Must be an unsigned integer.");
                                    it = itAtValue;
                                    skipLine(meta, it);
                                    continue;
                                }
                                field->arrayKw = kw::Const;
                                field->arrayPtr = arrayPtr;
                                break;
                            default:
                                printErrorCustom(meta, itAtValue, "Must be an unsigned integer.");
                                it = itAtValue;
                                skipLine(meta, it);
                                continue;
                            }
                        }
                        else {
                            it = itAtValue;
                            if (findValue(meta, it, arrayPtr, arrayKw, arrayIdx)) {
                                switch (arrayKw) {
                                case kw::Enum:
                                case kw::Min:
                                case kw::Max:
                                case kw::Count:
                                    field->arrayKw = arrayKw;
                                    field->arrayPtr = arrayPtr;
                                    field->arrayIdx = arrayIdx;
                                    break;
                                default:
                                    printErrorCustom(meta, itAtValue, "Must be an unsigned integer.");
                                    it = itAtValue;
                                    skipLine(meta, it);
                                    continue;
                                }
                            }
                            else {
                                printErrorCustom(meta, itAtValue, "Must be an unsigned integer.");
                                it = itAtValue;
                                skipLine(meta, it);
                                continue;
                            }
                        }
                    }
                    //field->isArrayFixedSize = true;
                    //field->isByOffset = false;
                    if (it->str != "]") {
                        printErrorWrongToken(meta, it, "]");
                        skipLine(meta, it);
                        continue;
                    }
                    if (field->typeKw == kw::Bytes) {
                        field->isOptional = false;
                    }
                    if (next(meta, it, false) == next_r('y', 'y')) {
                        printErrorWrongToken(meta, it, "end of line");
                        skipLine(meta, it);
                        continue;
                    }
                }
            }
            else {
                printErrorWrongToken(meta, it, "'=' or '['");
                skipLine(meta, it);
                continue;
            }
        }

        if (m_detailed) {
            std::cout << "  STRUCT FIELD: ";
            if (field->isOptional) {
                std::cout << "optional ";
            }
            if (field->isBuiltIn) {
                std::cout << kw_to_keyword[static_cast<uint8_t>(field->typeKw)];
            }
            else if (field->typePtr != nullptr) {
                std::cout << "(";
                switch (field->typePtr->type()) {
                case kw::Enum:
                case kw::Struct:
                case kw::Union:
                    std::cout << kw_to_keyword[static_cast<uint8_t>(field->typePtr->type())];
                    break;
                default:
                    std::cout << "warning";
                    break;
                }
                std::cout << ") ";
                for (const auto& ns : field->typePtr->namespace_) {
                    std::cout << ns << "::";
                }
                switch (field->typePtr->type()) {
                case kw::Enum:
                    std::cout << field->typePtr->enumMeta()->name;
                    break;
                case kw::Struct:
                    std::cout << field->typePtr->structMeta()->name;
                    break;
                case kw::Union:
                    std::cout << field->typePtr->unionMeta()->name;
                    break;
                default:
                    std::cout << "warning";
                    break;
                }
            }
            else {
                std::cout << "warning";
            }
            std::cout << " " << clr::blue << field->name << clr::reset;
            if (field->isArray) {
                std::cout << "[";
                switch (field->arrayKw) {
                case kw::UInt8:
                case kw::UInt16:
                case kw::UInt32:
                case kw::UInt64:
                    std::cout << clr::cyan << field->arraySize << clr::reset;
                    break;
                case kw::Const:
                    std::cout << clr::cyan
                        << field->arrayPtr->constMeta()->name << clr::reset;
                    break;
                case kw::Enum:
                    std::cout << clr::cyan
                        << field->arrayPtr->enumMeta()->valuesVec[field->valueIdx].first
                        << "(" << field->arrayPtr->enumMeta()->valuesVec[field->valueIdx].second
                        << ")" << clr::reset;
                    break;
                case kw::Min:
                    std::cout << clr::cyan
                        << field->arrayPtr->enumMeta()->name << ".min("
                        << field->arrayPtr->enumMeta()->valuesVec.front().second
                        << ")" << clr::reset;
                    break;
                case kw::Max:
                    std::cout << clr::cyan
                        << field->arrayPtr->enumMeta()->name << ".max("
                        << field->arrayPtr->enumMeta()->valuesVec.back().second
                        << ")" << clr::reset;
                    break;
                case kw::Count:
                    std::cout << clr::cyan
                        << field->arrayPtr->enumMeta()->name << ".count("
                        << field->arrayPtr->enumMeta()->valuesVec.size()
                        << ")" << clr::reset;
                    break;
                default:
                    break;
                }
                std::cout << "]";
            }
            if (!field->valueStr.empty()) {
                std::cout << " = " << clr::cyan << field->valueStr << clr::reset;
            }
            else switch (field->valueKw) {
            case kw::Const:
                std::cout << " = " << clr::cyan
                    << field->valuePtr->constMeta()->name << clr::reset;
                break;
            case kw::Enum:
                std::cout << " = " << clr::cyan
                    << field->valuePtr->enumMeta()->valuesVec[field->arrayIdx].first
                    << "(" << field->valuePtr->enumMeta()->valuesVec[field->arrayIdx].second
                    << ")" << clr::reset;
                break;
            case kw::Min:
                std::cout << " = " << clr::cyan
                    << field->valuePtr->enumMeta()->name << ".min("
                    << field->valuePtr->enumMeta()->valuesVec.front().second
                    << ")" << clr::reset;
                break;
            case kw::Max:
                std::cout << " = " << clr::cyan
                    << field->valuePtr->enumMeta()->name << ".max("
                    << field->valuePtr->enumMeta()->valuesVec.back().second
                    << ")" << clr::reset;
                break;
            case kw::Count:
                std::cout << " = " << clr::cyan
                    << field->valuePtr->enumMeta()->name << ".count("
                    << field->valuePtr->enumMeta()->valuesVec.size()
                    << ")" << clr::reset;
                break;
            default:
                break;
            }
            std::cout << std::endl;
        }
        --errorsCount;
    }
    if (errorsCount > 0) {
        m_parsingErrorsCount += errorsCount;
        return false;
    }
    //std::sort(structMeta->fieldsVec.begin(), structMeta->fieldsVec.end(),
    //    [](const AST::FieldMeta* l, const AST::FieldMeta* r) {
    //        return l->isOptional < r->isOptional;
    //    });
    objectMeta->putStructMeta(std::move(structMeta));
    m_ast.insertObject(meta, itAtStructName->str, std::move(objectMeta));
    return true;
}

// <union> <NAME> <{> (
//   (<NAMESPACE><:><:>)* <TYPE> <NAME> (<[> (<NAMESPACE><:><:>)* <CONSTANT> <]>)?
//   ...
// )+
bool INBCompiler::parseUnion(AST::ParsingMeta& meta, tokens_it& it) {
    if (next(meta, it, true, true) != next_r('y', 'y')) { // <union> <?>
        return false;
    }
    if (!isNameCorrect(it->str)) { // <union> <?>
        printErrorIncorrectName(meta, it);
        skipLine(meta, it);
        return false;
    }
    if (findKeyword(it->str) != kw::UNDEFINED) { // <union> <?>
        printErrorWrongKeywordUsage(meta, it);
        skipLine(meta, it);
        return false;
    }
    const AST::ObjectMeta* checkObject = m_ast.findObject(meta.namespace_, it->str);
    if (checkObject != nullptr) { // <union> <?>
        printErrorNameAlreadyInUse(meta, it, checkObject);
        skipLine(meta, it);
        return false;
    }
    const tokens_it itAtUnionName = it; // <union> <NAME>
    std::unique_ptr<AST::UnionMeta> unionMeta = std::make_unique<AST::UnionMeta>();
    unionMeta->name = it->str;
    if (next(meta, it, true) != next_r('y')) { // <union> <NAME> <?>
        return false;
    }
    if (it->str == ":") { // <union> <NAME> <:>
        if (next(meta, it, true) != next_r('y')) {
            return false;
        }
        for (uint8_t i = 0; i < 10; ++i) {
            // <union> <NAME> <:> <?>
            if (it->str == "{") {
                break;
            }
            const kw attribute = findKeyword(it->str);
            switch (attribute) {
            case kw::MurMur3:
            case kw::CRC32:
            case kw::NoHeader:
                unionMeta->attribute = attribute;
                break;
            case kw::UInt8:
            case kw::UInt16:
            case kw::UInt32:
            case kw::UInt64:
                unionMeta->offsetType = attribute;
                break;
            default:
                printErrorWrongToken(meta, it, "uint8\", \"uint16\", \"uint32\", "
                    "\"uint64\", \"crc32\", \"mmh3\" or \"no_header");
                skipLine(meta, it);
                return false;
            }
            if (next(meta, it, true) != next_r('y')) {
                return false;
            }
        }
    }
    if (it->str != "{") { // ... <{>
        printErrorWrongToken(meta, it);
        skipLine(meta, it);
        return false;
    }
    if (next(meta, it, true) != next_r('y')) { // ... <{> <?>
        return false;
    }
    if (m_detailed) {
        std::cout << "UNION: ";
        for (const auto& ns : meta.namespace_) {
            std::cout << ns << "::";
        }
        std::cout << clr::blue << unionMeta->name << clr::reset << std::endl;
    }
    uint32_t errorsCount = 0;
    while (true) {
        if (it == meta.tokens.end()) {
            return false;
        }
        if (it->str == "}") {
            next(meta, it, false);
            break;
        }
        ++errorsCount;
        std::unique_ptr<AST::FieldMeta> fieldPtr = std::make_unique<AST::FieldMeta>();
        AST::FieldMeta* field = fieldPtr.get();
        kw keyword = kw::UNDEFINED;
        keyword = findKeyword(it->str);
        if (isBuiltInType(keyword)) {
            field->isBuiltIn = true;
            field->isScalar = true;
            field->typeKw = keyword;
            auto itTemp = it;
            if (next(meta, it, true, true) != next_r('y', 'y')) {
                it = itTemp;
                skipLine(meta, it);
                continue;
            }
            if (field->typeKw == kw::Bytes) {
                field->isArray = true;
                field->isOptional = true;
                field->isScalar = false;
            }
        }
        else {
            auto itTypeBegin = it;
            field->typePtr = findObject(meta, it);
            auto itTypeEnd = it;
            if (itTypeEnd != meta.tokens.end() && itTypeEnd->line != itTypeBegin->line) {
                continue;
            }
            if (field->typePtr == nullptr) {
                printErrorCustom(meta, itTypeBegin, "Unknown type.", static_cast<uint32_t>(
                    itTypeEnd->pos + itTypeEnd->str.size() - itTypeBegin->pos));
                it = itTypeBegin;
                skipLine(meta, it);
                continue;
            }
            field->typeKw = field->typePtr->type();
            if (field->typeKw == kw::Enum) {
                field->isScalar = true;
            }
        }
        field->name = it->str;
        if (!isNameCorrect(field->name)) {
            printErrorIncorrectName(meta, it);
            skipLine(meta, it);
            continue;
        }
        if (unionMeta->fieldsMap.count(field->name) > 0) {
            printErrorNameAlreadyInUse(meta, it);
            skipLine(meta, it);
            continue;
        }

        unionMeta->fieldsVec.push_back(field);
        unionMeta->fieldsMap[field->name] = std::move(fieldPtr);

        if (next(meta, it, true) == next_r('y', 'y')) {
            if (it->str == "[") {
                field->isArray = true;
                if (next(meta, it, true, true) != next_r('y', 'y')) { // <[> <?>
                    continue;
                }
                if (it->str == "]") {
                    if (next(meta, it, false) == next_r('y', 'y')) {
                        printErrorWrongToken(meta, it, "end of line");
                        skipLine(meta, it);
                        continue;
                    }
                    field->isOptional = true;
                }
                else {
                    const tokens_it itAtValue = it;
                    const AST::ObjectMeta* arrayPtr = nullptr;
                    kw arrayKw = kw::UNDEFINED;
                    uint32_t arrayIdx = 0;
                    std::string arrayStr;
                    if (parseValue(meta, it, kw::UInt64, arrayStr, true)) {
                        field->arraySize = std::stoull(arrayStr);
                        field->arrayKw = kw::UInt64;
                    }
                    else {
                        it = itAtValue;
                        arrayPtr = findObject(meta, it);
                        if (arrayPtr) {
                            switch (arrayPtr->type()) {
                            case kw::Const:
                                if (arrayPtr->constMeta()->type != kw::UInt8
                                    && arrayPtr->constMeta()->type != kw::UInt16
                                    && arrayPtr->constMeta()->type != kw::UInt32
                                    && arrayPtr->constMeta()->type != kw::UInt64) {
                                    printErrorCustom(meta, itAtValue, "Must be an unsigned integer.");
                                    it = itAtValue;
                                    skipLine(meta, it);
                                    continue;
                                }
                                field->arrayKw = kw::Const;
                                field->arrayPtr = arrayPtr;
                                break;
                            default:
                                printErrorCustom(meta, itAtValue, "Must be an unsigned integer.");
                                it = itAtValue;
                                skipLine(meta, it);
                                continue;
                            }
                        }
                        else {
                            it = itAtValue;
                            if (findValue(meta, it, arrayPtr, arrayKw, arrayIdx)) {
                                switch (arrayKw) {
                                case kw::Enum:
                                case kw::Min:
                                case kw::Max:
                                case kw::Count:
                                    field->arrayKw = arrayKw;
                                    field->arrayPtr = arrayPtr;
                                    field->arrayIdx = arrayIdx;
                                    break;
                                default:
                                    printErrorCustom(meta, itAtValue, "Must be an unsigned integer.");
                                    it = itAtValue;
                                    skipLine(meta, it);
                                    continue;
                                }
                            }
                            else {
                                printErrorCustom(meta, itAtValue, "Must be an unsigned integer.");
                                it = itAtValue;
                                skipLine(meta, it);
                                continue;
                            }
                        }
                    }
                    //field->isArrayFixedSize = true;
                    //field->isByOffset = false;
                    if (it->str != "]") {
                        printErrorWrongToken(meta, it, "]");
                        skipLine(meta, it);
                        continue;
                    }
                    if (field->typeKw == kw::Bytes) {
                        field->isOptional = false;
                    }
                    if (next(meta, it, false) == next_r('y', 'y')) {
                        printErrorWrongToken(meta, it, "end of line");
                        skipLine(meta, it);
                        continue;
                    }
                }
            }
            else {
                printErrorWrongToken(meta, it, "'[' or 'end of line'");
                skipLine(meta, it);
                continue;
            }
        }

        if (m_detailed) {
            std::cout << "  UNION FIELD: ";
            if (field->isBuiltIn) {
                std::cout << kw_to_keyword[static_cast<uint8_t>(field->typeKw)];
            }
            else if (field->typePtr != nullptr) {
                std::cout << "(";
                switch (field->typePtr->type()) {
                case kw::Enum:
                case kw::Struct:
                case kw::Union:
                    std::cout << kw_to_keyword[static_cast<uint8_t>(field->typePtr->type())];
                    break;
                default:
                    std::cout << "warning";
                    break;
                }
                std::cout << ") ";
                for (const auto& ns : field->typePtr->namespace_) {
                    std::cout << ns << "::";
                }
                switch (field->typePtr->type()) {
                case kw::Enum:
                    std::cout << field->typePtr->enumMeta()->name;
                    break;
                case kw::Struct:
                    std::cout << field->typePtr->structMeta()->name;
                    break;
                case kw::Union:
                    std::cout << field->typePtr->unionMeta()->name;
                    break;
                default:
                    std::cout << "warning";
                    break;
                }
            }
            else {
                std::cout << "warning";
            }
            std::cout << " " << clr::blue << field->name << clr::reset;
            if (field->isArray) {
                std::cout << "[";
                switch (field->arrayKw) {
                case kw::UInt8:
                case kw::UInt16:
                case kw::UInt32:
                case kw::UInt64:
                    std::cout << clr::cyan << field->arraySize << clr::reset;
                    break;
                case kw::Const:
                    std::cout << clr::cyan
                        << field->arrayPtr->constMeta()->name << clr::reset;
                    break;
                case kw::Enum:
                    std::cout << clr::cyan
                        << field->arrayPtr->enumMeta()->valuesVec[field->arrayIdx].first
                        << "(" << field->arrayPtr->enumMeta()->valuesVec[field->arrayIdx].second
                        << ")" << clr::reset;
                    break;
                case kw::Min:
                    std::cout << clr::cyan
                        << field->arrayPtr->enumMeta()->name << ".min("
                        << field->arrayPtr->enumMeta()->valuesVec.front().second
                        << ")" << clr::reset;
                    break;
                case kw::Max:
                    std::cout << clr::cyan
                        << field->arrayPtr->enumMeta()->name << ".max("
                        << field->arrayPtr->enumMeta()->valuesVec.back().second
                        << ")" << clr::reset;
                    break;
                case kw::Count:
                    std::cout << clr::cyan
                        << field->arrayPtr->enumMeta()->name << ".count("
                        << field->arrayPtr->enumMeta()->valuesVec.size()
                        << ")" << clr::reset;
                    break;
                default:
                    break;
                }
                std::cout << "]";
            }
            std::cout << std::endl;
        }
        --errorsCount;
    }
    if (errorsCount > 0) {
        m_parsingErrorsCount += errorsCount;
        return false;
    }
    std::unique_ptr<AST::ObjectMeta> objectMeta = std::make_unique<AST::ObjectMeta>(
        meta.path, itAtUnionName->line, itAtUnionName->pos);
    objectMeta->putUnionMeta(std::move(unionMeta));
    m_ast.insertObject(meta, itAtUnionName->str, std::move(objectMeta));
    return true;
}

INBCompiler::next_r INBCompiler::next(const AST::ParsingMeta& meta, tokens_it& it,
    const bool isATokenRequired,
    const bool isATokenRequiredAtLine) const {

    if (it == meta.tokens.cend()) {
        if (isATokenRequired == true) {
            printErrorUnexpectedEndl(meta, it);
        }
        return next_r('n');
    }
    bool errorNoTokenAtLine = false;
    bool moreThenOneLine = false;
    while (true) {
        ++it;
        if (it == meta.tokens.cend()) {
            if (isATokenRequired == true && errorNoTokenAtLine == false) {
                printErrorUnexpectedEndl(meta, it);
            }
            return next_r('n');
        }
        else if (it->str == strEndl) {
            if (isATokenRequired == true && isATokenRequiredAtLine == true) {
                if (errorNoTokenAtLine == false) {
                    printErrorUnexpectedEndl(meta, it);
                    errorNoTokenAtLine = true;
                    //result = next_r::NoTokenAtLine;
                }
            }
            //else {
            //    //result = next_r::Token;
            //}
            moreThenOneLine = true;
        }
        else if (it->str.empty()) {
        }
        else {
            if (errorNoTokenAtLine == true) {
                return next_r('y', 'n');
            }
            else if (moreThenOneLine == true) {
                return next_r('y', 'n');
            }
            else {
                return next_r('y', 'y');
            }
        }
    }
    return next_r('n');
}
void INBCompiler::test_next() const {
    AST::ParsingMeta meta;
    meta.path = "void test_next()";
    tokens_it it;
    {
        const lines_t lines;
        tokenize(lines, meta.tokens);
        it = meta.tokens.begin();
        assert(next(meta, it, false) == next_r('n')); // silent
        it = meta.tokens.begin();
        assert(next(meta, it, true) == next_r('n')); // UnexpectedEndl
        it = meta.tokens.begin();
        assert(next(meta, it, true, true) == next_r('n')); // UnexpectedEndl
    }
    {
        const lines_t lines = {
            strEndl
        };
        tokenize(lines, meta.tokens);
        it = meta.tokens.begin();
        assert(next(meta, it, false) == next_r('n')); // silent
        it = meta.tokens.begin();
        assert(next(meta, it, true) == next_r('n')); // UnexpectedEndl
        it = meta.tokens.begin();
        assert(next(meta, it, true, true) == next_r('n')); // UnexpectedEndl
    }
    {
        const lines_t lines = {
            "first"
        };
        tokenize(lines, meta.tokens);
        it = meta.tokens.begin();
        assert(next(meta, it, false) == next_r('n')); // silent
        it = meta.tokens.begin();
        assert(next(meta, it, true) == next_r('n')); // UnexpectedEndl
        it = meta.tokens.begin();
        assert(next(meta, it, true, true) == next_r('n')); // UnexpectedEndl
    }
    {
        const lines_t lines = {
            "first second"
        };
        tokenize(lines, meta.tokens);
        it = meta.tokens.begin();
        assert(next(meta, it, false) == next_r('y', 'y')); // silent
        it = meta.tokens.begin();
        assert(next(meta, it, true) == next_r('y', 'y')); // silent
        it = meta.tokens.begin();
        assert(next(meta, it, true, true) == next_r('y', 'y')); // silent
    }
    {
        const lines_t lines = {
            "first",
            "second"
        };
        tokenize(lines, meta.tokens);
        it = meta.tokens.begin();
        assert(next(meta, it, false) == next_r('y', 'n')); // silent
        it = meta.tokens.begin();
        assert(next(meta, it, true) == next_r('y', 'n')); // silent
        it = meta.tokens.begin();
        assert(next(meta, it, true, true) == next_r('y', 'n')); // UnexpectedEndl
    }
    {
        const lines_t lines = {
            "first",
            "",
            "//",
            "second"
        };
        tokenize(lines, meta.tokens);
        it = meta.tokens.begin();
        assert(next(meta, it, false) == next_r('y', 'n')); // silent
        it = meta.tokens.begin();
        assert(next(meta, it, true) == next_r('y', 'n')); // silent
        it = meta.tokens.begin();
        assert(next(meta, it, true, true) == next_r('y', 'n')); // UnexpectedEndl
    }
    exit(0);
}

void INBCompiler::skipLine(const AST::ParsingMeta& meta, tokens_it& it) const {
    while (true) {
        if (it == meta.tokens.cend()) {
            return;
        }
        if (it->str == strEndl) {
            next(meta, it, false);
            break;
        }
        else {
            ++it;
        }
    }
}
