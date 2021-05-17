#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "common/grammars.h"
#include "common/string_utils.h"

using namespace std;

Grammar read_console_rules()
{
    vector<GrammarRule> rules;
    string line;

    while (getline(cin, line)) {
        GrammarRule gRule;

        string rule = line;
        int delimiterPos = rule.find("->");
        gRule.head = rule.substr(0, delimiterPos);
        string body = rule.substr(delimiterPos+2);
        gRule.body = split(body, " ");

        rules.push_back(gRule);
    }

    Grammar grammar;
    
    vector<GrammarRule>::iterator r_itr;
    for (r_itr = rules.begin(); r_itr < rules.end(); r_itr++) {
        GrammarRule r = *r_itr;
        
        if (grammar.rules_map.find(r.head) == grammar.rules_map.end()) {
            // grammar.rules_map.insert(pair<string, vector<GrammarRule>>(r.head, vector<GrammarRule>()));
            grammar.rules_map[r.head] = vector<GrammarRule>();
            grammar.symbols.push_back(r.head);
        }
        rules_map_t::iterator itr = grammar.rules_map.find(r.head);
        itr->second.push_back(r);
    }

    return grammar;
}