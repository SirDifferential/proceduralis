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

__kernel void satellite(__read_only image2d_t heightmap, __read_only image2d_t biomes, __write_only image2d_t sat)
{
    int x = get_global_id(0);
	int y = get_global_id(1);

	int2 coord = (int2) (x, y);

    int2 s = get_image_dim(biomes);
    float4 outcol;
    outcol.x = 255;
    outcol.y = 0;
    outcol.z = 0;
    outcol.w = 255;

    // palette for biome based color
    int3 biome_pal[] = {
        // Ocean
        {30, 31, 33}, {31, 36, 45}, {32, 45, 40},
        // Arctic
        {255,255,255}, {201,228,239}, {224, 226, 232}, {161, 180, 167},
        // Tundra
        {88, 110, 62}, {135, 151, 90},
        // Taiga
        {48, 73, 45}, {79, 101, 66},
        // Temperate grassland
        {102, 122, 77}, {69, 109, 64},
        // Temperate desert
        {255, 249, 180}, {252, 227, 132}, {206, 131, 68}, {185, 132, 71},
        // Savanna
        {207, 204, 107}, {233, 216, 107}, {227, 213, 111},
        // Tropical decidious forest
        {144, 182, 114}, {67, 102, 67},
        // tropical seasonal forest
        {115, 159, 80}, {72, 110, 67},
        // temperate rain forest
        {41, 61, 40}, {55, 77, 51}, {49, 76, 47},
        // Tropcail rain forest
        {45, 69, 42}, {48, 74, 45}, {60, 89, 56}
        };

    
    // palette for height based color
    int3 height_pal[] = {
        {146, 166, 118},
        {145, 162, 132},
        {162, 179, 151},
        {184, 200, 168},
        {215, 226, 196},
        {158, 169, 149}
        };

    

    float h = read_imagef(heightmap, sampler, coord).x;
    float b = read_imagef(biomes, sampler, coord).x;

    if (h < 1)
    {
        int offset = (coord.x+coord.y) % 2;
        outcol.x = linear_interpolate(biome_pal[0].x, biome_pal[0+offset].x, myabs(sin(coord.x / 256.0)));
        outcol.y = linear_interpolate(biome_pal[0].y, biome_pal[0+offset].y, myabs(sin(coord.x / 256.0)));
        outcol.z = linear_interpolate(biome_pal[0].z, biome_pal[0+offset].z, myabs(sin(coord.x / 256.0)));
    }


	write_imagef(sat, coord, outcol);
}
