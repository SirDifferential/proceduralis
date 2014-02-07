const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

float Cosine_Interpolate(float a, float b, float x)
{
	float ft = x * 3.1415927f;
	float f = (1.0f - cos(ft)) * 0.5f;

	return  a * ( 1 - f ) + b * f;
}

// Some crazy mathematical function I found on the interwebs
float Noise(int x, int y)
{
	int n = x + y * 57;
	n = ( n << 13 ) ^ n;
	return ( 1.f - ( ( n * ( n * n * 15731 + 789221 ) + 1376312589 ) & 0x7fffffff ) / 1073741824.0 );
}

// Random data from an initialized image
float4 imageNoise(int x, int y, __read_only image2d_t random_values)
{
    int2 coords = (int2)(x, y);
    int2 s = get_image_dim(random_values);
    if (coords.x < 0)
        coords.x = 0;
    else if (coords.x >= s.x)
        coords.x = s.x - 1;
    if (coords.y < 0)
        coords.y = 0;
    else if (coords.y >= s.y)
        coords.y = s.y - 1;
    
    
    return read_imagef(random_values, sampler, coords);
}

float SmoothedNoise1(float x, float y, __read_only image2d_t random_values)
{
	float corners = ( imageNoise(x-1, y-1, random_values).x+imageNoise(x+1, y-1, random_values).x+imageNoise(x-1, y+1, random_values).x+imageNoise(x+1, y+1, random_values).x ) / 16.0f;
	float sides   = ( imageNoise(x-1, y, random_values).x  +imageNoise(x+1, y, random_values).x  +imageNoise(x, y-1, random_values).x  +imageNoise(x, y+1, random_values) ).x /  8.0f;
	float center  =  imageNoise(x, y, random_values).x / 4.0f;
	return corners + sides + center;
}

float InterpolatedNoise1(float x, float y, __read_only image2d_t random_values)
{
	int integer_X    = (int)floor(x);
	float fractional_X = fabs (x - integer_X);

	int integer_Y    = (int)floor(y);
	float fractional_Y = fabs(y - integer_Y);

	float v1 = SmoothedNoise1(integer_X,     integer_Y, random_values);
	float v2 = SmoothedNoise1(integer_X + 1, integer_Y, random_values);
	float v3 = SmoothedNoise1(integer_X,     integer_Y + 1, random_values);
	float v4 = SmoothedNoise1(integer_X + 1, integer_Y + 1, random_values);

	float i1 = Cosine_Interpolate(v1 , v2 , fractional_X);
	float i2 = Cosine_Interpolate(v3 , v4 , fractional_X);

	return Cosine_Interpolate(i1 , i2 , fractional_Y);
}

float GetPerlin( float x, float y, float frequency, float persistence, int octaves, __read_only image2d_t random_values)
{
	float total = 0.0f;
	float p = persistence;
	float freq = frequency;
	int n = octaves -1;

	for( int i = 0; i < n; ++i )
	{
		float frequency2 = pow(freq, (float)i);
		float amplitude = pow(p, (float)i);

		total = total + InterpolatedNoise1(x * frequency2, y * frequency2, random_values) * amplitude;
	}

	return total;
}

float linear_interpolate(float x0, float x1, float alpha)
{
    if (alpha > 1.0)
        alpha = 1.0;
    else if (alpha <= 0)
        alpha = 0.0;
    float interpolation = x0 * (1.0 - alpha) + alpha * x1;
    
    return interpolation;
}

