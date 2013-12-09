#include "opencl_program.hpp"
#include "application.hpp"
#include "applicationFlags.hpp"
#include "toolbox.hpp"
#include "world.hpp"
#include "gui.hpp"

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
    frequency = new float(0.4f);
    persistence = new float(1.3f);
    octaves = new int(10);
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

    programSourceRaw = readSource(sourcepath);

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
    
    try
    {
        kernel = cl::Kernel(program, "simplexnoise", &error);
        print_errors("kernel()", error);
    }
    catch (cl::Error err)
    {
        std::cout << "-OpenCL Exception: " << err.what() << ", " << err.err() << std::endl;
        print_errors("kernel()", error);
    }

    cl::ImageFormat format;

    format.image_channel_data_type = CL_FLOAT;
    format.image_channel_order = CL_RGBA;
    

    int image_size = 1024*1024*4;
    image_buffer_in = new float[image_size];
    if (image_buffer_in == NULL)
        std::cout << "no go" << std::endl;
    int x = -1;
    int y = 0;
    float value = 0.0f;
    for (int i = 3; i < 1024*1024*4; i += 4)
    {
        x += 1;
        if (x == 1024)
        {
            y += 1;
            x = 0;
        }

        /*
        // Gradient
        image_buffer_in[i-3] = (x / 1024.0f) + (y / 1024.0f);
        image_buffer_in[i-2] = (x / 1024.0f) + (y / 1024.0f);
        image_buffer_in[i-1] = (x / 1024.0f) + (y / 1024.0f);
        image_buffer_in[i] = 1.0f;
        */

        /*
        // Checker
        if (x % 2 == 0)
            value = 1.0f;
        else
            value = 0.0f;
        image_buffer_in[i-3] = value;
        image_buffer_in[i-2] = value;
        image_buffer_in[i-1] = value;
        image_buffer_in[i] = 1.0f;
        */

        // Random values
        value = app.getToolbox()->giveRandomInt(1, 255) / 255.0f;
        image_buffer_in[i-3] = value;
        image_buffer_in[i-2] = value;
        image_buffer_in[i-1] = value;
        image_buffer_in[i] = 1.0f;
        
    }
    image_buffer_out = new float[image_size];
    row_pitch = 1024 * 4 * sizeof(float);
    origin.push_back(0);
    origin.push_back(0);
    origin.push_back(0);

    region.push_back(1024);
    region.push_back(1024);
    region.push_back(1);

    try
    {
        image_a = new cl::Image2D(context, CL_MEM_READ_ONLY, format, 1024, 1024, 0);
        image_b = new cl::Image2D(context, CL_MEM_WRITE_ONLY, format, 1024, 1024, 0);
        cl_frequency = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(float), NULL, &error);
        cl_persistence = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(float), NULL, &error);
        cl_octaves = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(int), NULL, &error);
    }
    catch (cl::Error e)
    {
        std::cout << "!OpenCL: Error at creating buffers: " << e.what() << ", " << e.err() << std::endl;
    }

    try
    {
        error = commandQueue.enqueueWriteImage(*image_a, CL_TRUE, origin, region, row_pitch, 0, (void*) image_buffer_in);
        error = commandQueue.enqueueWriteImage(*image_b, CL_TRUE, origin, region, row_pitch, 0, (void*) image_buffer_out);
        error = commandQueue.enqueueWriteBuffer(cl_persistence, CL_TRUE, 0, sizeof(float), frequency, NULL, &event);
        error = commandQueue.enqueueWriteBuffer(cl_frequency, CL_TRUE, 0, sizeof(float), persistence, NULL, &event);
        error = commandQueue.enqueueWriteBuffer(cl_octaves, CL_TRUE, 0, sizeof(int), octaves, NULL, &event);
    }
    catch (cl::Error e)
    {
        std::cout << "!OpenCL: Error pushing data in the device buffers: " << e.what() << ", " << e.err() << std::endl;
        print_errors("commandQueue.enqueueWriteBuffer", error);
    }

    try
    {
        error = kernel.setArg(0, *image_a);
        error = kernel.setArg(1, *image_b);
        error = kernel.setArg(2, cl_frequency);
        error = kernel.setArg(3, cl_persistence);
        error = kernel.setArg(4, cl_octaves);
    }
    catch (cl::Error err)
    {
        std::cout << "-OpenCL: Error setting kernel arguments: " << err.what() << ", " << err.err() << std::endl;
        print_errors("kernel.setArg", error);
    }

    app.getGUI()->persistenceString = app.getToolbox()->combineStringAndFloat("Persistence: ", *persistence);
    app.getGUI()->frequencyString = app.getToolbox()->combineStringAndFloat("Frequency: ", *frequency);
    app.getGUI()->octaveString = app.getToolbox()->combineStringAndInt("Octaves: ", *octaves);
}


