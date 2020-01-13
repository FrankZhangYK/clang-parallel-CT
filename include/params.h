/*
*  X-ray CT System parameters
*
* dAngle   : Measure from 0 until the angle [degree; (float)]
* dView	: Gap between view_(k) - view_(k-1) [degree (float)]
* nView	: # of the views [element (uint)]
* DSO      : Distance from the Source to the Object    [mm (float)]
* DSD      : Distance from the Source to the Detector  [mm (float)]
*/
float   dAngle          = 360.0f;
int     nView           = 360;
float   dView           = dAngle/nView;
float   DSO             = 400.0f;
float   DSD             = 800.0f;

/*
*  X-ray CT Detector parameters
*
* pdDct[2]         : Detector pitch [mm (float)]
* pnDct[2]         : Number of detector [element (uint)]
* pdOffsetDct[2]	: Index of shifted detector [element (float; +, -)]
* -----------------------------------------------------
* '*Dct[Y]' parameters are only used when 3D CT system. 
* -----------------------------------------------------
*/

float   pdDct[2]        = {1.0f, 0.7f};
int     pnDct[2]        = {1, 400};
float   pdOffsetDct[2]  = {0.0f, 0.0f};

/*
*  Image Object parameters
*
*  pdImg[3]        : The resolution of the element of the image voxel
*  pnImg[3]        : The number of the element of the the image voxel
*  pdOffsetImg[3]  : The offset (shift) from the centor of the image
* ----------------------------------------------------
* '*ImgZ' parameters are only used when 3D CT system. 
* ----------------------------------------------------
*/

float   pdImg[3]        = {1, 1, 1};
int     pnImg[3]        = {256, 256, 1};
float   pdOffsetImg[3]  = {0.0f, 0.0f, 0.0f};
