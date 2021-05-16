.PHONY : test, build
test:
	g++ main.cpp -o test.out
	./test
build:
	g++ main.cpp -o gen.out