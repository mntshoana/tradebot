# Tradebot
This project is aimed at having an understanding of trading through code. The project was created using c++ mainly for more experience in building an application from scratch. Ideas can be inspiring but they are useless if are not willed into existance. This will soon grow into an automated trading bot.

Feel free to use this source code however you please. #Learn c++.

## Requirements
The project mostly uses c++. To be able to use this code, you will need to have a Qt compiler as well as libcurl installed in your system. The make.pro file is configured to use c++17, this is not a requirement. This code was only tested on MacOS (Catalina and Big Sur). I will continue using Qt version 6. 

Another requirement you must have already installed on your computer is libcurl. http requests are invoked using this library and it must be present. 

Note: Python will also be used for financial machine learning in future, it is now also a required in order to compile.

## Usage
Compile using

````
git git@github.com:mntshoana/tradebot.git 
cd tradebot
mkdir build && cd build
````
then
````
qmake ../src
make
````
or
````
qmake ../src -spec macx-xcode
````

Note: A few files have been excluded from the repository.
 - A header file containing the login credentials has been excluded.
 - The .csv file which stores any offline tick data is also not included. The source code will refer to the location using an absolute path defined using #define CSV_FILE_PATH. Note that changing this path will still not save any ticks, not without having a .csv file that exists in that location.
 - You will also need to upadte the main.pro file with the location for your libcurl, (i.e, include paths, libs).
