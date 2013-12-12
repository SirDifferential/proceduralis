// http://stackoverflow.com/questions/2755750/diamond-square-algorithm

const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

__kernel void diamondsquare(__read_only image2d_t random_values, __write_only image2d_t heightmap, __global float* frequency, __global float* persistence, __global int* octaves)
{
    int2 dimensions = get_image_dim(heightmap);
    int width = dimensions.x;
    int height = dimensions.y;
    int data_size = width+1;
    float seed = 1000;

    int2 top_left = (int2)(0, 0);
    int2 top_right = (int2)(width-1, 0);
    int2 bottom_left = (int2)(0, height-1);
    int2 bottom_right = (int2)(width-1, height-1);

    float4 seedvalue;
    seedvalue.x = seed;
    seedvalue.y = seed;
    seedvalue.z = seed;
    seedvalue.w = seed;

    write_imagef(heightmap, top_left, seedvalue);
    write_imagef(heightmap, top_right, seedvalue);
    write_imagef(heightmap, bottom_left, seedvalue);
    write_imagef(heightmap, bottom_right, seedvalue);

    float h = seed / 2;
    for (int sideLength = data_size-1; sideLength >= 2; sideLength /= 2, h /= 2.0)
    {
        int halfSide = sideLength/2;
        for (int x = 0; x < data_size-1; x += sideLength)
        {
            for (int y = 0; y < data_size-1; y += sideLength)
            {
                
            }
        }
    }
}
