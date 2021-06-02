.PHONY : test, build
test:
	g++ main.cpp grammar_reader.cpp common/string_utils.cpp common/grammar.cpp -o test.out
	./test
build:
	g++ main.cpp core/GrammarReader.cpp core/ParsingGenerator.cpp core/HTMLRenderer.cpp common/string_utils.cpp common/grammar.cpp -o gen.out