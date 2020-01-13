#include "./include/util.h"
#include "./include/params.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

using namespace std;


void    runConvolution1d(float *pout, float *pin,
                            float DSO, float DSD, float dView, int nView,
                            float *pdDct, int *pnDct, float *pdOffsetDct,
                            float *pdImg, int *pnImg, float *pdOffsetImg);


int main()
{
    /*
     * Data Load
    */

    char pchFile[128];
    FILE *pfid;

    float   *pin    = (float *) malloc(sizeof(float)*pnDct[Y]*pnDct[X]*nView);
    memset(pin, 0, sizeof(float)*pnDct[Y]*pnDct[X]*nView);

    sprintf(pchFile, "prj_view%d_dct%d.raw", nView, pnDct[X]);
    pfid = fopen(pchFile, "rb");

    fread(pin, sizeof(float), pnDct[Y]*pnDct[X]*nView, pfid);
    fclose (pfid); pfid = 0;

    /*
     *  Filtered sinogram data
     */
    
    float   *pout   = (float *) malloc(sizeof(float)*pnDct[Y]*pnDct[X]*nView);
    memset(pout, 0, sizeof(float)*pnDct[Y]*pnDct[X]*nView);

    /*
     *  Run filtering operator
     */
    runConvolution1d(pout, pin,
                        DSO, DSD, dView, nView,
                        pdDct, pnDct, pdOffsetDct,
                        pdImg, pnImg, pdOffsetImg);
        
    sprintf(pchFile, "flt_view%d_dct%d.raw", nView, pnDct[X]);
    pfid = fopen(pchFile, "wb");

    fwrite(pout, sizeof(float), pnDct[Y]*pnDct[X]*nView, pfid);
    fclose(pfid); pfid = 0;

    free(pin);  pin = 0;
    free(pout); pout = 0;

    return 0;
}

void    runConvolution1d(float *pout, float *pin,
                        float DSO, float DSD, float dView, int nView,
                        float *pdDct, int *pnDct, float *pdOffsetDct,
                        float *pdImg, int *pnImg, float *pdOffsetImg) {
    
    float   pnDctExt[2] = {pnDct[Y], 2*pnDct[X] - 1};
    
    float   *pker       = (float*)   malloc(pnDctExt[X] * sizeof(float));
    memset(pker, 0, pnDctExt[X] * sizeof(float));
    
    generate_filter(pker, pdDct[X], pnDct[X]);
    
    for (int iView = 0; iView < nView; iView++) {
        convolution1d(&pout[pnDct[X]*iView], &pin[pnDct[X]*iView], pker, pnDct[X]);
    }
    
    free(pker);
    pker = 0;
        
    return ;
}