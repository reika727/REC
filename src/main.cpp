#include "lexical-analysis.hpp"
#include <iostream>

int main()
{
    const auto source =
        "  123     +   456 \n + 442  *    (789-111)+3343 *323 \n + 1   \t\v\f   \n";
    lexical_analysis::lexer lx(source);
    for (auto tk : lx.get_tokens()) {
        std::cout << tk.surface << std::endl;
    }
}
