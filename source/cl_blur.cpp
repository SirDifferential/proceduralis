#include "cl_blur.hpp"
#include "application.hpp"
#include "applicationFlags.hpp"
#include "toolbox.hpp"
#include "world.hpp"
#include "gui.hpp"
#include "spriteutils.hpp"
#include "datastorage.hpp"

CL_Blur::CL_Blur(std::string s) : CL_Program(s)
{
    blur_size = new int();
    *blur_size = 5;
    kernelSigma = 20.5;
}

void CL_Blur::loadProgram()
{
    CL_Program::loadProgram();
    
    try
    {
        kernel = cl::Kernel(program, "blur", &error);
        print_errors("kernel()", error);
    }
    catch (cl::Error err)
    {
        std::cout << "-OpenCL Exception: " << err.what() << ", " << err.err() << std::endl;
        print_errors("kernel()", error);
    }

    try
    {    
        cl_blur_size = cl::Buffer(context, CL_MEM_WRITE_ONLY, sizeof(int), NULL, &error);
    }
    catch (cl::Error e)
    {
        std::cout << "!OpenCL: Error at creating buffers: " << e.what() << ", " << e.err() << std::endl;
    }

    try
    {
        error = commandQueue.enqueueWriteBuffer(cl_blur_size, CL_TRUE, 0, sizeof(int), blur_size, NULL, &event);
    }
    catch (cl::Error e)
    {
        std::cout << "!OpenCL: Error pushing data in the device buffers: " << e.what() << ", " << e.err() << std::endl;
        print_errors("commandQueue.enqueueWriteBuffer", error);
    }
}

void CL_Blur::runKernel()
{
    if (image_buffer_in == NULL)
    {
        std::cout << "-CL_Blur: cannot run kernel: image_buffer_in is NULL" << std::endl;
        return;
    }

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
    app.getSpriteUtils()->setPixels(outputTarget, outputName, map_done, 1024, 1024);

    delete[] map_done;
}

// Calculates Gaussian convolution
double gaussianConvolution(int x, int y, double sigma)
{
    double e = 2.71828182846;
    double pi = 3.14159;
    double left_side = 1 / (2.0 * pi * (sigma*sigma));
    double right_side = pow(e, -1*( (x*x) + (y*y) / (2 * (sigma*sigma)) ));
    double out = left_side * right_side;
    return out;
}

void CL_Blur::createGaussKernel(int radius, double sigma)
{
    if (gaussKernel != NULL)
        delete[] gaussKernel;
    
    gaussKernel = app.getToolbox()->giveFloatArray2D(radius, radius);

    int distance_x = 0;
    int distance_y = 0;
    double sum = 0.0;

    int W = radius;
    double mean = W/2;
    for (int x = 0; x < W; ++x) for (int y = 0; y < W; ++y)
    {
        gaussKernel[x][y] = exp( -0.5 * (pow((x-mean)/sigma, 2.0) + pow((y-mean)/sigma,2.0)) )
                            / (2 * 3.14159 * sigma * sigma);
        sum += gaussKernel[x][y];
    }
    
    
    /*
    // calculate gaussian convolution kernel
    for (int i = 0; i < radius; i++)
    {
        for (int j = 0; j < radius; j++)
        {
            // How far this element is from the center
            distance_x = abs(radius/2 - i);
            distance_y = abs(radius/2 - j);

            // calculate the actual gaussian convolution value for this element
            gaussKernel[i][j] = gaussianConvolution(distance_x, distance_y, sigma);

            // Remember the total sum of all values so normalization can be performed
            sum += gaussKernel[i][j];
        }
    }
    */

    // Normalize values so the kernel adds up to 1
    for (int i = 0; i < radius; i++)
    {
        for (int j = 0; j < radius; j++)
        {
            gaussKernel[i][j] = gaussKernel[i][j] / sum;
        }
    }

    float* tempkern = new float[radius*radius];
    int i = 0;
    for (int x = 0; x < radius; x++)
    {
        for (int y = 0; y < radius; y++)
        {
            tempkern[i] = gaussKernel[x][y];
            i++;
        }
    }

    cl::ImageFormat format;
    format.image_channel_data_type = CL_FLOAT;
    format.image_channel_order = CL_LUMINANCE;

    size_t row_pitch2 = radius * sizeof(float);
    cl::size_t<3> origin2;
    origin2.push_back(0);
    origin2.push_back(0);
    origin2.push_back(0);

    cl::size_t<3> region2;
    region2.push_back(radius);
    region2.push_back(radius);
    region2.push_back(1);

    int image_size = radius*radius;

    try
    {
        cl_gaussKernel = new cl::Image2D(context, CL_MEM_READ_ONLY, format, radius, radius, 0);
    }
    catch (cl::Error e)
    {
        std::cout << "!OpenCL: Error at creating buffers: " << e.what() << ", " << e.err() << std::endl;
    }

    try
    {
        error = commandQueue.enqueueWriteImage(*cl_gaussKernel, CL_TRUE, origin2, region2, row_pitch2, 0, (void*) tempkern);
    }
    catch (cl::Error e)
    {
        std::cout << "!OpenCL: Error pushing data in the device buffers: " << e.what() << ", " << e.err() << std::endl;
        print_errors("commandQueue.enqueueWriteBuffer", error);
    }

    try
    {
        error = kernel.setArg(3, *cl_gaussKernel);
    }
    catch (cl::Error err)
    {
        std::cout << "-OpenCL: Error setting kernel arguments: " << err.what() << ", " << err.err() << std::endl;
        print_errors("kernel.setArg", error);
    }

    delete[] tempkern;
}


void CL_Blur::cleanup()
{
    CL_Program::cleanup();
    delete[] image_buffer_in;
    delete[] image_buffer_out;
}

void CL_Blur::setInputBuffer(float* in)
{
    image_buffer_in = in;

    if (image_buffer_in == NULL)
    {
        std::cout << "-OpenCL: image_buffer_in is null" << std::endl;
        return;
    }

    cl::ImageFormat format;

    format.image_channel_data_type = CL_FLOAT;
    format.image_channel_order = CL_RGBA;

    int image_size = 1024*1024*4;
    
    int x = -1;
    int y = 0;
    float value = 0.0f;
    
    if (image_buffer_out != NULL)
        delete[] image_buffer_out;
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
    }
    catch (cl::Error e)
    {
        std::cout << "!OpenCL: Error at creating buffers: " << e.what() << ", " << e.err() << std::endl;
    }

    try
    {
        error = commandQueue.enqueueWriteImage(*image_a, CL_TRUE, origin, region, row_pitch, 0, (void*) image_buffer_in);
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

void CL_Blur::setBlurSize(int i)
{
    *blur_size = i;

    createGaussKernel(*blur_size, kernelSigma);

    try
    {
        error = commandQueue.enqueueWriteBuffer(cl_blur_size, CL_TRUE, 0, sizeof(int), blur_size, NULL, &event);
        error = kernel.setArg(2, cl_blur_size);
    }
    catch (cl::Error err)
    {
        std::cout << "-OpenCL: Error setting blur size: " << err.what() << ", " << err.err() << std::endl;
        print_errors("commandQueue.enqueueWriteBuffer", error);
    }
}

void CL_Blur::event1()
{
}

void CL_Blur::event2()
{
}

void CL_Blur::event3()
{
}

void CL_Blur::event4()
{
}

void CL_Blur::event5()
{
}

void CL_Blur::event6()
{
}

void CL_Blur::event7()
{
}

void CL_Blur::event8()
{
}
