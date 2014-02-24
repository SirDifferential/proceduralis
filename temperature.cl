
const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

__kernel void temperature(__read_only image2d_t random_values,  __write_only image2d_t windmap)
{
    // Get the (x,y) coordinates of our desired point
    int x = get_global_id(0);
	int y = get_global_id(1);

	int2 coord = (int2) (x, y);
    int2 s = get_image_dim(windmap);
    
    float4 outcol;
    outcol.x = 1.0;
    outcol.y = 1.0;
    outcol.x = 1.0;
    outcol.w = 1.0;

	write_imagef(windmap, coord, outcol);
}
