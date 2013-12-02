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
public:
    CL_Program();

    char* loadProgram(std::string file_path);
    void runKernel();

    int checkError(cl_int error, const char* n);
    void printPlatformInfo(cl::Platform p);
};

#endif
