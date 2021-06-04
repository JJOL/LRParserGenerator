#include "grammars.h"
#include <string>
#include <vector>
#include <set>
#include <iostream>

bool isVariable(const String& symbol)
{
    return symbol[0] >= 'A' && symbol[0] <= 'Z';
}
bool isLiteral(const String& symbol)
{
    return !isVariable(symbol);
}

Grammar Grammar::extended()
{
    Grammar extendedG;

    // Define Extended Rule
    String extStartSymb = headVariable + "'";
    extendedG.headVariable = extStartSymb;

    GrammarRule extRule;
    extRule.head = extStartSymb;
    extRule.body.push_back(headVariable);

    // Add Extended Rule as first rule of extended grammar
    extendedG.variables.reserve(variables.size()+1);
    extendedG.variables.push_back(extStartSymb);
    extendedG.variableRulesMap[extStartSymb] = std::vector<GrammarRule>();
    extendedG.variableRulesMap[extStartSymb].push_back(extRule);
    extendedG.rules.reserve(rules.size()+1);
    extendedG.rules.push_back(extRule);

    // Copy original grammar | Dont extendedG.variables = variables, because we have added to front an extra one
    std::vector<String>::iterator s_it;
    for (String var : variables) {
        extendedG.variables.push_back(var);
        extendedG.variableRulesMap[var] = variableRulesMap[var];
    }
    for (GrammarRule r : rules)
        extendedG.rules.push_back(r);

    extendedG.literals = literals;
    extendedG.literals.insert("$");

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
        for (String var : variables) {
            for (GrammarRule rule : variableRulesMap[var]) {
                auto indexIt = varFirstsMap.find(var);
                if (indexIt == varFirstsMap.end()) {
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

                for (String newLit : toAdd) {
                    if (varFirstsMap[var].find(newLit) == varFirstsMap[var].end()) {
                        varFirstsMap[var].insert(newLit);
                        hasAdded = true;
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
        for (String var : variables) {
            for (GrammarRule rule : rules) {
                for (int i = 0; i < rule.body.size(); i++) {
                    if (rule.body[i] == var) {
                        auto indexIt = varFollowsMap.find(var);
                        if (indexIt == varFollowsMap.end()) {
                            varFollowsMap[var] = std::set<String>();
                            hasAdded = true;
                        }

                        std::set<String> toAdd;
                        // Rule 2. of FOLLOWS ALGORITHM (Follows are Firsts of the next variable in production)
                        if (i < rule.body.size()-1) {
                            toAdd = varFirstsMap[rule.body[i+1]];
                        }
                        // Rule 3. of FOLLOWS ALGORITHM (Follows are Follows of the head variable of production)
                        else {
                            toAdd = varFollowsMap[rule.head];
                        }

                        for (String newLit : toAdd) {
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

String RuleProgress::toString() const
{
    String s = rule.head + " -> ";
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
    copyKernelRules(_kernel);
    // Expand Closure
    generateClosure(g);
    // Get Possible Actions
    for (auto pRule : closureRules) {
        if (!pRule.completed())
            actions.insert(pRule.nextSymbol);
    }
}

GrammarState::GrammarState(int _state, const KernelsSet& _kernel, GrammarState* _from, String _fromAction, const Grammar& g) 
: GrammarState(_state, _kernel, g)
{
    from = _from;
    fromAction = _fromAction;
}

KernelsSet GrammarState::move(String actionSymb)
{
    std::cout << "Moving S(" << state << "), " << actionSymb << std::endl;
    KernelsSet newKernelRules;
    for (auto pRule : closureRules) {
        if (pRule.nextSymbol == actionSymb && !pRule.completed()) {
            std::cout << "Applying Rule: " << pRule.toString() << std::endl;
            newKernelRules.insert(++pRule);
        }
    }

    return newKernelRules;
}

void GrammarState::copyKernelRules(const KernelsSet& _kernel)
{
    for (auto pRule : _kernel) {
        kernelRules.push_back(pRule);
        closureRules.push_back(pRule);
    }
}

void GrammarState::generateClosure(const Grammar& g)
{
    // cout << "Expanding S(" << state << ") closure..." << endl;
    std::set<String> checkedVars;
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