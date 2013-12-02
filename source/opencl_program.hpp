#ifndef __OPENCL_PROGRAM_HPP_
#define __OPENCL_PROGRAM_HPP_

#include <GL/glew.h>
#include "cl.hpp"

class CL_Program
{
private:
    unsigned int device_used;
    std::vector<cl::Device> devices;
    cl::Context context;
    cl::CommandQueue commandQueue;
    cl::Program program;
    std::string programSourceRaw;
    cl::Program::Sources programSource;
    cl::Kernel kernel;
    cl_int error;
    cl::Event event;

    std::string sourcepath;

    cl::Buffer cl_a;
    cl::Buffer cl_b;
    cl::Buffer cl_c;
    int num;

public:
    CL_Program(std::string s);

    char* readSource(std::string file_path);
    void loadProgram();
    void runKernel();

    void print_errors(std::string function, cl_int error);
    void printPlatformInfo(cl::Platform p);
};

#endif
