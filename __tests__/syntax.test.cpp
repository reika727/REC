#include "../src/lexical-analysis.hpp"
#include <string>
#include <functional>
#include <memory>
#include <utility>
#include <format>
#include <stdexcept>
#include <iostream>

namespace syntax_analysis {
    class parse_error final : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;

        parse_error(std::format_string<std::size_t, std::size_t> msg, const lexical_analysis::token &t)
            : std::runtime_error(std::format(msg, t.row_index + 1, t.column_index + 1))
        {
        }
    };

    template <class T>
    class expression {
    public:
        virtual T evaluate() const = 0;
    };

    template <class T, class BinaryOperator>
    class binary_expression final : public expression<T> {
    private:
        const std::unique_ptr<const expression<T>> left_argument, right_argument;

    public:
        binary_expression(
            std::unique_ptr<const expression<T>> left_argument,
            std::unique_ptr<const expression<T>> right_argument
        )
            : left_argument(std::move(left_argument)),
              right_argument(std::move(right_argument))
        {
        }

        T evaluate() const override
        {
            return BinaryOperator()(left_argument->evaluate(), right_argument->evaluate());
        }
    };

    template <class T>
    using addition = binary_expression<T, std::plus<T>>;

    template <class T>
    using subtraction = binary_expression<T, std::minus<T>>;

    template <class T>
    using multiplication = binary_expression<T, std::multiplies<T>>;

    template <class T>
    using division = binary_expression<T, std::divides<T>>;

    template <class T>
    class number final : public expression<T> {
    private:
        const T value;

    public:
        number(const T value)
            : value(value)
        {
        }

        T evaluate() const override
        {
            return value;
        }
    };

    /*
     * expression :=
     *     term , {("+" | "-") , term} ;
     * term :=
     *     factor , {("*" | "/") , factor} ;
     * factor :=
     *     number , | "(" , expression , ")" ;
     * number :=
     *     digit , {digit} ;
     * digit :=
     *     "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" ;
     */

    template <class T>
    std::unique_ptr<const expression<T>> get_expression(lexical_analysis::lexer &lx);

    template <class T>
    std::unique_ptr<const number<T>> get_number(lexical_analysis::lexer &lx);

    template <>
    std::unique_ptr<const number<int>> get_number<int>(lexical_analysis::lexer &lx)
    {
        if (const auto il = lx.consume_if(lexical_analysis::token_kinds::IntegerLiteral)) {
            return std::make_unique<number<int>>(std::stoi(il->surface));
        } else {
            if (const auto front_token = lx.front_token()) {
                throw parse_error("an integer literal is expected at [{}:{}]", *front_token);
            } else {
                throw parse_error("an integer literal is expected but file ends");
            }
        }
    }

    template <class T>
    std::unique_ptr<const expression<T>> get_factor(lexical_analysis::lexer &lx)
    {
        if (auto op = lx.consume_if(lexical_analysis::token_kinds::OpenParenthesis)) {
            auto expression = get_expression<T>(lx);
            if (!lx.consume_if(lexical_analysis::token_kinds::CloseParenthesis)) {
                throw parse_error("parenthesis at [{}:{}] is not closed", *op);
            }
            return expression;
        } else {
            return get_number<T>(lx);
        }
    }

    template <class T>
    std::unique_ptr<const expression<T>> get_term(lexical_analysis::lexer &lx)
    {
        for (auto factor = get_factor<T>(lx); ;) {
            if (lx.consume_if(lexical_analysis::token_kinds::Multiplication)) {
                factor = std::make_unique<multiplication<T>>(std::move(factor), get_factor<T>(lx));
            } else if (lx.consume_if(lexical_analysis::token_kinds::Division)) {
                factor = std::make_unique<division<T>>(std::move(factor), get_factor<T>(lx));
            } else {
                return factor;
            }
        }
    }

    template <class T>
    std::unique_ptr<const expression<T>> get_expression(lexical_analysis::lexer &lx)
    {
        for (auto term = get_term<T>(lx); ;) {
            if (lx.consume_if(lexical_analysis::token_kinds::Addition)) {
                term = std::make_unique<addition<T>>(std::move(term), get_term<T>(lx));
            } else if (lx.consume_if(lexical_analysis::token_kinds::Subtraction)) {
                term = std::make_unique<subtraction<T>>(std::move(term), get_term<T>(lx));
            } else {
                return term;
            }
        }
    }
}

int main()
{
    const auto source =
        "  123     +   456 \n + 442  *    (789-111)+3343 *323 \n + 1   \t\v\f   \n";
    lexical_analysis::lexer lx(source);
    const auto syntax_tree_root = syntax_analysis::get_expression<int>(lx);
    std::cout << source << " = " << syntax_tree_root->evaluate() << std::endl;
}
