const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

float myhypot(float x, float y)
{
    return sqrt((x*x) + (y*y));
}

float mystrangehypot(float x, float y)
{
    return (sqrt((x*x) + (y*y)));
}

// Simple brute force voronoi diagram
__kernel void voronoi(__global float* voronoi_points_x, __global float*  voronoi_points_y, __global float* colors, __global int* data_points, __global float* superregions_x, __global float* superregions_y, __global float* supercolors, __global int* supercount, __write_only image2d_t heightmap, __global float* middle_colors)
{
    // Get the (x,y) coordinates of our desired point
    int x = get_global_id(0);
	int y = get_global_id(1);

	int2 coord = (int2) (x, y);

    float dmin = myhypot(1023, 1023);
    float d = 0.0f;
    int voronoi_cell = 0;

    // Check this point against EVERY datapoint
    // After this loop, the program has solved which of the datapoints is closest
    // to this (x,y) coordinate. This is stupid brute-forcing, but works on the GPU    
    for (int i = 0; i < *data_points; i++)
    {
        d = myhypot(voronoi_points_x[i]-x, voronoi_points_y[i]-y);
        if (d < dmin)
        {
            dmin = d;
            voronoi_cell = i;
        }
    }
    
	float voronoi_color = colors[voronoi_cell];
    float uppercell = middle_colors[voronoi_cell];
	float4 outvalue;
	outvalue.x = voronoi_color;
    
    // Now see to which superregion this point belongs
    
    dmin = myhypot(1023, 1023);
    d = 0.0f;
    int super_cell = 0;
    
    for (int i = 0; i < *supercount; i++)
    {
        d = mystrangehypot(superregions_x[i]-x, superregions_y[i]-y);
        if (d < dmin)
        {
            dmin = d;
            super_cell = i;
        }
    }
    
    float supercol = supercolors[super_cell];
    
	outvalue.y = supercol;
	outvalue.z = 0.0f;
	outvalue.w = 1.0f;
	write_imagef(heightmap, coord, outvalue);
}
