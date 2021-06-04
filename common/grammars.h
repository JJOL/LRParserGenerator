#ifndef _GRAMMARS_H
#define _GRAMMARS_H

#include <string>
#include <vector>
#include <map>
#include <set>

using String = std::string;
using StringVector = std::vector<String>;

class GrammarRule
{
public:
    int id;
    String head;
    std::vector<String> body;
};

using GrammarRulesMap = std::map<String, std::vector<GrammarRule>>;

class Grammar
{
public:
    std::set<String> literals;
    std::vector<String> variables;
    std::vector<GrammarRule> rules;
    GrammarRulesMap variableRulesMap;
    std::map<String,std::set<String>> varFollowsMap;
    std::map<String,std::set<String>> varFirstsMap;
    String headVariable;
public:
    Grammar extended();
    void generateFollows();
    void generateFirsts();
};

bool isVariable(const String& symbol);
bool isLiteral(const String& symbol);

class RuleProgress {
public:
    GrammarRule rule;
    int pos;
    String nextSymbol;
public:
    RuleProgress(const GrammarRule& _rule);
    bool completed() const;
    String toString() const;
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
    String fromAction;
    std::vector<RuleProgress> kernelRules;
    std::vector<RuleProgress> closureRules;
    std::set<String> actions;
    std::map<String, GrammarState*> actionStateMap;
public:
    GrammarState() {}
    GrammarState(int _state): state(_state) {}
    GrammarState(int _state, const KernelsSet& _kernelsSet, const Grammar& g);
    GrammarState(int _state, const KernelsSet& _kernelsSet, GrammarState* _from, String _fromAction, const Grammar& g);
    
    KernelsSet move(String actionSymb);
    
    int getState() const;

    std::vector<RuleProgress> getCompletedRules() const;

private:
    void copyKernelRules(const KernelsSet& _kernel);
    void generateClosure(const Grammar& g);
    void extractActionSymbols();
};

struct Parser
{
public:
    Grammar grammar;
    std::map<int, GrammarState*> stateMap;
    GrammarState *root;
    Parser(Grammar _grammar, std::map<int, GrammarState*> _stateMap): grammar(_grammar), stateMap(_stateMap) {}
    ~Parser();
};

#endif