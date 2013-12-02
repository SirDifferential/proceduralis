#include "application.hpp"
#include "opencl_program.hpp"

int main(int argc, char** argv)
{
    //app.readConfig();
    //return app.run();

    CL_Program p;
    p.runKernel();

    return 0;
}
