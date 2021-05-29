/**
 * Author: Juan Jose Olivera
 * Fecha: 29 de Mayo del 2021
 * Programa: Generador de Tablas de Parseo LR(0) para gramaticas sin epsilon.
 * Descripcion: Herramienta que genera en HTML tablas de Parseo LR(0) dado un conjunto de reglas gramaticales.
*/
#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>
#include <list>
#include <set>
#include "common/string_utils.h"
#include "common/grammars.h"
#include "core/GrammarReader.h"
#include "core/ParsingGenerator.h"

using String = std::string;

class StatePrinter
{
private:
    GrammarState *state;
public:
    StatePrinter(GrammarState* _state) : state(_state) {}
    std::string getKernelString() const
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
    std::string getClosureString(GrammarState *from) const
    {
        if (from == nullptr || from->getState() != state->from->getState()) return "";


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

    std::string getGotoString(GrammarState* fromState, const std::string& symb)
    {
        return std::string("goto(") + std::to_string(fromState->getState()) + ","  + symb + ")";
    }
};

int main(int argc, char **argv)
{
    String grammarName = "gr";
    if (argc > 1) {
        grammarName = String(argv[1]);
    }


    // 1. Leer Reglas de Gramatica
    GrammarReader reader;
    Grammar grammar = reader.readRulesFromConsole();
    Grammar extGrammar = grammar.extended();
    extGrammar.generateFollows();

    std::cout << "FIRSTS(X):" << std::endl;
    for (auto kv : extGrammar.varFirstsMap) {
        std::cout << "FIRST(" << kv.first << ") = { ";
        for (auto lit : kv.second) {
            std::cout << lit << ", ";
        }
        std::cout << "}" << std::endl;
    }

    std::cout << "FOLLOWS(X):" << std::endl;
    for (auto kv : extGrammar.varFollowsMap) {
        std::cout << "FOLLOW(" << kv.first << ") = { ";
        for (auto lit : kv.second) {
            std::cout << lit << ", ";
        }
        std::cout << "}" << std::endl;
    }

    // Print Read Rules
    std::vector<String>::iterator s_it;
    for (s_it = extGrammar.variables.begin(); s_it < extGrammar.variables.end(); s_it++) {
        std::cout << "Rules for '" << *s_it << "':" << std::endl;

        rules_map_t::iterator rs_it = extGrammar.variableRulesMap.find(*s_it);
        std::vector<GrammarRule> rules = rs_it->second;

        // Print each Rule
        std::vector<GrammarRule>::iterator r_ptr;
        for (r_ptr = rules.begin(); r_ptr < rules.end(); r_ptr++) {
            GrammarRule r = *r_ptr;
            std::cout << "Head: " << r.head << std::endl;
            std::cout << "Body: " << std::endl;
            std::vector<String>::iterator b_itr;
            for (b_itr = r.body.begin(); b_itr < r.body.end(); b_itr++) {
                std::cout << "- " << *b_itr << std::endl;
            }
        }
    }

    // 2. Generar Diferentes Estados
    ParsingGenerator generator;
    GrammarState* root = generator.generate(extGrammar);

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

    // 3. Generar Tabla
    
    
    String tableOut;

    String lrClosureTableHTML;
    std::cout << "LR CLOSURE TABLE" << std::endl;
    {
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
        tableOut = tableHead;
        
        String content;
        std::list<GrammarState*> toVisit;
        std::set<GrammarState*> visited;
        toVisit.push_back(root);

        StatePrinter sprinter(root);
        content = String("<tr><td>")
            + "</td><td>"
            + sprinter.getKernelString() + "</td><td class=\"td-state\">"
            + std::to_string(root->getState()) + "</td><td>"
            + sprinter.getClosureString(nullptr) 
            + "</td></tr>";
        tableOut += content;

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
                tableOut += content;
            }
        }

        tableOut += tableFoot;
    }
    lrClosureTableHTML = tableOut;
    std::cout << lrClosureTableHTML << std::endl;


    String lrTableHTML;
    std::cout << "LR TABLE" << std::endl;
    {
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

        tableOut = tableHead + "<tbody>";

        String content;
        for (auto stateEntry : stateMap) {
            int stateIntex = stateEntry.first;
            GrammarState* state = stateEntry.second;

            content = "<tr><td class=\"td-state\">"+std::to_string(stateEntry.first)+"</td>";

            std::map<String, String> literalEntries;

            // Imprimir Reduces
            std::vector<RuleProgress> completedProgressRules =  state->getCompletedRules();
            for (auto pr : completedProgressRules) {
                if (pr.rule.head == extGrammar.headVariable) {
                    // Poner acc en <td>$</td>
                    literalEntries["$"] = "acc";
                }
                else {
                    int ruleIndex = pr.rule.id;
                    auto followLiterals = extGrammar.varFollowsMap[pr.rule.head];
                    for (auto lit : followLiterals) {
                        // Poner R[ruleIndex] en <td>lit</td>
                        literalEntries[lit] = "R"+std::to_string(ruleIndex);
                    }
                }
            }

            // Imprimir shifts dependiendo de literales
            for (auto lit : extGrammar.literals) {
                auto actionLink = state->actionStateMap.find(lit);
                if (actionLink != state->actionStateMap.end()) {
                    GrammarState* toState = (*actionLink).second;
                    literalEntries[lit] = "S"+std::to_string(toState->getState());
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
                    content += String("<td>") + std::to_string(toState->getState()) + "</td>";
                }
                else
                    content += String("<td>") + "" + "</td>";
            }
            
            tableOut += content;
        }

        tableOut += "</tbody></table>";
    }
    lrTableHTML = tableOut;
    std::cout << lrTableHTML << std::endl;


    // 4. Imprimir HTML
    std::ofstream outHtmlFile;
    outHtmlFile.open(grammarName+"_out.html");

    outHtmlFile << "<!DOCTYPE html><html><head>";
    outHtmlFile << "<style>table, th, td { border: 1px solid black; } .td-state { color: blue; } .kpr-completed { color: green; }</style>";
    outHtmlFile << "<title>" << grammarName << "</title></head><body>";

    outHtmlFile << lrClosureTableHTML << "<br>";
    outHtmlFile << lrTableHTML;

    outHtmlFile << "</body></html>";


    outHtmlFile.close();


    return 0;
}