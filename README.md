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
 - A .csv file meant to store any offline tick data is excluded. The code will refer to 'src/data/' using an absolute path function. Place your .csv file in this folder. Then rename it to match the code or change the code to match the name of the file.
 - You will also need to update the Qt main.pro project file with the paths for libcurl, (i.e, include paths, libs).
