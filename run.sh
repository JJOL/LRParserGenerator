# Autor: Juan Jose Olivera
# Modulo: Script Run de Generador de Tabla de Parseo LR(0)
# Compila el codigo de ser necesario y corre el programa

g++ main.cpp core/GrammarReader.cpp core/ParsingGenerator.cpp common/string_utils.cpp common/grammar.cpp -o gen.out

if [ -z "$1" ]
then
    echo "./run.sh <archivo-de-gramatica> [nombre-de-gramatica]"
    echo "Ej: ./run.sh test/arith.txt Arithmetic"
else
    cat $1 | ./gen.out $2
fi