void CL_Program::runKernel()
{
    //std::cout << "+OpenCL: Kernel running" << std::endl;

    error = commandQueue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(1024, 1024), cl::NullRange, NULL, &event);
    print_errors("commandQueue.enqueueNDRangeKernel", error);

    commandQueue.finish();

    float* map_done = new float[1024*1024*4];
    error = commandQueue.enqueueReadImage(*image_b, CL_TRUE, origin, region, row_pitch, 0, map_done);
    //error = commandQueue.enqueueReadBuffer(*image_b, CL_TRUE, 0, sizeof(float) * num *num, b_done, NULL, &event);
    print_errors("commandQueue.enqueueReadImage", error);

    /*
    for (int i = 0; i < 8*8*4; i++)
    {
        std::cout << "map_done[" << i << "] = " << map_done[i] << std::endl;
    }
    */

    app.getWorld()->setWorld(map_done, 1024, 1024);

    delete[] map_done;
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
    delete[] a;
    delete[] b;
    delete[] image_buffer_in;
    delete[] image_buffer_out;
    delete frequency;
    delete persistence;
    delete octaves;
}

void CL_Program::event1()
{
    *persistence += 0.1f;
    try
    {
        error = commandQueue.enqueueWriteBuffer(cl_persistence, CL_TRUE, 0, sizeof(float), persistence, NULL, &event);
    }
    catch (cl::Error e)
    {
        std::cout << "!OpenCL: Error writing buffer at event 1: " << e.what() << ", " << e.err() << std::endl;
    }
    runKernel();

    app.getGUI()->persistenceString = app.getToolbox()->combineStringAndFloat("Persistence: ", *persistence);
}

void CL_Program::event2()
{
    *persistence -= 0.1f;
    try
    {
        error = commandQueue.enqueueWriteBuffer(cl_persistence, CL_TRUE, 0, sizeof(float), persistence, NULL, &event);
    }
    catch (cl::Error e)
    {
        std::cout << "!OpenCL: Error writing buffer at event 2: " << e.what() << ", " << e.err() << std::endl;
    }
    runKernel();

    app.getGUI()->persistenceString = app.getToolbox()->combineStringAndFloat("Persistence: ", *persistence);
}

void CL_Program::event3()
{
    *frequency += 0.1f;
    try
    {
        error = commandQueue.enqueueWriteBuffer(cl_frequency, CL_TRUE, 0, sizeof(float), frequency, NULL, &event);
    }
    catch (cl::Error e)
    {
        std::cout << "!OpenCL: Error writing buffer at event 3: " << e.what() << ", " << e.err() << std::endl;
    }
    runKernel();

    app.getGUI()->frequencyString = app.getToolbox()->combineStringAndFloat("frequency: ", *frequency);
}

void CL_Program::event4()
{
    *frequency -= 0.1f;
    try
    {
        error = commandQueue.enqueueWriteBuffer(cl_frequency, CL_TRUE, 0, sizeof(float), frequency, NULL, &event);
    }
    catch (cl::Error e)
    {
        std::cout << "!OpenCL: Error writing buffer at event 4: " << e.what() << ", " << e.err() << std::endl;
    }
    runKernel();

    app.getGUI()->frequencyString = app.getToolbox()->combineStringAndFloat("frequency: ", *frequency);
}

void CL_Program::event5()
{
    *octaves += 1;
    try
    {
        error = commandQueue.enqueueWriteBuffer(cl_octaves, CL_TRUE, 0, sizeof(int), octaves, NULL, &event);
    }
    catch (cl::Error e)
    {
        std::cout << "!OpenCL: Error writing buffer at event 5: " << e.what() << ", " << e.err() << std::endl;
    }
    runKernel();

    app.getGUI()->octaveString = app.getToolbox()->combineStringAndInt("octaves: ", *octaves);
}

void CL_Program::event6()
{
    *octaves -= 1;
    try
    {
        error = commandQueue.enqueueWriteBuffer(cl_octaves, CL_TRUE, 0, sizeof(int), octaves, NULL, &event);
    }
    catch (cl::Error e)
    {
        std::cout << "!OpenCL: Error writing buffer at event 6: " << e.what() << ", " << e.err() << std::endl;
    }
    runKernel();

    app.getGUI()->octaveString = app.getToolbox()->combineStringAndInt("octaves: ", *octaves);
}

void CL_Program::event7()
{
}

void CL_Program::event8()
{
}
