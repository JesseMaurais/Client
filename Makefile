all: test

clean:
	rm test

test:
	$(CXX) -std=c++17 sys.cpp file.cpp test.cpp main.cpp -o test
