__kernel void simple_world(__read_only image2d_t random_values, __write_only image2d_t heightmap)
{
    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
    int x = get_global_id(0);
	int y = get_global_id(1);
	
	int2 coord = (int2) (x, y);

    float4 pixel = read_imagef(random_values, sampler, coord);
    //pixel.x = 0.2;
    //pixel.y = 0.5;
    //pixel.z = 0.8;
    //pixel.w = 1.0f;
    write_imagef(heightmap, coord, pixel);
}
