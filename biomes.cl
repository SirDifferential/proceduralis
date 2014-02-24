
const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;


float linear_interpolate(float x0, float x1, float alpha)
{
    if (alpha > 1.0)
        alpha = 1.0;
    else if (alpha <= 0)
        alpha = 0.0;
    float interpolation = x0 * (1.0 - alpha) + alpha * x1;
    
    return interpolation;
}


float myabs(float i)
{
    if (i < 0)
        return i*-1;
    return i;
}

__kernel void biomes(__read_only image2d_t heightmap, __read_only image2d_t precipitation, __read_only image2d_t temperature,  __write_only image2d_t biomes)
{
    // Get the (x,y) coordinates of our desired point
    int x = get_global_id(0);
	int y = get_global_id(1);

	int2 coord = (int2) (x, y);

    int2 s = get_image_dim(biomes);

    float h = read_imagef(heightmap, sampler, coord).x;
    float p = read_imagef(precipitation, sampler, coord).x;
    float t = read_imagef(temperature, sampler, coord).x;

    // Lower temperature by height
    float lowered_temp = t * linear_interpolate(1.0, 0.0, h / 140.0);

    

    float4 outcol;
    outcol.x = lowered_temp;
    outcol.y = outcol.x;
    outcol.z = outcol.x;
    outcol.w = 255;

	write_imagef(biomes, coord, outcol);
}
