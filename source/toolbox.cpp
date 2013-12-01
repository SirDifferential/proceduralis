#include "toolbox.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <list>
#include <chrono>
#include <iostream>

// Constructs our toolbox. Sets the random seed
Toolbox::Toolbox()
{    fprintf(stderr, "+Toolbox: A toolbox bought from the hardware store for 4.99 euros...\n");
    timesRandomized = 0;
    srand(time(0));
}

Toolbox::~Toolbox()
{
    fprintf(stderr, "A crappy toolbox being sold on ebay...\n");
}

// Returns a random integer
int Toolbox::giveRandomInt()
{
    timesRandomized++;
    return rand();
}

// Returns a random integer in range (min, max)
int Toolbox::giveRandomInt(int min, int max)
{
    return ((rand()%(max-min))+min);
}

// Checks if the specified file exists
bool Toolbox::fileExists(std::string path)
{
    bool result = false;
    std::ifstream fileChecker(path.c_str());
    if (fileChecker)
        result = true;
    else
        result = false;
    fileChecker.close();
    return result;
}

// Searches for a file upwards from the current dir
std::string Toolbox::searchFile(int range, std::string path)
{
    std::string beginPart = "";
    std::string entirePath = path;
    if (fileExists(entirePath))
        return entirePath;
    for (int i = 1; i < range; i++)
    {
        beginPart = appendToStringNTimes("../", i);
        entirePath = beginPart;
        entirePath.append(path);
        if (fileExists(entirePath))
            return entirePath;
    }
    return "";
}

/**
* Generic find for any type of vector
*/
template<typename T>
typename T::const_iterator Toolbox::findFromVector(T const& c, typename T::value_type const& v) {
    return std::find(c.begin(), c.end(), v);
}

/**
* Returns true if element is in a vector
*/
template<typename T>
bool Toolbox::isInVector(T const& c, typename T::value_type const& v) {
    typename std::vector<T>::iterator it = std::find(c.begin(), c.end(), v);
    if (it != c.end())
    {
        return true;
    }
    return false;
}

std::string Toolbox::appendToStringNTimes(std::string in, int numOfTimes)
{
    std::string out;
    for (int i = 0; i < numOfTimes; i++)
    {
        out.append(in);
    }
    return out;
}

// Converts a char into a string
std::string Toolbox::charToString(char c)
{
    std::stringstream stream;
    std::string out;
    stream << c;
    stream >> out;
    
    return out;
}

std::string Toolbox::intToString(int i)
{
    std::stringstream stream;
    std::string out;
    stream << i;
    stream >> out;

    return out;
}

// Converts std::vector<double> into std::vector<int>
// Used by the objLoader
std::vector<int> Toolbox::doubleVectorToIntVector(std::vector<double> in)
{
    std::vector<int> output;
    for (int i = 0; i < in.size(); i++)
    {
        output.push_back(int(in.at(i)));
    }
    return output;
}

// Returns the resolution of x*y in a string format
std::string Toolbox::resolutionToString(int sizeX, int sizeY)
{
    std::string output;
    std::stringstream container;
    container << sizeX;
    container << "x";
    container << sizeY;
    output = container.str();
    return output;
}

std::string Toolbox::stripString(std::string source, int quantity, std::string mode)
{
    std::string output;
    if (mode.compare("fromEnd") == 0)
    {
        for (int i = 0; i < source.length()-quantity; i++)
        {
            output.append(charToString(source.at(i)));
        }
    }
    else if (mode.compare("fromBegin") == 0)
    {
        for (int i = source.length()-quantity; i < source.length(); i++)
        {
            output.append(charToString(source.at(i)));
        }
    }
    else
    {
        fprintf(stderr, "!Toolbox: Wrong string stripping mode given, assuming fromEnd\n");
        for (int i = 0; i < source.length() -quantity; i++)
        {
            output.append(charToString(source.at(i)));
        }
    }
    return output;
}

int Toolbox::boolToInt(bool b)
{
    if (b != 0)
        return true;
    else
        return false;
}

// Reverses a string. For example: banana becomes ananab
std::string Toolbox::reverseString(std::string toReverse)
{
    std::string out = "";
    for (int i = toReverse.length()-1; i >= 0; i--)
    {
        out.append(charToString(toReverse[i]));
    }
    return out;
}

// Returns a new string consisting of in1 and in2
std::string Toolbox::combineStringAndInt(std::string in1, int in2)
{
    std::string out = "";
    std::stringstream buffer;
    buffer << in1;
    buffer << in2;
    out = buffer.str();
    return out;
}
// Returns a new string containing the int followed by the string
std::string Toolbox::combineIntAndString(int i, std::string s)
{
    std::string out = "";
    std::stringstream buffer;
    buffer << i;
    buffer << s;
    out = buffer.str();
    return out;
}

// Returns a new string consisting of in1 and in2 and in3
std::string Toolbox::combineStringAndInts(std::string in1, int in2, int in3)
{
    std::string out = "";
    std::stringstream buffer;
    buffer << in1;
    buffer << in2;
    buffer << ", ";
    buffer << in3;
    out = buffer.str();
    return out;
}

