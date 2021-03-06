#ifndef __CLWINDDIR_HPP_
#define __CLWINDDIR_HPP_

#include "opencl_program.hpp"

class CL_Winddir : public CL_Program
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

public:
    CL_Winddir(std::string s);
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
