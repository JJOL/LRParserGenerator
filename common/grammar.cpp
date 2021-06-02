#include "grammars.h"
#include <string>
#include <vector>
#include <set>
#include <iostream>

using namespace std;
using String = std::string;

bool isVariable(const std::string& symbol)
{
    return symbol[0] >= 'A' && symbol[0] <= 'Z';
}
bool isLiteral(const std::string& symbol)
{
    return !isVariable(symbol);
}

Grammar Grammar::extended()
{
    Grammar extendedG;

    // Define Extended Rule
    string startSymb = variables[0];
    string extStartSymb = startSymb + "'";
    extendedG.headVariable = extStartSymb;

    GrammarRule extRule;
    extRule.head = extStartSymb;
    extRule.body.push_back(startSymb);

    // Add Extended Rule as first rule of extended grammar
    extendedG.variables.reserve(variables.size()+1);
    extendedG.variables.push_back(extStartSymb);
    extendedG.variableRulesMap[extStartSymb] = vector<GrammarRule>();
    extendedG.variableRulesMap[extStartSymb].push_back(extRule);
    extendedG.rules.reserve(rules.size()+1);
    extendedG.rules.push_back(extRule);

    // Copy original grammar
    vector<string>::iterator s_it;
    for (s_it = variables.begin(); s_it < variables.end(); s_it++) {
        extendedG.variables.push_back(*s_it);
        extendedG.variableRulesMap[*s_it] = variableRulesMap[*s_it];
    }
    for (GrammarRule r : rules)
        extendedG.rules.push_back(r);

    extendedG.literals = literals;
    extendedG.literals.insert("$");
    extendedG.varFollowsMap = varFollowsMap;

    return extendedG;
}

void Grammar::generateFirsts()
{
    // Rule 1. of FIRSTS ALGORITHM
    for (auto lit : literals) {
        varFirstsMap[lit] = std::set<String>();
        varFirstsMap[lit].insert(lit);
    }

    bool hasAdded;
    do {
        hasAdded = false;
        for (auto var : variables) {
            for (auto rule : rules) {
                if (rule.head == var) {
                    auto it = varFirstsMap.find(var);
                    if (it == varFirstsMap.end()) {
                        varFirstsMap[var] = std::set<String>();
                        hasAdded = true;
                    }
                    String firstBodySymb = rule.body[0];
                    
                    // Rule 2. of FIRSTS ALGORITHM
                    std::set<String> toAdd;
                    if (isVariable(firstBodySymb)) {
                        toAdd = varFirstsMap[firstBodySymb];
                    } else {
                        toAdd.insert(firstBodySymb);
                    }

                    for (auto newLit : toAdd) {
                        if (varFirstsMap[var].find(newLit) == varFirstsMap[var].end()) {
                            varFirstsMap[var].insert(newLit);
                            hasAdded = true;
                        }
                    }
                }
            }
        }

    } while (hasAdded);
    
}

void Grammar::generateFollows()
{
    generateFirsts();

    // Rule 1. of FOLLOWS ALGORITHM
    varFollowsMap[headVariable] = std::set<String>();
    varFollowsMap[headVariable].insert("$");

    bool hasAdded;
    do {
        hasAdded = false;
        for (auto var : variables) {
            for (auto rule : rules) {
                for (int i = 0; i < rule.body.size(); i++) {
                    if (rule.body[i] == var) {
                        auto it = varFollowsMap.find(var);
                        if (it == varFollowsMap.end()) {
                            varFollowsMap[var] = std::set<String>();
                            hasAdded = true;
                        }

                        std::set<String> toAdd;
                        // Rule 2. of FOLLOWS ALGORITHM
                        if (i < rule.body.size()-1) {
                            toAdd = varFirstsMap[rule.body[i+1]];
                        }
                        // Rule 3. of FOLLOWS ALGORITHM
                        else {
                            toAdd = varFollowsMap[rule.head];
                        }

                        for (auto newLit : toAdd) {
                            if (varFollowsMap[var].find(newLit) == varFollowsMap[var].end()) {
                                varFollowsMap[var].insert(newLit);
                                hasAdded = true;
                            }
                        }
                    }
                }
            }
        }

    } while (hasAdded);
}

