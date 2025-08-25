#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

int main () {
  std::cout << "Calculate the average of n, inputed digits" << std::endl;
    std::string input;
    std::cout << "Enter Text (type: 'quit' to stop and calculate your average): " << std::endl;

    std::vector<double> numbers;
    while(std::getline(std::cin, input) && input != "quit"){
      size_t pos;
     try {
        // Attempt to convert to double
        double double_val = std::stod(input, &pos);
        // If the entire string was consumed
        if (pos == input.length()) {
            std::cout << "Input is a float (or double): " << double_val << std::endl;
            numbers.push_back(double_val);
        } else {
            std::cout << "Input is not a valid number (or contains extra characters): " << input << std::endl;
        }
        } catch (const std::invalid_argument& e) {
         std::cout << "Input is not a valid number: " << input << std::endl;
        } catch (const std::out_of_range& e) {
         std::cout << "Input is a number but out of range for double: " << input << std::endl;
        }
      std::cout << "You Entered: " << input << std::endl; 
      std::cout << "_: ";
    }
    std::cout << "Done" << std::endl;
    size_t numberLength = numbers.size();
    double average; 
    for(int i = 0; i < numberLength; i++){
      average += numbers[i];
    }
    std::cout << "Average: " << average/numberLength << std::endl;

  return 0;
}
