#ifndef __CLBLUR_HPP_
#define __CLBLUR_HPP_

#include "opencl_program.hpp"

class CL_Blur : public CL_Program
{
private:
    // Input and output buffers in CPU memory
    float* image_buffer_in;
    float* image_buffer_out;
    int* blur_size;
    float** gaussKernel;
    double kernelSigma;

    // Input and output buffers in OpenCL memory
    cl::Image2D* image_a;
    cl::Image2D* image_b;
    cl::Buffer cl_blur_size;
    cl::Image2D* cl_gaussKernel;

    // Values used for setting offsets in image read / write operations
    cl::size_t<3> origin;
    cl::size_t<3> region;
    size_t row_pitch;

    bool rerangeOutput;

public:
    CL_Blur(std::string s);
    void init();
    void runKernel();
    void cleanup();

    void createGaussKernel(int rad, double sig);

    void setInputBuffer(float* in);
    void resetInputBuffer();
    void setBlurSize(int i);
    void setRerange(bool i);

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
