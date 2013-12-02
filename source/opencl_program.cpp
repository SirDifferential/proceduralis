#include "opencl_program.hpp"
#include "application.hpp"
#include "applicationFlags.hpp"
#include "toolbox.hpp"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <iterator>
#include <utility>

#define __NO_STD_VECTOR // Use cl::vector instead of STL version

CL_Program::CL_Program(std::string filepath)
{
    sourcepath = filepath;
}

char* CL_Program::readSource(std::string filename)
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
        std::cout << "+OpenCL: Loaded file: " << filename << std::endl;
        text.assign(memblock);
    }
    else
    {
        std::cout << "-OpenCL: Unable to open file " << filename << std::endl;
        return nullptr;
    }
    return memblock;
}

/**
* Prints the various return values for OpenCL function calls in human-readable string form
*/
void CL_Program::print_errors(std::string function, cl_int error)
{
    if (error == 0)
        std::cout << "+OpenCL: ";
    else
        std::cout << "-OpenCL: ";
     std::cout << function << ": " << app.getToolbox()->opencl_error_string(error) << std::endl;
}

/**
* Loads the OpenCL program by loading the source code, setting up devices and context,
* as well as building the actual kernel
*/
void CL_Program::loadProgram()
{
    const std::string hw("Hello World\n");

    std::vector<cl::Platform> platforms;
    error = cl::Platform::get(&platforms);
    print_errors("cl::Platform::get", error);
    std::cout <<  "Available platforms: " << platforms.size() << std::endl;

    if (platforms.size() == 0)
    {
       std::cout << "-OpenCL: There are no available platforms. This probably means proper GPU drivers aren't installed." << std::endl;
    }

    std::string platformVendor;

    if (app.getApplicationFlags()->opencl_devices_debug)
    {
        std::remove("gpu_debug.txt");
        for (auto iter : platforms)
        {
            printPlatformInfo(iter);
        }
    }

    device_used = 0;

    cl_context_properties properties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0 };
    context = cl::Context(CL_DEVICE_TYPE_GPU, properties);
    devices = context.getInfo<CL_CONTEXT_DEVICES>();
    std::cout << "+OpenCL: Devices available: " << devices.size();

    commandQueue = cl::CommandQueue(context, devices[device_used], 0, &error);
    print_errors("cl::CommandQueue", error);

    readSource(sourcepath);

    std::cout << "+OpenCL: Kernel size: " << programSourceRaw.size() << std::endl;
    std::cout << "+OpenCL: Kernel: " << programSourceRaw << std::endl;

    try
    {
        programSource = cl::Program::Sources(1, std::make_pair(programSourceRaw.c_str(), programSourceRaw.size()));
        program = cl::Program(context, programSource);
    }
    catch (cl::Error er)
    {
        std::cout << "-OpenCL Exception: " << er.what() << ", " << er.err() << std::endl;
    }

    try
    {
        kernel = cl::Kernel(program, "part1", &error);
        print_errors("kernel", error);
    }
    catch (cl::Error err)
    {
        std::cout << "-OpenCL Exception: " << err.what() << ", " << err.err() << std::endl;
    }

    // arrays stored in CPU memory
    num = 10;
    float* a = new float[num];
    float* b = new float[num];
    float* c = new float[num];
    for (int i = 0; i < num; i++)
    {
        a[i] = 1.0f * i;
        b[i] = 1.0f * i;
        c[i]= 0.0f;
    }

    std::cout << "+OpenCL: Creating OpenCL arrays" << std::endl;
    size_t array_size = sizeof(float) * num;

    // Two input arrays
    cl_a = cl::Buffer(context, CL_MEM_READ_ONLY, array_size, NULL, &error);
    print_errors("cl::Buffer a", error);
    cl_b = cl::Buffer(context, CL_MEM_READ_ONLY, array_size, NULL, &error);
    print_errors("cl::Buffer b", error);
    // One output array
    cl_c = cl::Buffer(context, CL_MEM_WRITE_ONLY, array_size, NULL, &error);
    print_errors("cl::Buffer c", error);

    std::cout << "+OpenCL: Sending data to the GPU" << std::endl,
    error = commandQueue.enqueueWriteBuffer(cl_a, CL_TRUE, 0, array_size, a, NULL, &event);
    print_errors("commandQueue.enqueueWriteBuffer a", error);
    error = commandQueue.enqueueWriteBuffer(cl_a, CL_TRUE, 0, array_size, b, NULL, &event);
    print_errors("commandQueue.enqueueWriteBuffer b", error);
    error = commandQueue.enqueueWriteBuffer(cl_a, CL_TRUE, 0, array_size, c, NULL, &event);
    print_errors("commandQueue.enqueueWriteBuffer c", error);

    error = kernel.setArg(0, cl_a);
    print_errors("kernel.setArg a", error);
    error = kernel.setArg(0, cl_a);
    print_errors("kernel.setArg b", error);
    error = kernel.setArg(0, cl_a);
    print_errors("kernel.setArg c", error);


    delete a;
    delete b;
    delete c;

}


void CL_Program::runKernel()
{
    std::cout << "+OpenCL: Kernel running" << std::endl;

    error = commandQueue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(num), cl::NullRange, NULL, &event);
    print_errors("commandQueue.enqueueNDRangeKernel", error);

    commandQueue.finish();

    float* c_done = new float[num];
    error = commandQueue.enqueueReadBuffer(cl_c, CL_TRUE, 0, sizeof(float) * num, &c_done, NULL, &event);
    print_errors("commandQueue.enqueueReadBuffer", error);

    for (int i = 0; i < num; i++)
    {
        std::cout << "c_done[" << i << "] = " << c_done[i] << std::endl;
    }

    delete c_done;
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