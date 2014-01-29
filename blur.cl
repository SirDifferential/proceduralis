float4 blurred_value(int2 coord, __read_only image2d_t input_data, __global int* blursize)
{
    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

    int blur_size = *blursize;
    int blur_size_squared = (2*blur_size)*(2*blur_size);
    float sum_x = 0.0f;
    float sum_y = 0.0f;
    float sum_z = 0.0f;
    float sum_a = 0.0f;
    
    int2 s = get_image_dim(input_data);

    for (int a = -blur_size; a <= blur_size; a++)
    {
        for (int b = -blur_size; b <= blur_size; b++)
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

float4 gaussianBlur(int2 coord, __read_only image2d_t input_data, __global int* blursize, __read_only image2d_t blurkernel)
{

    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

    int blur_size = *blursize;
    float4 color;
    float multiplier = 0.0f;
    float sum_x = 0.0f;
    float sum_y = 0.0f;
    float sum_z = 0.0f;
    float sum_w = 0.0f;

    for (int i = -blur_size/2; i < blur_size/2; i++)
    {
        for (int j = -blur_size/2; j < blur_size/2; j++)
        {
            int2 kernelcoords = (int2) (i + blur_size/2, j + blur_size/2);
            int2 coords = (int2)(coord.x + i, coord.y + j);

            color = read_imagef(input_data, sampler, coords);
            multiplier = read_imagef(blurkernel, sampler, kernelcoords).x;
            sum_x += color.x * multiplier;
            sum_y += color.y * multiplier;
            sum_z += color.z * multiplier;
            sum_w += color.w * multiplier;
        }
    }

    float4 out;
    out.x = sum_x;
    out.y = sum_y;
    out.z = sum_z;
    out.w = sum_w;
    return out;
}

__kernel void blur(__read_only image2d_t input_image, __write_only image2d_t output_image, __global int* blursize, __read_only image2d_t blurkernel)
{
    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;
    int x = get_global_id(0);
	int y = get_global_id(1);
	
	int2 coord = (int2) (x, y);
    
    float4 outvalue = gaussianBlur(coord, input_image, blursize, blurkernel);

    write_imagef(output_image, coord, outvalue);
}
