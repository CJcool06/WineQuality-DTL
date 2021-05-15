winequality: winequality-out
	g++ *.o -o winequality.bin

winequality-out:
	g++ -std=c++11 -c -w -Iheaders/ cpp/*.cpp main.cpp