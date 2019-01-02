all: test

clean:
	rm test

test: sys.cpp file.cpp test.cpp main.cpp
	$(CXX) -std=c++17 -g sys.cpp file.cpp test.cpp main.cpp -o test
