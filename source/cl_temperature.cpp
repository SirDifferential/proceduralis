#include "cl_temperature.hpp"
#include "application.hpp"
#include "applicationFlags.hpp"
#include "toolbox.hpp"
#include "world.hpp"
#include "gui.hpp"
#include "spriteutils.hpp"
#include "datastorage.hpp"
#include "cl_blur.hpp"

CL_Temperature::CL_Temperature(std::string s) : CL_Program(s)
{
}

void CL_Temperature::loadProgram()
{
    CL_Program::loadProgram();
    
    try
    {
        kernel = cl::Kernel(program, "temperature", &error);
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
    image_buffer_out = new float[image_size];
    row_pitch = 1024 * 4 * sizeof(float);
    origin.push_back(0);
    origin.push_back(0);
    origin.push_back(0);

    region.push_back(1024);
    region.push_back(1024);
    region.push_back(1);

    float value;

    for (int i = 3; i < 1024*1024*4; i += 4)
    {
        value = app.getToolbox()->giveRandomFloat();
        image_buffer_in[i-3] = value;
        image_buffer_in[i-2] = value;
        image_buffer_in[i-1] = value;
        image_buffer_in[i] = 1.0f;
    }

    try
    {    
        image_a = new cl::Image2D(context, CL_MEM_READ_ONLY, format, 1024, 1024, 0);
        image_b = new cl::Image2D(context, CL_MEM_WRITE_ONLY, format, 1024, 1024, 0);
    }
    catch (cl::Error e)
    {
        std::cout << "!OpenCL: Error at creating buffers: " << e.what() << ", " << e.err() << std::endl;
    }

    try
    {
        error = commandQueue.enqueueWriteImage(*image_a, CL_TRUE, origin, region, row_pitch, 0, (void*) image_buffer_in);
        error = commandQueue.enqueueWriteImage(*image_b, CL_TRUE, origin, region, row_pitch, 0, (void*) image_buffer_out);
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
    }
    catch (cl::Error err)
    {
        std::cout << "-OpenCL: Error setting kernel arguments: " << err.what() << ", " << err.err() << std::endl;
        print_errors("kernel.setArg", error);
    }
}

void CL_Temperature::runKernel()
{
    if (image_buffer_in == NULL)
    {
        std::cout << "-CL_Temperature: cannot run kernel: image_buffer_in is NULL" << std::endl;
        return;
    }

    try
    {
        error = commandQueue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(1024, 1024), cl::NullRange, NULL, &event);
    }
    catch (cl::Error err)
    {
        std::cout << "-CL_Temperature: Error running kernel: " << err.what() << ", " << err.err() << std::endl;
        print_errors("commandQueue.enqueueNDRangeKernel", error);
    }
    

    commandQueue.finish();

    float* map_done = new float[1024*1024*4];
    error = commandQueue.enqueueReadImage(*image_b, CL_TRUE, origin, region, row_pitch, 0, map_done);
    print_errors("commandQueue.enqueueReadImage", error);

    if (outputTarget == nullptr)
    {
        delete[] map_done;
        return;
    }

    app.getSpriteUtils()->setPixelsNorerange(outputTarget, outputName, map_done, 1024, 1024);

    delete[] map_done;
}

void CL_Temperature::cleanup()
{
    CL_Program::cleanup();
    delete[] image_buffer_in;
    delete[] image_buffer_out;
}

void CL_Temperature::event1()
{
}

void CL_Temperature::event2()
{
}

void CL_Temperature::event3()
{
}

void CL_Temperature::event4()
{
}

void CL_Temperature::event5()
{
}

void CL_Temperature::event6()
{
}

void CL_Temperature::event7()
{
}

void CL_Temperature::event8()
{
}
