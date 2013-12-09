#ifndef __APPLICATIONFLAGS_
#define __APPLICATIONFLAGS_

class ApplicationFlags
{
public:
    ApplicationFlags();

    bool opencl_devices_debug;
    bool use_GPU;
    bool print_cl_programs;
};

#endif
