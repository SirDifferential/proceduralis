float blurred_value(int2 coord, __read_only image2d_t input_data)
{
    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

    
    int blur_size = 3;
    float sum = 0.0f;
    
    for (int a = -blur_size; a < blur_size; a++)
    {
        for (int b = -blur_size; b < blur_size; b++)
        {
            int2 coords = (int2)(coord.x + a, coord.y + b);
            sum += read_imagef(input_data, sampler, coords).x;
        }
    }
    
    float out = sum / 49.0f;
    
    
    return out;
}

__kernel void blu(__read_only image2d_t input_map, __write_only image2d_t output_map)
{
    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
    int x = get_global_id(0);
	int y = get_global_id(1);
	
	int2 coord = (int2) (x, y);
    
    float b = blurred_value(coord, input_map);
    float4 outvalue;
    outvalue.x = b;
    outvalue.y = b;
    outvalue.z = b;
    outvalue.w = 1.0f;
    
    write_imagef(output_map, coord, outvalue);
}
