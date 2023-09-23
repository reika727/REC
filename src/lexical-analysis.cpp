#include "lexical-analysis.hpp"
#include <map>
#include <variant>
#include <regex>
#include <cctype>

namespace lexical_analysis {
    namespace {
        const std::map<token_kinds, std::variant<std::string_view, std::regex>> token_expressions {
            {token_kinds::Signed,                      "signed"},
            {token_kinds::Unsigned,                    "unsigned"},
            {token_kinds::Char,                        "char"},
            {token_kinds::Short,                       "short"},
            {token_kinds::Int,                         "int"},
            {token_kinds::Long,                        "long"},
            {token_kinds::Float,                       "float"},
            {token_kinds::Double,                      "double"},
            {token_kinds::Void,                        "void"},
            {token_kinds::Struct,                      "struct"},
            {token_kinds::Union,                       "union"},
            {token_kinds::Enum,                        "enum"},
            {token_kinds::Const,                       "const"},
            {token_kinds::Volatile,                    "volatile"},
            {token_kinds::Auto,                        "auto"},
            {token_kinds::Extern,                      "extern"},
            {token_kinds::Static,                      "static"},
            {token_kinds::Register,                    "register"},
            {token_kinds::Typedef,                     "typedef"},
            {token_kinds::If,                          "if"},
            {token_kinds::Else,                        "else"},
            {token_kinds::Switch,                      "switch"},
            {token_kinds::Case,                        "case"},
            {token_kinds::Default,                     "default"},
            {token_kinds::While,                       "while"},
            {token_kinds::Do,                          "do"},
            {token_kinds::For,                         "for"},
            {token_kinds::Break,                       "break"},
            {token_kinds::Continue,                    "continue"},
            {token_kinds::Return,                      "return"},
            {token_kinds::Goto,                        "goto"},
            {token_kinds::Sizeof,                      "sizeof"},
            {token_kinds::Identifier,                  std::regex(R"([A-Za-z_]\w*)")},
            {token_kinds::IntegerLiteral,              std::regex(R"(\d+)")},
            {token_kinds::CharacterLiteral,            std::regex(R"('(?:[^'\\]|\\.)+')")},
            {token_kinds::StringLiteral,               std::regex(R"("(?:[^"\\]|\\.)*")")},
            {token_kinds::OpenBrace,                   "{"},
            {token_kinds::CloseBrace,                  "}"},
            {token_kinds::Semicolon,                   ";"},
            {token_kinds::Elipsis,                     "..."},
            {token_kinds::OpenParenthesis,             "("},
            {token_kinds::CloseParenthesis,            ")"},
            {token_kinds::OpenBracket,                 "["},
            {token_kinds::CloseBracket,                "]"},
            {token_kinds::Asterisk,                    "*"},
            {token_kinds::Comma,                       ","},
            {token_kinds::Colon,                       ":"},
            {token_kinds::Equal,                       "="},
            {token_kinds::Dot,                         "."},
            {token_kinds::BitwiseNot,                  "~"},
            {token_kinds::LogicalNagation,             "!"},
            {token_kinds::Address,                     "&"},
            {token_kinds::UnaryPlus,                   "+"},
            {token_kinds::UnaryMinus,                  "-"},
            {token_kinds::Increment,                   "++"},
            {token_kinds::Decrement,                   "--"},
            {token_kinds::Division,                    "/"},
            {token_kinds::Remainder,                   "%"},
            {token_kinds::LessThan,                    "<"},
            {token_kinds::GreaterThan,                 ">"},
            {token_kinds::LessThanOrEqualTo,           "<="},
            {token_kinds::GreaterThanOrEqualTo,        ">="},
            {token_kinds::EqualTo,                     "=="},
            {token_kinds::NotEqualTo,                  "!="},
            {token_kinds::BitwiseOr,                   "|"},
            {token_kinds::BitwiseXor,                  "^"},
            {token_kinds::BitwiseLeftShift,            "<<"},
            {token_kinds::BitwiseRightShift,           ">>"},
            {token_kinds::LogicalAnd,                  "&&"},
            {token_kinds::LogicalOr,                   "||"},
            {token_kinds::TernaryIf,                   "?"},
            {token_kinds::AdditionAssignment,          "+="},
            {token_kinds::SubtractionAssignment,       "-="},
            {token_kinds::MultiplicationAssignment,    "*="},
            {token_kinds::DivisionAssignment,          "/="},
            {token_kinds::RemainderAssignment,         "%="},
            {token_kinds::BitwiseAndAssignment,        "&="},
            {token_kinds::BitwiseOrAssignment,         "|="},
            {token_kinds::BitwiseXorAssignment,        "^="},
            {token_kinds::BitwiseLeftShiftAssignment,  "<<="},
            {token_kinds::BitwiseRightShiftAssignment, ">>="},
            {token_kinds::MemberAccessThroughPointer,  "->"}
        };

        struct surface_getter final {
            const std::string_view compared;

            surface_getter(const std::string_view compared)
                : compared(compared)
            {
            }

            std::optional<std::string> operator()(const std::string_view token_string) const
            {
                if (compared.starts_with(token_string)) {
                    return std::string{token_string.begin(), token_string.end()};
                } else {
                    return std::nullopt;
                }
            }

            std::optional<std::string> operator()(const std::regex &re) const
            {
                if (std::cmatch m; std::regex_search(compared.begin(), compared.end(), m, re, std::regex_constants::match_continuous)) {
                    return m.str();
                } else {
                    return std::nullopt;
                }
            }
        };
    }

    lexer::lexer(std::string_view source)
    {
        using std::operator""sv;
        constexpr auto const_lines_view = std::views::split("\n"sv) | std::views::enumerate | std::views::as_const;
        constexpr auto drop_space_view = std::views::drop_while(::isspace);
        for (const auto [row_index, row] : source | const_lines_view) {
            for (std::ranges::subrange trail = row | drop_space_view; trail; trail = trail | drop_space_view) {
                std::optional<token> found_token = std::nullopt;
                for (const auto &[token_kind, expression] : token_expressions) {
                    if (auto surface = std::visit(surface_getter{std::string_view{trail}}, expression)) {
                        if (!found_token || found_token->surface.length() < surface->length()) {
                            found_token.emplace(
                                *surface,
                                static_cast<std::size_t>(row_index),
                                static_cast<std::size_t>(trail.begin() - row.begin()),
                                token_kind
                            );
                        }
                    }
                }
                if (found_token) {
                    tokens.push_back(*found_token);
                    trail.advance(found_token->surface.length());
                } else {
                    throw unknown_token_error(row_index, trail.begin() - row.begin());
                }
            }
        }
        left_tokens = {tokens.begin(), tokens.end()};
    }

    std::optional<token> lexer::front_token() const
    {
        if (left_tokens) {
            return left_tokens.front();
        } else {
            return std::nullopt;
        }
    }

    std::optional<token> lexer::consume_if(const token_kinds token_kind)
    {
        if (auto t = front_token(); t.has_value() && t->token_kind == token_kind) {
            left_tokens.advance(1);
            return t;
        } else {
            return std::nullopt;
        }
    }
}
