# include <stdlib.h>
# include <gsim/se_lct.h>
# include <math.h>
# include <time.h>
# include <string>

#include <iostream>
#include <fstream>

# ifdef SETUT_FLGLUT
# include <gsim/gs_ogl.h>
# include "setut_flglut.h"
# else
# include <GL/glut.h>
#endif

// *******************************************************************************
// DEFINE TEST EXAMPLE 
// *******************************************************************************

# define END 12345.6

static double x_Example0[] = { -2.0, -2.0, 2.0, 2.0, END,
        END};
static double y_Example0[] = { -2.0, 2.0, 2.0, -2.0, END,
        END};
    
static const double * x_CurExample = x_Example0;
static const double * y_CurExample = y_Example0;
static SeLct *TheLct;
static GsPolygon CurPath;
static GsPolygon CurChannel;
static float Radius = 0.15f;
    
static void create_lct( SeLct *TheLct )
{
    const double *x_data = x_CurExample;
    const double *y_data = y_CurExample;
    
    GsPolygon pol;
    
    while( *x_data != END )
    { 
        pol.push().set( (float)x_data[0], (float)y_data[0]); 
        x_data+=1; y_data+=1;
    }
    TheLct->init(pol,0.0001f);
    
    while( *++x_data != END )
    {
        pol.size(0);
        while( *x_data != END )
        {
            pol.push().set( (float)x_data[0], (float)y_data[0] );
            x_data += 1; y_data += 1;
            
        }
        TheLct->insert_polygon(pol);
    }
}

static int insert_polygon_from_array( SeLct *TheLct, float *x0, float *y0, int n0 )
{
    int k;
    GsPolygon pol;
    
    int polygonID;
    
    pol.size(0);
    for(k=0;k<n0;k++)
    {
        pol.push().set( (float)x0[k], (float)y0[k] );
    }
    polygonID = TheLct->insert_polygon( pol );
    
    return polygonID;
}

