#pragma once
#include<string>
#include<fstream>

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


	/**
	 * @brief	A card is used to store the information present inside a card in a FITS file header
	 * @details	A card takes a keyword,value,comment and position/offset in file and is used to store a perticular card from FITS file
	 * @author	Gopi Krishna Menon
	
	*/
	struct CARD {

		std::string keyword;
		std::string value;
		std::string comment;



	};


	/**
	 * @brief	The main class for parsing the primary header of a FITS file
	 * @details	Primary Header is responsible for reading the data from the data file and parse the data and provide the ability for the 
	 *          user to query a keywords value or write/update a  CARD back into the file
	 * @author	Gopi Krishna Menon

	*/
	class primary_header {

		std::string file_data_buffer;
		bool memory_mapped{ false };


	public:
		void readData();
		const int& operator [](const std::string& keyword);
		void insert();



	};



}