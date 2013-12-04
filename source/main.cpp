#include "application.hpp"
#include "opencl_program.hpp"

int main(int argc, char** argv)
{
    CL_Program c1("simple_world.cl");
    c1.loadProgram();

    c1.runKernel();

    app.readConfig();
    return app.run();
}
