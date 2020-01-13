#include "./include/util.h"
#include "./params.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

using namespace std;

void    runProjection(float *pout, float *pin,
                        float DSO, float DSD, float dView, int nView,
                        float *pdDct, int *pnDct, float *pdOffsetDct,
                        float *pdImg, int *pnImg, float *pdOffsetImg);

/*
 * Please read the Ch.3 Image Reconstruction
 *
 * Implementation for projection operator based on Ch.3 Equation (3.5) & (3.6)
 * Projection operator is implemented using ray-driven method
*/

int main ()
{
    /*
     * Data Load
    */

    char pchFile[128];
    FILE *pfid;

    float   *pin    = (float *) malloc(sizeof(float)*pnImg[Y]*pnImg[X]*pnImg[Z]);
    memset(pin, 0, sizeof(float)*pnImg[Y]*pnImg[X]*pnImg[Z]);

    sprintf(pchFile, "input%d.raw", pnImg[Y]);
    pfid = fopen(pchFile, "rb");

    fread(pin, sizeof(float), pnImg[Y]*pnImg[X]*pnImg[Z], pfid);
    fclose (pfid); pfid = 0;

    /*
     *  Sinogram data
     */
    
    float   *pout   = (float *) malloc(sizeof(float)*pnDct[Y]*pnDct[X]*nView);
    memset(pout, 0, sizeof(float)*pnDct[Y]*pnDct[X]*nView);

    /*
     *  Run projection operator
     */
    
    runProjection(pout, pin,
                    DSO, DSD, dView, nView,
                    pdDct, pnDct, pdOffsetDct,
                    pdImg, pnImg, pdOffsetImg);

    sprintf(pchFile, "prj_dct%d_view%d.raw", pnDct[X], nView);
    pfid = fopen(pchFile, "wb");

    fwrite(pout, sizeof(float), pnDct[Y]*pnDct[X]*nView, pfid);
    fclose(pfid); pfid = 0;

    free(pin);  pin = 0;
    free(pout); pout = 0;
    
    return 0;
}

void    runProjection(float *pout, float *pin,
                        float DSO, float DSD, float dView, int nView,
                        float *pdDct, int *pnDct, float *pdOffsetDct,
                        float *pdImg, int *pnImg, float *pdOffsetImg) {
    
    /*
     *  Initialize projection parameters
     */
    float   pdSizeImg[2]	= {pnImg[Y]*pdImg[Y], pnImg[X]*pdImg[X]};
    
    float   dDiameter       = sqrt(pdSizeImg[Y]*pdSizeImg[Y] + pdSizeImg[X]*pdSizeImg[X]);
    float   dRadius         = dDiameter/2.0f;
    
    float   dSample         = min(pdImg[X], pdImg[Y]);
    int     nSample         = int(ceil(dDiameter/dSample));

    float	dBeta, dGamma;
    
    float   pdOriImg[2]     = {0.0f, 0.0f};
    float   pdOriNorDir[2]  = {0.0f, 0.0f};
    
    float   pdPosImg[2]     = {0.0f, 0.0f};
    float   pdNorDir[2]     = {0.0f, 0.0f};
    
    float   pnCurIdImg[2]   = {0.0f, 0.0f};
    
    float   out;
    
    // Ch.3 Equation (3.6)
    // Projection operator
    for (int iView = 0; iView < nView; iView++) {
        dBeta   = iView*dView * PI / 180.0f;
        
        // Rotation angle for geometry
        for (int iDctX = 0; iDctX < pnDct[X]; iDctX++) {    
            pdOriImg[Y]     = -dRadius;
            pdOriImg[X]     = id2pos(iDctX + pdOffsetDct[X], pdDct[X], pnDct[X]);
        
            dGamma          = 0.0f;
            
            // Normal vector of incident X-ray [mm, mm] 
            pdOriNorDir[Y]	= dSample*cos(dGamma);
            pdOriNorDir[X]	= dSample*sin(dGamma);
            
            // Ch.3 Equation (3.5)
            // Rotated X-ray source position [mm, mm]
            pdPosImg[Y] = -sin(dBeta)*pdOriImg[X] + cos(dBeta)*pdOriImg[Y];
            pdPosImg[X] = cos(dBeta)*pdOriImg[X] + sin(dBeta)*pdOriImg[Y];
            
            // Ch.3 Equation (3.5)
            // Rotated normal vector of incident X-ray [mm, mm]
            pdNorDir[Y] = -sin(dBeta)*pdOriNorDir[X] + cos(dBeta)*pdOriNorDir[Y];
            pdNorDir[X] = cos(dBeta)*pdOriNorDir[X] + sin(dBeta)*pdOriNorDir[Y];
            
            out             = 0.0f;
            
            for (int iSample = 0; iSample < nSample; iSample++) {
                pnCurIdImg[Y]	= pos2id(pdPosImg[Y] + iSample*pdNorDir[Y], pdImg[Y], pnImg[Y]) - pdOffsetImg[Y];
                pnCurIdImg[X]	= pos2id(pdPosImg[X] + iSample*pdNorDir[X], pdImg[X], pnImg[X]) - pdOffsetImg[X];
                
                out             += interpolation2d(pin, pnCurIdImg[Y], pnImg[Y], pnCurIdImg[X], pnImg[X]);
            }
            
            pout[pnDct[X]*iView + iDctX]     = dSample*out;
        }
    }
    
    return ;
}