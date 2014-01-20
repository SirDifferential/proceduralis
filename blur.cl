float4 blurred_value(int2 coord, __read_only image2d_t input_data, __global int* blursize)
{
    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

    int blur_size = *blursize;
    int blur_size_squared = blur_size*blur_size;
    float sum_x = 0.0f;
    float sum_y = 0.0f;
    float sum_z = 0.0f;
    float sum_a = 0.0f;
    
    int2 s = get_image_dim(input_data);

    for (int a = -blur_size; a < blur_size; a++)
    {
        for (int b = -blur_size; b < blur_size; b++)
        {
            int2 coords = (int2)(coord.x + a, coord.y + b);
            if (coords.x < 0)
                coords.x = 0;
            else if (coords.x >= s.x)
                coords.x = s.x - 1;
            if (coords.y < 0)
                coords.y = 0;
            else if (coords.y >= s.y)
                coords.y = s.y -1;
            sum_x += read_imagef(input_data, sampler, coords).x;
            sum_y += read_imagef(input_data, sampler, coords).y;
            sum_z += read_imagef(input_data, sampler, coords).z;
            sum_a += read_imagef(input_data, sampler, coords).w;
        }
    }
    
    float4 out;
    out.x = sum_x / blur_size_squared;
    out.y = sum_y / blur_size_squared;
    out.z = sum_z / blur_size_squared;
    out.w = sum_a / blur_size_squared;
    
    return out;
}

__kernel void blur(__read_only image2d_t input_image, __write_only image2d_t output_image, __global int* blursize)
{
    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
    int x = get_global_id(0);
	int y = get_global_id(1);
	
	int2 coord = (int2) (x, y);
    
    float4 outvalue = blurred_value(coord, input_image, blursize);

    write_imagef(output_image, coord, outvalue);
}
