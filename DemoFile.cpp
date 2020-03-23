// FITS_primary_header.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <chrono>
#include "fits.h"
#include "fits_standard_spec.h"

typedef char LogicalType;

int main(){
    
    using namespace fits;
   
    primary_header<fits_standard_spec> prime_header;

    auto read_successful=prime_header.readData("FITS_FULL.txt"); // Or some other filename 

    if (read_successful) {
        
        // Reads the value of meanC100
        auto mean_c100_value = prime_header.get<double>("MEANC100");
        
        if (mean_c100_value.has_value()) {

            std::cout << *mean_c100_value <<'\n'; // value comes out to be 0.3916293 ( Adjust cout precision if the value is not precise )
        }

        // History - May contain multiple values hence we try to get the value in vector
        auto history_value = prime_header.get<std::vector<std::string>>("HISTORY");
        if (history_value.has_value()) {
        
            std::cout << "HISTORY VALUE : \n";
            for (auto& value : *history_value) {
                std::cout << value<<'\n';
            }        
        }


        // BSCALE value will be replaced with 32.0 if already present in FITS file
        prime_header.insert("BSCALE", 32.0, "Something");

        // Similar to BSCALE
        prime_header.insert("SIMPLE", 'F', "Does not conform to fits standard");

        // This card will be inserted at the end before END keywords as according to the FITS standard except for the required keywords
        // The position does not matter
        prime_header.insert("GSOC", "BOOST", "This fits reader and writer is for boost");


        // Internal data structure was updated automatically
        auto gsoc_value = prime_header.get<std::string>("GSOC");
        if (gsoc_value.has_value()) {
        
            std::cout << *gsoc_value;
        }
        
        // Write the scheduled changes back to File
        prime_header.writeToFile("MASTER.txt");    
    }
    std::cin.get();
}
