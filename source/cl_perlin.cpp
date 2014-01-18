#include "cl_perlin.hpp"
#include "application.hpp"
#include "applicationFlags.hpp"
#include "toolbox.hpp"
#include "world.hpp"
#include "gui.hpp"
#include "spriteutils.hpp"
#include "datastorage.hpp"
#include "worldGenerator.hpp"
#include "cl_blur.hpp"

CL_Perlin::CL_Perlin(std::string s) : CL_Program(s)
{
    frequency = new float();
    *frequency = 0.3f;
    persistence = new float();
    *persistence = 1.3f;
    octaves = new int();
    *octaves = 10;
}

void CL_Perlin::loadProgram()
{
    CL_Program::loadProgram();
    
    try
    {
        kernel = cl::Kernel(program, "perlinnoise", &error);
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
        std::cout << "-OpenCL: image_buffer_in is null" << std::endl;
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
        value = app.getToolbox()->giveRandomInt(50, 255) / 255.0f;
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
        error = commandQueue.enqueueWriteBuffer(cl_persistence, CL_TRUE, 0, sizeof(float), persistence, NULL, &event);
        error = commandQueue.enqueueWriteBuffer(cl_frequency, CL_TRUE, 0, sizeof(float), frequency, NULL, &event);
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

void CL_Perlin::runKernel()
{
    error = commandQueue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(1024, 1024), cl::NullRange, NULL, &event);
    print_errors("commandQueue.enqueueNDRangeKernel", error);

    commandQueue.finish();

    float* map_done = new float[1024*1024*4];
    error = commandQueue.enqueueReadImage(*image_b, CL_TRUE, origin, region, row_pitch, 0, map_done);
    //error = commandQueue.enqueueReadBuffer(*image_b, CL_TRUE, 0, sizeof(float) * num *num, b_done, NULL, &event);
    print_errors("commandQueue.enqueueReadImage", error);

    if (outputTarget == nullptr)
    {
        delete[] map_done;
        return;
    }
    app.getSpriteUtils()->setPixels(outputTarget, outputName, map_done, 1024, 1024);

    app.forceredraw();

    /*
    std::shared_ptr<CL_Blur> temp = std::dynamic_pointer_cast<CL_Blur>(app.getProgram("blur"));
    temp->setBlurSize(2);
    temp->setInputBuffer(map_done);
    temp->setOutputTarget(app.getDataStorage()->getSprite("perlinblurred"), "perlinblurred");
    temp->runKernel();
    temp->setInputBuffer(NULL);
    */

    delete[] map_done;
}


void CL_Perlin::cleanup()
{
    CL_Program::cleanup();
    delete[] image_buffer_in;
    delete[] image_buffer_out;
    delete frequency;
    delete persistence;
    delete octaves;
}

void CL_Perlin::postevent()
{
    runKernel();

    app.getGUI()->persistenceString = app.getToolbox()->combineStringAndFloat("Persistence: ", *persistence);
    app.forceredraw();
    app.getWorldGenerator()->formSuperRegions();

    app.getGUI()->persistenceString = app.getToolbox()->combineStringAndFloat("Persistence: ", *persistence);
    app.getGUI()->frequencyString = app.getToolbox()->combineStringAndFloat("frequency: ", *frequency);
    app.getGUI()->octaveString = app.getToolbox()->combineStringAndInt("octaves: ", *octaves);
}

void CL_Perlin::event1()
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
    postevent();
}

void CL_Perlin::event2()
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
    postevent();
}

void CL_Perlin::event3()
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
    postevent();
}

void CL_Perlin::event4()
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
    postevent();
}

void CL_Perlin::event5()
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
    postevent();
}

void CL_Perlin::event6()
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
    postevent();
}

void CL_Perlin::event7()
{
}

void CL_Perlin::event8()
{
}
