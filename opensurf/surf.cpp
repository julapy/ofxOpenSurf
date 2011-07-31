/*********************************************************** 
*  --- OpenSURF ---                                        *
*  This library is distributed under the GNU GPL. Please   *
*  contact chris.evans@irisys.co.uk for more information.  *
*                                                          *
*  C. Evans, Research Into Robust Visual Features,         *
*  MSc University of Bristol, 2008.                        *
*                                                          *
************************************************************/

#include "utils.h"

#include "surf.h"

//-------------------------------------------------------
//! SURF priors (these need not be done at runtime)
//const float pi = 3.14159f;

const double gauss25 [7][7] = {
  0.02350693969273,0.01849121369071,0.01239503121241,0.00708015417522,0.00344628101733,0.00142945847484,0.00050524879060,
  0.02169964028389,0.01706954162243,0.01144205592615,0.00653580605408,0.00318131834134,0.00131955648461,0.00046640341759,
  0.01706954162243,0.01342737701584,0.00900063997939,0.00514124713667,0.00250251364222,0.00103799989504,0.00036688592278,
  0.01144205592615,0.00900063997939,0.00603330940534,0.00344628101733,0.00167748505986,0.00069579213743,0.00024593098864,
  0.00653580605408,0.00514124713667,0.00344628101733,0.00196854695367,0.00095819467066,0.00039744277546,0.00014047800980,
  0.00318131834134,0.00250251364222,0.00167748505986,0.00095819467066,0.00046640341759,0.00019345616757,0.00006837798818,
  0.00131955648461,0.00103799989504,0.00069579213743,0.00039744277546,0.00019345616757,0.00008024231247,0.00002836202103
};

const double gauss33 [11][11] = {
  0.014614763,0.013958917,0.012162744,0.00966788,0.00701053,0.004637568,0.002798657,0.001540738,0.000773799,0.000354525,0.000148179,
  0.013958917,0.013332502,0.011616933,0.009234028,0.006695928,0.004429455,0.002673066,0.001471597,0.000739074,0.000338616,0.000141529,
  0.012162744,0.011616933,0.010122116,0.008045833,0.005834325,0.003859491,0.002329107,0.001282238,0.000643973,0.000295044,0.000123318,
  0.00966788,0.009234028,0.008045833,0.006395444,0.004637568,0.003067819,0.001851353,0.001019221,0.000511879,0.000234524,9.80224E-05,
  0.00701053,0.006695928,0.005834325,0.004637568,0.003362869,0.002224587,0.001342483,0.000739074,0.000371182,0.000170062,7.10796E-05,
  0.004637568,0.004429455,0.003859491,0.003067819,0.002224587,0.001471597,0.000888072,0.000488908,0.000245542,0.000112498,4.70202E-05,
  0.002798657,0.002673066,0.002329107,0.001851353,0.001342483,0.000888072,0.000535929,0.000295044,0.000148179,6.78899E-05,2.83755E-05,
  0.001540738,0.001471597,0.001282238,0.001019221,0.000739074,0.000488908,0.000295044,0.00016243,8.15765E-05,3.73753E-05,1.56215E-05,
  0.000773799,0.000739074,0.000643973,0.000511879,0.000371182,0.000245542,0.000148179,8.15765E-05,4.09698E-05,1.87708E-05,7.84553E-06,
  0.000354525,0.000338616,0.000295044,0.000234524,0.000170062,0.000112498,6.78899E-05,3.73753E-05,1.87708E-05,8.60008E-06,3.59452E-06,
  0.000148179,0.000141529,0.000123318,9.80224E-05,7.10796E-05,4.70202E-05,2.83755E-05,1.56215E-05,7.84553E-06,3.59452E-06,1.50238E-06
};

//-------------------------------------------------------

//! Destructor
Surf::~Surf()
{
}

//-------------------------------------------------------

//! Constructor
Surf::Surf(IplImage *img, IpVec &ipts)
: ipts(ipts)
{
  this->img = img;
}

//-------------------------------------------------------

//! Describe all features in the supplied vector
void Surf::getDescriptors(bool upright)
{
  // Check there are Ipoints to be described
  if (!ipts.size()) return;

  // Get the size of the vector for fixed loop bounds
  int ipts_size = (int)ipts.size();

  if (upright)
  {
    // U-SURF loop just gets descriptors
    for (int i = 0; i < ipts_size; ++i)
    {
      // Set the Ipoint to be described
      index = i;

      // Extract upright (i.e. not rotation invariant) descriptors
      getUprightDescriptor();
    }
  }
  else
  {
    // Main SURF-64 loop assigns orientations and gets descriptors
    for (int i = 0; i < ipts_size; ++i)
    {
      // Set the Ipoint to be described
      index = i;

      // Assign Orientations and extract rotation invariant descriptors
      getOrientation();
      getDescriptor();
    }
  }
}

