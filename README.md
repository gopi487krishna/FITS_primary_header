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

*readData()* : This function takes a filename and reading mode as argument and selects the appropriate function that parses the data and makes it available for querying.
**reading_mode** : reading_mode is an enumeration that allows programmers to specify how the data should be read.
As of now reading_mode supports **3** modes namely






<!--stackedit_data:
eyJoaXN0b3J5IjpbLTE1MjkzNDU0OTUsLTM4ODY3MDQyNiwxMD
c0NDMwNDMxLDExOTc0NjI2MjMsLTE2OTYwNzg5MDcsLTYxNTQw
MTQyNSwtMTk0MDIwODIyNywtMTc3NjIzNDE5NiwtNzAwOTg4OD
k1LDI4NzU2ODk4MywxMzM3ODY1MDY5LDk3OTIwMzUxOSwyMTE3
OTA0NDY3LDEwNTc4NjM2ODIsMTE0MTcxMDcwNCwzMDgzODQzOD
ksMjExOTk0MDE2NywxODEzNTA5NDY2LDExMTQxMTk3MTBdfQ==

-->