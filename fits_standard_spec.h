#pragma once

#include<complex>
#include <vector>
#include <string>
#include <algorithm>
#include <variant>
#include <boost/spirit/include/qi.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace fits {



	class fits_standard_spec {
		const std::vector<std::string>required_keywords = { "SIMPLE","BITPIX","NAXIS" };
		const std::vector<std::pair<std::string, char>>reserved_keywords = {
			{"HISTORY",'N'},
			{"COMMENT",'N' },
			{"BUNIT",'C'},
			{"BSCALE",'F'},
			{"BZERO",'F'},
			{"BLANK",'I'},
			{"DATAMAX",'F'},
			{"DATAMIN",'F'},
			{"DATE",'C'},
			{"DATE-OBS",'C'},
			{"ORIGIN",'C'},
			{"TELESCOP",'C'},
			{"INSTRUME",'C'},
			{"OBSERVER",'C'},
			{"OBJECT",'C'},
			{"AUTHOR",'C'},
			{"REFERENC",'C'},
			{"EQUINOX",'F'},
			{"BLOCKED",'L'},


		};

	public:
		typedef std::variant<std::monostate, std::string, long long, double, char, std::complex<long long>, std::complex<double>> value_type;
		enum keyword_class {

			reserved,
			user_defined,
			blank,
			no_value,
			none

		};
		enum value_format {

			fixed,
			variable

		};
		fits_standard_spec() {
		}

		// Keyword Related Helpers
		std::pair<std::string, keyword_class> getKeyword(const std::string& raw_card);
		bool isReservedKeyword(const std::string& keyword);
		bool isRequiredKeyword(const std::string& keyword);
		bool isRequiredKeywordInOrder(const std::string& keyword, int index);

		// Value Related Helpers
		auto getReservedKeyPos(const std::string& keyword);
		value_type getValue(const std::string& raw_card, const std::string& keyword, keyword_class key_class);
		value_type getValueForReserved(const std::string& raw_card, const std::string& keyword);

		// Type parse related Helpers
		char parseLogical(const std::string& value_part);
		std::string parseCharacterString(const std::string& value_part);
		std::pair<long long, bool> parseInteger(const std::string& value_part);
		std::pair<double, bool>parseFloatingPoint(const std::string& value_part);

	};



	// Implementation ( Could be defined in terms of a compiled binary as well )
	auto fits_standard_spec::getReservedKeyPos(const std::string& keyword) {
	
		return std::find_if(reserved_keywords.begin(), reserved_keywords.end(), [&keyword](std::pair<std::string, char> keyw) {return keyword == keyw.first; });
	
	
	}
	
	bool fits_standard_spec::isReservedKeyword(const std::string& keyword) {
		return std::find_if(reserved_keywords.begin(), reserved_keywords.end(), [&keyword](std::pair<std::string, char> keyw) {return keyword == keyw.first; }) != reserved_keywords.end();
	}

	bool fits_standard_spec::isRequiredKeyword(const std::string& keyword) {
		return std::find(required_keywords.begin(), required_keywords.end(), keyword) != required_keywords.end();
	}



	std::pair<long long, bool> fits_standard_spec::parseInteger(const std::string& value_part) {

		// A lot of optimizations can be done here
		auto current_position = value_part.begin();

		auto comment_position = std::find(value_part.begin(), value_part.end(), '/');

		long long resultant_integer;
		if (comment_position != value_part.end()) {

			if (*(comment_position - 1) == ' ') { comment_position--; }
			if (boost::spirit::qi::parse(current_position, comment_position, boost::spirit::qi::long_long, resultant_integer) && current_position == comment_position) {

				return { resultant_integer,true };
			}


		}
		else {

			if (boost::spirit::qi::parse(current_position, value_part.end(), boost::spirit::qi::long_long, resultant_integer) && current_position == value_part.end()) {

				return { resultant_integer,true };
			}


		}

		return { resultant_integer,false };



	}
	std::pair<double, bool> fits_standard_spec::parseFloatingPoint(const std::string& value_part) {

		// A lot of optimizations can be done here
		auto current_position = value_part.begin();

		auto comment_position = std::find(value_part.begin(), value_part.end(), '/');

		double resultant_fp;
		if (comment_position != value_part.end()) {

			if (*(comment_position - 1) == ' ') { comment_position--; }
			if (boost::spirit::qi::parse(current_position, comment_position, boost::spirit::qi::double_, resultant_fp) && current_position == comment_position) {

				return { resultant_fp,true };
			}


		}
		else {

			if (boost::spirit::qi::parse(current_position, value_part.end(), boost::spirit::qi::double_, resultant_fp) && current_position == value_part.end()) {

				return { resultant_fp,true };
			}


		}

		return { resultant_fp,false };



	}
	std::string fits_standard_spec::parseCharacterString(const std::string& value_part) {

		std::string value_string;
		value_string.reserve(70);

		if (value_part[0] == '\'') {


			if (value_part.length() >= 21 && value_part[19] == '\'' && value_part[20] != '\'') {

				// Means it followed fixed format ( This is actually not needed )
				std::copy_n(value_part.begin() + 1, 7, std::back_inserter(value_string));

			}
			else {

				// Copying it the old fashioned way
				std::string temp_string;
				auto current_pos = value_part.begin() + 1;
				auto ending_pos = value_part.end();
				while (current_pos != ending_pos) {

					if ((current_pos != ending_pos - 1) && (*current_pos == '\'') && (*(current_pos + 1)) == '\'') {

						temp_string.push_back(*current_pos);
						current_pos += 2;
						continue;

					}
					else if (*current_pos == '\'') {

						value_string = std::move(temp_string);
						break;

					}

					temp_string.push_back(*current_pos++);


				}
			}


		}

		return value_string;



	}
	fits_standard_spec::value_type fits_standard_spec::getValueForReserved(const std::string& raw_card, const std::string& keyword) {

		// Need to optimize this code further
		if (keyword == "COMMENT" | keyword == "HISTORY") {
		
			std::string::const_iterator starting_pos = raw_card.begin();

			if (raw_card[8] == '=') {
				std::advance(starting_pos, 9);
			}
			else {
				std::advance(starting_pos, 8);
			}	

			return std::string(starting_pos, raw_card.end());
		}

		auto reserved_key_type = getReservedKeyPos(keyword)->second;
		std::string r_c(raw_card.begin() + 10, raw_card.end());
		boost::algorithm::trim(r_c);

		if (reserved_key_type == 'L'){
		
			if (auto x = parseLogical(r_c); x != 'N') { return x; }

		
		}
		else if (reserved_key_type == 'F') {
		
			if (auto x = parseFloatingPoint(r_c); x.second) { return x.first; }
		
		}
		else if (reserved_key_type == 'I') {
		
			if (auto x = parseInteger(r_c); x.second) { return x.first; }
		}
		else if (reserved_key_type == 'C') {
		
			if (auto x = parseCharacterString(r_c); !x.empty()) { return x; }
		}

		return std::monostate{};

	}
	char fits_standard_spec::parseLogical(const std::string& value_part) {

		if (value_part[0] == 'T' || value_part[0] == 'F') { return value_part[0]; }

		// Indicates no value
		return 'N';

	}
	fits_standard_spec::value_type fits_standard_spec::getValue(const std::string& raw_card, const std::string& keyword, keyword_class key_class) {



		// Check for  Logical 
		if (key_class == keyword_class::reserved) {

			getValueForReserved(raw_card, keyword);


		}


		if (key_class == keyword_class::user_defined) {


			std::string r_c(raw_card.begin() + 10, raw_card.end());
			boost::algorithm::trim(r_c);


			if (auto x = parseLogical(r_c); x != 'N') { return x; }

			if (auto x = parseCharacterString(r_c); !x.empty()) { return x; }

			if (auto x = parseInteger(r_c); x.second) { return x.first; }

			if (auto x = parseFloatingPoint(r_c); x.second) { return x.first; }


		}

		return std::monostate{};
	}
	bool fits_standard_spec::isRequiredKeywordInOrder(const std::string& keyword, int index) {

		// This function is basically a hack for getting performance and will work with fits parser only.

		if (index > required_keywords.size()) return false;

		if (required_keywords[index] == keyword) {
			// Element in its spec pos
			return true;
		}

		// Not in spec pos
		return false;
	}
	std::pair<std::string, fits_standard_spec::keyword_class> fits_standard_spec::getKeyword(const std::string& raw_card) {


		std::string result_keyword; // Will store the keyword
		keyword_class key_class;

		auto keyword_start_pos = raw_card.begin();
		auto keyword_end_pos = keyword_start_pos + 7;

		while (*(keyword_end_pos) == ' ' && keyword_end_pos != raw_card.begin()) { keyword_end_pos--; } // Neglect all the trailing spaces

		if (keyword_end_pos == keyword_start_pos && *keyword_end_pos == ' ') {

			key_class = keyword_class::blank;

		}
		else {


			std::string keyword;
			std::copy_if(keyword_start_pos, keyword_end_pos + 1, std::back_inserter(keyword), [](char key_char) {

				return isupper(key_char) || isdigit(key_char) || key_char == '-' || key_char == '_';

				});

			// If the keyword satisfies all criteria's then the length will be same
			auto distance = std::distance(keyword_start_pos, keyword_end_pos);
			if (distance == keyword.length() - 1)
			{

				if (isReservedKeyword(keyword)) {

					// Checking whether the format is valid is done at the value end
					key_class = keyword_class::reserved;
					result_keyword = std::move(keyword);

				}
				else if (raw_card[8] == '=' && raw_card[9] == ' ') {

					result_keyword = std::move(keyword);
					key_class = keyword_class::user_defined;

				}
				else {


					result_keyword = std::move(keyword);
					key_class = keyword_class::no_value;

				}
			}
			else {

				key_class = keyword_class::none;

			}
		}

		// Return the keyword along with its class
		return { result_keyword,key_class };

	}

}



