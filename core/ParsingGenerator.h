#ifndef _PARSING_GENERATOR_H
#define _PARSING_GENERATOR_H
#include "../common/grammars.h"

class ParsingGenerator
{
public:
    Parser* generate(Grammar g);
};
#endif