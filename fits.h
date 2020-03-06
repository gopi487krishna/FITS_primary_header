#pragma once
#include<string>
#include<fstream>
#include <sstream>
#include <unordered_map>
#include <iterator>
#include <iostream>
#include <filesystem>
#include <optional>
#include <boost/iostreams/device/mapped_file.hpp>

// For testing
#include <variant>



namespace fits {

	constexpr int size_of_card = 80;


	enum reading_mode {

		string_buffer,
		memory_map,
		stream

	};

	template<class parsing_policy>
	class fits_parser : parsing_policy {
		std::unordered_multimap<std::string, typename parsing_policy::value_type> header_data;

	public:
		bool parseOnStringBuffer(const std::string& filename);
		bool parseOnStream(const std::string& filename);
		bool parseOnMappedFile(const std::string& filename);
		bool parseCard(const std::string& raw_card, int& card_count, bool& found_end, int header_size_hint);
		template<class conversion_type>
		std::optional<conversion_type> get(const std::string& keyword);


	};


	template<class parsing_policy>
	class primary_header {


		reading_mode read_mode;
		fits_parser<parsing_policy> parser_instance;

	public:

		bool readData(const std::string& filename, reading_mode r_mode = reading_mode::stream);
		template<class return_type>
		std::optional<return_type> get(const std::string& keyword);
		void insert() {}
	};




	// Implementations
	//-------------------------------------------------------------------------------------------------------------------------------------------------------------------

	//-------------------  FOR TAG DISPATCHING -------------------------------------------------------------------//

	template<typename T>
	struct is_vector : public std::false_type {};
	template<typename T, typename A>
	struct is_vector<std::vector<T, A>> :public std::true_type {};


	//-------------------  FOR TAG DISPATCHING -------------------------------------------------------------------//



	template<class parsing_policy>
	template<class conversion_type>
	std::optional<conversion_type> fits_parser<parsing_policy>::get(const std::string& keyword) {

		// Check if the conversion type is a vector ( Then the code for returning range of values )
		if constexpr (is_vector<conversion_type>::value) {

			// Get all values  for a perticular key
			auto values_range = header_data.equal_range(keyword);
			// if no elements in unordered_multimap with that perticular keyword 
			if (values_range.first != header_data.end()) {
				// Create a vector that will hold all the values from the range
				conversion_type values;
				for (auto values_iter = values_range.first; values_iter != values_range.second; values_iter++)
				{
					typename parsing_policy::value_type value = values_iter->second;
					// Convert the value from variant type to its actual type
					if (auto pval = std::get_if<conversion_type::value_type>(&value)) {
						values.push_back(*pval);
					}
					else {
						return {}; // Failed to convert to the required type
					}
				}
				return values; // Successfully converted to required type

			}
			return {}; // No values associated with that keyword
		}
		else {

			auto element_pos = header_data.find(keyword);
			if (element_pos != header_data.end())
			{
				typename parsing_policy::value_type value = element_pos->second;
				if (auto pval = std::get_if<conversion_type>(&value)) {

					return *pval;
				}
				else {
					return {};
				}
			}
			return {};
		}
	}


	template<class parsing_policy>
	template<class return_type>
	std::optional<return_type> primary_header<parsing_policy>::get(const std::string& keyword) {
		return parser_instance.get<return_type>(keyword);
	}


	template<class parsing_policy>
	bool fits_parser<parsing_policy>::parseOnStringBuffer(const std::string& filename) {

		// Fetch raw card is a lambda that fetches a raw_card from the iterator
		auto fetch_raw_card = [](std::string::iterator& iter) {

			// This can be further optimized by using string_view	
			auto cur_pos = iter;
			iter = iter + 80;
			return std::move(std::string(cur_pos, iter));
		};


		// Open file for reading
		std::ifstream input_file_stream(filename);
		if (input_file_stream.is_open()) {


			// Get the file size in bytes
			auto file_size = std::filesystem::file_size(filename);


			std::string string_buffer;
			string_buffer.reserve(file_size);
			string_buffer.assign((std::istreambuf_iterator<char>(input_file_stream)), (std::istreambuf_iterator<char>()));

			std::string::iterator  cur_buf = string_buffer.begin(); // Iterators for traversing the entire stream
			std::string::iterator  buf_end = string_buffer.end();

			// Checks if atleast three cards are present as three required keywords should be present ( so 3 cards )

			if (auto total_cards = file_size / 80; total_cards >= 2)
			{
				auto header_size_hint = total_cards > 999 ? (total_cards * 30) / 100 : total_cards;

				bool header_end = false;
				int card_count = 0;

				while (cur_buf != buf_end && !header_end) {


					auto raw_card = fetch_raw_card(cur_buf);

					auto parse_successful = parseCard(std::move(raw_card), card_count, header_end, header_size_hint);
					if (!parse_successful) {

						return false;
					}

				}

				// This means header was completely  read or file only consisted of header
				return true;

			}
			// Header corrupted or not present
		}
		return false; // Unable to open file
	}

