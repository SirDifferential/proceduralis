float interpolate(float x0, float x1, float alpha)
{
    float interpolation = x0 * (1.0 - alpha) + alpha * x1;
    return interpolation;
}

float perlinnoise(int2 coord, __read_only image2d_t input_data, int octave)
{
    
    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
    
    float final_noise = 0.0f;

    int x = coord.x;
    int y = coord.y;
    
    int greatest_multiple_x = (coord.x / 32) * 32;
    int greatest_multiple_y = (coord.y / 32) * 32;
    int2 coords = (int2)(greatest_multiple_x, greatest_multiple_y);
    float out = read_imagef(input_data, sampler, coords).x;
    
    return out;
}

__kernel void simple_world(__read_only image2d_t input_map, __write_only image2d_t output_map)
{
    const sampler_t sampler = CLK_NORMALIZED_COORDS_TRUE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
    int x = get_global_id(0);
	int y = get_global_id(1);
	
	int2 coord = (int2) (x, y);
    
    float a = perlinnoise(coord, input_map, 1);
    float4 outvalue;
    outvalue.x = a;
    outvalue.y = a;
    outvalue.z = a;
    outvalue.w = 1.0f;
    
    write_imagef(output_map, coord, outvalue);
}