//-------------------------------------------------------

//! Assign the supplied Ipoint an orientation
void Surf::getOrientation()
{
  Ipoint *ipt = &ipts.at(index);
  float gauss, scale = ipt->scale;
  int s = fRound(scale), r = fRound(ipt->y), c = fRound(ipt->x);
  std::vector<float> resX, resY, Ang;

  // calculate haar responses for points within radius of 6*scale
  for(int i = -6; i <= 6; ++i) 
  {
    for(int j = -6; j <= 6; ++j) 
    {
      if(i*i + j*j < 36) 
      {
        gauss = static_cast<float>(gauss25[abs(i)][abs(j)]);
        resX.push_back( gauss * haarX(r+j*s, c+i*s, 4*s) );
        resY.push_back( gauss * haarY(r+j*s, c+i*s, 4*s) );
        Ang.push_back(getAngle(resX.back(), resY.back()));
      }
    }
  }

  // calculate the dominant direction 
  float sumX, sumY;
  float max=0, old_max = 0, orientation = 0, old_orientation = 0;
  float ang1, ang2, ang;

  // loop slides pi/3 window around feature point
  for(ang1 = 0; ang1 < 2*pi;  ang1+=0.15f) {
    ang2 = ( ang1+pi/3.0f > 2*pi ? ang1-5.0f*pi/3.0f : ang1+pi/3.0f);
    sumX = sumY = 0; 
    for(unsigned int k = 0; k < Ang.size(); k++) 
    {
      // get angle from the x-axis of the sample point
      ang = Ang.at(k);

      // determine whether the point is within the window
      if (ang1 < ang2 && ang1 < ang && ang < ang2) 
      {
        sumX+=resX.at(k);  
        sumY+=resY.at(k);
      } 
      else if (ang2 < ang1 && 
        ((ang > 0 && ang < ang2) || (ang > ang1 && ang < 2*pi) )) 
      {
        sumX+=resX.at(k);  
        sumY+=resY.at(k);
      }
    }

    // if the vector produced from this window is longer than all 
    // previous vectors then this forms the new dominant direction
    if (sumX*sumX + sumY*sumY > max) 
    {
      // store largest orientation
      max = sumX*sumX + sumY*sumY;
      orientation = getAngle(sumX, sumY);
    }
  }

  // assign orientation of the dominant response vector
  ipt->orientation = orientation;
}

//-------------------------------------------------------

//! Get the modified descriptor. See Agrawal ECCV 08
//! Modified descriptor contributed by Pablo Fernandez
void Surf::getDescriptor()
{
  int y, x, sample_x, sample_y, count=0;
  int i = 0, ix = 0, j = 0, jx = 0, xs = 0, ys = 0;
  float scale, *desc, dx, dy, mdx, mdy, co, si;
  float gauss_s1 = 0.f, gauss_s2 = 0.f;
  float rx = 0.f, ry = 0.f, rrx = 0.f, rry = 0.f, len = 0.f;
  float cx = -0.5f, cy = 0.f; //Subregion centers for the 4x4 gaussian weighting

  Ipoint *ipt = &ipts.at(index);
  scale = ipt->scale;
  x = fRound(ipt->x);
  y = fRound(ipt->y);  
  co = cos(ipt->orientation);
  si = sin(ipt->orientation);
  desc = ipt->descriptor;

  i = -8;

  //Calculate descriptor for this interest point
  //Area of size 24 s x 24 s
  //***********************************************
  while(i < 12)
  {
    j = -8;
    i = i-4;

    cx += 1.f;
    cy = -0.5f;

    while(j < 12) 
    {
      dx=dy=mdx=mdy=0.f;
      cy += 1.f;

      j = j - 4;

      ix = i + 5;
      jx = j + 5;

      xs = fRound(x + ( -jx*scale*si + ix*scale*co));
      ys = fRound(y + ( jx*scale*co + ix*scale*si));

      for (int k = i; k < i + 9; ++k) 
      {
        for (int l = j; l < j + 9; ++l) 
        {
          //Get coords of sample point on the rotated axis
          sample_x = fRound(x + (-l*scale*si + k*scale*co));
          sample_y = fRound(y + ( l*scale*co + k*scale*si));

          //Get the gaussian weighted x and y responses
          gauss_s1 = gaussian(xs-sample_x,ys-sample_y,2.5f*scale);

          rx = haarX(sample_y, sample_x, 2*fRound(scale));
          ry = haarY(sample_y, sample_x, 2*fRound(scale));

          //Get the gaussian weighted x and y responses on rotated axis
          rrx = -rx*si + ry*co;
          rry = rx*co + ry*si;

          rrx = gauss_s1*rrx;
          rry = gauss_s1*rry;

          dx += rrx;
          dy += rry;
          mdx += fabs(rrx);
          mdy += fabs(rry);

        }
      }

      //Add the values to the descriptor vector
      gauss_s2 = gaussian(cx-2.0f,cy-2.0f,1.5f);

      desc[count++] = dx*gauss_s2;
      desc[count++] = dy*gauss_s2;
      desc[count++] = mdx*gauss_s2;
      desc[count++] = mdy*gauss_s2;

      len += dx*dx + dy*dy + mdx*mdx + mdy*mdy;

      j += 9;
    }
    i += 9;
  }

  //Convert to Unit Vector
  len = sqrt(len);
  for(int i = 0; i < 64; i++)
    desc[i] /= len;

}


