#include "cl_satellite.hpp"
#include "application.hpp"
#include "applicationFlags.hpp"
#include "toolbox.hpp"
#include "world.hpp"
#include "gui.hpp"
#include "spriteutils.hpp"
#include "datastorage.hpp"

CL_Satellite::CL_Satellite(std::string s) : CL_Program(s)
{
}

void CL_Satellite::init()
{
    try
    {
        kernel = cl::Kernel(program, "satellite", &error);
        print_errors("kernel()", error);
    }
    catch (cl::Error err)
    {
        std::cout << "-OpenCL Exception: " << err.what() << ", " << err.err() << std::endl;
        print_errors("kernel()", error);
    }


    cl::ImageFormat format;
    format.image_channel_data_type = CL_FLOAT;
    format.image_channel_order = CL_LUMINANCE;

    cl::ImageFormat format2;
    format2.image_channel_data_type = CL_FLOAT;
    format2.image_channel_order = CL_RGBA;

    cl::ImageFormat format3;
    format3.image_channel_data_type = CL_FLOAT;
    format3.image_channel_order = CL_RG;

    int image_size1 = 1024*1024;
    
    height = new float[image_size1];
    biomes = new float[image_size1];
    row_pitch = 1024 * sizeof(float);
    origin.push_back(0);
    origin.push_back(0);
    origin.push_back(0);

    region.push_back(1024);
    region.push_back(1024);
    region.push_back(1);

    auto heightmap = app.getDataStorage()->getImage("heightmap");
    auto biomemap = app.getDataStorage()->getImage("biomes");

    int count = 0;
    for (int y = 0; y < 1024; y++)
    {
        for (int x = 0; x < 1024; x++)
        {
            height[count] = heightmap->getPixel(x, y).r;
            biomes[count] = biomemap->getPixel(x, y).r;
            count++;
        }
    }

    try
    {
        cl_height = new cl::Image2D(context, CL_MEM_READ_ONLY, format, 1024, 1024, 0);
        cl_biomes = new cl::Image2D(context, CL_MEM_READ_ONLY, format, 1024, 1024, 0);
        cl_satellite = new cl::Image2D(context, CL_MEM_WRITE_ONLY, format2, 1024, 1024, 0);
    }
    catch (cl::Error e)
    {
        std::cout << "!OpenCL: Error at creating buffers: " << e.what() << ", " << e.err() << std::endl;
    }

    try
    {
        error = commandQueue.enqueueWriteImage(*cl_height, CL_TRUE, origin, region, row_pitch, 0, (void*) height);
        error = commandQueue.enqueueWriteImage(*cl_biomes, CL_TRUE, origin, region, row_pitch, 0, (void*) biomes);
    }
    catch (cl::Error e)
    {
        std::cout << "!OpenCL: Error pushing data in the device buffers: " << e.what() << ", " << e.err() << std::endl;
        print_errors("commandQueue.enqueueWriteBuffer", error);
    }

    try
    {
        error = kernel.setArg(0, *cl_height);
        error = kernel.setArg(1, *cl_biomes);
        error = kernel.setArg(2, *cl_satellite);
    }
    catch (cl::Error err)
    {
        std::cout << "-OpenCL: Error setting kernel arguments: " << err.what() << ", " << err.err() << std::endl;
        print_errors("kernel.setArg", error);
    }
}

void CL_Satellite::runKernel()
{
    error = commandQueue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(1024, 1024), cl::NullRange, NULL, &event);
    print_errors("commandQueue.enqueueNDRangeKernel", error);

    commandQueue.finish();

    float* map_done = new float[1024*1024*4];
    row_pitch = 1024 * 4 * sizeof(float);
    error = commandQueue.enqueueReadImage(*cl_satellite, CL_TRUE, origin, region, row_pitch, 0, map_done);
    print_errors("commandQueue.enqueueReadImage", error);

    if (outputTarget == nullptr)
    {
        delete[] map_done;
        return;
    }

    app.getSpriteUtils()->setPixelsNorerange(outputTarget, outputName, map_done, 1024, 1024);

    delete[] map_done;
}

void CL_Satellite::cleanup()
{
    CL_Program::cleanup();
    delete[] height;
    delete[] biomes;;
}

void CL_Satellite::event1()
{
}

void CL_Satellite::event2()
{
}

void CL_Satellite::event3()
{
}

void CL_Satellite::event4()
{
}

void CL_Satellite::event5()
{
}

void CL_Satellite::event6()
{
}

void CL_Satellite::event7()
{
}

void CL_Satellite::event8()
{
}
