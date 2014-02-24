
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

__kernel void temperature(__read_only image2d_t random_values,  __write_only image2d_t temperaturemap)
{
    // Get the (x,y) coordinates of our desired point
    int x = get_global_id(0);
	int y = get_global_id(1);

	int2 coord = (int2) (x, y);
    int2 s = get_image_dim(temperaturemap);

    float r  = read_imagef(random_values, sampler, coord).x;

    float distance = 0;

    // More distance to the center -> colder
    if (coord.y < s.y / 2.0)
    {
        distance = (coord.y - (s.y/2.0))*-1;
    }
    else
    {
        distance = coord.y - (s.y / 2.0);
    }

    float temperature1 = linear_interpolate(1.0, 0.25, distance / (s.y / 2.0));

    float temperature2 = myabs(sin(convert_float(x / (s.x/16.0)))) * linear_interpolate(0.25, 0.85, myabs(sin(convert_float(x / (s.x/12.0))))) * myabs(sin(y / (s.y / 2.0)));

    float4 outcol;
    outcol.x = temperature1 * linear_interpolate(0.5, 1.0, temperature2) * 255;
    outcol.y = outcol.x;
    outcol.z = outcol.x;
    outcol.w = 255;

	write_imagef(temperaturemap, coord, outcol);
}