//-------------------------------------------------------

//! Get the upright descriptor vector of the provided Ipoint
void Surf::getUprightDescriptor()
{
  int y, x, count=0;
  float scale, *desc, dx, dy, mdx, mdy;
  float gauss, rx, ry, len = 0.f;

  Ipoint *ipt = &ipts.at(index);
  scale = ipt->scale;
  y = fRound(ipt->y);  
  x = fRound(ipt->x);
  desc = ipt->descriptor;

  // Calculate descriptor for this interest point
  for (int i = -10; i < 10; i+=5)
  {
    for (int j = -10; j < 10; j+=5) 
    {
      dx=dy=mdx=mdy=0;
      for (int k = i; k < i + 5; ++k) 
      {
        for (int l = j; l < j + 5; ++l) 
        {
          // get Gaussian weighted x and y responses
          gauss = static_cast<float>(gauss33[abs(k)][abs(l)]);
          rx = gauss * haarX(fRound(k*scale+y), fRound(l*scale+x), 2*fRound(scale));
          ry = gauss * haarY(fRound(k*scale+y), fRound(l*scale+x), 2*fRound(scale));

          dx += rx;
          dy += ry;
          mdx += fabs(rx);
          mdy += fabs(ry);
        }
      }

      // add the values to the descriptor vector
      desc[count++] = dx;
      desc[count++] = dy;
      desc[count++] = mdx;
      desc[count++] = mdy;

      // store the current length^2 of the vector
      len += dx*dx + dy*dy + mdx*mdx + mdy*mdy;
    }
  }

  // convert to unit vector
  len = sqrt(len);
  for(int i = 0; i < 64; i++)
    desc[i] /= len;

}

//-------------------------------------------------------

//! Calculate the value of the 2d gaussian at x,y
inline float Surf::gaussian(int x, int y, float sig)
{
  return (1.0f/(2.0f*pi*sig*sig)) * exp( -(x*x+y*y)/(2.0f*sig*sig));
}

//-------------------------------------------------------

//! Calculate the value of the 2d gaussian at x,y
inline float Surf::gaussian(float x, float y, float sig)
{
  return 1.0f/(2.0f*pi*sig*sig) * exp( -(x*x+y*y)/(2.0f*sig*sig));
}

//-------------------------------------------------------

//! Calculate Haar wavelet responses in x direction
inline float Surf::haarX(int row, int column, int s)
{
  return BoxIntegral(img, row-s/2, column, s, s/2) 
    -1 * BoxIntegral(img, row-s/2, column-s/2, s, s/2);
}

//-------------------------------------------------------

//! Calculate Haar wavelet responses in y direction
inline float Surf::haarY(int row, int column, int s)
{
  return BoxIntegral(img, row, column-s/2, s/2, s) 
    -1 * BoxIntegral(img, row-s/2, column-s/2, s/2, s);
}

//-------------------------------------------------------

//! Get the angle from the +ve x-axis of the vector given by (X Y)
float Surf::getAngle(float X, float Y)
{
  if(X >= 0 && Y >= 0)
    return atan(Y/X);

  if(X < 0 && Y >= 0)
    return pi - atan(-Y/X);

  if(X < 0 && Y < 0)
    return pi + atan(Y/X);

  if(X >= 0 && Y < 0)
    return 2*pi - atan(-Y/X);

  return 0;
}