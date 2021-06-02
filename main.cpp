/**
 * Author: Juan Jose Olivera
 * Fecha: 29 de Mayo del 2021
 * Programa: Generador de Tablas de Parseo LR(0) para gramaticas sin epsilon.
 * Descripcion: Herramienta que genera en HTML tablas de Parseo LR(0) dado un conjunto de reglas gramaticales.
*/
#define _DEBUG
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
#include "core/HTMLRenderer.h"

using String = std::string;

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

    // Imprimir First y Follow para debuggear
    #ifdef _DEBUG
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

    #endif

    // 2. Generar Diferentes Estados
    ParsingGenerator generator;
    Parser* parser = generator.generate(extGrammar);

    // 3. Generar Tabla
    HTMLRenderer renderer;

    std::ofstream outHtmlFile;
    outHtmlFile.open(grammarName+"_out.html");

    outHtmlFile << "<!DOCTYPE html><html><head>";
    outHtmlFile << "<style>table, th, td { border: 1px solid black; } .td-state { color: blue; } .kpr-completed { color: green; }</style>";
    outHtmlFile << "<title>" << grammarName << "</title></head><body>";
    outHtmlFile << renderer.getLRClosureTable(parser) << "<br>";
    outHtmlFile << renderer.getLRParsingTable(parser);
    outHtmlFile << "</body></html>";

    outHtmlFile.close();

    return 0;
}