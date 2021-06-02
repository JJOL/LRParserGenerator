#include "ParsingGenerator.h"
#include <list>
#include <set>
#include <unordered_map>
#include <string>
#include <iostream>

std::map<int, GrammarState*> groupIntoTable(GrammarState *root)
{
    // Generar una tabla de estados indexada por identificador de estado
    std::map<int, GrammarState*> stateMap;
    std::list<GrammarState*> toVisit;
    std::set<GrammarState*> visited;
    toVisit.push_back(root);
    while (!toVisit.empty()) {
        GrammarState* state = toVisit.front();
        toVisit.pop_front();
        auto it = visited.find(state);
        if (it != visited.end())
            continue;
        else
            visited.insert(state);

        for (auto vk : state->actionStateMap) {
            toVisit.push_back(vk.second);
        }

        stateMap[state->getState()] = state;
    }

    return stateMap;
}

void printKernelSet(const KernelsSet& kernel)
{
    std::cout << "{ ";
    for (auto pr : kernel)
        std::cout << pr.toString() << "; ";
    std::cout << "}" << std::endl;
}

bool myKernelSetFind(
    const RuleProgress& pr,
    const KernelsSet& ks
)
{
    for (auto it = ks.begin(); it != ks.end(); it++) {
        const RuleProgress& r = *it;
        if (!(r < pr) && !(pr < r)) return true;
    }
    return false;
}

struct KernelSetCompare
{
    bool operator() (const KernelsSet& ks1, const KernelsSet& ks2) const
    {
        for (auto k : ks1) {
            // if (ks2.find(k) == ks2.end()) {
            if (!myKernelSetFind(k, ks2))
                return true;
        }
        return false;
    }
};

GrammarState* myKernelMapFind(
    const std::map<KernelsSet, GrammarState*, KernelSetCompare>& map,
    const KernelsSet& keySet, bool debug)
{
    KernelSetCompare kc;
    for (auto kv : map) {
        if (!kc(kv.first, keySet) && !kc(keySet, kv.first)) {
            return kv.second;
        }
    }
    return nullptr;
}

Parser* ParsingGenerator::generate(Grammar g)
{
    std::set<GrammarState*> visited;
    std::list<GrammarState*> toVisit;
    std::map<KernelsSet, GrammarState*, KernelSetCompare> kernelsStateMap;

    GrammarRule firstRule = g.variableRulesMap[g.variables[0]][0];
    KernelsSet initKernel;
    initKernel.insert(RuleProgress(firstRule));

    int STATE_ID = 0;
    GrammarState* state0 = new GrammarState(STATE_ID++, initKernel, g);

    toVisit.push_back(state0);

    while (toVisit.size() > 0) {
        GrammarState* state = toVisit.front();
        toVisit.pop_front();

        auto it = visited.find(state);
        if (it != visited.end()) {
            std::cout << "We have visited state (" << state->getState() << "). Skiping!" << std::endl;
            continue;
        } else {
            std::cout << "We haven't visited state (" << state->getState() << "). Checking..." << std::endl;
            visited.insert(state);
        }

    //     // Process Actions
        std::cout << "Checking actions:" << std::endl;
        for (std::string a : state->actions) {
            std::cout << "a: " << a << std::endl;
            KernelsSet newKernelRules = state->move(a);
            std::cout << "Generated Kernel: " << std::endl;
            printKernelSet(newKernelRules);
            GrammarState* nextState;

            // if (state->getState() == 1 && a == "F") {
            //     auto it = kernelsStateMap.find(newKernelRules);
            //     if (it == kernelsStateMap.end()) {
            //         std::cout << "We didnt find a matching kernel within:" << std::endl;
            //         for (auto kv : kernelsStateMap) {
            //             std::cout << "Kernel:" << std::endl;
            //             printKernelSet(kv.first);
            //         }
            //     } else {
            //         std::cout << "We found a matching kernel!" << std::endl;
            //     } 
            // }
            
            auto it = kernelsStateMap.find(newKernelRules);

            auto found = myKernelMapFind(kernelsStateMap, newKernelRules, state->getState() == 1);

            // if (it != kernelsStateMap.end()) {
            if (found != nullptr) {
                // nextState = it->second;
                nextState = found;
            } else {
                nextState = new GrammarState(STATE_ID++, newKernelRules, state, a, g);
                std::cout << "Adding KernelSet to Map..." << std::endl;
                kernelsStateMap.insert({newKernelRules,  nextState});
                toVisit.push_back(nextState);
            }

            state->actionStateMap[a] = nextState;
        }
    }



    Parser* parser = new Parser(g, groupIntoTable(state0));
    parser->root = state0;
    return parser;
}