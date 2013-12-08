// http://vimeo.com/29074357

float Cosine_Interpolate(float a, float b, float x)
{
	float ft = x * 3.1415927f;
	float f = (1.0f - cos(ft)) * 0.5f;

	return  a * ( 1 - f ) + b * f;
}

float Noise(int x, int y)
{
	int n = x + y * 57;
	n = ( n << 13 ) ^ n;
	return ( 1.f - ( ( n * ( n * n * 15731 + 789221 ) + 1376312589 ) & 0x7fffffff ) / 1073741824.0 );    
}

float SmoothedNoise1(float x, float y)
{
	float corners = ( Noise(x-1, y-1)+Noise(x+1, y-1)+Noise(x-1, y+1)+Noise(x+1, y+1) ) / 16.0f;
	float sides   = ( Noise(x-1, y)  +Noise(x+1, y)  +Noise(x, y-1)  +Noise(x, y+1) ) /  8.0f;
	float center  =  Noise(x, y) / 4.0f;
	return corners + sides + center;
}

float InterpolatedNoise1(float x, float y)
{
	int integer_X    = (int)floor(x);
	float fractional_X = fabs (x - integer_X);

	int integer_Y    = (int)floor(y);
	float fractional_Y = fabs(y - integer_Y);

	float v1 = SmoothedNoise1(integer_X,     integer_Y);
	float v2 = SmoothedNoise1(integer_X + 1, integer_Y);
	float v3 = SmoothedNoise1(integer_X,     integer_Y + 1);
	float v4 = SmoothedNoise1(integer_X + 1, integer_Y + 1);

	float i1 = Cosine_Interpolate(v1 , v2 , fractional_X);
	float i2 = Cosine_Interpolate(v3 , v4 , fractional_X);

	return Cosine_Interpolate(i1 , i2 , fractional_Y);
}

float GetPerlin( float x, float y, float frequency, float persistence, int octaves)
{
	float total = 0.0f;
	float p = persistence;
	float freq = frequency;
	int n = octaves -1;

	for( int i = 0; i < n; ++i )
	{
		float frequency2 = pow(freq, (float)i);
		float amplitude = pow(p, (float)i);

		total = total + InterpolatedNoise1(x * frequency2, y * frequency2) * amplitude;
	}

	return total;
}

__kernel void perlinnoise(__read_only image2d_t random_values, __write_only image2d_t heightmap, __global float* frequency, __global float* persistence, __global int* octaves)
{
    int x = get_global_id(0);
	int y = get_global_id(1);

	int2 coord = (int2) (x, y);

	float out = GetPerlin(x, y, *frequency, *persistence, *octaves);
	float4 outvalue;
	outvalue.x = out;
	outvalue.y = out;
	outvalue.z = out;
	outvalue.w = 1.0f;
	write_imagef(heightmap, coord, outvalue);
}
