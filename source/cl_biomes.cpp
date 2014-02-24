#include "cl_biomes.hpp"
#include "application.hpp"
#include "applicationFlags.hpp"
#include "toolbox.hpp"
#include "world.hpp"
#include "gui.hpp"
#include "spriteutils.hpp"
#include "datastorage.hpp"
#include "cl_blur.hpp"

CL_Biomes::CL_Biomes(std::string s) : CL_Program(s)
{
}

void CL_Biomes::init()
{
    try
    {
        kernel = cl::Kernel(program, "biomes", &error);
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

    int image_size = 1024*1024;
    
    image_buffer_height = new float[image_size];
    image_buffer_precipitation = new float[image_size];
    image_buffer_temperature = new float[image_size];
    row_pitch = 1024 * sizeof(float);
    origin.push_back(0);
    origin.push_back(0);
    origin.push_back(0);

    region.push_back(1024);
    region.push_back(1024);
    region.push_back(1);

    auto height_img = app.getDataStorage()->getImage("heightmap");
    auto preci_img = app.getDataStorage()->getImage("precipitation_blurred");
    auto temp_img = app.getDataStorage()->getImage("temperature");

    float value;
    int count = 0;
    for (int y = 0; y < 1024; y++)
    {
        for (int x = 0; x < 1024; x++)
        {
            image_buffer_height[count] = height_img->getPixel(x, y).r;
            image_buffer_precipitation[count] = preci_img->getPixel(x, y).r;
            image_buffer_temperature[count] = temp_img->getPixel(x, y).r;
            count++;
        }
    }
    try
    {    
        image_height = new cl::Image2D(context, CL_MEM_READ_ONLY, format, 1024, 1024, 0);
        image_precipitation = new cl::Image2D(context, CL_MEM_READ_ONLY, format, 1024, 1024, 0);
        image_temperature = new cl::Image2D(context, CL_MEM_READ_ONLY, format, 1024, 1024, 0);
        image_biomes = new cl::Image2D(context, CL_MEM_WRITE_ONLY, format2, 1024, 1024, 0);
    }
    catch (cl::Error e)
    {
        std::cout << "!OpenCL: Error at creating buffers: " << e.what() << ", " << e.err() << std::endl;
    }

    try
    {
        error = commandQueue.enqueueWriteImage(*image_height, CL_TRUE, origin, region, row_pitch, 0, (void*) image_buffer_height);
        error = commandQueue.enqueueWriteImage(*image_precipitation, CL_TRUE, origin, region, row_pitch, 0, (void*) image_buffer_precipitation);
        error = commandQueue.enqueueWriteImage(*image_temperature, CL_TRUE, origin, region, row_pitch, 0, (void*) image_buffer_temperature);
    }
    catch (cl::Error e)
    {
        std::cout << "!OpenCL: Error pushing data in the device buffers: " << e.what() << ", " << e.err() << std::endl;
        print_errors("commandQueue.enqueueWriteBuffer", error);
    }

    try
    {
        error = kernel.setArg(0, *image_height);
        error = kernel.setArg(1, *image_precipitation);
        error = kernel.setArg(2, *image_temperature);
        error = kernel.setArg(3, *image_biomes);
    }
    catch (cl::Error err)
    {
        std::cout << "-OpenCL: Error setting kernel arguments: " << err.what() << ", " << err.err() << std::endl;
        print_errors("kernel.setArg", error);
    }
}

void CL_Biomes::runKernel()
{
    try
    {
        error = commandQueue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(1024, 1024), cl::NullRange, NULL, &event);
    }
    catch (cl::Error err)
    {
        std::cout << "-CL_Biomes: Error running kernel: " << err.what() << ", " << err.err() << std::endl;
        print_errors("commandQueue.enqueueNDRangeKernel", error);
    }

    commandQueue.finish();

    float* map_done = new float[1024*1024*4];
    row_pitch = 1024 * 4 * sizeof(float);
    error = commandQueue.enqueueReadImage(*image_biomes, CL_TRUE, origin, region, row_pitch, 0, map_done);
    print_errors("commandQueue.enqueueReadImage", error);

    if (outputTarget == nullptr)
    {
        delete[] map_done;
        return;
    }

    app.getSpriteUtils()->setPixelsNorerange(outputTarget, outputName, map_done, 1024, 1024);

    delete[] map_done;
}

void CL_Biomes::cleanup()
{
    CL_Program::cleanup();
    delete[] image_buffer_height;
    delete[] image_buffer_precipitation;
    delete[] image_buffer_temperature;
    delete[] image_buffer_out;
}

void CL_Biomes::event1()
{
}

void CL_Biomes::event2()
{
}

void CL_Biomes::event3()
{
}

void CL_Biomes::event4()
{
}

void CL_Biomes::event5()
{
}

void CL_Biomes::event6()
{
}

void CL_Biomes::event7()
{
}

void CL_Biomes::event8()
{
}