RuleProgress::RuleProgress(const GrammarRule& _rule) {
    rule = _rule;
    pos = 0;
    nextSymbol = rule.body[pos];
}

RuleProgress operator++(const RuleProgress& r)
{
    RuleProgress newRule = r;
    newRule.pos++;
    if (newRule.pos < newRule.rule.body.size())
        newRule.nextSymbol = newRule.rule.body[newRule.pos];
    else
        newRule.nextSymbol = "$";
    return newRule;
}

std::string RuleProgress::toString() const
{
    std::string s = rule.head + " -> ";
    for (int i = 0; i < rule.body.size(); i++) {
        if (i == pos)
            s += " ." + rule.body[i];
        else
            s += " " + rule.body[i];
    }
    if (pos == rule.body.size()) s += ".";

    // s += "(" + to_string(pos) + "," + nextSymbol + ")";

    return s;
}

bool RuleProgress::completed() const
{
    return pos >= rule.body.size();
}

bool operator<(const RuleProgress& r1, const RuleProgress& r2)
{
    // True if unequal false if equal
    if (r1.pos != r2.pos || r1.nextSymbol != r2.nextSymbol) return true;
    if (r1.rule.head != r2.rule.head) return true;
    return r1.rule.body != r2.rule.body;
}

GrammarState::GrammarState(int _state, const KernelsSet& _kernel, const Grammar& g)
{
    state = _state;
    from = nullptr;
    // Save Kernels In our Memory
    saveKRP(_kernel);
    // Expand Closure
    generateClosure(g);
    // Get Possible Actions
    for (auto pRule : closureRules) {
        if (!pRule.completed())
            actions.insert(pRule.nextSymbol);
    }
}

GrammarState::GrammarState(int _state, const KernelsSet& _kernel, GrammarState* _from, std::string _fromAction, const Grammar& g) 
: GrammarState(_state, _kernel, g)
{
    from = _from;
    fromAction = _fromAction;
}

KernelsSet GrammarState::move(std::string actionSymb)
{
    cout << "Moving S(" << state << "), " << actionSymb << endl;
    KernelsSet newKernelRules;
    for (auto pRule : closureRules) {
        if (pRule.nextSymbol == actionSymb && !pRule.completed()) {
            cout << "Applying Rule: " << pRule.toString() << endl;
            newKernelRules.insert(++pRule);
        }
    }

    return newKernelRules;
}

void GrammarState::saveKRP(const KernelsSet& _kernel)
{
    for (auto pRule : _kernel) {
        kernelRules.push_back(pRule);
        closureRules.push_back(pRule);
    }
}

void GrammarState::generateClosure(const Grammar& g)
{
    // cout << "Expanding S(" << state << ") closure..." << endl;
    std::set<std::string> checkedVars;
    for (int i = 0; i < closureRules.size(); i++) {
        RuleProgress& pRule = closureRules[i];
        // cout << "Checking symbol: " << pRule.nextSymbol << endl;
        if (isVariable(pRule.nextSymbol)
                && checkedVars.find(pRule.nextSymbol) == checkedVars.end()) {
            // cout << pRule.nextSymbol << " is a var and we have't seen it! Adding Rules!" << endl;
            checkedVars.insert(pRule.nextSymbol);

            auto rules = g.variableRulesMap.find(pRule.nextSymbol)->second;
            for (auto r : rules) {
                closureRules.push_back(RuleProgress(r));
            }
        }
    }
}

int GrammarState::getState() const
{
    return state;
}

std::vector<RuleProgress> GrammarState::getCompletedRules() const
{
    std::vector<RuleProgress> completedRules;
    for (auto r : kernelRules) {
        if (r.completed())
            completedRules.push_back(r); 
    }

    return completedRules;
}


Parser::~Parser()
{
    for (auto entry : stateMap) {
        delete entry.second;
    }
}