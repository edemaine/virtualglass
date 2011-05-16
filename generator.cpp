#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "primitives.h"
#include "constants.h"
#include "cane.h"

#ifdef UNDEF
Cane* make_zanfirico_ballottini_N()
{
        Cane* white, * stretched_white, * white_line, * twisted_line;
        int i;

        white = init_cane();
        white->color.r = 1.0;
        white->color.g = 1.0;
        white->color.b = 1.0;

        stretched_white = init_cane();
        stretched_white->num_subcanes = 1;
        stretched_white->subcanes[0] = white;
        stretched_white->stretch = 0.0015;

        white_line = init_cane();
        white_line->num_subcanes = 7;
        for (i = 0; i < 7; ++i)
        {
                white_line->subcanes[i] = stretched_white;
                if (i < 3)
                {
                        white_line->subcane_locs[i].r = (i + 1) / 48.0; 
                        white_line->subcane_locs[i].theta = 0;
                }
                else
                {
                        white_line->subcane_locs[i].r = (i - 3.0) / 48.0; 
                        white_line->subcane_locs[i].theta = 3.141;
                }
        }

        twisted_line = init_cane();
        twisted_line->num_subcanes = 1;
        twisted_line->subcanes[0] = white_line;
        twisted_line->twist = 3.0;

        return twisted_line; 
}


Cane* make_zanfirico_nastri_S()
{
        Cane* white, * stretched_white, * white_tunnel;
        Cane* blue, * stretched_blue;
        Cane* nested_base, * stretched_base, * twisted_base;
        int i;

        white = init_cane();
        white->color.r = 1.0;
        white->color.g = 1.0;
        white->color.b = 1.0;

        stretched_white = init_cane();
        stretched_white->num_subcanes = 1;
        stretched_white->subcanes[0] = white;
        stretched_white->stretch = 0.03;

        white_tunnel = init_cane();
        white_tunnel->num_subcanes = 24;
        for (i = 0; i < 24; ++i)
        {
                white_tunnel->subcanes[i] = stretched_white;
                white_tunnel->subcane_locs[i].r = 1;
                white_tunnel->subcane_locs[i].theta = 2 * PI / 24 * i;;
        }
 
        blue = init_cane();
        blue->color.r = 0.5;
        blue->color.g = 0.5;
        blue->color.b = 1.0;

        stretched_blue = init_cane();
        stretched_blue->num_subcanes = 1;
        stretched_blue->subcanes[0] = blue;
        stretched_blue->stretch = 0.4;
      
        nested_base = init_cane();
        nested_base->num_subcanes = 2;
        nested_base->subcanes[0] = stretched_blue;
        nested_base->subcanes[1] = white_tunnel;

        stretched_base = init_cane();
        stretched_base->num_subcanes = 1;
        stretched_base->subcanes[0] = nested_base;
        stretched_base->stretch = 0.05;

        twisted_base = init_cane();
        twisted_base->num_subcanes = 1;
        twisted_base->subcanes[0] = stretched_base;
        twisted_base->subcane_locs[0].r = 0.03;
        twisted_base->subcane_locs[0].theta = 0;
        twisted_base->twist = -2.5;
 
        return twisted_base;
}

