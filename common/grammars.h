#ifndef _GRAMMARS_H
#define _GRAMMARS_H

#include <string>
#include <vector>
#include <map>

typedef class GrammarRule
{
public: 
    std::string head;
    std::vector<std::string> body;
} GrammarRule;

typedef std::map<std::string, std::vector<GrammarRule>> rules_map_t;

typedef class Grammar
{
public:
    std::vector<std::string> literals;
    std::vector<std::string> symbols;

    rules_map_t rules_map;

} Grammar;


#endif