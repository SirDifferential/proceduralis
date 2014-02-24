#include "cl_precipitation.hpp"
#include "application.hpp"
#include "applicationFlags.hpp"
#include "toolbox.hpp"
#include "world.hpp"
#include "gui.hpp"
#include "spriteutils.hpp"
#include "datastorage.hpp"
#include "cl_blur.hpp"

CL_Precipitation::CL_Precipitation(std::string s) : CL_Program(s)
{
}

void CL_Precipitation::init()
{
    try
    {
        kernel = cl::Kernel(program, "precipitation", &error);
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
    
    regions = new float[image_size];
    winds = new float[image_size];
    precipitation = new float[image_size];
    row_pitch = 1024 * sizeof(float);
    origin.push_back(0);
    origin.push_back(0);
    origin.push_back(0);

    region.push_back(1024);
    region.push_back(1024);
    region.push_back(1);

    int** worldregions = app.getWorld()->getRegionMap();
    auto windsimage = app.getDataStorage()->getImage("windblurred");

    int count = 0;
    for (int y = 0; y < 1024; y++)
    {
        for (int x = 0; x < 1024; x++)
        {
            regions[count] = worldregions[x][y];
            winds[count] = windsimage->getPixel(x, y).b / 50.0;
            count++;
        }
    }

    try
    {
        cl_regions = new cl::Image2D(context, CL_MEM_READ_ONLY, format, 1024, 1024, 0);
        cl_winds = new cl::Image2D(context, CL_MEM_READ_ONLY, format, 1024, 1024, 0);
        cl_precipitation = new cl::Image2D(context, CL_MEM_WRITE_ONLY, format2, 1024, 1024, 0);
    }
    catch (cl::Error e)
    {
        std::cout << "!OpenCL: Error at creating buffers: " << e.what() << ", " << e.err() << std::endl;
    }

    try
    {
        error = commandQueue.enqueueWriteImage(*cl_regions, CL_TRUE, origin, region, row_pitch, 0, (void*) regions);
        error = commandQueue.enqueueWriteImage(*cl_winds, CL_TRUE, origin, region, row_pitch, 0, (void*) winds);
    }
    catch (cl::Error e)
    {
        std::cout << "!OpenCL: Error pushing data in the device buffers: " << e.what() << ", " << e.err() << std::endl;
        print_errors("commandQueue.enqueueWriteBuffer", error);
    }

    try
    {
        error = kernel.setArg(0, *cl_regions);
        error = kernel.setArg(1, *cl_winds);
        error = kernel.setArg(2, *cl_precipitation);
    }
    catch (cl::Error err)
    {
        std::cout << "-OpenCL: Error setting kernel arguments: " << err.what() << ", " << err.err() << std::endl;
        print_errors("kernel.setArg", error);
    }
}

void CL_Precipitation::runKernel()
{
    error = commandQueue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(1024, 1024), cl::NullRange, NULL, &event);
    print_errors("commandQueue.enqueueNDRangeKernel", error);

    commandQueue.finish();

    float* map_done = new float[1024*1024*4];
    row_pitch = 1024 * 4 * sizeof(float);
    error = commandQueue.enqueueReadImage(*cl_precipitation, CL_TRUE, origin, region, row_pitch, 0, map_done);
    print_errors("commandQueue.enqueueReadImage", error);

    if (outputTarget == nullptr)
    {
        delete[] map_done;
        return;
    }

    app.getSpriteUtils()->setPixels(outputTarget, outputName, map_done, 1024, 1024);

    // Blur rainfall a bit
    std::shared_ptr<CL_Blur> temp = std::dynamic_pointer_cast<CL_Blur>(app.getProgram("blur"));
    temp->setBlurSize(25);
    temp->setInputBuffer(map_done);
    temp->setOutputTarget(app.getDataStorage()->getSprite("precipitation_blurred"), "precipitation_blurred");
    temp->runKernel();
    temp->resetInputBuffer();


    delete[] map_done;
}

void CL_Precipitation::cleanup()
{
    CL_Program::cleanup();
    delete[] regions;
    delete[] winds;
    delete[] precipitation;
}

void CL_Precipitation::event1()
{
}

void CL_Precipitation::event2()
{
}

void CL_Precipitation::event3()
{
}

void CL_Precipitation::event4()
{
}

void CL_Precipitation::event5()
{
}

void CL_Precipitation::event6()
{
}

void CL_Precipitation::event7()
{
}

void CL_Precipitation::event8()
{
}
