#pragma once

#include <vector>
#include <string>
#include <algorithm>


namespace fits {
	class fits_standard_spec {

		enum keyword_class {

			reserved,
			user_defined,
			blank,
			none

		};


		//const std::unordered_map<std::string, int> required_keywords_order{ {"SIMPLE",0},{"BITPIX",1},{"NAXIS",2} };
		const std::vector<std::string>required_keywords = { "SIMPLE","BITPIX","NAXIS" };
		const std::vector<std::string>reserved_keywords = { "HISTORY","COMMENT" };

	public:

		fits_standard_spec() {
		}
		std::pair<std::string, keyword_class> getKeyword(const std::string& raw_card);
		std::string getValue() { return ""; }
		bool isReservedKeyword(const std::string& keyword) { return std::find(reserved_keywords.begin(), reserved_keywords.end(), keyword) != reserved_keywords.end(); }
		bool isRequiredKeyword(const std::string& keyword) { return std::find(required_keywords.begin(), required_keywords.end(), keyword) != required_keywords.end(); }
		bool isRequiredKeywordInOrder(const std::string& keyword, int index);
	};


	// Implementation ( Could be defined in terms of a compiled binary as well



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
			std::copy_if(keyword_start_pos, keyword_end_pos+1, std::back_inserter(keyword), [](char key_char) {

				return isupper(key_char) || isdigit(key_char) || key_char == '-' || key_char == '_';

				});

			// If the keyword satisfies all criteria's then the length will be same
			auto distance = std::distance(keyword_start_pos,keyword_end_pos);
			if (distance == keyword.length()-1)
			{

				if (raw_card[8] == '=' && raw_card[9] == ' ') {

					result_keyword = std::move(keyword);
					key_class = keyword_class::user_defined;

				}
				else {

					if (isReservedKeyword(keyword)) { key_class = keyword_class::reserved; result_keyword = std::move(keyword); }
					else {

						key_class = keyword_class::none;
					}
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



