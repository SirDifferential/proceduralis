
__kernel void simplexnoise(__read_only image2d_t random_values, __write_only image2d_t heightmap, __global float* frequency, __global float* persistence, __global int* octaves)
{
    int x = get_global_id(0);
	int y = get_global_id(1);

	int2 coord = (int2) (x, y);

	float out = octave_noise_2d(*octaves, *persistence, *frequency, x, y);
	float4 outvalue;
	outvalue.x = out;
	outvalue.y = out;
	outvalue.z = out;
	outvalue.w = 1.0f;
	write_imagef(heightmap, coord, outvalue);
}

