#ifndef _TIMER_H
#define _TIMER_H

#include <chrono>
#include <iostream>

class Timer {
public:
  Timer() {
    this->total = 0.0;
  }

  void start() {
    this->s = std::chrono::high_resolution_clock::now(); 
  }

  void end() {
    this->e = std::chrono::high_resolution_clock::now();
  }
  void printTime() {
    std::chrono::duration<double, std::milli> ms = e - s;
    std::cout << ms.count() << std::endl;
  }
  std::chrono::duration<double, std::milli> getElapsedTime() {
    return e - s;
  }


private:
  std::chrono::high_resolution_clock::time_point s;
  std::chrono::high_resolution_clock::time_point e;
  double total;
};
#endif
