#include "cl_voronoi.hpp"
#include "application.hpp"
#include "applicationFlags.hpp"
#include "toolbox.hpp"
#include "world.hpp"
#include "gui.hpp"
#include "spriteutils.hpp"
#include "datastorage.hpp"

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
    *data_points = 2000;
    input_data_x = new float[*data_points];
    input_data_y = new float[*data_points];
    colors = new float[*data_points];

    for (int i = 0; i < *data_points; i++)
    {
        input_data_x[i] = app.getToolbox()->giveRandomInt(0, 1024);
        input_data_y[i] = app.getToolbox()->giveRandomInt(0, 1024);
        colors[i] = app.getToolbox()->giveRandomInt(0, 255);
    }

    size_t array_size = sizeof(float) * *data_points;
    
    try
    {
        image_b = new cl::Image2D(context, CL_MEM_WRITE_ONLY, format, 1024, 1024, 0);
        cl_input_a = cl::Buffer(context, CL_MEM_READ_WRITE, array_size, NULL, &error);
        cl_input_b = cl::Buffer(context, CL_MEM_READ_WRITE, array_size, NULL, &error);
        cl_colors = cl::Buffer(context, CL_MEM_READ_WRITE, array_size, NULL, &error);
        cl_data_points = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(int), NULL, &error);
    }
    catch (cl::Error e)
    {
        std::cout << "!OpenCL: Error at creating buffers: " << e.what() << ", " << e.err() << std::endl;
    }

    try
    {
        error = commandQueue.enqueueWriteImage(*image_b, CL_TRUE, origin, region, row_pitch, 0, (void*) image_buffer_out);
        error = commandQueue.enqueueWriteBuffer(cl_input_a, CL_TRUE, 0, array_size, input_data_x, NULL, &event);
        error = commandQueue.enqueueWriteBuffer(cl_input_b, CL_TRUE, 0, array_size, input_data_y, NULL, &event);
        error = commandQueue.enqueueWriteBuffer(cl_colors, CL_TRUE, 0, array_size, colors, NULL, &event);
        error = commandQueue.enqueueWriteBuffer(cl_data_points, CL_TRUE, 0, sizeof(int), data_points, NULL, &event);
    }
    catch (cl::Error e)
    {
        std::cout << "!OpenCL: Error pushing data in the device buffers: " << e.what() << ", " << e.err() << std::endl;
        print_errors("commandQueue.enqueueWriteBuffer", error);
    }

    try
    {
        error = kernel.setArg(0, cl_input_a);
        error = kernel.setArg(1, cl_input_b);
        error = kernel.setArg(2, cl_colors);
        error = kernel.setArg(3, cl_data_points);
        error = kernel.setArg(4, *image_b);
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

