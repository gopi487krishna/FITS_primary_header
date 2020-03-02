#pragma once
#include<string>
#include<fstream>
#include <sstream>
#include <unordered_map>
#include <iterator>
#include <iostream>
#include <filesystem>

// For testing
#include <variant>


namespace fits {

	constexpr int size_of_card = 80;

	template<typename parsing_policy>
	class fits_parser : parsing_policy {

	public:
		bool parseOnStream(const std::string& filename);
		bool parseOnMappedFile(/*boost mapped file here*/) { return true; }


	};


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
		if (!memory_mapped) { return parser_instance.parseOnStream(filename); }

		return parser_instance.parseOnMappedFile();// Use boost mapped file interface

	}


	template<class parsing_policy>
	bool fits_parser<parsing_policy>::parseOnStream(const std::string& filename) {


		// Fetch raw card is a lambda that fetches a raw_card from the iterator
		auto fetch_raw_card = [](std::istreambuf_iterator<char>& iter) {
			std::string raw_card;   //Raw card containing keyword value and comments
			raw_card.reserve(size_of_card); // raw_card is always 80 hence reserve early to avoid reallocations
			for (int count = 0; count < size_of_card; count++) raw_card.push_back(*iter++);
			return raw_card;

		};


		// Open file for reading
		std::ifstream input_file_stream(filename);
		if (input_file_stream.is_open()) {
			std::istreambuf_iterator<char> inp_iter(input_file_stream); // Iterators for traversing the entire stream
			std::istreambuf_iterator<char> end_of_file;

			// Checks if atleast three cards are present as three required keywords should be present ( so 3 cards )
			if (auto total_card_count = std::filesystem::file_size(filename) / 80; total_card_count >= 2)
			{

				// Check if the first three cards contain the required keywords in order
				for (int current_card_count = 0; current_card_count < 3; current_card_count++)
				{

					std::string raw_card = fetch_raw_card(inp_iter); //Raw card containing keyword value and comment
					auto [keyword, key_class] = this->getKeyword(raw_card);

					if (!this->isRequiredKeywordInOrder(keyword, current_card_count)) return false;

					auto value = this->getValue(raw_card, keyword, key_class);

					
					// Put the keyword and value and offset into collection
				}

				// Parse the Cards with user defined keywords
				while (inp_iter != end_of_file) {

					std::string raw_card = fetch_raw_card(inp_iter); // Fetch a raw record
					if (auto [keyword, key_class] = this->getKeyword(raw_card); key_class != parsing_policy::keyword_class::none) {
						if (key_class == parsing_policy::keyword_class::reserved || key_class == parsing_policy::keyword_class::user_defined)
						{
							typename parsing_policy::value_type val = this->getValue(raw_card, keyword, key_class);
						//Push the keyword with no value into the collection

						}

						if (key_class == parsing_policy::keyword_class::no_value) {

								if (keyword == "END")
								{
									break;
								}
							//Push the keyword with no value into the collection

						}
					}
					else { return false; }
				}
			}
			return false; // Records were not greater than or equal to 3 ( Fails to satify basic fits requirements )
		}
		return false; //Unable to open the input file
	}
}