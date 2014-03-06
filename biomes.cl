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

    // Use an analogy of the Whittaker Diagram for generating approximates of realistic biomes
    // In the usual Whittaker diagram the ranges are about this:
    float preci = linear_interpolate(0, 300, p / 255.0);
    float temp = linear_interpolate(-20, 35, t / 255.0);

    if (h < 1)
    {
        // ocean
        outcol.x = 150;
        outcol.y = 150;
        outcol.z = 255;
        outcol2.x = 0;
    }
    else
    {
        // Use precipitation for primary designation
        if (preci <= 10)
        {
            // Then, based on temperature, figure out what kind of biome this is
            if (temp <= -10)
            {
                // arctic. No plant life
                // Example: Antractic
                // Not an actual real biome in real life, but used for creating a perma-frost area with snow
                outcol2.x = 1;
                outcol.x = 236;
                outcol.y = 242;
                outcol.z = 241;
            }
            else
            {
                // Temperate desert, temperature varies from 0C to 20C
                // Example: Colorado Plateau
                outcol2.x = 4;
                outcol.x = 137;
                outcol.y = 114;
                outcol.z = 64;
            }
        }
        else if (preci <= 50)
        {
            if (temp <= -10)
            {
                // arctic. No plant life
                // Example: Antractic
                // Not an actual real biome in real life, but used for creating a perma-frost area with snow
                outcol2.x = 1;
                outcol.x = 236;
                outcol.y = 242;
                outcol.z = 241;
            }
            else if (temp <= -5)
            {
                // Tundra. Some plant life
                // Examples: northern Canada, Russia and Finland
                // Mostly frozen. Temperature varies from negative celsius to max of 10C
                outcol2.x = 2;
                outcol.x = 198;
                outcol.y = 149;
                outcol.z = 91;
            }
            else if (temp <= 15)
            {
                // Temperate grassland, temperature varies from -40C to 37C
                // Example: Eurasian steppes
                outcol2.x = 3;
                outcol.x = 106;
                outcol.y = 86;
                outcol.z = 51;
            }
            else if (temp <= 20)
            {
                // Temperate desert, temperature varies from 0C to 20C
                // Example: Colorado Plateau
                outcol2.x = 4;
                outcol.x = 137;
                outcol.y = 114;
                outcol.z = 64;
            }
            else
            {
                // Savanna. Temperatures are on average 17C
                // examples: African central parts, South-eastern India, Northern Australia
                outcol2.x = 5;
                outcol.x = 118;
                outcol.y = 109;
                outcol.z = 50;
            }
        }
        else if (preci < 100)
        {
            if (temp <= 15)
            {
                // Taiga, boreal forest. Temperature in range -54C to 30C, mean temperature at 3C
                // Example: Most of Nordic nations, northern Russia
                outcol2.x = 6;
                outcol.x = 63;
                outcol.y = 70;
                outcol.z = 37;
            }
            else if (temp <= 12)
            {
                // Temperate deciduous forest. Mean temperature is 10C
                // Example: northern mainland Europe, eastern North America
                outcol2.x = 7;
                outcol.x = 87;
                outcol.y = 117;
                outcol.z = 45;
            }
            else if (temp <= 20)
            {
                // Temperate desert
                outcol2.x = 4;
                outcol.x = 137;
                outcol.y = 114;
                outcol.z = 64;
            }
            else
            {
                // Savanna
                outcol2.x = 5;
                outcol.x = 118;
                outcol.y = 109;
                outcol.z = 50;
            }
        }
        else if (preci <= 200)
        {
            if (temp <= -15)
            {
                // arctic
                outcol2.x = 1;
                outcol.x = 236;
                outcol.y = 242;
                outcol.z = 241;
            }
            if (temp <= 13)
            {
                // Taiga
                outcol2.x = 6;
                outcol.x = 63;
                outcol.y = 70;
                outcol.z = 37;
            }
            else if (temp <= 20)
            {
                // Temperature Deciduous Forest
                outcol2.x = 7;
                outcol.x = 87;
                outcol.y = 117;
                outcol.z = 45;
            }
            else
            {
                // Tropical Seasonal Forest, average annual temperatures over 20C
                // Trees grow more densely than savanna and lose leaves during the dry season
                // Examples: Large areas of eastern and southern Africa, middle Africa, Southern China, Mexico
                outcol2.x = 8;
                outcol.x = 95;
                outcol.y = 89;
                outcol.z = 41;
            }
        }
        else if (preci <= 400)
        {
            if (temp < -10)
            {
                // Whittaker diagram doesn't consider this combo, but we'll use arctic anyway
                outcol2.x = 1;
                outcol.x = 236;
                outcol.y = 242;
                outcol.z = 241;
            }
            else if (temp < 0)
            {
                // Even though the Whittaker diagram doesn't have biomes for this high rainfall with
                // with this low temperature, there are some anomalies in this simulation that create this combo
                // Use tundra
                outcol2.x = 2;
                outcol.x = 198;
                outcol.y = 149;
                outcol.z = 91;
            }
            else if (temp < 20)
            {
                // Temperate Rain Forest. Mean annual temperature between 4C and 12C
                // north-western United States, Ireland, Japan
                // A heavily forested area that can be cold during some times of the year
                outcol2.x = 9;
                outcol.x = 53;
                outcol.y = 54;
                outcol.z = 22;
            }
            else
            {
                // Tropical rain forest. Temperature in range 20C and 34C
                // What most people imagine when they hear the word "jungle"
                outcol2.x = 10;
                outcol.x = 65;
                outcol.y = 73;
                outcol.z = 34;
            }
        }
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
        outcol.x = 150;
        outcol.y = 150;
        outcol.z = 150;
    }

    outcol.w = 255;

	write_imagef(biomes, coord, outcol);
    write_imagei(biome_codes, coord, outcol2);
}
