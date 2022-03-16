// Testing file provided for EECS 281 Lab 7
#include "hashtable.h"
#include <iostream>
#include <string>
int main() {
  // VERY BASIC EXAMPLE (will likely not compile with default/empty hashtable.h
  // file)
  HashTable<std::string, int> midterm;
  std::cout << midterm.insert("sam", 50) << " ";
  std::cout << midterm.insert("fee", 100) << " ";
  std::cout << midterm.insert("milo", 95) << " ";
  std::cout << midterm.insert("gabe", 88) << " \n";
  std::cout << midterm["sam"] << " ";
  std::cout << midterm["fee"] << " ";
  std::cout << midterm["milo"] << " ";
  std::cout << midterm["gabe"] << " \n";
  std::cout << midterm.erase("sam") << " ";
  std::cout << midterm["sam"] << "\n";
  // ADD MORE TESTS OF YOUR OWN

  struct BadHasher {
    size_t operator()(const std::string &s) {
      return s.empty() ? 0 : s[0] - 'A';
    }
  };

  HashTable<std::string, int, BadHasher> bad_table;
  for (int i = 0; i < 10; i++) {
    bad_table.insert(std::to_string(i), 20);
  }
  bad_table.insert("51", 50);
  bad_table["513"] = 5;
}