#ifndef __CLVORONOI_HPP_
#define __CLVORONOI_HPP_

#include "opencl_program.hpp"

class CL_Voronoi : public CL_Program
{
private:
    // Image buffers in CPU memory
    float* image_buffer_in;
    float* image_buffer_out;

    // Image buffers in OpenCL memory
    cl::Image2D* image_a;
    cl::Image2D* image_b;

    // Offsets for read / write operations
    cl::size_t<3> origin;
    cl::size_t<3> region;
    size_t row_pitch;

    // Constants used by the OpenCL program, in CPU memory
    float* input_data_x;
    float* input_data_y;
    int* data_points;
    float* colors;

    // Constants in OpenCL memory
    cl::Buffer cl_data_points;
    cl::Buffer cl_input_a;
    cl::Buffer cl_input_b;
    cl::Buffer cl_colors;

public:
    CL_Voronoi(std::string s);
    void loadProgram();
    void runKernel();
    void cleanup();

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