static void create_lct_example1(SeLct *TheLct, std::string map, int nfz)
{
    int k;
    GsPolygon pol;

    //Establish the map boundaries (large enough to work for all map types)
    int n0 = 4; 
    float x0[n0], y0[n0];        
    x0[0] = -10500.0;  y0[0] = -10500.0;
    x0[1] = -10500.0;  y0[1] = 10500.0;
    x0[2] = 10500.0;   y0[2] = 10500.0;
    x0[3] = 10500.0;  y0[3] = -10500.0;    
    pol.size(0);
    for(k=0;k<n0;k++)
    {
        pol.push().set( (float)x0[k], (float)y0[k] );
    }
    TheLct->init(pol,0.00001f);
    
    
    //Establish the NFZ(s) for each map
    std::cout << "Map name:  " << map << "\n";
    std::cout << "No-fly zone:  " << nfz << "\n";    
    int polyID;    
    
    //RANDOM MAP
    if ( map == "random"){
        if ( nfz == 1){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = -500;      y1[0] = 2000;
            x1[1] = 500;      y1[1] = 2000;
            x1[2] = 500;    y1[2] = -2000;
            x1[3] = -500;    y1[3] = -2000;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 2){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = -500;      y1[0] = 2500;
            x1[1] = 500;      y1[1] = 2500;
            x1[2] = 500;    y1[2] = -2500;
            x1[3] = -500;    y1[3] = -2500;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 3){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = -500;      y1[0] = 3000;
            x1[1] = 500;      y1[1] = 3000;
            x1[2] = 500;    y1[2] = -3000;
            x1[3] = -500;    y1[3] = -3000;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 4){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = -500;      y1[0] = 3500;
            x1[1] = 500;      y1[1] = 3500;
            x1[2] = 500;    y1[2] = -3500;
            x1[3] = -500;    y1[3] = -3500;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 5){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = -500;      y1[0] = 4000;
            x1[1] = 500;      y1[1] = 4000;
            x1[2] = 500;    y1[2] = -4000;
            x1[3] = -500;    y1[3] = -4000;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 6){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = -500;      y1[0] = 4500;
            x1[1] = 500;      y1[1] = 4500;
            x1[2] = 500;    y1[2] = -4500;
            x1[3] = -500;    y1[3] = -4500;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 7){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = -500;      y1[0] = 5000;
            x1[1] = 500;      y1[1] = 5000;
            x1[2] = 500;    y1[2] = -5000;
            x1[3] = -500;    y1[3] = -5000;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 8){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = -500;      y1[0] = 5500;
            x1[1] = 500;      y1[1] = 5500;
            x1[2] = 500;    y1[2] = -5500;
            x1[3] = -500;    y1[3] = -5500;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 9){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = -500;      y1[0] = 6000;
            x1[1] = 500;      y1[1] = 6000;
            x1[2] = 500;    y1[2] = -6000;
            x1[3] = -500;    y1[3] = -6000;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 10){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = -500;      y1[0] = 6500;
            x1[1] = 500;      y1[1] = 6500;
            x1[2] = 500;    y1[2] = -6500;
            x1[3] = -500;    y1[3] = -6500;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 11){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = -500;      y1[0] = 7000;
            x1[1] = 500;      y1[1] = 7000;
            x1[2] = 500;    y1[2] = -7000;
            x1[3] = -500;    y1[3] = -7000;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 12){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = -500;      y1[0] = 7500;
            x1[1] = 500;      y1[1] = 7500;
            x1[2] = 500;    y1[2] = -7500;
            x1[3] = -500;    y1[3] = -7500;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }
        }
    //CLUSTERS MAP
    if ( map == "clusters"){ 

        // NFZ 1-13 ARE VERTICAL BETWEEN WESTERN CLUSTER AND EASTERN CLUSTERS
        if ( nfz == 1){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 0;      y1[0] = 2500;
            x1[1] = 1000;      y1[1] = 2500;
            x1[2] = 1000;    y1[2] = -2500;
            x1[3] = 0;    y1[3] = -2500;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }        

        if ( nfz == 2){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 0;      y1[0] = 3000;
            x1[1] = 1000;      y1[1] = 3000;
            x1[2] = 1000;    y1[2] = -3000;
            x1[3] = 0;    y1[3] = -3000;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 3){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 0;      y1[0] = 3500;
            x1[1] = 1000;      y1[1] = 3500;
            x1[2] = 1000;    y1[2] = -3500;
            x1[3] = 0;    y1[3] = -3500;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 4){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 0;      y1[0] = 4000;
            x1[1] = 1000;      y1[1] = 4000;
            x1[2] = 1000;    y1[2] = -4000;
            x1[3] = 0;    y1[3] = -4000;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 5){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 0;      y1[0] = 4500;
            x1[1] = 1000;      y1[1] = 4500;
            x1[2] = 1000;    y1[2] = -4500;
            x1[3] = 0;    y1[3] = -4500;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 6){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 0;      y1[0] = 5000;
            x1[1] = 1000;      y1[1] = 5000;
            x1[2] = 1000;    y1[2] = -5000;
            x1[3] = 0;    y1[3] = -5000;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 7){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 0;      y1[0] = 5500;
            x1[1] = 1000;      y1[1] = 5500;
            x1[2] = 1000;    y1[2] = -5500;
            x1[3] = 0;    y1[3] = -5500;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 8){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 0;      y1[0] = 6000;
            x1[1] = 1000;      y1[1] = 6000;
            x1[2] = 1000;    y1[2] = -6000;
            x1[3] = 0;    y1[3] = -6000;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 9){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 0;      y1[0] = 6500;
            x1[1] = 1000;      y1[1] = 6500;
            x1[2] = 1000;    y1[2] = -6500;
            x1[3] = 0;    y1[3] = -6500;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 10){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 0;      y1[0] = 7000;
            x1[1] = 1000;      y1[1] = 7000;
            x1[2] = 1000;    y1[2] = -7000;
            x1[3] = 0;    y1[3] = -7000;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 11){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 0;      y1[0] = 7500;
            x1[1] = 1000;      y1[1] = 7500;
            x1[2] = 1000;    y1[2] = -7500;
            x1[3] = 0;    y1[3] = -7500;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 12){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 0;      y1[0] = 8000;
            x1[1] = 1000;      y1[1] = 8000;
            x1[2] = 1000;    y1[2] = -8000;
            x1[3] = 0;    y1[3] = -8000;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 13){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 0;      y1[0] = 8500;
            x1[1] = 1000;      y1[1] = 8500;
            x1[2] = 1000;    y1[2] = -8500;
            x1[3] = 0;    y1[3] = -8500;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }
        
        //NFZ 14-? ARE HORIZONTAL BETWEEN NORTHERN CLUSTERS AND SOUTHERN CLUSTER
        if ( nfz == 14){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 5000;      y1[0] = 0;
            x1[1] = 5000;      y1[1] = -1000;
            x1[2] = 1000;    y1[2] = -1000;
            x1[3] = 1000;    y1[3] = 0;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 15){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 5500;      y1[0] = 0;
            x1[1] = 5500;      y1[1] = -1000;
            x1[2] = 500;    y1[2] = -1000;
            x1[3] = 500;    y1[3] = 0;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }  

        if ( nfz == 16){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 6000;      y1[0] = 0;
            x1[1] = 6000;      y1[1] = -1000;
            x1[2] = 0;    y1[2] = -1000;
            x1[3] = 0;    y1[3] = 0;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 17){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 6500;      y1[0] = 0;
            x1[1] = 6500;      y1[1] = -1000;
            x1[2] = -500;    y1[2] = -1000;
            x1[3] = -500;    y1[3] = 0;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 18){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 7000;      y1[0] = 0;
            x1[1] = 7000;      y1[1] = -1000;
            x1[2] = -1000;    y1[2] = -1000;
            x1[3] = -1000;    y1[3] = 0;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 19){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 7500;      y1[0] = 0;
            x1[1] = 7500;      y1[1] = -1000;
            x1[2] = -1500;    y1[2] = -1000;
            x1[3] = -1500;    y1[3] = 0;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 20){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 8000;      y1[0] = 0;
            x1[1] = 8000;      y1[1] = -1000;
            x1[2] = -2000;    y1[2] = -1000;
            x1[3] = -2000;    y1[3] = 0;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 21){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 8500;      y1[0] = 0;
            x1[1] = 8500;      y1[1] = -1000;
            x1[2] = -2500;    y1[2] = -1000;
            x1[3] = -2500;    y1[3] = 0;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 22){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 9000;      y1[0] = 0;
            x1[1] = 9000;      y1[1] = -1000;
            x1[2] = -3000;    y1[2] = -1000;
            x1[3] = -3000;    y1[3] = 0;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 23){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 9500;      y1[0] = 0;
            x1[1] = 9500;      y1[1] = -1000;
            x1[2] = -3500;    y1[2] = -1000;
            x1[3] = -3500;    y1[3] = 0;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 24){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 9500;      y1[0] = 0;
            x1[1] = 9500;      y1[1] = -1000;
            x1[2] = -4000;    y1[2] = -1000;
            x1[3] = -4000;    y1[3] = 0;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 25){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 9500;      y1[0] = 0;
            x1[1] = 9500;      y1[1] = -1000;
            x1[2] = -4500;    y1[2] = -1000;
            x1[3] = -4500;    y1[3] = 0;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 26){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 9500;      y1[0] = 0;
            x1[1] = 9500;      y1[1] = -1000;
            x1[2] = -5000;    y1[2] = -1000;
            x1[3] = -5000;    y1[3] = 0;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        }

        //GRID MAP
    if ( map == "grid"){
        if ( nfz == 1){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 2250;      y1[0] = 2500;
            x1[1] = 2750;      y1[1] = 2500;
            x1[2] = 2750;    y1[2] = 500;
            x1[3] = 2250;    y1[3] = 500;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 2){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 2250;      y1[0] = 2750;
            x1[1] = 2750;      y1[1] = 2750;
            x1[2] = 2750;    y1[2] = 250;
            x1[3] = 2250;    y1[3] = 250;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 3){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 2250;      y1[0] = 3000;
            x1[1] = 2750;      y1[1] = 3000;
            x1[2] = 2750;    y1[2] = 0;
            x1[3] = 2250;    y1[3] = 0;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 4){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 2250;      y1[0] = 3250;
            x1[1] = 2750;      y1[1] = 3250;
            x1[2] = 2750;    y1[2] = -250;
            x1[3] = 2250;    y1[3] = -250;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 5){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 2250;      y1[0] = 3500;
            x1[1] = 2750;      y1[1] = 3500;
            x1[2] = 2750;    y1[2] = -500;
            x1[3] = 2250;    y1[3] = -500;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 6){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 2250;      y1[0] = 3750;
            x1[1] = 2750;      y1[1] = 3750;
            x1[2] = 2750;    y1[2] = -750;
            x1[3] = 2250;    y1[3] = -750;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 7){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 2250;      y1[0] = 4000;
            x1[1] = 2750;      y1[1] = 4000;
            x1[2] = 2750;    y1[2] = -1000;
            x1[3] = 2250;    y1[3] = -1000;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 8){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 2250;      y1[0] = 4250;
            x1[1] = 2750;      y1[1] = 4250;
            x1[2] = 2750;    y1[2] = -1250;
            x1[3] = 2250;    y1[3] = -1250;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 9){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 2250;      y1[0] = 4500;
            x1[1] = 2750;      y1[1] = 4500;
            x1[2] = 2750;    y1[2] = -1500;
            x1[3] = 2250;    y1[3] = -1500;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 10){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 2250;      y1[0] = 4750;
            x1[1] = 2750;      y1[1] = 4750;
            x1[2] = 2750;    y1[2] = -1750;
            x1[3] = 2250;    y1[3] = -1750;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 11){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 2250;      y1[0] = 5000;
            x1[1] = 2750;      y1[1] = 5000;
            x1[2] = 2750;    y1[2] = -2000;
            x1[3] = 2250;    y1[3] = -2000;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 12){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 2250;      y1[0] = 5250;
            x1[1] = 2750;      y1[1] = 5250;
            x1[2] = 2750;    y1[2] = -2250;
            x1[3] = 2250;    y1[3] = -2250;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }
        }
        //CIRCLE MAP
    if ( map == "circle"){
        if ( nfz == 1){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 2200;      y1[0] = 4000;
            x1[1] = 3200;      y1[1] = 4000;
            x1[2] = 3200;    y1[2] = -4000;
            x1[3] = 2200;    y1[3] = -4000;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 2){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 2200;      y1[0] = 4500;
            x1[1] = 3200;      y1[1] = 4500;
            x1[2] = 3200;    y1[2] = -4500;
            x1[3] = 2200;    y1[3] = -4500;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 3){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 2200;      y1[0] = 5000;
            x1[1] = 3200;      y1[1] = 5000;
            x1[2] = 3200;    y1[2] = -5000;
            x1[3] = 2200;    y1[3] = -5000;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 4){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 2200;      y1[0] = 5500;
            x1[1] = 3200;      y1[1] = 5500;
            x1[2] = 3200;    y1[2] = -5500;
            x1[3] = 2200;    y1[3] = -5500;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 5){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 2200;      y1[0] = 6000;
            x1[1] = 3200;      y1[1] = 6000;
            x1[2] = 3200;    y1[2] = -6000;
            x1[3] = 2200;    y1[3] = -6000;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 6){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 2200;      y1[0] = 6500;
            x1[1] = 3200;      y1[1] = 6500;
            x1[2] = 3200;    y1[2] = -6500;
            x1[3] = 2200;    y1[3] = -6500;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 7){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 2200;      y1[0] = 7000;
            x1[1] = 3200;      y1[1] = 7000;
            x1[2] = 3200;    y1[2] = -7000;
            x1[3] = 2200;    y1[3] = -7000;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 8){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 2200;      y1[0] = 7500;
            x1[1] = 3200;      y1[1] = 7500;
            x1[2] = 3200;    y1[2] = -7500;
            x1[3] = 2200;    y1[3] = -7500;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 9){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 2200;      y1[0] = 8000;
            x1[1] = 3200;      y1[1] = 8000;
            x1[2] = 3200;    y1[2] = -8000;
            x1[3] = 2200;    y1[3] = -8000;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 10){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 2200;      y1[0] = 8500;
            x1[1] = 3200;      y1[1] = 8500;
            x1[2] = 3200;    y1[2] = -8500;
            x1[3] = 2200;    y1[3] = -8500;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 11){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 2200;      y1[0] = 9000;
            x1[1] = 3200;      y1[1] = 9000;
            x1[2] = 3200;    y1[2] = -9000;
            x1[3] = 2200;    y1[3] = -9000;

            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }


        if ( nfz == 12){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 2200;      y1[0] = 9500;
            x1[1] = 3200;      y1[1] = 9500;
            x1[2] = 3200;    y1[2] = -9500;
            x1[3] = 2200;    y1[3] = -9500;


            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }

        if ( nfz == 13){
            int n1 = 4;
            float x1[n1], y1[n1];
            x1[0] = 2200;      y1[0] = 10000;
            x1[1] = 3200;      y1[1] = 10000;
            x1[2] = 3200;    y1[2] = -10000;
            x1[3] = 2200;    y1[3] = -10000;


            polyID = insert_polygon_from_array(TheLct, x1, y1, n1);
            }
        
        }

}

static void get_path( float x1, float y1, float x2, float y2, float Radius, SeLct *TheLct,
    GsPolygon *newPath, GsPolygon *newChannel )
{
    bool found = TheLct->search_channel( x1, y1, x2, y2, Radius );
    if( !found )
    {
        newPath->size(0);
        newChannel->size(0);
    }
    else
    {        
        
        static GsArray<GsPnt2> cedges;
        static GsArray<GsPnt2> ucedges;
        TheLct->get_mesh_edges ( &cedges, &ucedges );
        
        printf("Number of constrained edges: %d\n",cedges.size());
        printf("Number of unconstrained edges: %d\n",ucedges.size());
        
        //SAVE CONSTRAINED EDGES TO FILE
        std::ofstream cedgesFile;
        cedgesFile.open("cedges.txt");
        int j;
        for(j=0;j<cedges.size();j+=2)
        {
            //printf("[%f,%f]\n",cedges[j].x,cedges[j].y);
            cedgesFile << cedges[j].x << "," << cedges[j].y << ",\n";
            cedgesFile << cedges[j+1].x << "," << cedges[j+1].y << ",\n";
            cedgesFile << "np.NaN,np.NaN,\n";
            // cedgesFile << "NaN,NaN,\n";
        }
        cedgesFile.close();        

        //SAVE UNCONSTRAINED EDGES TO FILE
        std::ofstream ucedgesFile;
        ucedgesFile.open("ucedges.txt");        
        for(j=0;j<ucedges.size();j+=2)
        {
            //printf("[%f,%f]\n",ucedges[j].x,ucedges[j].y);
            ucedgesFile << ucedges[j].x << "," << ucedges[j].y << ",\n";
            ucedgesFile << ucedges[j+1].x << "," << ucedges[j+1].y << ",\n";
            ucedgesFile << "np.NaN,np.NaN,\n";
            // ucedgesFile << "NaN,NaN,\n";
        }
        ucedgesFile.close();   
        
        //SAVE PATH TO FILE
        printf("*****\n");
        printf("Start Point = [%f,%f]\n",x1,y1);
        printf("Final Point = [%f,%f]\n",x2,y2);        
        TheLct->make_funnel_path( *newPath, Radius, GS_TORAD(10.0)); // GS_TORAD sets the angle resolution for the turn arc
        int pathSize;
        pathSize = newPath->size();        
        std::ofstream pathFile;
        pathFile.open("path.txt");        
        int k;
        for(k=0;k<pathSize;k++)
        {
            pathFile << newPath->get(k).x << "," << newPath->get(k).y << ",\n";
        }
        pathFile.close();        
    }
}

static int insert_heading_constraint( float xIn, float yIn, float heading, float Radius,
    SeLct *TheLct, bool direction )
{
    float radiusBuffer = 0.01f;
    float PI = 3.14159;  
    float x0, y0, x1, y1, x2, y2, x3, y3;
    float xB, yB, xS, yS;    
    float dir;
    if( direction )
        dir = -1.0;
    else
        dir = 1.0;        
    
    xS = xIn + dir*(Radius + radiusBuffer)*cos( heading );
    yS = yIn + dir*(Radius + radiusBuffer)*sin( heading );
    
    xB = xIn + -1.0*dir*( radiusBuffer )*cos(heading);
    yB = yIn + -1.0*dir*( radiusBuffer )*sin(heading);
    
    x0 = xB + (Radius + radiusBuffer)*cos( heading + PI*0.5 );
    y0 = yB + (Radius + radiusBuffer)*sin( heading + PI*0.5 );
    
    x1 = xS + (Radius + radiusBuffer)*cos( heading + PI*0.5 );
    y1 = yS + (Radius + radiusBuffer)*sin( heading + PI*0.5 );
    
    x2 = xS + (Radius + radiusBuffer)*cos( heading - PI*0.5 );
    y2 = yS + (Radius + radiusBuffer)*sin( heading - PI*0.5 );
    
    x3 = xB + (Radius + radiusBuffer)*cos( heading - PI*0.5 );
    y3 = yB + (Radius + radiusBuffer)*sin( heading - PI*0.5 );
    
    GsPolygon temp;    
    
    temp.push().set( x0, y0 );
    temp.push().set( x1, y1 );
    temp.push().set( x2, y2 );
    temp.push().set( x3, y3 );
    temp.push().set( x2, y2 );
    temp.push().set( x1, y1 );
    
    int polyID;
    polyID = TheLct->insert_polygon( temp );
    return polyID;
}

void uxas_main(float xstart, float ystart, float xend, float yend, std::string map, int nfz)
{
    TheLct = new SeLct;
    
    // create_lct(TheLct);  
    create_lct_example1(TheLct, map, nfz);

    int nPoints = 1;   
    int nPaths = nPoints;
    GsPolygon *newChannel;
    newChannel = new GsPolygon[nPaths];
    GsPolygon *newPath;
    newPath = new GsPolygon[nPaths];
    //Change Radius Here!
    Radius = 85.0f;   
    float xK[nPaths], yK[nPaths];
    float xJ[nPaths], yJ[nPaths];
    int k;
    int polyStart, polyFinal; 
    xK[0] =  xstart;
    yK[0] =  ystart;    
    xJ[0] =  xend;
    yJ[0] =  yend;  

    for(k=0;k<nPoints;k++)
    {       
        //polyStart = insert_heading_constraint(xK[k],xK[k],psiK[k],Radius,TheLct,1);
        //polyFinal = insert_heading_constraint(xJ[k],yJ[k],psiJ[k],Radius,TheLct,0);

        get_path( xK[k], yK[k], xJ[k], yJ[k], Radius, TheLct, &newPath[k], &newChannel[k] );
        
        //TheLct->remove_polygon( polyStart);
        //TheLct->remove_polygon( polyFinal); 
    }  
    
}

