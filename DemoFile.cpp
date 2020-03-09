// FITS_primary_header.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "fits.h"
#include "fits_standard_spec.h"
#include <chrono>


int main(){
    
    using namespace fits;
    primary_header<fits_standard_spec> prime_header;
    prime_header.readData("FITS_FULL.txt"); // Present in test files folder

    auto value = prime_header.get< double>("MEANC100");
    if (value) {
    
        std::cout << *value<<std::endl; // value comes out to be 0.3916293
    }

    prime_header.insert("KEANUSAN", (double)487.0,"John Wick", 5);
    prime_header.writeToFile("MASTER.txt");

    std::cin.get();
}

