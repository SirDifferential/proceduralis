#ifndef __OPENCL_PROGRAM_HPP_
#define __OPENCL_PROGRAM_HPP_

#include "cl.hpp"

class CL_Program
{
private:
public:
    CL_Program();

    int checkError(cl_int error, const char* name);

    void test();
};

#endif
