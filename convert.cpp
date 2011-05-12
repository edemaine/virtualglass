/*
Converts a cane (constructed as a DAG) into a set of triangles
in 3-space, each with a set of coordinates and a color. 
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "types.h"
#include "constants.h"


#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

Color brighten_color(Color c)
{
        Color i;
        i.r = MIN(c.r + 0, 1.0); 
        i.g = MIN(c.g + 0, 1.0); 
        i.b = MIN(c.b + 0, 1.0); 
        return i;
}

Color darken_color(Color c)
{
        Color i;
        i.r = MAX(c.r - 0.1, 0.0); 
        i.g = MAX(c.g - 0.1, 0.0); 
        i.b = MAX(c.b - 0.1, 0.0); 
        return i;
}

void recurse(Cane* c, Triangle* triangles, int* cur_tri, 
        Point origin_bot_pt, Point origin_top_pt, float origin_angle_bot, float origin_angle_top, float origin_stretch, int axial, int illuminated_subcane, int res_mode)
{
        Triangle tmp_t;
        int angular, i, angular_resolution, axial_resolution, illumination;
        Point p1, p2, p3, p4, subcane_origin_bot_pt, subcane_origin_top_pt;
        float subcane_origin_angle_bot, subcane_origin_angle_top, subcane_stretch;

        switch (res_mode)
        {
                case LOW_RESOLUTION:
                        angular_resolution = LOW_ANGULAR_RESOLUTION;
                        axial_resolution = LOW_AXIAL_RESOLUTION;
                        break;
                case HIGH_RESOLUTION:
                        angular_resolution = HIGH_ANGULAR_RESOLUTION;
                        axial_resolution = HIGH_AXIAL_RESOLUTION;
                        break;
                default:
                        exit(1);
        }

        if (c->num_subcanes == 0 && (illuminated_subcane != ALL_SUBCANES && illuminated_subcane != NO_SUBCANES))
        {
                fprintf(stderr, "Error: Attempt to illuminate subcanes of a cane w/o subcanes.\n");  
                exit(1);
        }

        if (c->twist != 0 && c->stretch != 1)
        {
                fprintf(stderr, "Error: Cannot stretch and twist same cane object (nest them).\n");
                exit(1);
        }

        if ((c->twist != 0 || c->stretch != 1) && c->num_subcanes == 0)
        {
                fprintf(stderr, "Error: Cannot stretch/twist in a colored cane object (nest colored object in stretch/twist object).\n");
                exit(1);
        }


        if (c->num_subcanes == 0)
        {
                for (angular = 0; angular < angular_resolution; ++angular)
                {
                        p1.x = origin_bot_pt.x + origin_stretch * cos(2 * PI * angular / angular_resolution); 
                        p1.y = origin_bot_pt.y + origin_stretch * sin(2 * PI * angular / angular_resolution); 
                        p1.z = ((float) axial) / axial_resolution; 

                        p2.x = origin_top_pt.x + origin_stretch * cos(2 * PI * angular / angular_resolution); 
                        p2.y = origin_top_pt.y + origin_stretch * sin(2 * PI * angular / angular_resolution); 
                        p2.z = ((float) axial + 1) / axial_resolution;        

                        p3.x = origin_bot_pt.x + origin_stretch * cos(2 * PI * (angular+1) / angular_resolution); 
                        p3.y = origin_bot_pt.y + origin_stretch * sin(2 * PI * (angular+1) / angular_resolution); 
                        p3.z = ((float) axial) / axial_resolution;        

                        p4.x = origin_top_pt.x + origin_stretch * cos(2 * PI * (angular+1) / angular_resolution); 
                        p4.y = origin_top_pt.y + origin_stretch * sin(2 * PI * (angular+1) / angular_resolution); 
                        p4.z = ((float) axial + 1) / axial_resolution; 


                        tmp_t.v1 = p2;
                        tmp_t.v2 = p1;
                        tmp_t.v3 = p4;
                        if (illuminated_subcane == ALL_SUBCANES)
                                tmp_t.c = brighten_color(c->color);
                        else
                                tmp_t.c = darken_color(c->color);

                        triangles[*cur_tri] = tmp_t;
                        *cur_tri += 1;

                        tmp_t.v1 = p1;
                        tmp_t.v2 = p3;
                        tmp_t.v3 = p4;
                        if (illuminated_subcane == ALL_SUBCANES)
                                tmp_t.c = brighten_color(c->color);
                        else
                                tmp_t.c = darken_color(c->color);

                        triangles[*cur_tri] = tmp_t;
                        *cur_tri += 1;
                }
        } 
        else
        {
                if (c->twist != 0)
                {
                        for (i = 0; i < c->num_subcanes; ++i)
                        {
                                subcane_origin_bot_pt.x = origin_bot_pt.x + origin_stretch * c->subcane_locs[i].r * 
                                        cos(c->subcane_locs[i].theta + 2 * PI * c->twist * axial / axial_resolution + origin_angle_bot); 
                                subcane_origin_bot_pt.y = origin_bot_pt.y + origin_stretch * c->subcane_locs[i].r * 
                                        sin(c->subcane_locs[i].theta + 2 * PI * c->twist * axial / axial_resolution + origin_angle_bot); 
                                subcane_origin_top_pt.x = origin_top_pt.x + origin_stretch * c->subcane_locs[i].r * 
                                        cos(c->subcane_locs[i].theta + 2 * PI * c->twist * (axial + 1) / axial_resolution + origin_angle_top); 
                                subcane_origin_top_pt.y = origin_top_pt.y + origin_stretch * c->subcane_locs[i].r * 
                                        sin(c->subcane_locs[i].theta + 2 * PI * c->twist * (axial + 1) / axial_resolution + origin_angle_top); 
                                subcane_origin_angle_bot = origin_angle_bot + 2 * PI * c->twist * (axial) / axial_resolution;
                                subcane_origin_angle_top = origin_angle_top + 2 * PI * c->twist * (axial + 1) / axial_resolution;
                                if (i == illuminated_subcane || illuminated_subcane == ALL_SUBCANES)
                                        illumination = ALL_SUBCANES;
                                else
                                        illumination = NO_SUBCANES;

                                recurse(c->subcanes[i], triangles, cur_tri, subcane_origin_bot_pt, 
                                        subcane_origin_top_pt, subcane_origin_angle_bot, 
                                        subcane_origin_angle_top, origin_stretch, axial, illumination, res_mode);
                        }
                }
                else if (c->stretch != 1.0)
                {
                        /*
                        There are a few things that happen when you stretch:
                        1. The color material gets thinner.
                        2. Everything gets drawn towards the center.
                        3. The axial gets 'blown up' in that things are dilated along the axial direction.
                        */
                        for (i = 0; i < c->num_subcanes; ++i)
                        {
                                subcane_stretch = origin_stretch * c->stretch;
                                subcane_origin_bot_pt.x = origin_bot_pt.x + subcane_stretch * c->subcane_locs[i].r * 
                                        cos(c->subcane_locs[i].theta + origin_angle_bot);
                                subcane_origin_bot_pt.y = origin_bot_pt.y + subcane_stretch * c->subcane_locs[i].r * 
                                        sin(c->subcane_locs[i].theta + origin_angle_bot);
                                subcane_origin_top_pt.x = origin_top_pt.x + subcane_stretch * c->subcane_locs[i].r * 
                                        cos(c->subcane_locs[i].theta + origin_angle_top);
                                subcane_origin_top_pt.y = origin_top_pt.y + subcane_stretch * c->subcane_locs[i].r * 
                                        sin(c->subcane_locs[i].theta + origin_angle_top);
                                subcane_origin_angle_bot = origin_angle_bot;
                                subcane_origin_angle_top = origin_angle_top;


                                if (i == illuminated_subcane || illuminated_subcane == ALL_SUBCANES)
                                        illumination = ALL_SUBCANES;
                                else
                                        illumination = NO_SUBCANES;

                                recurse(c->subcanes[i], triangles, cur_tri, subcane_origin_bot_pt, 
                                        subcane_origin_top_pt, subcane_origin_angle_bot, 
                                        subcane_origin_angle_top, subcane_stretch, axial, illumination, res_mode);
                        }
                }
                else
                {
                        for (i = 0; i < c->num_subcanes; ++i)
                        {
                                subcane_origin_bot_pt.x = origin_bot_pt.x + origin_stretch * c->subcane_locs[i].r * 
                                        cos(c->subcane_locs[i].theta + origin_angle_bot); 
                                subcane_origin_bot_pt.y = origin_bot_pt.y + origin_stretch * c->subcane_locs[i].r * 
                                        sin(c->subcane_locs[i].theta + origin_angle_bot); 
                                subcane_origin_top_pt.x = origin_top_pt.x + origin_stretch * c->subcane_locs[i].r * 
                                        cos(c->subcane_locs[i].theta + origin_angle_top); 
                                subcane_origin_top_pt.y = origin_top_pt.y + origin_stretch * c->subcane_locs[i].r * 
                                        sin(c->subcane_locs[i].theta + origin_angle_top); 
                                subcane_origin_angle_bot = origin_angle_bot;
                                subcane_origin_angle_top = origin_angle_top;

                                if (i == illuminated_subcane || illuminated_subcane == ALL_SUBCANES)
                                        illumination = ALL_SUBCANES;
                                else
                                        illumination = NO_SUBCANES;

                                recurse(c->subcanes[i], triangles, cur_tri, subcane_origin_bot_pt, 
                                        subcane_origin_top_pt, subcane_origin_angle_bot, 
                                        subcane_origin_angle_top, origin_stretch, axial, illumination, res_mode);
                        }
                }
        }
}

