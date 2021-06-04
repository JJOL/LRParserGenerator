#include "HTMLRenderer.h"
#include <iostream>
#include <list>

using String = std::string;


String StatePrinter::getKernelString() const
{
    std::string str = "{";
    for (RuleProgress pr : state->kernelRules) {
        if (pr.completed())
            str += "<span style=\"color: green\">" + pr.toString() + "; </span>";
        else
            str += "<span>" + pr.toString() + "; </span>";
    }

    str += "}";
    return str;
}

String StatePrinter::getClosureString(GrammarState* from) const
{
    if (from != nullptr && (from->getState() != state->from->getState()))
        return "";

    std::string str = "{";
    for (RuleProgress pr : state->closureRules) {
        if (pr.completed())
            str += "<span style=\"color: green\">" + pr.toString() + "; </span>";
        else
            str += "<span>" + pr.toString() + "; </span>";
    }

    str += "}";
    return str;
}

String StatePrinter::getGotoString(GrammarState* fromState, const std::string& symb) const
{
    return std::string("goto(") + std::to_string(fromState->getState()) + ","  + symb + ")";
}


String HTMLRenderer::getLRClosureTable(Parser* parser) const
{
    String lrClosureTableHTML;
    std::cout << "LR CLOSURE TABLE" << std::endl;
    String tableHead = 
    "<table>"
    "<thead>"
    "<tr><th colspan=\"4\">SLR closure table</th></tr>"
    "<tr>"
    "<th>Goto</th><th>Kernel</th><th>State</th><th>Closure</th>"
    "</tr>"
    "</thead>"
    "<tbody>";
    String tableFoot = 
    "</tbody>"
    "</table>";
    lrClosureTableHTML = tableHead;
    
    String content;
    std::list<GrammarState*> toVisit;
    std::set<GrammarState*> visited;
    GrammarState* root = parser->root;
    toVisit.push_back(root);

    StatePrinter sprinter(root);
    content = String("<tr><td>")
        + "</td><td>"
        + sprinter.getKernelString() + "</td><td class=\"td-state\">"
        + std::to_string(root->getState()) + "</td><td>"
        + sprinter.getClosureString(nullptr) 
        + "</td></tr>";
    lrClosureTableHTML += content;

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
            GrammarState* toState = vk.second;
            sprinter = StatePrinter(toState);

            String closureString = "";
            String content = String("<tr><td>")
                + sprinter.getGotoString(state, vk.first) +"</td><td>"
                + sprinter.getKernelString() + "</td><td class=\"td-state\">"
                + std::to_string(toState->getState()) + "</td><td>"
                + sprinter.getClosureString(state)
                + "</td></tr>";
            lrClosureTableHTML += content;
        }
    }

    lrClosureTableHTML += tableFoot;

    #ifdef _DEBUG
    std::cout << lrClosureTableHTML << std::endl;
    #endif

    return lrClosureTableHTML;
}

std::string HTMLRenderer::getLRParsingTable(Parser* parser) const
{
    Grammar extGrammar = parser->grammar;

    String lrTableHTML;
    std::cout << "LR TABLE" << std::endl;
    
    int nlits = extGrammar.literals.size(),
        nvars = extGrammar.variables.size();
    int nsymbs = nlits + nvars;
    
    String tableHead = 
    String("<table>")
    + "<thead>"
    + "<tr><th colspan=\""+std::to_string(nsymbs+1)+"\">LR table</th></tr>"
    + "<tr>"
    + "<th rowspan=\"2\">State</th><th colspan=\""+std::to_string(nlits)+"\">ACTION</th><th colspan=\""+std::to_string(nvars)+"\">GOTO</th>"
    + "</tr>";
    tableHead += "<tr>";
    for (auto l : extGrammar.literals)
        tableHead += String("<td>") + l + "</td>";
    for (auto v : extGrammar.variables)
        tableHead += String("<td>") + v + "</td>";
    tableHead += "</tr>";
    tableHead += "</thead>";

    lrTableHTML = tableHead + "<tbody>";

    String content;
    for (auto stateEntry : parser->stateMap) {
        int stateIntex = stateEntry.first;
        GrammarState* state = stateEntry.second;

        content = "<tr><td class=\"td-state\">"+std::to_string(stateEntry.first)+"</td>";

        std::map<String, String> literalEntries;

        // Imprimir Reduces
        std::vector<RuleProgress> completedProgressRules =  state->getCompletedRules();
        for (auto pr : completedProgressRules) {
            if (pr.rule.head == extGrammar.headVariable) {
                // Poner acc en <td>$</td>
                literalEntries["$"] += "<span style=\"color: green\">acc</span>";
            }
            else {
                int ruleIndex = pr.rule.id;
                auto followLiterals = extGrammar.varFollowsMap[pr.rule.head];
                for (auto lit : followLiterals) {
                    // Poner R[ruleIndex] en <td>lit</td>
                    if (literalEntries[lit] != "") literalEntries[lit] += ";";
                    literalEntries[lit] += String("R")+"<span style=\"color: green\">"+std::to_string(ruleIndex) + "</span>";
                }
            }
        }

        // Imprimir shifts dependiendo de literales
        for (auto lit : extGrammar.literals) {
            auto actionLink = state->actionStateMap.find(lit);
            if (actionLink != state->actionStateMap.end()) {
                GrammarState* toState = (*actionLink).second;
                if (literalEntries[lit] != "") literalEntries[lit] += ";";
                literalEntries[lit] += String("S") + "<span style=\"color: blue\">" + std::to_string(toState->getState()) + "</span>";
            }
        }

        for (auto lit : extGrammar.literals) {
            content += "<td>" + literalEntries[lit] + "</td>";
        }
        // Imprimir Gotos dependiendo de acciones en variables
        for (auto var : extGrammar.variables) {
            auto actionLink = state->actionStateMap.find(var);
            if (actionLink != state->actionStateMap.end()) {
                GrammarState* toState = (*actionLink).second;
                content += String("<td>") + "<span style=\"color: blue\">" + std::to_string(toState->getState()) + "</span>" + "</td>";
            }
            else
                content += String("<td>") + "" + "</td>";
        }
        
        lrTableHTML += content;
    }

    lrTableHTML += "</tbody></table>";

    #ifdef _DEBUG
    std::cout << lrTableHTML << std::endl;
    #endif

    return lrTableHTML;
}