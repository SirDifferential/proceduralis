const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

float linear_interpolate(float x0, float x1, float alpha)
{
    if (alpha > 1.0)
        alpha = 1.0;
    else if (alpha <= 0)
        alpha = 0.0;
    float interpolation = x0 * (1.0 - alpha) + alpha * x1;
    
    return interpolation;
}


float myabs(float i)
{
    if (i < 0)
        return i*-1;
    return i;
}

__kernel void biomes(__read_only image2d_t heightmap, __read_only image2d_t precipitation, __read_only image2d_t temperature,  __write_only image2d_t biomes, __write_only image2d_t biome_codes)
{
    // Get the (x,y) coordinates of our desired point
    int x = get_global_id(0);
	int y = get_global_id(1);

	int2 coord = (int2) (x, y);

    int2 s = get_image_dim(biomes);
    float4 outcol;
    outcol.x = 0;
    outcol.y = 0;
    outcol.z = 0;

    int4 outcol2;

    float h = read_imagef(heightmap, sampler, coord).x;
    float p = read_imagef(precipitation, sampler, coord).x;
    float t = read_imagef(temperature, sampler, coord).x;

    // Lower temperature by height
    float height_temp = linear_interpolate(1.0, 0.0, h / 140.0);

    // Use an analogy of the Whittaker Diagram for generating approximates of realistic biomes
    // In the usual Whittaker diagram the ranges are about this:
    float preci = linear_interpolate(0, 300, p);
    float temp = linear_interpolate(-20, 35, t * height_temp / 255.0);

    if (h < 1)
    {
        // ocean
        outcol.x = 150;
        outcol.y = 150;
        outcol.z = 255;
        outcol2.x = 0;
    }
    else if (temp < -5)
    {
        // tundra
        outcol.x = 236;
        outcol.y = 243;
        outcol.z = 245;
        outcol2.x = 1;
    }
    else if (temp >= -5 && temp <= 5 && preci > 50)
    {
        // taiga
        outcol.x = 62;
        outcol.y = 94;
        outcol.z = 62;
        outcol2.x = 2;
    }
    else if (temp > -5 && temp <= 20 && preci <= 120)
    {
        // temperate grassland
        outcol.x = 70;
        outcol.y = 113;
        outcol.z = 60;
        outcol2.x = 3;
    }
    else if (temp > -5 && preci <= 50)
    {
        // subtropical desert
        outcol.x = 138;
        outcol.y = 131;
        outcol.z = 76;
        outcol2.x = 4;
    }
    else if (temp > 15 && preci <= 100)
    {
        // savanna
        outcol.x = 214;
        outcol.y = 206;
        outcol.z = 104;
        outcol2.x = 5;
    }
    else if (temp > 0 && temp <= 20 && preci <= 200)
    {
        // temperate deciduous forest
        outcol.x = 127;
        outcol.y = 169;
        outcol.z = 110;
        outcol2.x = 6;
    }
    else if (temp > 20 && preci <= 230)
    {
        // tropical seasonal forest
        outcol.x = 64;
        outcol.y = 92;
        outcol.z = 65;
        outcol2.x = 7;
    }
    else if (temp > 5 && temp <= 20 && preci > 150)
    {
       // temperate rain forest
        outcol.x = 54;
        outcol.y = 79;
        outcol.z = 48;
        outcol2.x = 8;
    }
    else if (temp > 20 && preci > 230)
    {
        // tropical rain forest
        outcol.x = 56;
        outcol.y = 85;
        outcol.z = 55;
        outcol2.x = 9;
    }


    // 102 or higher: mountains
    // 86-102: hills
    // 86 - 1: flats

    if (h < 1)
    {
        outcol2.y = 0;
    }
    else if (h <= 86)
    {
        // flats
        outcol2.y = 1;
    }
    else if (h > 86 && h <= 102)
    {
        // mountain-y
        outcol2.y = 2;
    }
    else
    {
        // mountains
        outcol2.y = 3;
    }

    outcol.w = 255;

	write_imagef(biomes, coord, outcol);
    write_imagei(biome_codes, coord, outcol2);
}
