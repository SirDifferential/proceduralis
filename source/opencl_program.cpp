#include "opencl_program.hpp"
#include "application.hpp"
#include "applicationFlags.hpp"
#include "toolbox.hpp"
#include "world.hpp"
#include "gui.hpp"
#include "spriteutils.hpp"
#include "datastorage.hpp"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <iterator>
#include <utility>

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
    if (app.getApplicationFlags()->use_GPU)
    {
        std::cout << "+OpenCL: Attempting to use GPU as OpenCL device" << std::endl;
        std::cout << "+OpenCL: If this causes errors, switch to CPU by changing \"use_GPU\" to \"no\" in config.json" << std::endl;
        try
        {
            context = cl::Context(CL_DEVICE_TYPE_GPU, properties);
        }
        catch (cl::Error e)
        {
            std::cout << "----------------------------------------" << std::endl;
            std::cout << e.what() << ", " << e.err() << std::endl;
            std::cout << "-OpenCL: Could not use GPU as OpenCL device. Most of the time this is due to GPU drivers not having the required functionality." << std::endl;
            std::cout << "-OpenCL: I'm switching to CPU OpenCL. This is slower, but should work" << std::endl;
            std::cout << "----------------------------------------" << std::endl;
            try
            {
                context = cl::Context(CL_DEVICE_TYPE_CPU, properties);
                std::cout << "+OpenCL: I was able to create a backup context using the CPU as OpenCL device" << std::endl;
                std::cout << "+OpenCL: Consider tweaking your GPU drivers later so that the program runs faster." << std::endl;
                app.getApplicationFlags()->use_GPU = false;
            }
            catch (cl::Error e2)
            {
                std::cout << "----------------------------------------" << std::endl;
                std::cout << e.what() << ", " << e.err() << std::endl;
                std::cout << "-OpenCL: I was not able to use CPU as a backup OpenCL device. Something real bad is going on.\nAborting.\nContact the software author!" << std::endl;
                std::cout << "----------------------------------------" << std::endl;    
            app.exit();
                return;
            }
        }
    }
    else
    {
        std::cout << "+OpenCL: Attempting to use CPU as OpenCL device" << std::endl;
        std::cout << "+OpenCL: If you have modern GPU drivers, please switch to GPU for better performance" << std::endl;
        std::cout << "+OpenCL: This can be done by changing \"use_GPU\" to \"yes\" in config.json" << std::endl;
        try
        {
            context = cl::Context(CL_DEVICE_TYPE_CPU, properties);
        }
        catch (cl::Error e)
        {
            std::cout << "----------------------------------------" << std::endl;
            std::cout << e.what() << ", " << e.err() << std::endl;
            std::cout << "-OpenCL: Error at creating context with CPU as OpenCL device" << std::endl;
            std::cout << "-OpenCL: This should not happen, but it did. Trying GPU as a backup device" << std::endl;
            std::cout << "----------------------------------------" << std::endl;
            try
            {
                context = cl::Context(CL_DEVICE_TYPE_GPU, properties);
            }
            catch (cl::Error e2)
            {
                std::cout << "----------------------------------------" << std::endl;
                std::cout << e2.what() << ", " << e.err() << std::endl;
                std::cout << "-OpenCL: Using GPU as a backup device failed. This is probably due to problems with the GPU drivers" << std::endl;
                std::cout << "-OpenCL: There were no OpenCL capable devices. The program cannot continue :(" << std::endl;
                std::cout << "----------------------------------------" << std::endl;
                app.exit();
                return;
            }
        }
    }
    devices = context.getInfo<CL_CONTEXT_DEVICES>();
    std::cout << "+OpenCL: Devices available: " << devices.size() << std::endl;

    commandQueue = cl::CommandQueue(context, devices[device_used], 0, &error);
    print_errors("cl::CommandQueue", error);

    programSourceRaw = readSource(sourcepath);

    if (app.getApplicationFlags()->print_cl_programs)
    {
        std::cout << "+OpenCL: Kernel size: " << programSourceRaw.size() << std::endl;
        std::cout << "+OpenCL: Kernel: " << programSourceRaw << std::endl;
    }

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
        error = program.build(devices);
    }
    catch (cl::Error err)
    {
        std::cout << "-OpenCL Exception: " << err.what() << ", " << err.err() << std::endl;
        print_errors("program.build()", error);
    }

    std::cout << "Build status: " << program.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(devices[0]) << std::endl;
    std::cout << "Build Options:\t" << program.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(devices[0]) << std::endl;
    std::cout << "Build Log:\t " << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]) << std::endl;
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

void CL_Program::cleanup()
{
}

void CL_Program::setOutputTarget(SpritePtr s)
{
    outputTarget = s;
}

void CL_Program::event1() { }
void CL_Program::event2() { }
void CL_Program::event3() { }
void CL_Program::event4() { }
void CL_Program::event5() { }
void CL_Program::event6() { }
void CL_Program::event7() { }
void CL_Program::event8() { }
void CL_Program::event9() { }

std::string CL_Program::getSourceName() { return sourcepath; }
