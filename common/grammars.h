#ifndef _GRAMMARS_H
#define _GRAMMARS_H

#include <string>
#include <vector>
#include <map>
#include <set>

using StringVector = std::vector<std::string>;

class GrammarRule
{
public:
    int id;
    std::string head;
    std::vector<std::string> body;
};

using rules_map_t = std::map<std::string, std::vector<GrammarRule>>;

class Grammar
{
public:
    std::set<std::string> literals;
    std::vector<std::string> variables;
    rules_map_t variableRulesMap;
    std::vector<GrammarRule> rules;
    std::map<std::string,std::set<std::string>> varFollowsMap;
    std::map<std::string,std::set<std::string>> varFirstsMap;
    std::string headVariable;
public:
    Grammar extended();
    void generateFollows();
    void generateFirsts();
};

bool isVariable(const std::string& symbol);
bool isLiteral(const std::string& symbol);

class RuleProgress {
public:
    GrammarRule rule;
    int pos;
    std::string nextSymbol;
public:
    RuleProgress(const GrammarRule& _rule);
    bool completed() const;
    std::string toString() const;
    friend bool operator<(const RuleProgress& r1, const RuleProgress& r2);
    friend RuleProgress operator++(const RuleProgress& r);
};

using KernelsSet = std::set<RuleProgress>;

class GrammarState
{
private:
    int state;
public:
    GrammarState* from;
    std::string fromAction;
    std::vector<RuleProgress> kernelRules;
    std::vector<RuleProgress> closureRules;
    std::set<std::string> actions;
    std::map<std::string, GrammarState*> actionStateMap;
public:
    GrammarState() {}
    GrammarState(int _state): state(_state) {}
    GrammarState(int _state, const KernelsSet& _kernelsSet, const Grammar& g);
    GrammarState(int _state, const KernelsSet& _kernelsSet, GrammarState* _from, std::string _fromAction, const Grammar& g);
    
    KernelsSet move(std::string actionSymb);
    
    int getState() const;

    std::vector<RuleProgress> getCompletedRules() const;

private:
    void saveKRP(const KernelsSet& _kernel);
    void generateClosure(const Grammar& g);
    void extractActionSymbols();
};

class Parser
{
public:
    Grammar grammar;
    std::map<int, GrammarState*> stateMap;
    GrammarState *root;
    Parser(Grammar _grammar, std::map<int, GrammarState*> _stateMap): grammar(_grammar), stateMap(_stateMap) {}
    ~Parser();
};

#endif