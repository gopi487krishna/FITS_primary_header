# FITS Reader

**FITS Reader** is a small C++ API that allows manipulation of **Primary HDU** in FITS files.

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
Primary Header class provides an external interface to the programmer for manipulation of primary HDU of  fits file.

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

**Return Value** : A Boolean value indicating whether the file could be successfully read or not.
```cpp
// By default stream mode is selected
prime_header.readData(filename,fits::reading_mode::stream);
```
The functions mentioned below are a wrapper over fits_parser functions that are exposed to the user. Hence in order to get detailed information about these functions kindly visit the fits_parser section
 
**get&lt;Type&gt;()** This function takes a keyword as argument and returns the value of that keyword back to the user ( if present ).
**Return Value** : A *std::optional&lt;Type&gt;* value .
```cpp
 auto value = prime_header.get< double>("MEANC100");
```

**insert()** : This function takes a position ( where to insert ) , keyword, value (optional), comment( optional ) as argument and is used to insert/ update a card in FITS Reader. 

>  This function does not write data back to the file but rather schedules it for writing . Actual writing is solely done by writeToFile() method

**Return Value** : A boolean indicating whether the changes could be made or not
```cpp
 prime_header.insert(4, "BSCALE", 32.0,"Something");
// or
 prime_header.insert(4, "NO_VAL");   
// or
 prime_header.insert(4, "VAL_NO_C",32.0);   
```

**writeToFile()** :  Write to File function takes a file name as argument and writes the complete updated FITS data ( this includes scheduled data from previous insert calls)  to the file. If file does not exists then a new file is created .
**Return Value** : A boolean indicating whether the write operation was successful or not

## Fits Parser
The details of this class are of importance only if you intend to develop custom parsing policies for the FITS Reader API.
*Note : Although the dependencies are kept to a minimum, kindly refrain yourself from using this class directly.*
**fits_parser** class as the name itself suggests, is responsible for parsing the con











**get&lt;Type&gt;()** : This function takes the keyword as its argument and based on the value type provided as the template parameter returns the value back to the user in the respective type wrapped around a **std::optional**
The value type needs to be passed as a template parameter to the function because internally the value data is stored inside a variant from which the value needs to be casted out.
*Note: Just to make things clear this does not have any significant runtime performance overhead.*

```cpp
// After reading the file
auto value=prime_header.get<double>("MEANC100");
if(value){std::cout<<*value<<"\n";}
```
**Return Value** : a *std::optional &lt;Type&gt;* that contains the value for the keyword. If the value was not found or the value could not be parsed to the provided type a *std::nullopt* is returned. Hence make sure to check before dereferencing.

**Note:**  For keywords that are multivalued in nature you can fetch the value by providing a *std::vector&lt;Type&gt;* as template parameter. ( See code below for example )
```cpp
auto values=prime_header.get<std::vector<std::string>>("HISTORY");
if( values.has_value()){
for(auto& value:values){ std::cout<< value<<"\n";}
}
```

<!--stackedit_data:
eyJoaXN0b3J5IjpbMzIyNzAyMSwxNjIwNTQ3MTEsMTI2MzYzOT
cwNiw0OTMyNTI5ODIsMTA3NjM4NzI4MiwtMTQ4MzgzMzA1NSwy
MDEwODE1NTY2LC0xNTI5MzQ1NDk1LC0zODg2NzA0MjYsMTA3ND
QzMDQzMSwxMTk3NDYyNjIzLC0xNjk2MDc4OTA3LC02MTU0MDE0
MjUsLTE5NDAyMDgyMjcsLTE3NzYyMzQxOTYsLTcwMDk4ODg5NS
wyODc1Njg5ODMsMTMzNzg2NTA2OSw5NzkyMDM1MTksMjExNzkw
NDQ2N119
-->