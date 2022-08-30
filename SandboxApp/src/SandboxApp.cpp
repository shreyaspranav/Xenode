#include <iostream>
#include <Xenode.h>

int main()
{

  float a, b;
  std::cout << "Enter two numbers: ";
  std::cin >> a >> b;
  std::cout << "Answer: " << Add(a, b) << std::endl;

  return 0;
}
