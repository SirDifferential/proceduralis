#include "cl_voronoi.hpp"
#include "application.hpp"
#include "applicationFlags.hpp"
#include "toolbox.hpp"
#include "world.hpp"
#include "gui.hpp"
#include "spriteutils.hpp"
#include "datastorage.hpp"
#include <algorithm>

CL_Voronoi::CL_Voronoi(std::string s) : CL_Program(s)
{
}

void CL_Voronoi::loadProgram()
{
    CL_Program::loadProgram();
    
    try
    {
        kernel = cl::Kernel(program, "voronoi", &error);
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

    image_buffer_out = new float[image_size];
    row_pitch = 1024 * 4 * sizeof(float);
    origin.push_back(0);
    origin.push_back(0);
    origin.push_back(0);

    region.push_back(1024);
    region.push_back(1024);
    region.push_back(1);

    data_points = new int();
    *data_points = 500;

    superregions = new int();
    *superregions = 18;

    voronoi_points_x = new float[*data_points];
    voronoi_points_y = new float[*data_points];
    superregions_x = new float[*superregions];
    superregions_y = new float[*superregions];
    colors = new float[*data_points];
    superregion_colors = new float[*superregions];

    // Create random coordiantes to work as the datapoints in the voronoi diagram
    // Also create colors according to the number of datapoints

    for (int i = 0; i < *data_points; i++)
    {
        voronoi_points_x[i] = app.getToolbox()->giveRandomInt(0, 1024);
        voronoi_points_y[i] = app.getToolbox()->giveRandomInt(0, 1024);
        colors[i] = app.getToolbox()->giveRandomInt(0, 255);
    }

    // Then choose a few random points and make these the cores of some larger regions
    // Assign some color for each superregion
    // It is assumed here that the number of superregions is much smaller than the voronoi datapoints,
    // and a multiplication by 10 is performed without any checks
    for (int i = 0; i < *superregions; i++)
    {
        superregions_x[i] = app.getToolbox()->giveRandomInt(0, 1024);
        superregions_y[i] = app.getToolbox()->giveRandomInt(0, 1024);
        superregion_colors[i] = app.getToolbox()->giveRandomInt(0, 255);
    }

    size_t voronoi_points_size = sizeof(float) * *data_points;
    size_t superregions_size = sizeof(float) * *superregions;
    
    try
    {
        image_b = new cl::Image2D(context, CL_MEM_WRITE_ONLY, format, 1024, 1024, 0);
        cl_voronoi_points_x = cl::Buffer(context, CL_MEM_READ_WRITE, voronoi_points_size, NULL, &error);
        cl_voronoi_points_y = cl::Buffer(context, CL_MEM_READ_WRITE, voronoi_points_size, NULL, &error);
        cl_colors = cl::Buffer(context, CL_MEM_READ_WRITE, voronoi_points_size, NULL, &error);
        cl_data_points = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(int), NULL, &error);
        cl_superregions_x = cl::Buffer(context, CL_MEM_READ_WRITE, superregions_size, NULL, &error);
        cl_superregions_y = cl::Buffer(context, CL_MEM_READ_WRITE, superregions_size, NULL, &error);
        cl_superregions = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(int), NULL, &error);
        cl_superregion_colors = cl::Buffer(context, CL_MEM_READ_WRITE, superregions_size, NULL, &error);
    }
    catch (cl::Error e)
    {
        std::cout << "!OpenCL: Error at creating buffers: " << e.what() << ", " << e.err() << std::endl;
    }

    try
    {
        error = commandQueue.enqueueWriteImage(*image_b, CL_TRUE, origin, region, row_pitch, 0, (void*) image_buffer_out);
        error = commandQueue.enqueueWriteBuffer(cl_voronoi_points_x, CL_TRUE, 0, voronoi_points_size, voronoi_points_x, NULL, &event);
        error = commandQueue.enqueueWriteBuffer(cl_voronoi_points_y, CL_TRUE, 0, voronoi_points_size, voronoi_points_y, NULL, &event);
        error = commandQueue.enqueueWriteBuffer(cl_colors, CL_TRUE, 0, voronoi_points_size, colors, NULL, &event);
        error = commandQueue.enqueueWriteBuffer(cl_data_points, CL_TRUE, 0, sizeof(int), data_points, NULL, &event);
        error = commandQueue.enqueueWriteBuffer(cl_superregions_x, CL_TRUE, 0, superregions_size, superregions_x, NULL, &event);
        error = commandQueue.enqueueWriteBuffer(cl_superregions_y, CL_TRUE, 0, superregions_size, superregions_y, NULL, &event);
        error = commandQueue.enqueueWriteBuffer(cl_superregion_colors, CL_TRUE, 0, superregions_size, superregion_colors, NULL, &event);
        error = commandQueue.enqueueWriteBuffer(cl_superregions, CL_TRUE, 0, sizeof(int), superregions, NULL, &event);
    }
    catch (cl::Error e)
    {
        std::cout << "!OpenCL: Error pushing data in the device buffers: " << e.what() << ", " << e.err() << std::endl;
        print_errors("commandQueue.enqueueWriteBuffer", error);
    }

    try
    {
        error = kernel.setArg(0, cl_voronoi_points_x);
        error = kernel.setArg(1, cl_voronoi_points_y);
        error = kernel.setArg(2, cl_colors);
        error = kernel.setArg(3, cl_data_points);
        error = kernel.setArg(4, cl_superregions_x);
        error = kernel.setArg(5, cl_superregions_y);
        error = kernel.setArg(6, cl_superregion_colors);
        error = kernel.setArg(7, cl_superregions);
        error = kernel.setArg(8, *image_b);
    }
    catch (cl::Error err)
    {
        std::cout << "-OpenCL: Error setting kernel arguments: " << err.what() << ", " << err.err() << std::endl;
        print_errors("kernel.setArg", error);
    }
}

void CL_Voronoi::runKernel()
{
    error = commandQueue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(1024, 1024), cl::NullRange, NULL, &event);
    print_errors("commandQueue.enqueueNDRangeKernel", error);

    commandQueue.finish();

    float* map_done = new float[1024*1024*4];
    error = commandQueue.enqueueReadImage(*image_b, CL_TRUE, origin, region, row_pitch, 0, map_done);
    print_errors("commandQueue.enqueueReadImage", error);

    if (outputTarget == nullptr)
    {
        delete[] map_done;
        return;
    }
    app.getSpriteUtils()->setPixels(outputTarget, "voronoi_cells", map_done, 1024, 1024);

    delete[] map_done;
}


void CL_Voronoi::cleanup()
{
    CL_Program::cleanup();
    delete[] image_buffer_in;
    delete[] image_buffer_out;

    delete[] voronoi_points_x;
    delete[] voronoi_points_y;

    delete[] superregions_x;
    delete[] superregions_y;

    delete[] colors;
    delete[] superregion_colors;

    delete data_points;
    delete superregions;
}

void CL_Voronoi::event1()
{
}

void CL_Voronoi::event2()
{
}

void CL_Voronoi::event3()
{
}

void CL_Voronoi::event4()
{
}

void CL_Voronoi::event5()
{
}

void CL_Voronoi::event6()
{
}

void CL_Voronoi::event7()
{
}

void CL_Voronoi::event8()
{
}

