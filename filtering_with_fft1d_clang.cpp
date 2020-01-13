/*
 * IF, datatype is double
 * mex -L.\lib -L.\include -llibfftw3-3.lib  filtering_with_fft1d_cpu.cpp
 *
 * ELSE IF, datatype is single
 * mex -L.\lib -L.\include -llibfftw3f-3.lib  filtering_with_fft1d_cpu.cpp
 *
 * ELSE IF, datatype is long
 * mex -L.\lib -L.\include -llibfftw3l-3.lib  filtering_with_fft1d_cpu.cpp
 *
 */

#include "./include/util.h"
#include "./params.h"
#include "./include/fftw3.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

using namespace std;

void    runFourierTransform1d(float *pout, float *pin,
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

    sprintf(pchFile, "prj_dct%d_view%d.raw", pnDct[X], nView);
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
    runFourierTransform1d(pout, pin,
                        DSO, DSD, dView, nView,
                        pdDct, pnDct, pdOffsetDct,
                        pdImg, pnImg, pdOffsetImg);
        
    sprintf(pchFile, "flt_prj_dct%d_view%d.raw", pnDct[X], nView);
    pfid = fopen(pchFile, "wb");

    fwrite(pout, sizeof(float), pnDct[Y]*pnDct[X]*nView, pfid);
    fclose(pfid); pfid = 0;

    free(pin);  pin = 0;
    free(pout); pout = 0;

    return 0;
}

void    runFourierTransform1d(float *pout, float *pin,
                        float DSO, float DSD, float dView, int nView,
                        float *pdDct, int *pnDct, float *pdOffsetDct,
                        float *pdImg, int *pnImg, float *pdOffsetImg) {
    
    
    int     nDctExtX	= pow(2, ceil(log2(2.0f*pnDct[X])));
    float   pnDctExt[2] = {pnDct[Y], nDctExtX};
    
    float   *pker       = (float*)   malloc(pnDctExt[X] * sizeof(float));
    memset(pker, 0, pnDctExt[X] * sizeof(float));
    
    float   *pview      = (float*)   malloc(pnDctExt[X] * sizeof(float));
    memset(pview, 0, pnDctExt[X] * sizeof(float));
    
    generate_filter(pker, pdDct[X], pnDct[X]);
    
    /*
     *
     */  
    
//     fftwf_complex	*pker_ft	= (fftwf_complex*)  fftwf_malloc(pnDct[X] * sizeof(fftwf_complex));
    fftwf_complex	*pker_ft	= (fftwf_complex*)  fftwf_malloc(pnDctExt[X] * sizeof(fftwf_complex));
    memset(pker_ft, 0, pnDctExt[X] * sizeof(fftwf_complex));
    
//     fftwf_complex	*pview_ft	= (fftwf_complex*)  fftwf_malloc(pnDct[X] * sizeof(fftwf_complex));
    fftwf_complex	*pview_ft	= (fftwf_complex*)  fftwf_malloc(pnDctExt[X] * sizeof(fftwf_complex));
    memset(pview_ft, 0, pnDctExt[X] * sizeof(fftwf_complex));

    
	fftwf_plan p, pi;
	p = fftwf_plan_dft_r2c_1d(pnDctExt[X], pker, pker_ft, FFTW_ESTIMATE);    
    
    fftwf_execute(p); 
    
    /*
     *
     */
    
    memset(pker, 0, pnDctExt[X] * sizeof(float));
    
	p = fftwf_plan_dft_r2c_1d(pnDctExt[X], pker, pview_ft, FFTW_ESTIMATE);
    pi= fftwf_plan_dft_c2r_1d(pnDctExt[X], pview_ft, pview, FFTW_ESTIMATE);
    
    fftwf_complex pview_ft_;
    
    for (int iView = 0; iView < nView; iView++) {
        
        memcpy(pker, &pin[pnDct[X]*iView], pnDct[X] * sizeof(float));
        fftwf_execute(p);  
        
        for (int iDctX = 0; iDctX < pnDctExt[X]; iDctX++) {
            pview_ft_[REAL]         = pview_ft[iDctX][REAL];
            pview_ft_[IMAG]         = pview_ft[iDctX][IMAG];
            pview_ft[iDctX][REAL]   = 1.0f/pnDctExt[X]*(pview_ft_[REAL]*pker_ft[iDctX][REAL] - pview_ft_[IMAG]*pker_ft[iDctX][IMAG]);
            pview_ft[iDctX][IMAG]   = 1.0f/pnDctExt[X]*(pview_ft_[REAL]*pker_ft[iDctX][IMAG] + pview_ft_[IMAG]*pker_ft[iDctX][REAL]);
        }

        fftwf_execute(pi); 
        memcpy(&pout[pnDct[X]*iView], &pview[pnDct[X] - 1], pnDct[X] * sizeof(float));
    }
    
	fftwf_destroy_plan(p);
    fftwf_destroy_plan(pi);
    fftwf_free(pker_ft); 
    fftwf_free(pview_ft); 
    free(pker);
    free(pview);
        
    return ;
}