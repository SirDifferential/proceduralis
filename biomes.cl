
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

__kernel void biomes(__read_only image2d_t heightmap, __read_only image2d_t precipitation, __read_only image2d_t temperature,  __write_only image2d_t biomes)
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

    float h = read_imagef(heightmap, sampler, coord).x;
    float p = read_imagef(precipitation, sampler, coord).x;
    float t = read_imagef(temperature, sampler, coord).x;

    // Lower temperature by height
    float lowered_temp = t * linear_interpolate(1.0, 0.0, h / 140.0);


    // 102 or higher: mountains
    // 86-102: hills
    // 86 - 0: flats

    // First check if the height is great enough to simply make this area into mountain biome without any further considerations

    if (h > 100)
    {
        if (lowered_temp < 20)
        {
            outcol.x = 252;
            outcol.y = 248;
            outcol.z = 204;
        }
        else
        {
            outcol.x = 233;
            outcol.y = 187;
            outcol.z = 129;
        }
    }
    else if (h > 1)
    {
        if (lowered_temp > 150 && p < 50)
        {
            // Very dry desert
            outcol.x = 255;
            outcol.y = 209;
            outcol.z = 150;
        }
        else if (lowered_temp > 150 && p < 100)
        {
            // desert
            outcol.x = 230;
            outcol.y = 167;
            outcol.z = 122;
        }
        else if (lowered_temp > 150 && p < 150)
        {
            // dry savanna
            outcol.x = 103;
            outcol.y = 94;
            outcol.z = 56;
        }
        else if (lowered_temp > 150 && p < 200)
        {
            // wet savanna
            outcol.x = 73;
            outcol.y = 79;
            outcol.z = 46;
        }
        else if (lowered_temp > 150 && p < 255)
        {
            // jungle
            outcol.x = 34;
            outcol.y = 59;
            outcol.z = 34;
        }

        else if (lowered_temp < 20)
        {
            // Arctic glacier
            outcol.x = 188;
            outcol.y = 219;
            outcol.z = 239;
        }
        else if (lowered_temp < 40)
        {
            // arctic
            outcol.x = 251;
            outcol.y = 249;
            outcol.z = 242;
        }
        else if (lowered_temp < 60 && p < 50)
        {
            // dry arctic-temperate
            outcol.x = 30;
            outcol.y = 32;
            outcol.z = 16;
        }
        else if (lowered_temp < 60 && p > 50)
        {
            // wet arctic-temperatate
            outcol.x = 46;
            outcol.y = 53;
            outcol.z = 36;
        }
        else if (lowered_temp < 100 && p < 50)
        {
            // dry temperate
            outcol.x = 42;
            outcol.y = 63;
            outcol.z = 30;
        }
        else if (lowered_temp < 100 && p > 50)
        {
            // wet temperate
            outcol.x = 69;
            outcol.y = 86;
            outcol.z = 56;
        }
        else if (lowered_temp < 130 && p < 80)
        {
            // dry mediterranean
            outcol.x = 141;
            outcol.y = 116;
            outcol.z = 73;
        }
        else if (lowered_temp < 130 && p > 80)
        {
            // wet mediterranean
            outcol.x = 90;
            outcol.y = 118;
            outcol.z = 54;
        }
        else if (lowered_temp < 255)
        {
            // tropical dry
            outcol.x = 181;
            outcol.y = 126;
            outcol.z = 99;
        }
    }
    else
    {
        outcol.x = 150;
        outcol.y = 150;
        outcol.z = 255;
    }

    outcol.w = 255;

	write_imagef(biomes, coord, outcol);
}
