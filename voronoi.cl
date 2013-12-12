const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

float myhypot(float x, float y)
{
    return sqrt((x*x) + (y*y));
}

__kernel void voronoi(__global float* input_data_x, __global float* input_data_y, __global float* colors, __global int* data_points, __write_only image2d_t heightmap)
{
    int x = get_global_id(0);
	int y = get_global_id(1);

	int2 coord = (int2) (x, y);

    float dmin = myhypot(1023, 1023);
    float d = 0.0f;
    int j = 0;
    
    for (int i = 0; i < *data_points; i++)
    {
        d = myhypot(input_data_x[i]-x, input_data_y[i]-y);
        if (d < dmin)
        {
            dmin = d;
            j = i;
        }
    }
    
	float out = colors[j];
	float4 outvalue;
	outvalue.x = out;
	outvalue.y = out;
	outvalue.z = out;
	outvalue.w = 1.0f;
	write_imagef(heightmap, coord, outvalue);
}
