# FITS Reader

**FITS Reader** is a small C++ API that allows manipulation of **Primary header** in FITS files.

It provides the users with a *clean* and *easy to use* API with support for *reading*, *writing* and *accessing* header data of FITS file.

**Note :** This API is developed as a part of GSOC Programming Competency Test for Boost.org under the Astronomy Section.

## Why should you choose this API ?

Developing APIs or libraries is a piece of art in itself. The traditional rules of writing applications do not work in the realm of API. 
**An API should be written in such a way that it is:**

 - *Easy to use*-  Programmer should not feel that he is using a third-party library, rather the API should seem like the  part of  the language's vocabulary itself. 
 - *Highly modular* - Low Coupling High Cohesion makes highly adaptable code.
 - *Adaptable to the needs of a User*  - "The aspects of design that have the potential to change in the future should be allowed to change and not fixed "
 - *Robust* - Should handle the errors and corner cases in the best possible way without losing data 
 
 and the list goes on.....

FITS Reader API has been designed keeping in mind the above principles (Please refer to the sections below for examples ) . It features 

 - A policy-based design allowing users to write and control most of the parts of Reader ( Things like adding custom rules, changing the way of parsing a value, adding support for parsing new types of values and so on. (This really comes handy when the FILE has been custom designed according to the needs of a User ).
 - No extra hassle to setup before reading fits files ( Just set a filename, reading mode ) and done that's it!.
 - Easy to use functions for manipulating and writing data back to the file
- Very few dependencies ( For memory-mapped files - boost )
- Conformance to the FITS standard
- Ability to handle both the fixed and variable format based data
- Good performance ( Mostly uses the standard library and boost instead of handwritten implementation for optimal performance )

## How to use this library ?

 - Download the latest [release](https://github.com/gopi487krishna/FITS_primary_header/releases) of FITS Reader library from the Releases Section of the Repository
 - Now unzip the file  that you just downloaded and place it in your projects directory
 - Inside the FITS Reader folder you will find two folders named **include**, **library** . It contains all the header files that will be required to use this API in your project and should be included in the include directory of your project settings
 - To include the folders in your Visual Studio based project
	- Go to Project -> Project Properties->C/C++ ->Additional Include Directories and add the location of the *"include"* folder unzipped before.
	- Similarly go to Linker ->Add Additional Library Directories and add the location of "library" folder unzipped before.
  - As you might have noticed  the only library dependency is a boost library. In fact that library is required for memory-mapped file support. If  boost is already configured for your project then there is no need to add this folder to your additional library directories

To test if everything works correctly write the following code and check whether it compiles or not.

```cpp
int main(){
    
    using namespace fits;
    primary_header<fits_standard_spec> prime_header;
    prime_header.readData("FITS_FULL.txt"); // Present in test files folder

    auto value = prime_header.get< double>("MEANC100");
    if (value) {
    
        std::cout << *value<<std::endl; // value comes out to be 0.3916293
    }
    std::cin.get();
}
```
## Basic Structure of FITS Reader

The FITS Reader API is divided into two classes and one policy of which any class that follows the contract of policy is accepted ( Policy is enforced at compile time not runtime ).

![enter image description here](https://lh3.googleusercontent.com/8tIWdIGxiYJPyYwhMngnhAFVHDe8Yj2Al1aqracH-01uD55Qfe99hzQMpZm4uo2mhx3TQHCefzkFC1ZTk-M2zru3jvUIB_wi8m5BYD_AMHR5Dm-evIRW9Y7CH8orugjObaUV5btm=w800)

## Primary Header
Primary Header class provides an external interface to the programmer for manipulation of primary header of  fits file.

This class provides the following abilities to the user

 - Reading Files
 - Query the value for the keywords parsed by the reader from a FITS  file
 - Insert or Update cards as per the needs of the programmer
 - Update the FITS file with new data

Most of the functions in this class are a wrapper over the fits parser class. This kind of division has been done in order to improve the readability of code.

> This class ensures that the users only see that part of API that they need and nothing more unless they really want to

**Member Functions :**

**readData()** : This function takes a filename and reading mode as argument and selects the appropriate function that parses the data and makes the data available for querying.

*reading_mode* : reading_mode is an enumeration that allows programmers to specify how the data should be read.
As of now reading_mode supports **3** modes namely

![enter image description here](https://lh3.googleusercontent.com/p7gL-phThGf3HGzfoM53F_1-d7nAe6XTnFyEXjcAmP2fAccbtMEdOkwGlMzBwkvfkAfOx9pAs4c1UxMZb43QLRQJ9aqdZJk37VZawTujoW4AQUnJJd0XFHuT8yuXeURltp3btTO3=w700)

**string_buffer** (buffered_mode) : The complete data from the FITS file is loaded onto a buffer from where the parser does the parsing.
Although efficient for files that are a few *kilobytes* in size ( RAM is a lot  faster than secondary storage), loading large files into the memory hurts performance as well as RAM space ( Exhaustion of memory can occur in case of loading very large files ).

**memory_map** : This is the best option when it comes to both large and small files. Memory mapping working by mapping file into a portion of process address space. The programmers can perform operations on files as if it were held in raw memory.
The *kernel* treats the small chunks of files as pages and uses the same paging and efficient caching algorithms that it uses which results in better performance even in case of very large files.

*FITS Reader uses boost::iostreams::memory_mapped library ( Which is one of the most popular and fastest in town ) which provides a thin wrapper around the underlying memmap calls making the source code compatible across a variety of platforms*

**stream** :  This mode uses the old school fstream IO library for reading the file. It is average in terms of performance and memory consumption but as the reads are sequential gives a significant advantage. memory_mapped mode should be used if the file size is very large whereas use stream_based mode if the file size is a few megabytes in size.

 - **Return Value** : A Boolean value indicating whether the file could be successfully read or not.
```cpp
// By default stream mode is selected
prime_header.readData(filename,fits::reading_mode::stream);
```
The functions mentioned below are a wrapper over fits_parser functions that are exposed to the user. Hence in order to get detailed information about these functions kindly visit the fits_parser section
 
**get&lt;Type&gt;()** This function takes a keyword as argument and returns the value of that keyword back to the user ( if present ).
 - **Return Value** : A *std::optional&lt;Type&gt;* value .
```cpp
 auto value = prime_header.get< double>("MEANC100");
```

**insert()** : This function takes a position ( where to insert ) , keyword, value (optional), comment( optional ) as argument and is used to insert/ update a card in FITS Reader. 

>  This function does not write data back to the file but rather schedules it for writing . Actual writing is solely done by writeToFile() method

 - **Return Value** : A boolean indicating whether the changes could be made or not
```cpp
 prime_header.insert(4, "BSCALE", 32.0,"Something");
// or
 prime_header.insert(4, "NO_VAL");   
// or
 prime_header.insert(4, "VAL_NO_C",32.0);   
```

**writeToFile()** :  Write to File function takes a file name as argument and writes the complete updated FITS data ( this includes scheduled data from previous insert calls)  to the file. If file does not exists then a new file is created .

 - **Return Value** : A boolean indicating whether the write operation was successful or not

## Fits Parser

> The details of this class are meant for people who wish to develop
> custom parsing policies for the Fits Reader API.

Fits Parser provides a collection of functions for reading as well as writing the primary header of a FITS file. It is used by the primary_header class  that provides an external interface to the users for working with FITS files. 

**Parsing Policy** : One good feature of the FITS reader API is its ability to take custom parsing policy as a parameter allowing the programmers to redefine the way of how the files can be parsed. 
This results in high flexibility as with custom parsing policy you can set your own rules for parsing keywords, create specialized keywords or introduce a new type ( Best example is that of custom date type). Also with custom parsing policy, you can change the way a type is parsed ( like setting precision for floating-point numbers ). 
Due to its custom parsing policy FITS reader becomes future proof to a very large extent as newer changes in the standard can easily be accommodated.
Also as the parsing policy is enforced at Compile Time rather than Runtime there is 0 performance overhead. But there is a small compile-time overhead which is a smaller price to pay for as compared to flexiblity.
To get further details about how the custom parsing policy can  be implemented kindly visit the Parsing Policy Section

**Member Functions** : ( Primary Header has a wrapper around some of these functions exposing to  the User )

**get&lt;Type&gt;()** : This function takes the keyword as its argument and based on the value type provided as the template parameter returns the value back to the user in the respective type wrapped around a **std::optional**
The value type needs to be passed as a template parameter to the function because internally the value data is stored inside a variant from which the value needs to be casted out.
*Note: Just to make things clear this does not have any significant runtime performance overhead.*

 - **Return Value** : a *std::optional &lt;Type&gt;* that contains the value for the keyword. If the value was not found or the value could not be parsed to the provided type a *std::nullopt* is returned. Hence make sure to check before dereferencing.

**Note:**  For keywords that are multivalued in nature you can fetch the value by providing a *std::vector&lt;Type&gt;* as template parameter. ( See code below for example )

**insert()** : Insert member function is used to create a new card in FITS Reader. Calling the insert member function with appropriate arguments causes the reader to update its internal structure with new {keyword,value} and schedules the card to be written to file.

> This member function only schedules the card to be written but does not actually write data back to the file. To write the data please refer writeToFile function.

**writeToFile()** : It takes a filename as argument and writes the updated data into the file . This type of design has been used in order to prevent frequent writes that cause a performance lag. It writes the original file as it is but also accommodates the changes that you have made. This includes updating the value of a keyword, adding a new keyword-value ( card ) etc.

**parseOnStringBuffer(file)** : As the name itself suggests parseOnStringBuffer parses the primary header of a fits file by copying the entire FITS data into memory and then calling the **parse** function.  For smaller files this method is performance efficient but larger files can cause a lag and memory exhaustion.

 - **Return Value:** A boolean that indicates if all the keywords and values could be successfully parsed.

**parseOnStream(file)** : This function opens a stream for reading from the **file** and fetches the data in chunks of 80 bytes and provides it to the parse function that does the parsing. The 80 bytes are fetched until the parse reports the **END** keyword or the file reading is complete ( which means the header is corrupted ).

 - **Return Value:** A boolean that indicates if all the keywords and values could be successfully parsed.

**parseOnMappedFile(file)** : This is one of the most efficient methods of parsing a FITS file as it uses the boost::iostreams::mapped_file_source class for reading the data from the file. The internal logic is same as compared to 
**parseOnStringBuffer()** as the file can be accessed like raw memory for accessing data.
  - **Return Value:** A boolean that indicates if all the keywords and values could be successfully parsed.

**parseCard()** : parseCard takes a raw_card of 80 bytes as its argument and splits the card into keyword and value ( The keyword and values are parsed according to the  parsing policy ) .

**Basic Steps of How Parsing Takes Place :**

 - Parse the required keywords and check if they are present in required order. After parsing each card add the keyword and value into the **header_data** ( An unordered multimap ) for storage.
 - After the required cards have been parsed and ensured that they confirm to the standard parse all the other cards one by one . Parsing of keyword results in  
	

	 - Keyword name
	 - Keyword class
	 - Multivalued or not

 - Value parsing results in actual parsing of the values to the required type and returning it.
 - Based on the keyword class , its multivalued nature and type of value the keyword-value pair is inserted/ updated  into the  **header_data**. and its offset/position is added into **offset map**
 - After the parsing is complete, all the keywords along with their values are available for editing or querying.
 

> All the steps shown above occur in the given order but how they should be performed can be controlled by defining a custom parsing policy

## Parsing Policy

The parsing policy at its essence specifies how the card needs to be parsed and stored, Its basic features include

1.  To provide functionality for parsing the keyword
2.  To provide functionality for parsing the values.
3.  To provide functionality for checking the keyword's nature
4.  To provide support for serializing a type back to string

Basic Structure of a Custom Parsing Policy class must contain the following 
```cpp
class custom_parsing_policy{
  public:
  typedef std::variant</*Types That are Supported */> value_type;
  enum keyword_class {

			reserved,
			user_defined,
			blank,
			no_value,
			none

		};
   struct keyword_info { // Depends on whether you need it!

			std::string name;
			keyword_class key_class;
			bool multivalued{ false };

		};
   keyword_info getKeyword(const std::string& raw_card); // Prefer a string view
   bool isRequiredKeywordInOrder(const std::string& keyword, int index);// Index tells the current card position of keyword in file
   bool isMultivalued(const std::string& keyword);
   value_type parseValue(const std::string& raw_card, const std::string& keyword, keyword_class key_class);
   std::string cnvToString(/*Make overloads for all types*/);
  } 
```
**getKeyword()** : Returns the keyword from the raw card along with its class and mulitvalued status

**isRequiredKeywordInOrder()** : Using the index determines whether the given keyword is in order or not. The interface for this function may change after sometime.

 **isMultivalued()** : Returns true if a keyword is multivalued ( As of now only HISTORY and COMMENT keywords are multivalued in nature )
 
 **parseValue()** : Parses the value from raw card based on keyword and keyword class and returns the value or a std::monostate ( Limitation with the variant will be removed in subsequent versions )
 
 **cnvToString()** : cnvToString() basically converts a value of specific type to string. You need to provide either overloads or template it for all the types that are supported by your parsing policy class.


**Tips on Writing a custom parsing policy class :**

1.  Value parsing: In the case of user-defined keywords the type of value is not known beforehand and hence, we need to try parsing the value with each type until it gets successful ( Things change if you are going for lazy evaluation mode ) On the other hand Reserved keywords have their value's type known before and therefore should be parsed directly into that specific type instead of hit and trial method. This certainly improves performance. Hence try having two different parsing functions for both these classes of keywords.
2.  Parsing value is not simple at all. If your policy supports fixed and variable parsing formats then there are several corner cases that need to be handled appropriately. Handling these cases does consume some cycles but some cycles can be compensated by using high-performance libraries for parsing the values. Using a library like **boost::spirit::qi** or other third party libraries can significantly improve performance.
3.  Value Type: A value_type should have the ability to store all the values for Types that have been defined by the programmer and can be parsed

	1.  The first thing that comes to mind when thinking about the concept of value_type is type-safe unions or std::variant. Although highly efficient at run-time they consume a significant amount of memory depending upon the largest type for each value
	2.  Another technique is to use std::any that can perform type erasure on the value and store it. Its memory efficient but suffers from a performance bottleneck.
4. Using the default **fits_standard_spec** parsing policy is enough at most times since it is conformant to the FITS Standards. But if your organization has some specific needs or the **fits_standard_spec** has got outdated then only create your own policy class. And make sure to make it a static library. It significantly reduces the compile time and in some cases provides some more optimization as well.
5. If you are writing your own policy class then make sure to use the standard library for in your code. Do not try to reinvent the wheel again and again as the standard library makes your code much clearer and efficient.

<!--stackedit_data:
eyJoaXN0b3J5IjpbMTkyMjcwOTQxMCwxNDU3NTg0ODM1LDIwNT
c3MzM1MzEsMTQwNTAyODY0OCwxODc5MTQyMTA5LDEzNDQ0MDI4
NjEsLTEwNjg3MTE0MjMsMTYyMDU0NzExLDEyNjM2Mzk3MDYsND
kzMjUyOTgyLDEwNzYzODcyODIsLTE0ODM4MzMwNTUsMjAxMDgx
NTU2NiwtMTUyOTM0NTQ5NSwtMzg4NjcwNDI2LDEwNzQ0MzA0Mz
EsMTE5NzQ2MjYyMywtMTY5NjA3ODkwNywtNjE1NDAxNDI1LC0x
OTQwMjA4MjI3XX0=
-->