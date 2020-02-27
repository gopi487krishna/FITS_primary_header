#pragma once
#include<string>
#include<fstream>
#include <sstream>
#include <unordered_map>
#include <iterator>
#include <iostream>
/**
 * @file fits.h
 * @author Gopi Krishna Menon
 * @details Contains the classes to parse FITS files
*/


/**
 * @brief This namespace contains all the classes required to parse the FITS primary header
 * @author Gopi Krishna Menon
*/
namespace fits {


	class fits_standard_spec {

		std::unordered_map<std::string, int> required_keywords_order;

	public:

		fits_standard_spec() {

			required_keywords_order.reserve(3);

			required_keywords_order["SIMPLE"] = 0;


		}
		std::string getKeyword() { return ""; }
		std::string getValue() { return ""; }
		bool isrequiredKeyword() { return true; }



	};


	template<typename parsing_policy>
	class fits_parser : parsing_policy {

	public:
		bool parseOnStream( std::ifstream& input_file_stream);
		bool parseOnMappedFile(/*boost mapped file here*/) { return true; }


	};



	/**
	 * @brief	The main class for parsing the primary header of a FITS file
	 * @details	Primary Header is responsible for reading the data from the data file and parse the data and provide the ability for the
	 *          user to query a keywords value or write/update a  CARD back into the file
	 * @author	Gopi Krishna Menon

	*/
	template<class parsing_policy>
	class primary_header {

		bool memory_mapped{ false };
		fits_parser<parsing_policy> parser_instance;

	public:

		bool readData(const std::string& filename, bool memmap = false);
		const int& operator [](const std::string& keyword) {}
		void insert() {}
	};




	// Implementations
	//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

	template<class parsing_policy>
	bool primary_header<parsing_policy>::readData(const std::string& filename, bool requires_memmap) {

		memory_mapped = requires_memmap;
		if (!memory_mapped) {

			std::ifstream input_file_stream(filename);

			if (input_file_stream.is_open()) {

				
				return parser_instance.parseOnStream(input_file_stream);



			}
			// Not able to open input file 
			return false;
		}

		
		//boost mapped calling
		// Code for memory mapped calling

		return parser_instance.parseOnMappedFile();

	}


	template<class parsing_policy>
	bool fits_parser<parsing_policy>::parseOnStream(std::ifstream& input_file_stream) {

		std::istreambuf_iterator<char> inp_iter(input_file_stream);
		std::istreambuf_iterator<char> end_of_file;

		while (inp_iter != end_of_file) {
		
			std::string raw_card;
			
			// raw_card is always 80 hence reserve early to avoid reallocations
			raw_card.reserve(80);

			//Put character by character into the string
			for (int count = 0; count < 80; count++) {
			
				raw_card.push_back(*inp_iter++);
			
			}

			// Do rest of things here

			//std::cout << raw_card << std::endl;
		
		}
		
		return true;
	
	
	}


}