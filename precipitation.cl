float linear_interpolate(float x0, float x1, float alpha)
{
    if (alpha > 1.0)
        alpha = 1.0;
    else if (alpha <= 0)
        alpha = 0.0;
    float interpolation = x0 * (1.0 - alpha) + alpha * x1;
    
    return interpolation;
}

float invert_angle(float r)
{
    float inverted = r + 3.14159;

    if (inverted >= 6.28318)
    {
        inverted = inverted - 6.28318;
    }

    return inverted;
}

int2 getNextSector(float rads, int2 dims, int2 current_coords)
{
    int2 current = current_coords;

    float sectorrange = 0.3927;
    if (rads < sectorrange)
    {
        // Go up
        current.y++;
    }
    else if (rads >= sectorrange && rads < sectorrange * 3)
    {
        // Go up right
        current.x++;
        current.y++;
    }
    else if (rads >= sectorrange * 3 && rads < 5 * sectorrange)
    {
        // Go right
        current.x++;
    }
    else if (rads >= 5 * sectorrange && rads < 7 * sectorrange)
    {
        // Go down right
        current.x++;
        current.y--;
    }
    else if (rads >= 7 * sectorrange && rads < 9 * sectorrange)
    {
        // Go down
        current.y++;
    }
    else if (rads >= 9 * sectorrange && rads < 11 * sectorrange)
    {
        // go down left
        current.x--;
        current.y--;
    }
    else if (rads >= 11 * sectorrange && rads < 13 * sectorrange)
    {
        // go left
        current.x--;
    }
    else if (rads >= 13 * sectorrange && rads < 15 * sectorrange)
    {
        // go up left
        current.x--;
        current.y++;
    }
    else
    {
        current.x++;
    }

    if (current.y >= dims.y)
        current.y = dims.y - 6;
    else if (current.y < 0)
        current.y = 6;

    if (current.x >= dims.x)
        current.x = 0;
    else if (current.x < 0)
        current.x = dims.x-1;

    return current;
}

__kernel void precipitation(__read_only image2d_t regionmap, __read_only image2d_t winds, __write_only image2d_t preci)
{
    int x = get_global_id(0);
	int y = get_global_id(1);

    const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_NEAREST;

	int2 coord = (int2) (x, y);
    int2 s = get_image_dim(regionmap);

    // If this is an ocean tile, do nothing
    int original_value = read_imagef(regionmap, sampler, coord).x;
    if (original_value < 5000)
    {
	    float out = 1.0;
	    float4 outvalue;
	    outvalue.x = 1.0;
	    outvalue.y = 1.0;
	    outvalue.z = 1.0;
	    outvalue.w = 0;
	    write_imagef(preci, coord, outvalue);
    }
    else
    {
        int2 s = get_image_dim(regionmap);

        float regioncode = read_imagef(regionmap, sampler, coord).x;

        // Begin searching outwards, upwind to find water
        // Get the wind angle in this area
        float rads = read_imagef(winds, sampler, coord).x;

        int max_steps = 512;
        int current_steps = 0;
        int2 check_coord = coord;
        int2 next_coords;
        int flats_met = 0;
        int hills_met = 0;
        int mountains_met = 0;
        int region_code;
        int ocean_found = 0;
        float inverted_angle = 0;

        while (current_steps < max_steps && ocean_found == 0)
        {
            current_steps++;

            // Get the wind for this area
            rads = read_imagef(winds, sampler, check_coord).x;

            // Invert the wind angle
            inverted_angle = invert_angle(rads);

            // Get the coordinates for the next region upwind
            next_coords = getNextSector(inverted_angle, s, check_coord);

            // Check what kind of tile it is
            region_code = read_imagef(regionmap, sampler, next_coords).x;

            if (region_code < 5000)
            {
                // water found, stop searching
                ocean_found = 1;
            }
            else if (region_code > 5000 && region_code < 10000)
            {
                mountains_met++;
            }
            else if (region_code > 10000 && region_code < 20000)
            {
                hills_met++;
            }
            else
            {
                flats_met++;
            }

            // mark next region for checking
            check_coord = next_coords;
        }

        // Calculate total precipitation based on what was met
        float normalized_distance = linear_interpolate(1.0, 0.01, (current_steps / (float)max_steps));

        float latitude = 0;
        float latitude_coefficient = 0;
        if (coord.y < s.y / 2.0)
        {
            latitude = linear_interpolate(90, 0, coord.y / (s.y / 2.0));
            latitude_coefficient = linear_interpolate(1.0, 0.6, latitude / 90.0);
        }
        else
        {
            latitude = linear_interpolate(0, 90, (coord.y - (s.y/2.0)) / (float)(s.y / 2.0));
            latitude_coefficient = linear_interpolate(1.0, 0.6, latitude / 90.0);
        }

        
        float precipitation = (normalized_distance - (flats_met * 0.0001) + (hills_met * 0.0005) + (mountains_met * 0.025)) * latitude_coefficient;

        precipitation = precipitation * latitude_coefficient;

        if (precipitation > 1.0)
            precipitation = 1.0;
        else if (precipitation <= 0)
            precipitation = 0.001;

	    float4 outvalue;
	    outvalue.x = precipitation;
	    outvalue.y = outvalue.x;
	    outvalue.z = outvalue.x;
	    outvalue.w = 0;
	    write_imagef(preci, coord, outvalue);
    }
}
