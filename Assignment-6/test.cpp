#include <iostream>

void callee() {
  std::cout << "This is the callee function.\n";
  std::cout << "Called from function: " << __FUNCTION__ << ", line: " << __LINE__ << "\n";
}

void caller() {
  std::cout << "This is the caller function.\n";
  callee();
}

int main() {
  caller();
  return 0;
}
