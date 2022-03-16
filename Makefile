# Makefile for EECS 281 Lab 7
# Identifier: 2C4A3C53CD5AD45A7BEA3AE5356A3B1622C9D04B

hash: hash.cpp
	g++ -std=c++1z -g hash.cpp -o hash

clean:
	rm -f hash

submit:
	tar -czvf lab7.tar.gz hashtable.h
