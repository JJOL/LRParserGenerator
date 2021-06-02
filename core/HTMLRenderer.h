#ifndef _HTML_RENDERER_H
#define _HTML_RENDERER_H
#include "../common/grammars.h"
#include <string>

class StatePrinter
{
private:
    GrammarState *state;
public:
    StatePrinter(GrammarState* _state) : state(_state) {}
    std::string getKernelString() const;
    std::string getClosureString(GrammarState *from) const;
    std::string getGotoString(GrammarState* fromState, const std::string& symb) const;
};

class HTMLRenderer
{
public:
    std::string getLRClosureTable(Parser* parser) const;
    std::string getLRParsingTable(Parser* parser) const;
};
#endif