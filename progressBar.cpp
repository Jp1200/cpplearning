#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <string>

void process_info(int process_time){
    std::this_thread::sleep_for(std::chrono::milliseconds(process_time));
}
void print_progress_bar(double progress){
  int bar_length = 50;
  int filled_length = static_cast<int>(progress * bar_length);
  std::string barChar = "#";
  std::cout << "[";

  for(int i = 0; i < filled_length; i++){
    std::cout << barChar;
  }
  for (int i = filled_length; i < bar_length; i++) {
    std::cout << " ";
  }
  std::cout << "] " << static_cast<int>(progress * 100) << "%\r";
  std::cout.flush();
}


int main(){
  int num_files = 100;
  //int batch_size = 5;
  int mock_time_milli = 30;
  for (int i = 0; i <= num_files; ++i){
    print_progress_bar(static_cast<double>(i) / 100.0);
    process_info(mock_time_milli);
  }
  std::cout << std::endl;
  std::cout << "\nDone" << std::endl;
  return 0;
  
}
