#include "opencl_program.hpp"
#include "application.hpp"
#include "applicationFlags.hpp"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <iterator>
#include <utility>


#define __NO_STD_VECTOR // Use cl::vector instead of STL version

CL_Program::CL_Program()
{
    
}

int CL_Program::checkError(cl_int error, const char* name)
{
    if (error != CL_SUCCESS)
    {
        std::cerr << "Error: " << name << " (" << error << ")" << std::endl;
        return error;
    }
    return 0;
}

char* CL_Program::loadProgram(std::string filename)
{
    std::ifstream::pos_type size;
    char* memblock;
    std::string text;
    GLuint fSize;

    // Read the data, based on an example on cplusplus.com
    std::ifstream file (filename, std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open())
    {
        size = file.tellg();
        fSize = (GLuint) size;

        memblock = new char[1 + fSize]; 
        file.seekg (0, std::ios::beg);  
        file.read (memblock, size);  
        file.close();
        // Remember to terminate the string with \0. Important for compiling the shader
        memblock[size] = '\0';
        fprintf(stderr, "# Shader: file %s loaded\n", filename);
        text.assign(memblock);
    }
    else
    {
        fprintf(stderr, "! Shader: Unable to open file %s\n", filename);
        return nullptr;
    }
    return memblock;
}

void CL_Program::printPlatformInfo(cl::Platform p)
{
    std::string platformVendor;
    std::string platformName;
    std::string platformVersion;
    std::string platformProfile;
    std::string icd_suffix;
    std::string platform_ext;
    p.getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &platformVendor);
    p.getInfo((cl_platform_info)CL_PLATFORM_NAME, &platformName);
    p.getInfo((cl_platform_info)CL_PLATFORM_VERSION, &platformVersion);
    p.getInfo((cl_platform_info)CL_PLATFORM_PROFILE, &platformProfile);
    p.getInfo((cl_platform_info)CL_PLATFORM_ICD_SUFFIX_KHR, &icd_suffix);
    p.getInfo((cl_platform_info)CL_PLATFORM_EXTENSIONS, &platform_ext);
    
    std::cout << "\tVendor: " << platformVendor << "\n\tName: " << platformName << "\n\tVersion: " << platformVersion << "\n\tPlatormProfile: " << platformProfile << "\n\ticdsuffix: " << icd_suffix << "\n\tplatform extensions: " << platform_ext << std::endl;

    std::ofstream out("gpu_debug.txt", std::ios::app );
    out << "\tVendor: " << platformVendor << "\n\tName: " << platformName << "\n\tVersion: " << platformVersion << "\n\tPlatormProfile: " << platformProfile << "\n\ticdsuffix: " << icd_suffix << "\n\tplatform extensions: " << platform_ext;
    out.close();
}

void CL_Program::runKernel()
{
    const std::string hw("Hello World\n");

    std::vector<cl::Platform> platformList;
    cl::Platform::get(&platformList);
    checkError(platformList.size() != 0 ? CL_SUCCESS : -1, "cl::Platform::get");
    std::cout <<  "Platform number is " << platformList.size() << std::endl;
    std::string platformVendor;

    if (app.getApplicationFlags()->opencl_devices_debug)
    {
        std::remove("gpu_debug.txt");
        for (auto iter : platformList)
        {
            printPlatformInfo(iter);
        }
    }

    platformList[0].getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &platformVendor);
    std::cout << "Selected platform is by: " << platformVendor << std::endl;
    cl_context_properties contextprops[3] = { CL_CONTEXT_PLATFORM, (cl_context_properties)(platformList[0])(), 0};
    context = cl::Context(CL_DEVICE_TYPE_GPU, contextprops, NULL, NULL, &error);
    checkError(error, "Context::Context()");

    char* outH = new char[hw.length() + 1];
    cl::Buffer outCL(context, CL_MEM_WRITE_ONLY |CL_MEM_USE_HOST_PTR, hw.length() + 1, outH, &error);
    checkError(error, "Buffer::Buffer()");

    devices = context.getInfo<CL_CONTEXT_DEVICES>();    
    checkError(devices.size() > 0 ? CL_SUCCESS : -1, "devices.size() > 0");

    programSourceRaw = loadProgram("lesson1.cl");
    programSource = cl::Program::Sources(1, std::make_pair(programSourceRaw.c_str(), programSourceRaw.length()+1));

    program = cl::Program(context, programSource);
    error = program.build(devices, "");
    checkError(error, "Program::build()");

    kernel = cl::Kernel(program, "hello", &error);
    checkError(error, "Kernel::Kernel()");
    error = kernel.setArg(0, outCL);
    checkError(error, "Kernel::setArg()");

    commandQueue = cl::CommandQueue(context, devices[0], 0, &error);
    checkError(error, "CommandQueue::CommandQueue()");

    error = commandQueue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(hw.length()+1), cl::NDRange(1,1), NULL, &event);
    checkError(error, "CommandQueue::enqueueNDRangeKernel()");

    event.wait();
    error = commandQueue.enqueueReadBuffer(outCL, CL_TRUE, 0, hw.length()+1, outH);
    checkError(error, "CommandQueue::enqueueReadBuffer()");
    std::cout << outH << std::endl;
    
    return;

}
