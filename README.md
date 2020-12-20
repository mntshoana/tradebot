# Tradebot
This is project is aimed at customizing an application, using c++. Hopefully, this can grow into an automated trading bot.

This project was created using c++ for the sole purpose of :
    - using REST api 
    - using the Qt library (mainly for cross platform gui).
    - more experience in building an application from scratch, or  specifically, from an idea. 
Feel free to use this source code for learning c++.
## Requirements
REST APIs in this source are invoked using http requests, thus, this code depends on libcurl. You should have this installed on your computer before attempting to compile this code.

Compilation depends on Qt and was tested on MacOS using Qt 5.15.2, however, I will continue using Qt6 in future. This project requires qmake to compile. 

## Usage
Compile using

````
git clone ...
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
