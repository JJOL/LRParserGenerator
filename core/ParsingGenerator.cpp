#include "ParsingGenerator.h"
#include <list>
#include <set>
#include <unordered_map>
#include <string>
#include <iostream>

// ------------------------------------------------------------------------------- //
// Custom Implementation for comparison operators in sets and maps that doesnt quite work well
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
            if (!myKernelSetFind(k, ks2))
                return true;
        }
        return false;
    }
};
// ------------------------------------------------------------------------------- //

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


GrammarState* findStateFromKernel(
    const std::map<KernelsSet, GrammarState*, KernelSetCompare>& map,
    const KernelsSet& keySet)
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

    int STATE_ID = 0;
    GrammarRule& firstRule = g.variableRulesMap[g.headVariable].front();
    KernelsSet state0Kernel;
    state0Kernel.insert(RuleProgress(firstRule));
    GrammarState* state0 = new GrammarState(STATE_ID++, state0Kernel, g);

    toVisit.push_back(state0);

    while (toVisit.size() > 0) {
        GrammarState* state = toVisit.front();
        toVisit.pop_front();

        auto indexIt = visited.find(state);
        if (indexIt != visited.end()) {
            std::cout << "We have visited state (" << state->getState() << "). Skiping!" << std::endl;
            continue;
        } else {
            std::cout << "We haven't visited state (" << state->getState() << "). Checking..." << std::endl;
            visited.insert(state);
        }

        // Process Actions
        std::cout << "Checking actions:" << std::endl;
        for (String a : state->actions) {
            KernelsSet newKernelRules = state->move(a);
            #ifdef _DEBUG
            std::cout << "a: " << a << std::endl;
            std::cout << "Generated Kernel: " << std::endl;
            printKernelSet(newKernelRules);
            #endif
            GrammarState* nextState;
            nextState = findStateFromKernel(kernelsStateMap, newKernelRules);
            if (nextState == nullptr) {
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