#pragma once
#include<string>
#include <optional>
#include "fits_parser.h"
namespace fits {

	enum reading_mode {

		string_buffer,
		memory_map,
		stream

	};


	template<class parsing_policy>
	class primary_header {


		reading_mode read_mode;
		fits_parser<parsing_policy> parser_instance;

	public:

		bool readData(const std::string& filename, reading_mode r_mode = reading_mode::stream);
		template<class return_type>
		std::optional<return_type> get(const std::string& keyword);
		template<typename Type>
		bool insert(const std::string& keyword, const Type& value, const std::string& comment);
		bool writeToFile(const std::string& filename);
	};


	// Implementations
	//-------------------------------------------------------------------------------------------------------------------------------------------------------------------
	template<class parsing_policy>
	template<typename Type>
	bool primary_header<parsing_policy>::insert(const std::string& keyword, const Type& value, const std::string& comment) {
		return parser_instance.insert(keyword, value, comment);
		
	}
	
	template<class parsing_policy>
	bool primary_header<parsing_policy>::writeToFile(const std::string& filename) {
		return parser_instance.writeToFile(filename);
	}



	template<class parsing_policy>
	template<class return_type>
	std::optional<return_type> primary_header<parsing_policy>::get(const std::string& keyword) {
		return parser_instance. template get<return_type>(keyword);
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
}