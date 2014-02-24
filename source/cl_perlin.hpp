#ifndef __CLPERLIN_HPP_
#define __CLPERLIN_HPP_

#include "opencl_program.hpp"

class CL_Perlin : public CL_Program
{
private:
    // Input and output buffers in CPU memory
    float* image_buffer_in;
    float* image_buffer_out;

    // Input and output buffers in OpenCL memory
    cl::Image2D* image_a;
    cl::Image2D* image_b;

    // Values used for setting offsets in image read / write operations
    cl::size_t<3> origin;
    cl::size_t<3> region;
    size_t row_pitch;

    // OpenCL buffers for constants used in this program
    cl::Buffer cl_frequency;
    cl::Buffer cl_persistence;
    cl::Buffer cl_octaves;

    // And application-side constants for setting the OpenCL constants
    float* frequency;
    float* persistence;
    int* octaves;

public:
    CL_Perlin(std::string s);
    void init();
    void runKernel();
    void cleanup();

    void postevent();

    // Events for changing constants
    void event1();
    void event2();
    void event3();
    void event4();
    void event5();
    void event6();
    void event7();
    void event8();
};

#endif
