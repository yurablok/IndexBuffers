#pragma once
#include "stdafx.h"
#include "colored_cout.h"

#include "ast_meta.h"

class INBCompiler {
public:
    INBCompiler();

    static std::string version();

    void read(const std::string& input, const bool detailed = false);
    void write(const std::string& outputSuffix, const Language& lang);

    void setStopOnFirstError(const bool stopOnFirstError);

private:
    AST m_ast;

    std::string getRelativeFilePath(const std::string& filePath);
    bool loadFileToLines(const std::string& filename, lines_t& lines);
    void tokenize(const lines_t& lines, tokens_t& tokens) const;
    bool parse(AST::ParsingMeta& meta);
    // signed dec
    bool parseValueInt(AST::ParsingMeta& meta, tokens_it& it,
        std::string& valueStr,
        const bool quiet = false) const;
    // unsigned dec, bin, hex
    // numeralSystem: 2, 10, 16
    bool parseValueUInt(AST::ParsingMeta& meta, tokens_it& it,
        std::string& valueStr, uint8_t& numeralSystem,
        const bool quiet = false) const;
    // float, scientific notation float
    bool parseValueFloat(AST::ParsingMeta& meta, tokens_it& it,
        std::string& valueStr,
        const bool quiet = false) const;
    bool parseValue(AST::ParsingMeta& meta, tokens_it& it,
        const kw& expectedValueType, std::string& valueStr,
        const bool quiet = false) const;
    bool parseInclude(AST::ParsingMeta& meta, tokens_it& it);
    bool parseNamespace(AST::ParsingMeta& meta, tokens_it& it);
    bool parseConst(AST::ParsingMeta& meta, tokens_it& it);
    bool parseEnum(AST::ParsingMeta& meta, tokens_it& it);
    bool parseStruct(AST::ParsingMeta& meta, tokens_it& it);
    bool parseUnion(AST::ParsingMeta& meta, tokens_it& it);

    // "_1abc"
    bool isNameCorrect(const std::string& name) const;
    // y - yes
    // n - no, 
    // w - warning, type mismatch
    // e - error, type mismatch
    // .. - error
    char checkScalarTypeMatchingRange(const kw typeKwSrc, const kw typeKwDest) const;

    struct next_r {
        next_r() = delete;
        next_r(next_r&& other) = delete;
        // token: 'y'es, 'n'o, 'a'ny
        // at_line: 'y'es, 'n'o, 'a'ny
        explicit next_r(char token_, char at_line_ = 'a')
            : token(token_), at_line(at_line_) {}
        bool operator==(const next_r& other) const {
            if (other.token != 'a') {
                if (other.token != token) {
                    return false;
                }
            }
            if (other.at_line != 'a') {
                if (other.at_line != at_line) {
                    return false;
                }
            }
            return true;
        }
        bool operator!=(const next_r& other) const {
            return !this->operator==(other);
        }
        char token = 'a';
        char at_line = 'a';
    };
    next_r next(const AST::ParsingMeta& meta, tokens_it& it,
        const bool isATokenRequired,
        const bool isATokenRequiredAtLine = false) const;
    void test_next() const;
    void skipLine(const AST::ParsingMeta& meta, tokens_it& it) const;

    static void genCPP(const AST& ast, const std::string& out);

    tokens_t m_tokens;

    std::unordered_set<std::string> m_processedFilesPaths;

    AST::ObjectMeta* findObject(const AST::ParsingMeta& meta, tokens_it& it) const;
    // resultKw: UNDEFINED  resultObjectMeta: nullptr
    // resultKw: Const
    // resultKw: Enum       resultIdx: EnumMeta::valuesVec[idx]
    // resultKw: Min
    // resultKw: Max
    // resultKw: Count
    bool findValue(const AST::ParsingMeta& meta, tokens_it& it,
        const AST::ObjectMeta*& resultObjectMeta, kw& resultKw, uint32_t& resultIdx) const;


    void printErrorImpl_ErrorAt(const AST::ParsingMeta& meta, const tokens_it& it,
        const uint32_t tokenSize = UINT32_MAX) const;
    void printWarningImpl_WarningAt(const AST::ParsingMeta& meta, const tokens_it& it,
        const uint32_t tokenSize = UINT32_MAX) const;
    void printWarningAlreadyParsed(const AST::ParsingMeta& meta, const tokens_it& it) const;
    void printWarningCustom(const AST::ParsingMeta& meta, const tokens_it& it,
        const std::string& message, const uint32_t tokenSize = UINT32_MAX) const;
    void printErrorCustom(const AST::ParsingMeta& meta, const tokens_it& it,
        const std::string& message, const uint32_t tokenSize = UINT32_MAX) const;
    void printErrorWrongPath(const std::string& filePath) const;
    void printErrorWrongPath(const AST::ParsingMeta& meta, const tokens_it& it) const;
    void printErrorUnexpectedEndl(const AST::ParsingMeta& meta, const tokens_it& it,
        const std::string& explanation = std::string()) const;
    void printErrorWrongToken(const AST::ParsingMeta& meta, const tokens_it& it,
        const std::string& expected = std::string()) const;
    void printErrorWrongKeywordUsage(const AST::ParsingMeta& meta, const tokens_it& it) const;
    void printErrorNamespaceLimit(const AST::ParsingMeta& meta, const tokens_it& it) const;
    void printErrorNameAlreadyInUse(const AST::ParsingMeta& meta, const tokens_it& it,
        const AST::ObjectMeta* firstDefinition = nullptr) const;
    void printErrorIncorrectName(const AST::ParsingMeta& meta, const tokens_it& it) const;
    void printErrorWrongDigit(const std::string& path, const lines_t& lines,
        const uint32_t line, const uint32_t posWrong, const char wrongDigit) const;

    uint32_t m_schemaHash = 0;
    uint32_t m_parsingErrorsCount = 0;
    bool m_detailed = false;
    bool m_stopOnFirstError = false;
};
