#include "application.hpp"
#include "opencl_program.hpp"

int main(int argc, char** argv)
{
    //app.readConfig();
    //return app.run();

    CL_Program c1("lesson1.cl");
    c1.loadProgram();

    c1.runKernel();

    return 0;
}
