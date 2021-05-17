.PHONY : test, build
test:
	g++ main.cpp grammar_reader.cpp common/string_utils.cpp -o test.out
	./test
build:
	g++ main.cpp grammar_reader.cpp common/string_utils.cpp -o gen.out