std::string Toolbox::combineStringAndString(std::string s1, std::string s2)
{
    std::stringstream out;
    out << s1;
    out << s2;
    return out.str();
}

/**
* Removes a number of characters from a string, from the end
*/
std::string Toolbox::removeCharsFromString(std::string s, int count)
{
    std::stringstream ss;
    for (int i = 0; i < s.size() - count; i++)
        ss << s.at(i);
    return ss.str();
}

/**
* Calculates a distance between two sf::vector2f
*/
double Toolbox::vectorDistance(sf::Vector2f a, sf::Vector2f b)
{
    double out;

    out = sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y));

    return out;
}

/**
 * Returns the result of a linear interpolation where x0 and x1 are magnitudes, and alpha is the transition 
*/
double Toolbox::linearInterpolate(double x0, double x1, double alpha)
{
    //fprintf(stderr, "%f * (1.0 - %f) + %f * %f", x0, alpha, alpha, x1);
    if (alpha > 1.0)
        alpha = 1.0;
    else if (alpha <= 0)
        alpha = 0.0;
    double interpolation = x0 * (1.0 - alpha) + alpha * x1;
    //sfprintf(stderr, " = %f\n", interpolation);
    return interpolation;
}

double Toolbox::radiansToDegrees(double rads)
{
    return rads * (180.0/3.14159);
}

double Toolbox::degreesToRadians(double degs)
{
    return degs * (3.14159 / 180.0);
}

// Returns a two dimensional array of floats
float** Toolbox::giveFloatArray2D(int sizeX, int sizeY)
{
    float** out;
    out = new float*[sizeX];
    for (int i = 0; i < sizeX; i++)
    {
        out[i]= new float[sizeY];
    }
    return out;
}

// Returns a three dimensional array of floats
float*** Toolbox::giveFloatArray3D(int sizeX, int sizeY, int sizeZ)
{
    float*** out;
    out = new float**[sizeX];
    for (unsigned int i = 0; i < sizeX; i++)
    {
        out[i] = new float*[sizeY];
        for (unsigned int j = 0; j < sizeY; j++)
        {
            out[i][j] = new float[sizeZ];
        }
    }
    return out;
}

// Sets all elements in a 2D float array to 0
void Toolbox::clearFloatArray2D(float** arr, int sizeX, int sizeY)
{
    for (unsigned int i = 0; i < sizeX; i++)
    {
        for (unsigned int j = 0; j < sizeY; j++)
        {
            arr[i][j] = 0.0f;
        }
    }
}

// Sets all elements in a 3D float array to 0
void Toolbox::clearFloatArray3D(float*** arr, int sizeX, int sizeY, int sizeZ)
{
    for (unsigned int i = 0; i < sizeX; i++)
    {
        for (unsigned int j = 0; j < sizeY; j++)
        {
            for (unsigned int k = 0; k = sizeZ; k++)
            {
                arr[i][j][k] = 0.0f;
            }
        }
    }
}

void Toolbox::deleteFloatArray2D(float** arr, int sizeX)
{
    for (unsigned int i = 0; i < sizeX; i++)
        delete[] arr[i];
}

void Toolbox::deleteFloatArray3D(float*** arr, int sizeX, int sizeY)
{
    for (unsigned int i = 0; i < sizeX; i++)
    {
        for (unsigned int j = 0; j < sizeY; j++)
        {
            delete[] arr[i][j];
        }
    }
}

/**
* Takes a seperator string or symbol and a text, returns a vector with the string split into multiple
* strings by using the separator string as a cutting point
*/
std::vector<std::string> Toolbox::splitTextByString(char separator, std::string text)
{
    std::vector<std::string> out;

    std::vector<std::string> strings;
    std::istringstream temp1(text);
    std::string temp2;

    while (std::getline(temp1, temp2, separator))
    {
        out.push_back(temp2);
    }

    return out;
}

/**
* Takes an int stating the desired number of characters per line,
* and a string containing text
* Returns a std::sting containing the text split in lines
* that have at max lineLength characters
*/
std::string Toolbox::splitTextInLines(size_t line_length, std::string input)
{
    size_t curpos = 0;
    size_t nextpos = 0;

    std::list<std::string> lines;
    std::string substr = input.substr(curpos, line_length + 1);

    while (substr.length() == line_length + 1 && (nextpos = substr.rfind(' ')) != input.npos) {
        lines.push_back(input.substr(curpos, nextpos));
        curpos += nextpos + 1;
        substr = input.substr(curpos, line_length + 1);
    }

    if (curpos != input.length())
        lines.push_back(input.substr(curpos, input.npos));

    std::string out;

    for (auto iter = lines.begin(); iter != lines.end(); iter++)
    {
        out.append(*iter);
        out.append("\n");
    }
    return out;
}

/**
* Returns the current system time as seconds since epoch
*/
long Toolbox::getCurrentTime()
{
    return std::chrono::duration_cast<std::chrono::seconds> (std::chrono::system_clock::now().time_since_epoch()).count();
}

void Toolbox::print_vector(sf::Vector2f s)
{
    std::cout << s.x << "x" << s.y << std::endl;
}

