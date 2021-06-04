#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "../common/grammars.h"
#include "../common/string_utils.h"

#include "GrammarReader.h"

Grammar GrammarReader::readRulesFromConsole()
{
    std::vector<GrammarRule> rules;
    String line;

    // Read Rules from Console
    int ruleIndex = 0;
    while (getline(std::cin, line)) {
        GrammarRule gRule;
        gRule.id = ruleIndex++;

        String rule = line;
        auto parts = split(rule, " ");
        gRule.body.reserve(parts.size()-1);
        gRule.head = parts[0];

        // Ignore parts[1] = "->"
        for (int i = 2; i < parts.size(); i++) {
            gRule.body.push_back(parts[i]);
        }

        rules.push_back(gRule);
    }

    // Build Grammar from rules and variables / literals containers
    Grammar grammar;
    grammar.rules = rules;
    for (GrammarRule rule : rules) {
        if (grammar.variableRulesMap.find(rule.head) == grammar.variableRulesMap.end()) {
            grammar.variableRulesMap[rule.head] = std::vector<GrammarRule>();
            grammar.variables.push_back(rule.head);
        }
        grammar.variableRulesMap[rule.head].push_back(rule);

        for (String symb : rule.body) {
            if (isLiteral(symb))
                grammar.literals.insert(symb);
        }
    }
    grammar.headVariable = grammar.variables[0];

    return grammar;
}