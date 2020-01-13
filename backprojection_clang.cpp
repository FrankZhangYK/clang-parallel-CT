#include "./include/util.h"
#include "./include/params.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

using namespace std;

void    runBackprojection(float *pout, float *pin,
                        float DSO, float DSD, float dView, int nView,
                        float *pdDct, int *pnDct, float *pdOffsetDct,
                        float *pdImg, int *pnImg, float *pdOffsetImg);

/*
 * Please read the Ch.3 Image Reconstruction
 *
 * Implementation for backprojection operator based on Ch.3 Equation (3.22)
 * Backprojection operator is implemented using pixel-driven method
*/


// Execution time check 

clock_t start, end;
double result;


int main ()
{
    start = clock();

    /*
     * Data Load
    */

    char pchFile[128];
    FILE *pfid;

    float   *pin    = (float *) malloc(sizeof(float)*pnDct[Y]*pnDct[X]*nView);
    memset(pin, 0, sizeof(float)*pnDct[Y]*pnDct[X]*nView);

    sprintf(pchFile, "flt_view%d_dct%d.raw", nView, pnDct[X]);
    pfid = fopen(pchFile, "rb");

    fread(pin, sizeof(float), pnDct[Y]*pnDct[X]*nView, pfid);
    fclose (pfid); pfid = 0;
    /*
     *  Filtered sinogram data
     */
    
    float   *pout   = (float *) malloc(sizeof(float)*pnImg[Y]*pnImg[X]*pnImg[Z]);
    memset(pout, 0, sizeof(float)*pnImg[Y]*pnImg[X]*pnImg[Z]);

    /*
     *  Run backprojection operator
     */
    runBackprojection(pout, pin,
                        DSO, DSD, dView, nView,
                        pdDct, pnDct, pdOffsetDct,
                        pdImg, pnImg, pdOffsetImg);

    sprintf(pchFile, "output%d.raw", pnImg[Y]);
    pfid = fopen(pchFile, "wb");

    fwrite(pout, sizeof(float), pnImg[Y]*pnImg[X]*pnImg[Z], pfid);
    fclose(pfid); pfid = 0;

    free(pin);  pin = 0;
    free(pout); pout = 0;


    end = clock();

    result = (double)(end - start);
    printf("%.4f seconds.\n", result/CLOCKS_PER_SEC);


    return 0;
}

void    runBackprojection(float *pout, float *pin,
                        float DSO, float DSD, float dView, int nView,
                        float *pdDct, int *pnDct, float *pdOffsetDct,
                        float *pdImg, int *pnImg, float *pdOffsetImg) {
    
    /*
     *  Initialize backprojection parameters
     */
    
    float   dRadius;
    
    float	dBeta, dGamma, dPhi;
    
    float   pdOriImg[2]     = {0.0f, 0.0f};
    
    float   pdPosImg[2]     = {0.0f, 0.0f}; 
    float   pdDist[2]       = {0.0f, 0.0f};
    
    float   pnCurIdDct[2]   = {0.0f, 0.0f};
    
    float   out;
    
    // Ch.3 Equation (3.22)
    // Backprojection operator
    for (int iView = 0; iView < nView; iView++) {
        
        // Rotation angle for geometry
        dBeta   = -iView*dView * PI / 180.0f;
        
        for (int iImgX = 0; iImgX < pnImg[X]; iImgX++) {
            
            // X position of object
            pdPosImg[X]     = id2pos(iImgX + pdOffsetImg[X], pdImg[X], pnImg[X]);
            
            for (int iImgY = 0; iImgY < pnImg[Y]; iImgY++) {
                
                // Y position of object
                pdPosImg[Y]     = id2pos(iImgY + pdOffsetImg[Y], pdImg[Y], pnImg[Y]);
                
                // Calculate a detector position based on Figure 3.34
                dRadius         = sqrt(pdPosImg[Y]*pdPosImg[Y] + pdPosImg[X]*pdPosImg[X]);
                dPhi            = atan2(pdPosImg[Y], pdPosImg[X]);
                
                // Detector position related with (X, Y) position of object
                pdDist[X]       = dRadius*cos(dBeta - dPhi);
                
                pnCurIdDct[X]   = pos2id(pdDist[X], pdDct[X], pnDct[X]) - pdOffsetDct[X];
                
                out                             = interpolation1d(&pin[pnDct[X]*iView], pnCurIdDct[X], pnDct[X]);
                pout[pnImg[Y]*iImgX + iImgY]	+= PI/nView * out;
            }
        }
    }
    
    return ;
}