Cane* make_wiki_cane()
{
        Cane* white, * yellow, * light_brown, * light_blue, * twisted_bundle;
        Cane* stretched_white, * stretched_yellow, * stretched_light_brown, * stretched_light_blue;
        int i;

        // Create stock colored canes
        white = init_cane();
        white->color.r = 1.0;
        white->color.g = 1.0;
        white->color.b = 1.0;

        yellow = init_cane();
        yellow->color.r = 1.0;
        yellow->color.g = 0.95;
        yellow->color.b = 0.7;

        light_brown = init_cane();
        light_brown->color.r = 0.8; 
        light_brown->color.g = 0.5; 
        light_brown->color.b = 0.3; 

        light_blue = init_cane();
        light_blue->color.r = 0.4; 
        light_blue->color.g = 0.4; 
        light_blue->color.b = 0.8; 

        stretched_white = init_cane();
        stretched_white->stretch = 0.005;
        stretched_white->num_subcanes = 1;
        stretched_white->subcanes[0] = white;
         
        stretched_yellow = init_cane();
        stretched_yellow->stretch = 0.002;
        stretched_yellow->num_subcanes = 1;
        stretched_yellow->subcanes[0] = yellow;

        stretched_light_blue = init_cane();
        stretched_light_blue->stretch = 0.002;
        stretched_light_blue->num_subcanes = 1;
        stretched_light_blue->subcanes[0] = light_blue;

        stretched_light_brown = init_cane();
        stretched_light_brown->stretch = 0.002;
        stretched_light_brown->num_subcanes = 1;
        stretched_light_brown->subcanes[0] = light_brown;

        // Create bundle
        twisted_bundle = init_cane();
        twisted_bundle->num_subcanes = 34; // It's a bundle of 34 smaller canes
        twisted_bundle->twist = 2.0; 

        // Setup bundle information
        twisted_bundle->subcanes[0] = stretched_light_blue;
        twisted_bundle->subcanes[1] = stretched_light_blue;
        twisted_bundle->subcanes[2] = stretched_light_blue;
        twisted_bundle->subcanes[3] = stretched_light_blue;
        twisted_bundle->subcanes[4] = stretched_light_blue;
        twisted_bundle->subcanes[5] = stretched_light_blue;
        twisted_bundle->subcanes[6] = stretched_light_blue;
        twisted_bundle->subcanes[7] = stretched_yellow;
        twisted_bundle->subcanes[8] = stretched_yellow;
        twisted_bundle->subcanes[9] = stretched_light_blue;
        twisted_bundle->subcanes[10] = stretched_yellow;
        twisted_bundle->subcanes[11] = stretched_light_blue;
        twisted_bundle->subcanes[12] = stretched_light_brown;
        twisted_bundle->subcanes[13] = stretched_light_brown;
        twisted_bundle->subcanes[14] = stretched_light_brown;
        twisted_bundle->subcanes[15] = stretched_light_brown;
        twisted_bundle->subcanes[16] = stretched_white;
        for (i = 17; i < 34; ++i) // Duplicate first half of bundle 
        {
                twisted_bundle->subcanes[i] = twisted_bundle->subcanes[i-17];
        }
        for (i = 0; i < 34; ++i) // Place colored rods in a circle 
        {
                twisted_bundle->subcane_locs[i].r = 0.1;
                twisted_bundle->subcane_locs[i].theta = 2 * PI / 34 * i;
        } 

        return twisted_bundle;
}


Cane* stretch_test()
{
        Cane* red, * stretched_red, * twisted_duo, * single_twist, * ref;

        red = init_cane();
        red->color.r = 1.0;
        red->color.g = 0.5;
        red->color.b = 0.5;
       
        stretched_red = init_cane();
        stretched_red->stretch = 0.1;
        stretched_red->num_subcanes = 1;       
        stretched_red->subcanes[0] = red;

        twisted_duo = init_cane();
        twisted_duo->twist = 1.0;
        twisted_duo->num_subcanes = 2;
        twisted_duo->subcanes[0] = twisted_duo->subcanes[1] = stretched_red;
        twisted_duo->subcane_locs[0].r = 0.2;
        twisted_duo->subcane_locs[0].theta = 0;
        twisted_duo->subcane_locs[1].r = 0.2;
        twisted_duo->subcane_locs[1].theta = 3.141;
 
        single_twist = init_cane();
        single_twist->stretch = 0.1; 
        single_twist->num_subcanes = 1; 
        single_twist->subcanes[0] = twisted_duo;

        ref = init_cane();
        ref->num_subcanes = 2;
        ref->subcanes[0] = single_twist;
        ref->subcanes[1] = red;

        return ref; 
}
#endif


