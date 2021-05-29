#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "common/grammars.h"
#include "common/string_utils.h"

#include "GrammarReader.h"

using namespace std;

Grammar GrammarReader::readRulesFromConsole()
{
    vector<GrammarRule> rules;
    string line;

    int ruleIndex = 0;
    while (getline(cin, line)) {
        GrammarRule gRule;
        gRule.id = ruleIndex++;

        string rule = line;
        int delimiterPos = rule.find("->");
        gRule.head = rule.substr(0, delimiterPos);
        string body = rule.substr(delimiterPos+2);
        gRule.body = split(body, " ");

        rules.push_back(gRule);
    }

    Grammar grammar;
    grammar.rules = rules;
    for (auto r : rules) {
        if (grammar.variableRulesMap.find(r.head) == grammar.variableRulesMap.end()) {
            grammar.variableRulesMap[r.head] = vector<GrammarRule>();
            grammar.variables.push_back(r.head);
        }
        rules_map_t::iterator itr = grammar.variableRulesMap.find(r.head);
        itr->second.push_back(r);

        for (auto symb : r.body) {
            if (isLiteral(symb)) grammar.literals.insert(symb);
        }
    }
    grammar.headVariable = grammar.variables[0];

    // Calculate follows for each variable


    return grammar;
}