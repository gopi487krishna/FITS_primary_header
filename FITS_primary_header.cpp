// FITS_primary_header.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "fits.h"
#include <chrono>
class DummyParsingPolicy {




};

//
//void test_raw_fetch_header_from_buffer() {
//
//    fits::primary_header<DummyParsingPolicy> primeheader;
//    primeheader.readData("FITS_SATELLITE.txt");
//
//    primeheader.printrawCards(801);
//}

void test_something() 
{

    auto start = std::chrono::high_resolution_clock::now();
    fits::primary_header<DummyParsingPolicy> prime_header;
    prime_header.readData("F:\\Projects\\GSOC\\Boost_Primary_Fits_Header\\FITS_FILES\\FITS_SATELLITE.txt");
  
    auto end = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "Done: " << time.count();

}
int main(){
    
   
    //test_raw_fetch_header_from_buffer();
    test_something();
    std::cin.get();
}

