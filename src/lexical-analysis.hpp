#ifndef LEXICAL_ANALYSIS_HPP
#define LEXICAL_ANALYSIS_HPP
#include <vector>
#include <string>
#include <string_view>
#include <span>
#include <ranges>
#include <optional>
#include <format>
#include <stdexcept>

namespace lexical_analysis {
    enum class token_kinds {
        /* type specifiers */
        Signed, Unsigned, Char, Short, Int, Long,
        Float, Double,
        Void,
        /* struct and union */
        Struct, Union,
        /* enum */
        Enum,
        /* type qualifiers */
        Const, Volatile,
        /* storage class specifiers */
        Auto, Extern, Static, Register, Typedef,
        /* control statements */
        If, Else, Switch, Case, Default,
        While, Do, For,
        Break, Continue, Return, Goto,
        /* keyword operator */
        Sizeof,

        /* identifier */
        Identifier,

        /* integer literals (TODO) */
        IntegerLiteral,
        /* floating point literals (TODO) */
        /* character literal */
        CharacterLiteral,
        /* string literal */
        StringLiteral,

        /* pure puncutuators */
        OpenBrace, CloseBrace,
        Semicolon,
        Elipsis,
        //Sharp,
        //DoubleSharp,
        /* puncutuators as well as operators */
        OpenParenthesis, CloseParenthesis,
        OpenBracket, CloseBracket,
        Asterisk,
        Comma,
        Colon,
        Equal,
        Dot,

        /* unary opeators */
        BitwiseNot,
        LogicalNagation,
        Address,
        UnaryPlus, UnaryMinus,
        Increment, Decrement,
        /* binary operators */
        Division, Remainder,
        LessThan, GreaterThan, LessThanOrEqualTo, GreaterThanOrEqualTo, EqualTo, NotEqualTo,
        BitwiseOr, BitwiseXor, BitwiseLeftShift, BitwiseRightShift,
        LogicalAnd, LogicalOr,
        /* ternary operator */
        TernaryIf,
        /* assignment */
        AdditionAssignment,
        SubtractionAssignment,
        MultiplicationAssignment,
        DivisionAssignment,
        RemainderAssignment,
        BitwiseAndAssignment,
        BitwiseOrAssignment,
        BitwiseXorAssignment,
        BitwiseLeftShiftAssignment,
        BitwiseRightShiftAssignment,
        /* member accessor */
        MemberAccessThroughPointer,

        /* operators same as puncutuators */
        Dereference = Asterisk,
        Multiplication = Asterisk,
        OpenSubscript = OpenBracket, CloseSubscript = CloseBracket,
        SequentialEvaluation = Comma,
        TernaryElse = Colon,
        OpenFunctionCall = OpenParenthesis, CloseFunctionCall = CloseParenthesis,
        Assignment = Equal,
        MemberAccess = Dot,
        /* binary opeators same as unary operators */
        BitwiseAnd = Address,
        Addition = UnaryPlus,
        Subtraction = UnaryMinus
    };

    struct token final {
        const std::string surface;
        const std::size_t row_index, column_index;
        const token_kinds token_kind;
    };

    class unknown_token_error final : public std::runtime_error {
    public:
        unknown_token_error(const std::size_t row_index, const std::size_t column_index)
            : std::runtime_error(std::format("unknown token at [{}:{}]", row_index + 1, column_index + 1))
        {
        }
    };

    class lexer final {
    private:
        std::vector<token> tokens;
        std::ranges::subrange<std::vector<token>::const_iterator> left_tokens;

    public:
        std::span<const token> get_tokens() const
        {
            return tokens;
        }

        lexer(std::string_view source);

        std::optional<token> front_token() const;

        std::optional<token> consume_if(const token_kinds token_kind);
    };
}
#endif
