#include <iostream>
#include "parser.h"

int main() {
    Lambda e = parse("\\x.\\y.yx");
    std::cout << e << std::endl;

    std::cout << parse("\\x.y") << std::endl;
    std::cout << (parse("\\x.yx")) << std::endl;
    std::cout << (parse("(\\x.y)z")) << std::endl;
    std::cout << (parse("z")) << std::endl;

    std::cout << (alpha(parse("\\x. \\y.xy "), "z") == parse("\\z. \\y.zy ")) << std::endl;
    std::cout << (alpha(parse("\\x.\\x.xx"), "z") == parse("\\z.\\x.xx")) << std::endl;

    return 0;
}
