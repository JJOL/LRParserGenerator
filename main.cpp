#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include "common/string_utils.h"
#include "common/grammars.h"

using namespace std;

Grammar read_console_rules();

int main(int argc, char **argv)
{
    // 1. Leer Reglas de Gramatica
    Grammar grammar = read_console_rules();

    vector<string>::iterator s_it;
    for (s_it = grammar.symbols.begin(); s_it < grammar.symbols.end(); s_it++) {
        cout << "Rules for '" << *s_it << "':" << endl;

        rules_map_t::iterator rs_it = grammar.rules_map.find(*s_it);
        vector<GrammarRule> rules = rs_it->second;

        // Print Rules
        vector<GrammarRule>::iterator r_ptr;
        for (r_ptr = rules.begin(); r_ptr < rules.end(); r_ptr++) {
            GrammarRule r = *r_ptr;
            cout << "Head: " << r.head << endl;
            cout << "Body: " << endl;
            vector<string>::iterator b_itr;
            for (b_itr = r.body.begin(); b_itr < r.body.end(); b_itr++) {
                cout << "- " << *b_itr << endl;
            }
        }
    }

    // 2. Generar Diferentes Estados

    // 3. Generar Tabla

    // 4. Imprimir HTML

    return 0;
}