int num_canes(Cane* c)
{
        int subcanes, i;

        subcanes = 0;
        for (i = 0; i < c->num_subcanes; ++i)
        {
                subcanes += num_canes(c->subcanes[i]);
        }
        return (subcanes + 1);
}


void convert_to_triangles(Cane* c, Triangle** triangles, int* num_triangles, int illuminated_subcane, int res_mode)
{
        Point origin;
        int axial, axial_resolution, angular_resolution;

        origin.x = origin.y = origin.z = 0;

        if (*triangles != NULL)
        {
                free(*triangles);
                *triangles = NULL;
        }

        if (c == NULL)
        {
                *num_triangles = 0;
                return;
        }

        if (res_mode == LOW_RESOLUTION)
        {
                axial_resolution = LOW_AXIAL_RESOLUTION;
                angular_resolution = LOW_ANGULAR_RESOLUTION;
        }
        else // mode == HIGH_RESOLUTION
        {
                axial_resolution = HIGH_AXIAL_RESOLUTION;
                angular_resolution = HIGH_ANGULAR_RESOLUTION;
        }

        *triangles = (Triangle*) malloc(sizeof(Triangle) 
                * axial_resolution * angular_resolution * 2 * num_canes(c));
        *num_triangles = 0;

        for (axial = 0; axial < axial_resolution - 1; ++axial)
        {
                recurse(c, *triangles, num_triangles, origin, origin, 0, 0, 1, axial, illuminated_subcane, res_mode);
        }
}

