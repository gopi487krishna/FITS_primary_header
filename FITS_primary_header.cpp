// FITS_primary_header.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "fits.h"
#include "fits_standard_spec.h"
#include <chrono>

#define LOCAL

#ifdef LOCAL

std::string filename("F:\\Projects\\GSOC\\Boost_Primary_Fits_Header\\FITS_FILES\\FITS_SATELLITE.txt");


#else
std::string filename("FITS_SATELLITE.txt");

#endif // LOCAL



void test_memory_mapped() 
{
    fits::primary_header<fits::fits_standard_spec> prime_header;
    auto start = std::chrono::high_resolution_clock::now();
   auto result= prime_header.readData(filename,fits::reading_mode::memory_map);
   
    prime_header.insert("HISTORY", "This is the first record", -1);
    prime_header.insert("KEANUSAN", long long(487),-1,"KATTA");
    prime_header.insert("COMMENT", "I love this baby", 5);
    prime_header.insert("GOPI", long long(487), -1, "THE SBAONE");


    auto m=prime_header.writeToFile("MASTER.txt");

    auto value=prime_header.get<long long>("GOPI");
    auto value2 = prime_header.get<long long>("KEANUSAN");

    std::cout << *value << std::endl;
    std::cout << *value2 << std::endl;

    auto his_vec = prime_header.get<std::vector<std::string>>("HISTORY");
    
    for (auto element : *his_vec) {
    
        std::cout << element << std::endl;
    }

    /*std::cout.precision(20);
    auto values = prime_header.get<std::vector<std::string>>("HISTORY");
    if (values) {
        
        for(auto& value: *values)
        std::cout << value<<"\n"; 
    }
    auto single = prime_header.get<double>("MEANC100");
    if (single) { std::cout << *single << "\n"; }*/
    auto end = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "Done: " << time.count() << "\n";

}
void test_stream_based()
{
    fits::primary_header<fits::fits_standard_spec> prime_header;
    auto start = std::chrono::high_resolution_clock::now();
    prime_header.readData(filename, fits::reading_mode::stream);

    /*std::cout.precision(20);
    auto values = prime_header.get<std::vector<std::string>>("HISTORY");
    if (values) {

        for(auto& value: *values)
        std::cout << value<<"\n";
    }
    auto single = prime_header.get<double>("MEANC100");
    if (single) { std::cout << *single << "\n"; }*/
    auto end = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "Done: " << time.count() << "\n";

}
void test_string_buffer()
{
    fits::primary_header<fits::fits_standard_spec> prime_header;
    auto start = std::chrono::high_resolution_clock::now();
    prime_header.readData(filename, fits::reading_mode::string_buffer);
    /*std::cout.precision(20);
    auto values = prime_header.get<std::vector<std::string>>("HISTORY");
    if (values) {

        for(auto& value: *values)
        std::cout << value<<"\n";
    }
    auto single = prime_header.get<double>("MEANC100");
    if (single) { std::cout << *single << "\n"; }*/
    auto end = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "Done: " << time.count()<<"\n";

}



int main(){
    
    //test_raw_fetch_header_from_buffer();
    test_memory_mapped();
    /*test_stream_based();
    test_string_buffer();*/

    std::cin.get();
}

