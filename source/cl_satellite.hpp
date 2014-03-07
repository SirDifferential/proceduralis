#ifndef __CLSATELLITE_HPP_
#define __CLSATELLITE_HPP_

#include "opencl_program.hpp"

class CL_Satellite : public CL_Program
{
private:
    // Input and output buffers in CPU memory
    float* height;
    float* biomes;

    // Input and output buffers in OpenCL memory
    cl::Image2D* cl_height;
    cl::Image2D* cl_biomes;
    cl::Image2D* cl_satellite;

    // Values used for setting offsets in image read / write operations
    cl::size_t<3> origin;
    cl::size_t<3> region;
    size_t row_pitch;

public:
    CL_Satellite(std::string s);
    void init();
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
