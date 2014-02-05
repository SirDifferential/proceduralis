#ifndef _TOOLBOX_HPP_
#define _TOOLBOX_HPP_

#include <string>
#include <vector>
#ifdef WIN32
#include <windows.h>
#endif

#include <SFML/Graphics.hpp>
#include <iostream>
#include "cl.hpp"

class Toolbox
{
private:
public:
    Toolbox();
    ~Toolbox();
    
    int timesRandomized;
    
    int giveRandomInt();
    int giveRandomInt(int min, int max);
    float giveRandomFloat();
    
    bool fileExists(std::string path);
    
    std::string searchFile(int range, std::string path);
    
    template<class T>
    typename T::const_iterator findFromVector(T const& c, typename T::value_type const& v);

    template<class T>
    bool isInVector(T const& c, typename T::value_type const& v);

    std::string appendToStringNTimes(std::string in, int numOfTimes);
    std::string charToString(char c);
    std::string intToString(int i);
    std::string resolutionToString(int sizeX, int sizeY);
    int boolToInt(bool b);
    std::vector<int> doubleVectorToIntVector(std::vector<double> in);
    std::string stripString(std::string source, int quantity, std::string mode);
    std::string relativePathToAbsolute(std::string path);
    std::string reverseString(std::string toReverse);
    std::string combineStringAndInt(std::string in1, int in2);
    std::string combineStringAndInts(std::string in1, int in2, int in3);
    std::string combineStringAndString(std::string s1, std::string s2);
    std::string combineIntAndString(int i, std::string s);
    std::string removeCharsFromString(std::string s, int count);
    std::string combineStringAndFloat(std::string s, float f);
    std::string vec2ToString(sf::Vector2i i);

    double vectorDistance(sf::Vector2f a, sf::Vector2f b);
    double linearInterpolate(double x0, double x1, double alpha);
    double radiansToDegrees(double rads);
    double degreesToRadians(double degs);

    long getCurrentTime();

    std::vector<std::string> splitTextByString(char separator, std::string text);
    std::string splitTextInLines(size_t line_length, std::string text);

    int** giveIntArray2D(int size_x, int size_y);
    float** giveFloatArray2D(int sizeX, int sizeY);
    float*** giveFloatArray3D(int sizeX, int sizeY, int sizeZ);
    void clearFloatArray2D(float** arr, int sizeX, int sizeY);
    void deleteFloatArray2D(float** arr, int sizeX);
    void clearFloatArray3D(float*** arr, int sizeX, int sizeY, int sizeZ);
    void deleteFloatArray3D(float*** arr, int sizeX, int sizeY);
    void deleteIntArray2D(int** arr, int size_x);


	template <class T>
	void print_vector(T vec)
    {
        std::cout << vec.x << "x" << vec.y << std::endl;
    }
	std::string sfml_key_to_string(sf::Event e);

    const char* opencl_error_string(cl_int errorcode);

    sf::Color int_to_sfmlcolor(int i);

    bool colorValidRange(sf::Color target, sf::Color input, sf::Color tolerance);
};

#endif
