__kernel void simple_world(__global float* random_values, __global float* heightmap)
{
    unsigned int i = get_global_id(0);

    heightmap[i] = random_values[i];
}
