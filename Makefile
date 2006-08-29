

all:
	g++ map_unittest.cpp -o map_unittest -lstdc++ -g3 -I. -I/usr/include/
	g++ main.cpp -o test -lstdc++ -O3 -I.
