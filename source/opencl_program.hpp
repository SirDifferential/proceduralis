#ifndef __OPENCL_PROGRAM_HPP_
#define __OPENCL_PROGRAM_HPP_

#include "cl.hpp"
#include "precompiled.hpp"

class CL_Program
{
protected:
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

    SpritePtr outputTarget;
    std::string outputName;

public:
    CL_Program(std::string s);

    char* readSource(std::string file_path);
    void loadProgram();
    virtual void init() = 0;
    virtual void runKernel() = 0;

    void setOutputTarget(SpritePtr s, std::string n);

    void print_errors(std::string function, cl_int error);
    void printPlatformInfo(cl::Platform p);
    virtual void cleanup();

    virtual void event1();
    virtual void event2();
    virtual void event3();
    virtual void event4();
    virtual void event5();
    virtual void event6();
    virtual void event7();
    virtual void event8();
    virtual void event9();

    std::string getSourceName();
};

#endif