// Calculates the direction of wind based on 7 wind zones
// This is performed by first checking in which area our desired coordinate is,
// and then performing a linear interpolation between the two ends of that wind area
// Wind directions are described by radians
float getLatitudeWinddir(float coord_y)
{
    // North pole: south, 3.14159 rads
    // North circle: north, 0 rads
    // North tropic: east, 1.570795 rads
    // Equator: west 4.712385 rads
    // South tropic: east,  1.570795 rads
    // South circle: south, 3.14159 rads
    // South pole: north, 0 rads

    // north pole: +90 degrees
    // north circle: +66.33 degrees
    // north tropic: +23.27 degrees
    // equator: 0 degrees
    // south tropic: -23.27 degrees
    // south circle: -66.33 degrees
    // south pole: -90 degrees

    // Convert the height coordinate from (0...1) to the range of -90...90
    float latitude = linear_interpolate(90, -90, coord_y);

    if (latitude > 66 && latitude < 67)
        return 100;
    if (latitude > 23 && latitude < 24)
        return 100;
    if (latitude > 0 && latitude < 1)
        return 100;
    if (latitude <  -23 && latitude > -24)
        return 100;
    if (latitude <  -66 && latitude > -67)
        return 100;

    // For each latitude range, calculate influence of that wind region
    if (latitude >= 66.33)
    {
        // North pole - north cicle
        // First figure out relative position to the end points of this region (0...1)
        float latitude_influence = (90.0 - latitude) / 23.67; // 90 - 63.66
        // Then calculate the total wind direction by using linear interpolation between the two wind points
        return linear_interpolate(3.14159, 0, latitude_influence);
    }
    else if (latitude < 66.33 && latitude >= 23.27)
    {
        // North circle - north tropic
        float latitude_influence = (63.33 - latitude) / 43.06; // 66.33 - 23.27
        return linear_interpolate(0, 1.570795, latitude_influence);
    }
    else if (latitude < 23.27 && latitude >= 0)
    {
        // North tropic - equator
        float latitude_influence = (23.27 - latitude) / 23.27; // 23.27 - 0
        return linear_interpolate(1.570795, 4.712385, latitude_influence);
    }
    else if (latitude < 0 && latitude >= -23.27)
    {
        // Equator - south tropic
        float latitude_influence = latitude / -23.27; // 23.27 - 0
        return linear_interpolate(4.712385, 1.570795, latitude_influence);
    }
    else if (latitude < -23.27 && latitude >= -66.33)
    {
        // South tropic - south circle
        float latitude_influence = (-22.27 - latitude) / 43.06; // 66.33 - 23.27
        return linear_interpolate(1.570795, 3.14159, latitude_influence);
    }
    else
    {
        // South circle - south pole
        float latitude_influence = (-66.33 - latitude) / 23.67; // 90 - 63.66;
        return linear_interpolate(3.14159, 0, latitude_influence);
    }

}

__kernel void winddirection(__read_only image2d_t random_values,  __write_only image2d_t windmap)
{
    // Get the (x,y) coordinates of our desired point
    int x = get_global_id(0);
	int y = get_global_id(1);

	int2 coord = (int2) (x, y);
    int2 s = get_image_dim(windmap);

    float frequency = 0.06;
    float persistence = 1.7;
    int octaves = 8;

	float perlin = GetPerlin(x, y, frequency, persistence, octaves, random_values);

    float angle_rads = 0.0;

    // Convert y coordinate to 0...1 range
    float coord_y = coord.y / (float)s.y;

    float interpolated_direction = getLatitudeWinddir(coord_y);

    float t = interpolated_direction;

    float4 outcol;
    outcol.x = 0;
    outcol.y = 0;
    outcol.z = 0;
    outcol.w = 0;

    t *= perlin;

    // North pole: south, 3.14159 rads
    // North circle: north, 0 rads
    // North tropic: east, 1.570795 rads
    // Equator: west 4.712385 rads
    // South tropic: east,  1.570795 rads
    // South circle: south, 3.14159 rads
    // South pole: north, 0 rads

    if (t == 100)
    {
        outcol.x = 50;
        outcol.y = 255;
        outcol.z = 150;
    }
    else if (t < 1.570795)
        outcol.x = linear_interpolate(0.5, 1.0, t / 1.570795);
    else if (t > 1.570795 && t < 3.14159)
        outcol.y = linear_interpolate(0.5, 1.0, (3.14159 - t) / 1.570795);
    else if (t > 3.14159 && t < 4.712385)
        outcol.z = linear_interpolate(0.5, 1.0, (4.712385 - t) / 1.570795);
    else if (t > 4.712385 && t < 6)
    {
        outcol.x = 0.5;
        outcol.y = 0.5,
        outcol.z = 0.5;
    }
    else
    {

        outcol.x = 0.0;
        outcol.y = 0.0,
        outcol.z = 0.0;
    }

	write_imagef(windmap, coord, outcol);
}
