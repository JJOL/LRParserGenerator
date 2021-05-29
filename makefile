.PHONY : test, build
test:
	g++ main.cpp grammar_reader.cpp common/string_utils.cpp common/grammar.cpp -o test.out
	./test
build:
	g++ main.cpp GrammarReader.cpp ParsingGenerator.cpp common/string_utils.cpp common/grammar.cpp -o gen.out