	template<class parsing_policy>
	bool fits_parser<parsing_policy>::parseCard(const std::string& raw_card, int& card_count, bool& found_end, int header_size_hint) {

		header_data.reserve(header_size_hint);

		// found_end will only be set to true when the END keyword is found
		found_end = false;

		// Special Parsing for Required Keywords to ensure file is in correct format
		if (card_count < 2) {

			//None of the required keywords are multivalued as per the standard hence ignore third parameter
			auto [keyword, keyword_class, _] = this->getKeyword(raw_card);

			if (!keyword.empty()) {

				// No need to check for keyword class here because it is automatically check by the check below
				if (this->isRequiredKeywordInOrder(keyword, card_count)) {
					auto value = this->parseValue(raw_card, keyword, keyword_class);
					if (auto key_iter = header_data.find(keyword); key_iter == header_data.end()) {
						header_data.insert({ std::move(keyword),value });
					}
					else {
						key_iter->second = value;

					}
					++card_count; return true; // Successfully parsed the card and inserted into the collection

				}
				return false;  // Required keyword is not in order

			}
			return false; // Not able to parse the keyword

		}
		// Not a required_keyword
		if (auto [keyword, key_class, multivalued] = this->getKeyword(raw_card); key_class != parsing_policy::keyword_class::none) {
			if (key_class == parsing_policy::keyword_class::reserved || key_class == parsing_policy::keyword_class::user_defined)
			{
				typename parsing_policy::value_type value = this->parseValue(raw_card, keyword, key_class);

				if (auto k_iter = header_data.find(keyword); k_iter != header_data.end() && !multivalued) {

					k_iter->second = value;

				}
				else {

					header_data.insert({ std::move(keyword),value });
				}
			}
			else if (key_class == parsing_policy::keyword_class::no_value) {

				if (keyword == "END")
				{
					found_end = true;
					++card_count;
					return true;
				}
				header_data.insert({ std::move(keyword),std::monostate{} });



			}
			++card_count;
			return true;
		}
		return false;
	}

	template<class parsing_policy>
	bool primary_header<parsing_policy>::readData(const std::string& filename, reading_mode r_mode) {

		read_mode = r_mode;
		bool parse_successful = false;
		switch (read_mode) {


		case reading_mode::stream:
			parse_successful = parser_instance.parseOnStream(filename);
			break;
		case reading_mode::memory_map:
			parse_successful = parser_instance.parseOnMappedFile(filename);
			break;
		case reading_mode::string_buffer:
			parse_successful = parser_instance.parseOnStringBuffer(filename);
			break;

		}
		return parse_successful;
	}


	template<class parsing_policy>
	bool fits_parser<parsing_policy>::parseOnMappedFile(const std::string& filename) {

		// Fetch raw card is a lambda that fetches a raw_card from the iterator
		auto fetch_raw_card = [](auto& iter) {

			auto cur_pos = iter;
			iter = iter + 80;
			return std::move(std::string(cur_pos, iter));
		};


		using boost::iostreams::mapped_file_source;
		using boost::iostreams::stream;

		mapped_file_source memmap(filename);
		if (memmap.is_open()) {


			// Get the file size in bytes
			auto file_size = std::filesystem::file_size(filename);



			auto  cur_buf = memmap.begin(); // Iterators for traversing the entire stream
			auto  buf_end = memmap.end();

			// Checks if atleast three cards are present as three required keywords should be present ( so 3 cards )

			if (auto total_cards = file_size / 80; total_cards >= 2)
			{
				auto header_size_hint = total_cards > 999 ? (total_cards * 30) / 100 : total_cards;

				bool header_end = false;
				int card_count = 0;

				while (cur_buf != buf_end && !header_end) {


					auto raw_card = fetch_raw_card(cur_buf);

					auto parse_successful = parseCard(std::move(raw_card), card_count, header_end, header_size_hint);
					if (!parse_successful) {

						return false;
					}

				}

				// This means header was completely  read or file only consisted of header
				return true;

			}
			// Header corrupted or not present
		}
		return false; // Unable to open file






	}

	template<class parsing_policy>
	bool fits_parser<parsing_policy>::parseOnStream(const std::string& filename) {


		// Fetch raw card is a lambda that fetches a raw_card from the iterator
		auto fetch_raw_card = [](std::istreambuf_iterator<char>& iter) {
			std::string raw_card;   //Raw card containing keyword value and comments
			raw_card.reserve(size_of_card); // raw_card is always 80 hence reserve early to avoid reallocations
			for (int count = 0; count < size_of_card; count++) raw_card.push_back(*iter++);
			return std::move(raw_card);

		};


		// Open file for reading
		std::ifstream input_file_stream(filename);
		if (input_file_stream.is_open()) {
			std::istreambuf_iterator<char> inp_iter(input_file_stream); // Iterators for traversing the entire stream
			std::istreambuf_iterator<char> end_of_file;

			// Checks if atleast three cards are present as three required keywords should be present ( so 3 cards )
			if (auto total_cards = std::filesystem::file_size(filename) / 80; total_cards >= 2)
			{

				auto header_size_hint = total_cards > 999 ? (total_cards * 30) / 100 : total_cards;

				bool header_end = false;
				int card_count = 0;

				while (inp_iter != end_of_file && !header_end) {


					auto raw_card = fetch_raw_card(inp_iter);

					auto parse_successful = parseCard(std::move(raw_card), card_count, header_end, header_size_hint);
					if (!parse_successful) {

						return false;
					}

				}

				// This means header was completely  read or file only consisted of header
				return true;

			}
			// Header corrupted or not present
		}
		return false; // Unable to open file
	}
}