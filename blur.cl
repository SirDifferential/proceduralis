float blurred_value(int2 coord, __read_only image2d_t input_data)
{
    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

    
    //int2 tl = (int2)(coord.x-1, coord.y+1);
    //int2 ml = (int2)(coord.x-1, coord.y);
    //int2 bl = (int2)(coord.x-1, coord.y-1);
    //int2 tm = (int2)(coord.x, coord.y+1);
    //int2 bm = (int2)(coord.x, coord.y-1);
    //int2 tr = (int2)(coord.x+1, coord.y+1);
    //int2 mr = (int2)(coord.x+1, coord.y);
    //int2 br = (int2)(coord.x+1, coord.y-1);
    
    int blur_size = 12;
    float sum = 0.0f;
    
    for (int a = -blur_size; a < blur_size; a++)
    {
        for (int b = -blur_size; b < blur_size; b++)
        {
            int2 coords = (int2)(coord.x + a, coord.y + b);
            sum += read_imagef(input_data, sampler, coords).x;
        }
    }
    
    float out = sum / (blur_size*blur_size);
    
    
    return out;
}

__kernel void blur(__read_only image2d_t input_image, __write_only image2d_t output_image)
{
    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
    int x = get_global_id(0);
	int y = get_global_id(1);
	
	int2 coord = (int2) (x, y);
    
    float b = blurred_value(coord, input_image);
    float4 outvalue;
    outvalue.x = b;
    outvalue.y = b;
    outvalue.z = b;
    outvalue.w = 1.0f;
    
    //float4 pixel = read_imagef(input_image, sampler, coord);
    write_imagef(output_image, coord, outvalue);
}
