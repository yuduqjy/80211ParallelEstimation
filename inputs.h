#include <stdio.h>
#include <math.h>
#include <vector>
#include <complex.h>

#define PI M_PI
#define TWOPI (2.0*PI)
#define SIZESYMBOL 53*15
#define SAMPUTIL 53
#define OFDMBLK 15
#define P0 6
#define P1 20
#define P2 34
#define P3 48

using namespace std;

double OW2 = 9.6172e-08;

double complex tx_preamble_fft[53] = {
   8.8752 - 0.0000*I,
   8.8755 - 0.0000*I,
  -8.8752 + 0.0000*I,
  -8.8754 - 0.0000*I,
   8.8757 + 0.0000*I,
   8.8753 + 0.0000*I,
  -8.8749 - 0.0000*I,
   8.8755 - 0.0000*I,
  -8.8750 + 0.0000*I,
   8.8750 - 0.0000*I,
   8.8750 + 0.0000*I,
   8.8751 + 0.0000*I,
   8.8746 - 0.0000*I,
   8.8755 - 0.0000*I,
   8.8749 + 0.0000*I,
  -8.8753 + 0.0000*I,
  -8.8757 + 0.0000*I,
   8.8753 + 0.0000*I,
   8.8752 + 0.0000*I,
  -8.8747 - 0.0000*I,
   8.8747 + 0.0000*I,
  -8.8753 - 0.0000*I,
   8.8756 + 0.0000*I,
   8.8751 - 0.0000*I,
   8.8749 - 0.0000*I,
   8.8753 + 0.0000*I,
  -0.0002 + 0.0000*I,
   8.8753 - 0.0000*I,
  -8.8753 + 0.0000*I,
  -8.8754 + 0.0000*I,
   8.8755 + 0.0000*I,
   8.8757 - 0.0000*I,
  -8.8756 - 0.0000*I,
   8.8754 - 0.0000*I,
  -8.8752 + 0.0000*I,
   8.8751 + 0.0000*I,
  -8.8752 - 0.0000*I,
  -8.8755 - 0.0000*I,
  -8.8753 - 0.0000*I,
  -8.8751 + 0.0000*I,
  -8.8743 + 0.0000*I,
   8.8757 - 0.0000*I,
   8.8750 + 0.0000*I,
  -8.8752 + 0.0000*I,
  -8.8751 + 0.0000*I,
   8.8756 + 0.0000*I,
  -8.8754 + 0.0000*I,
   8.8752 - 0.0000*I,
  -8.8750 + 0.0000*I,
   8.8754 + 0.0000*I,
   8.8752 + 0.0000*I,
   8.8748 + 0.0000*I,
   8.8753 - 0.0000*I
};
double complex rx_preamble_fft[53] = {
   0.0802 + 0.0082*I,
   0.0827 + 0.0066*I,
  -0.0849 - 0.0052*I,
  -0.0841 - 0.0016*I,
   0.0793 - 0.0058*I,
   0.0766 - 0.0114*I,
  -0.0744 + 0.0134*I,
   0.0713 - 0.0192*I,
  -0.0703 + 0.0208*I,
   0.0702 - 0.0223*I,
   0.0717 - 0.0237*I,
   0.0735 - 0.0230*I,
   0.0756 - 0.0226*I,
   0.0772 - 0.0228*I,
   0.0783 - 0.0231*I,
  -0.0806 + 0.0237*I,
  -0.0802 + 0.0252*I,
   0.0784 - 0.0299*I,
   0.0768 - 0.0344*I,
  -0.0746 + 0.0366*I,
   0.0726 - 0.0431*I,
  -0.0702 + 0.0447*I,
   0.0703 - 0.0486*I,
   0.0682 - 0.0510*I,
   0.0679 - 0.0524*I,
   0.0685 - 0.0536*I,
   0.0002 + 0.0010*I,
   0.0681 - 0.0542*I,
  -0.0693 + 0.0547*I,
  -0.0687 + 0.0568*I,
   0.0679 - 0.0608*I,
   0.0651 - 0.0649*I,
  -0.0636 + 0.0657*I,
   0.0593 - 0.0726*I,
  -0.0548 + 0.0743*I,
   0.0518 - 0.0803*I,
  -0.0481 + 0.0814*I,
  -0.0444 + 0.0839*I,
  -0.0422 + 0.0855*I,
  -0.0415 + 0.0861*I,
  -0.0400 + 0.0858*I,
   0.0413 - 0.0877*I,
   0.0410 - 0.0876*I,
  -0.0366 + 0.0865*I,
  -0.0326 + 0.0886*I,
   0.0326 - 0.0908*I,
  -0.0239 + 0.0944*I,
   0.0209 - 0.0981*I,
  -0.0095 + 0.1031*I,
   0.0065 - 0.1051*I,
   0.0003 - 0.1073*I,
  -0.0055 - 0.1084*I,
  -0.0089 - 0.1055*I
};
double complex tx_symb[SIZESYMBOL] = {
   8.8746 + 0.0000*I,
   8.8751 + 0.0000*I,
   8.8757 + 0.0000*I,
   8.8749 + 0.0000*I,
   8.8760 + 0.0000*I,
   8.8756 + 0.0000*I,
   8.8749 - 0.0000*I,
   8.8755 + 0.0000*I,
   8.8753 - 0.0000*I,
   8.8746 - 0.0000*I,
  -8.8751 + 0.0000*I,
   8.8753 + 0.0000*I,
   8.8750 - 0.0000*I,
   8.8752 - 0.0000*I,
   8.8756 + 0.0000*I,
  -8.8749 - 0.0000*I,
  -8.8748 + 0.0000*I,
  -8.8755 + 0.0000*I,
   8.8753 + 0.0000*I,
   8.8750 + 0.0000*I,
   8.8758 + 0.0000*I,
  -8.8751 + 0.0000*I,
   8.8755 + 0.0000*I,
   8.8756 + 0.0000*I,
  -8.8753 + 0.0000*I,
   8.8753 + 0.0000*I,
  -0.0001 + 0.0000*I,
   8.8754 - 0.0000*I,
   8.8753 + 0.0000*I,
   8.8751 + 0.0000*I,
  -8.8755 - 0.0000*I,
  -8.8749 - 0.0000*I,
   8.8752 - 0.0000*I,
   8.8753 + 0.0000*I,
   8.8749 + 0.0000*I,
  -8.8752 - 0.0000*I,
  -8.8755 + 0.0000*I,
  -8.8746 + 0.0000*I,
  -8.8743 - 0.0000*I,
  -8.8752 + 0.0000*I,
  -8.8751 - 0.0000*I,
  -8.8755 + 0.0000*I,
   8.8753 + 0.0000*I,
  -8.8760 - 0.0000*I,
  -8.8753 + 0.0000*I,
  -8.8749 + 0.0000*I,
   8.8750 + 0.0000*I,
  -8.8753 - 0.0000*I,
   8.8750 + 0.0000*I,
   8.8754 - 0.0000*I,
  -8.8751 + 0.0000*I,
  -8.8755 - 0.0000*I,
   8.8760 - 0.0000*I,
  -8.8756 + 0.0000*I,
  -8.8751 - 0.0000*I,
  -8.8750 + 0.0000*I,
  -8.8752 + 0.0000*I,
  -8.8750 + 0.0000*I,
   8.8751 - 0.0000*I,
  -8.8747 - 0.0000*I,
   8.8750 - 0.0000*I,
  -8.8753 + 0.0000*I,
  -8.8751 + 0.0000*I,
   8.8751 + 0.0000*I,
   8.8755 - 0.0000*I,
  -8.8753 + 0.0000*I,
  -8.8752 - 0.0000*I,
  -8.8753 - 0.0000*I,
  -8.8757 - 0.0000*I,
   8.8757 - 0.0000*I,
  -8.8746 - 0.0000*I,
  -8.8753 + 0.0000*I,
   8.8752 - 0.0000*I,
  -8.8755 + 0.0000*I,
   8.8752 + 0.0000*I,
  -8.8750 - 0.0000*I,
  -8.8751 + 0.0000*I,
  -8.8748 - 0.0000*I,
  -8.8750 - 0.0000*I,
   0.0005 + 0.0000*I,
  -8.8755 + 0.0000*I,
  -8.8754 + 0.0000*I,
  -8.8751 - 0.0000*I,
   8.8749 + 0.0000*I,
   8.8752 + 0.0000*I,
   8.8749 + 0.0000*I,
   8.8754 + 0.0000*I,
   8.8748 + 0.0000*I,
  -8.8747 + 0.0000*I,
  -8.8757 + 0.0000*I,
  -8.8758 + 0.0000*I,
  -8.8745 + 0.0000*I,
   8.8750 - 0.0000*I,
  -8.8757 - 0.0000*I,
   8.8748 - 0.0000*I,
  -8.8749 + 0.0000*I,
   8.8748 + 0.0000*I,
  -8.8755 + 0.0000*I,
   8.8747 + 0.0000*I,
   8.8752 - 0.0000*I,
  -8.8749 + 0.0000*I,
   8.8749 - 0.0000*I,
   8.8752 - 0.0000*I,
  -8.8745 + 0.0000*I,
  -8.8747 - 0.0000*I,
   8.8756 + 0.0000*I,
   8.8753 + 0.0000*I,
  -8.8752 - 0.0000*I,
   8.8760 + 0.0000*I,
  -8.8751 + 0.0000*I,
  -8.8754 + 0.0000*I,
   8.8757 - 0.0000*I,
   8.8750 - 0.0000*I,
  -8.8759 + 0.0000*I,
   8.8757 - 0.0000*I,
  -8.8749 + 0.0000*I,
   8.8755 + 0.0000*I,
  -8.8752 - 0.0000*I,
   8.8749 + 0.0000*I,
  -8.8753 - 0.0000*I,
   8.8754 + 0.0000*I,
  -8.8752 - 0.0000*I,
   8.8749 - 0.0000*I,
   8.8749 + 0.0000*I,
   8.8752 + 0.0000*I,
   8.8749 - 0.0000*I,
   8.8749 - 0.0000*I,
  -8.8753 - 0.0000*I,
   8.8749 + 0.0000*I,
   8.8750 + 0.0000*I,
  -8.8751 + 0.0000*I,
  -8.8744 - 0.0000*I,
   0.0001 + 0.0000*I,
   8.8758 + 0.0000*I,
  -8.8760 + 0.0000*I,
   8.8752 + 0.0000*I,
  -8.8754 - 0.0000*I,
   8.8749 + 0.0000*I,
  -8.8750 + 0.0000*I,
   8.8751 + 0.0000*I,
  -8.8754 + 0.0000*I,
  -8.8755 + 0.0000*I,
  -8.8752 + 0.0000*I,
  -8.8755 - 0.0000*I,
  -8.8748 - 0.0000*I,
  -8.8751 - 0.0000*I,
   8.8750 - 0.0000*I,
   8.8755 + 0.0000*I,
   8.8751 + 0.0000*I,
  -8.8748 - 0.0000*I,
  -8.8756 - 0.0000*I,
  -8.8754 + 0.0000*I,
   8.8755 + 0.0000*I,
  -8.8752 - 0.0000*I,
   8.8752 - 0.0000*I,
  -8.8753 - 0.0000*I,
   8.8750 + 0.0000*I,
   8.8753 - 0.0000*I,
  -8.8756 + 0.0000*I,
  -8.8755 - 0.0000*I,
  -8.8749 - 0.0000*I,
   8.8753 + 0.0000*I,
  -8.8758 - 0.0000*I,
  -8.8755 - 0.0000*I,
   8.8748 - 0.0000*I,
   8.8758 + 0.0000*I,
   8.8754 + 0.0000*I,
  -8.8754 + 0.0000*I,
  -8.8751 + 0.0000*I,
   8.8750 + 0.0000*I,
  -8.8753 - 0.0000*I,
   8.8757 - 0.0000*I,
   8.8752 - 0.0000*I,
   8.8752 + 0.0000*I,
   8.8749 - 0.0000*I,
   8.8749 + 0.0000*I,
   8.8755 - 0.0000*I,
   8.8751 + 0.0000*I,
   8.8748 + 0.0000*I,
   8.8748 + 0.0000*I,
  -8.8753 + 0.0000*I,
   8.8750 - 0.0000*I,
  -8.8757 - 0.0000*I,
  -8.8750 - 0.0000*I,
   8.8748 - 0.0000*I,
  -0.0002 + 0.0000*I,
   8.8748 + 0.0000*I,
   8.8753 + 0.0000*I,
   8.8756 - 0.0000*I,
   8.8756 - 0.0000*I,
  -8.8751 - 0.0000*I,
   8.8754 + 0.0000*I,
   8.8759 + 0.0000*I,
   8.8747 + 0.0000*I,
   8.8751 + 0.0000*I,
  -8.8752 - 0.0000*I,
   8.8754 + 0.0000*I,
   8.8758 + 0.0000*I,
  -8.8747 - 0.0000*I,
  -8.8752 - 0.0000*I,
  -8.8749 + 0.0000*I,
   8.8750 + 0.0000*I,
   8.8748 - 0.0000*I,
  -8.8756 - 0.0000*I,
  -8.8750 - 0.0000*I,
   8.8752 + 0.0000*I,
  -8.8748 + 0.0000*I,
  -8.8748 + 0.0000*I,
  -8.8754 - 0.0000*I,
  -8.8755 + 0.0000*I,
  -8.8752 - 0.0000*I,
   8.8751 + 0.0000*I,
  -8.8754 + 0.0000*I,
   8.8751 + 0.0000*I,
   8.8760 + 0.0000*I,
  -8.8751 + 0.0000*I,
  -8.8753 + 0.0000*I,
  -8.8751 + 0.0000*I,
   8.8748 + 0.0000*I,
  -8.8754 + 0.0000*I,
   8.8755 - 0.0000*I,
  -8.8759 - 0.0000*I,
   8.8749 + 0.0000*I,
  -8.8754 - 0.0000*I,
  -8.8754 + 0.0000*I,
  -8.8751 - 0.0000*I,
  -8.8747 + 0.0000*I,
   8.8748 - 0.0000*I,
   8.8748 - 0.0000*I,
   8.8757 - 0.0000*I,
   8.8750 + 0.0000*I,
  -8.8749 + 0.0000*I,
   8.8756 + 0.0000*I,
   8.8747 + 0.0000*I,
  -8.8755 - 0.0000*I,
  -8.8751 - 0.0000*I,
  -8.8752 + 0.0000*I,
  -8.8752 - 0.0000*I,
  -0.0003 + 0.0000*I,
  -8.8749 + 0.0000*I,
   8.8751 + 0.0000*I,
   8.8749 + 0.0000*I,
   8.8750 + 0.0000*I,
   8.8748 - 0.0000*I,
  -8.8751 + 0.0000*I,
  -8.8747 + 0.0000*I,
   8.8754 + 0.0000*I,
  -8.8749 + 0.0000*I,
   8.8750 + 0.0000*I,
  -8.8748 - 0.0000*I,
   8.8758 + 0.0000*I,
  -8.8757 - 0.0000*I,
  -8.8753 - 0.0000*I,
  -8.8747 + 0.0000*I,
   8.8749 + 0.0000*I,
  -8.8743 - 0.0000*I,
  -8.8759 + 0.0000*I,
   8.8750 - 0.0000*I,
   8.8747 - 0.0000*I,
   8.8754 - 0.0000*I,
  -8.8753 - 0.0000*I,
   8.8753 - 0.0000*I,
   8.8756 + 0.0000*I,
   8.8752 + 0.0000*I,
   8.8752 + 0.0000*I,
   8.8754 - 0.0000*I,
  -8.8750 + 0.0000*I,
   8.8760 + 0.0000*I,
   8.8748 + 0.0000*I,
   8.8750 + 0.0000*I,
  -8.8746 - 0.0000*I,
   8.8759 + 0.0000*I,
  -8.8747 - 0.0000*I,
   8.8749 - 0.0000*I,
   8.8748 - 0.0000*I,
   8.8755 + 0.0000*I,
   8.8746 + 0.0000*I,
  -8.8747 - 0.0000*I,
   8.8753 + 0.0000*I,
  -8.8748 + 0.0000*I,
  -8.8747 + 0.0000*I,
  -8.8752 + 0.0000*I,
  -8.8749 - 0.0000*I,
  -8.8755 + 0.0000*I,
  -8.8749 + 0.0000*I,
  -8.8750 + 0.0000*I,
  -8.8751 + 0.0000*I,
   8.8752 - 0.0000*I,
  -8.8751 - 0.0000*I,
   8.8752 + 0.0000*I,
  -8.8753 + 0.0000*I,
   0.0001 + 0.0000*I,
   8.8749 - 0.0000*I,
   8.8756 - 0.0000*I,
   8.8751 - 0.0000*I,
  -8.8753 + 0.0000*I,
  -8.8749 + 0.0000*I,
   8.8752 - 0.0000*I,
  -8.8744 - 0.0000*I,
  -8.8750 + 0.0000*I,
   8.8749 - 0.0000*I,
   8.8753 + 0.0000*I,
   8.8750 - 0.0000*I,
  -8.8759 - 0.0000*I,
  -8.8747 + 0.0000*I,
   8.8752 - 0.0000*I,
  -8.8756 + 0.0000*I,
  -8.8753 + 0.0000*I,
   8.8747 + 0.0000*I,
   8.8754 + 0.0000*I,
  -8.8754 + 0.0000*I,
   8.8752 + 0.0000*I,
   8.8751 + 0.0000*I,
   8.8757 + 0.0000*I,
   8.8750 + 0.0000*I,
   8.8749 + 0.0000*I,
   8.8751 - 0.0000*I,
   8.8751 - 0.0000*I,
   8.8750 - 0.0000*I,
   8.8754 + 0.0000*I,
   8.8755 + 0.0000*I,
   8.8754 + 0.0000*I,
  -8.8754 - 0.0000*I,
  -8.8753 + 0.0000*I,
   8.8752 - 0.0000*I,
   8.8749 - 0.0000*I,
   8.8753 - 0.0000*I,
   8.8755 - 0.0000*I,
  -8.8756 + 0.0000*I,
  -8.8749 + 0.0000*I,
   8.8752 + 0.0000*I,
   8.8752 + 0.0000*I,
  -8.8757 + 0.0000*I,
  -8.8757 - 0.0000*I,
  -8.8748 + 0.0000*I,
  -8.8758 + 0.0000*I,
  -8.8757 + 0.0000*I,
  -8.8750 - 0.0000*I,
  -8.8752 + 0.0000*I,
  -8.8749 + 0.0000*I,
   8.8753 + 0.0000*I,
   8.8757 + 0.0000*I,
   8.8753 + 0.0000*I,
  -8.8750 + 0.0000*I,
  -0.0002 + 0.0000*I,
   8.8753 - 0.0000*I,
  -8.8748 + 0.0000*I,
  -8.8752 - 0.0000*I,
  -8.8758 - 0.0000*I,
   8.8754 + 0.0000*I,
  -8.8752 - 0.0000*I,
  -8.8753 + 0.0000*I,
  -8.8755 + 0.0000*I,
  -8.8751 + 0.0000*I,
   8.8755 - 0.0000*I,
  -8.8752 - 0.0000*I,
  -8.8751 - 0.0000*I,
   8.8747 - 0.0000*I,
   8.8751 + 0.0000*I,
  -8.8755 - 0.0000*I,
  -8.8752 + 0.0000*I,
   8.8753 + 0.0000*I,
   8.8754 + 0.0000*I,
   8.8747 - 0.0000*I,
   8.8748 - 0.0000*I,
   8.8751 + 0.0000*I,
  -8.8752 - 0.0000*I,
   8.8750 + 0.0000*I,
  -8.8755 + 0.0000*I,
  -8.8747 + 0.0000*I,
   8.8749 + 0.0000*I,
  -8.8756 + 0.0000*I,
  -8.8753 - 0.0000*I,
   8.8748 + 0.0000*I,
  -8.8748 + 0.0000*I,
   8.8750 + 0.0000*I,
   8.8750 - 0.0000*I,
  -8.8752 - 0.0000*I,
   8.8745 + 0.0000*I,
  -8.8753 + 0.0000*I,
  -8.8746 - 0.0000*I,
   8.8753 + 0.0000*I,
   8.8751 - 0.0000*I,
   8.8760 - 0.0000*I,
   8.8751 - 0.0000*I,
   8.8751 + 0.0000*I,
  -8.8747 + 0.0000*I,
   8.8757 - 0.0000*I,
  -8.8754 - 0.0000*I,
  -8.8754 + 0.0000*I,
   8.8754 - 0.0000*I,
  -8.8756 - 0.0000*I,
   8.8750 - 0.0000*I,
  -8.8759 + 0.0000*I,
  -8.8751 - 0.0000*I,
   8.8755 + 0.0000*I,
   8.8751 + 0.0000*I,
  -0.0001 + 0.0000*I,
   8.8754 - 0.0000*I,
  -8.8753 + 0.0000*I,
   8.8749 + 0.0000*I,
   8.8752 - 0.0000*I,
  -8.8748 - 0.0000*I,
   8.8750 + 0.0000*I,
   8.8757 + 0.0000*I,
  -8.8750 + 0.0000*I,
   8.8750 + 0.0000*I,
   8.8755 - 0.0000*I,
   8.8750 + 0.0000*I,
  -8.8751 - 0.0000*I,
  -8.8751 - 0.0000*I,
   8.8757 + 0.0000*I,
   8.8754 + 0.0000*I,
  -8.8755 + 0.0000*I,
  -8.8749 + 0.0000*I,
  -8.8748 + 0.0000*I,
  -8.8746 + 0.0000*I,
   8.8747 + 0.0000*I,
  -8.8754 - 0.0000*I,
   8.8754 - 0.0000*I,
   8.8751 - 0.0000*I,
  -8.8752 + 0.0000*I,
  -8.8754 - 0.0000*I,
   8.8748 + 0.0000*I,
  -8.8744 + 0.0000*I,
  -8.8758 + 0.0000*I,
  -8.8750 + 0.0000*I,
   8.8751 - 0.0000*I,
  -8.8751 - 0.0000*I,
  -8.8749 - 0.0000*I,
   8.8751 + 0.0000*I,
   8.8755 - 0.0000*I,
   8.8744 - 0.0000*I,
  -8.8755 - 0.0000*I,
   8.8754 + 0.0000*I,
   8.8748 - 0.0000*I,
   8.8749 + 0.0000*I,
  -8.8746 - 0.0000*I,
   8.8755 + 0.0000*I,
   8.8753 + 0.0000*I,
  -8.8753 - 0.0000*I,
   8.8752 + 0.0000*I,
   8.8756 + 0.0000*I,
  -8.8749 - 0.0000*I,
   8.8751 + 0.0000*I,
  -8.8753 - 0.0000*I,
   8.8750 + 0.0000*I,
   8.8752 + 0.0000*I,
   8.8750 + 0.0000*I,
   8.8750 + 0.0000*I,
  -0.0002 + 0.0000*I,
  -8.8750 - 0.0000*I,
  -8.8756 + 0.0000*I,
  -8.8755 + 0.0000*I,
  -8.8756 - 0.0000*I,
   8.8753 - 0.0000*I,
   8.8752 - 0.0000*I,
  -8.8752 + 0.0000*I,
   8.8750 + 0.0000*I,
   8.8754 + 0.0000*I,
   8.8755 - 0.0000*I,
  -8.8749 - 0.0000*I,
   8.8753 + 0.0000*I,
   8.8751 - 0.0000*I,
  -8.8751 + 0.0000*I,
   8.8752 - 0.0000*I,
  -8.8750 + 0.0000*I,
   8.8756 + 0.0000*I,
   8.8754 + 0.0000*I,
  -8.8754 - 0.0000*I,
   8.8752 + 0.0000*I,
   8.8749 + 0.0000*I,
  -8.8748 + 0.0000*I,
   8.8745 - 0.0000*I,
  -8.8756 + 0.0000*I,
   8.8754 + 0.0000*I,
  -8.8751 + 0.0000*I,
   8.8751 - 0.0000*I,
   8.8749 + 0.0000*I,
  -8.8754 + 0.0000*I,
   8.8753 - 0.0000*I,
  -8.8751 + 0.0000*I,
  -8.8752 + 0.0000*I,
   8.8752 + 0.0000*I,
   8.8754 + 0.0000*I,
   8.8757 - 0.0000*I,
   8.8754 - 0.0000*I,
  -8.8753 + 0.0000*I,
   8.8754 + 0.0000*I,
  -8.8753 + 0.0000*I,
   8.8751 + 0.0000*I,
   8.8754 + 0.0000*I,
  -8.8749 - 0.0000*I,
   8.8749 + 0.0000*I,
  -8.8750 + 0.0000*I,
   8.8743 + 0.0000*I,
  -8.8752 - 0.0000*I,
   8.8751 - 0.0000*I,
   8.8753 - 0.0000*I,
  -8.8754 - 0.0000*I,
   8.8750 + 0.0000*I,
  -8.8750 + 0.0000*I,
   8.8747 + 0.0000*I,
   0.0005 + 0.0000*I,
  -8.8752 + 0.0000*I,
   8.8760 + 0.0000*I,
  -8.8751 + 0.0000*I,
  -8.8755 - 0.0000*I,
  -8.8751 - 0.0000*I,
  -8.8749 + 0.0000*I,
  -8.8758 - 0.0000*I,
  -8.8752 + 0.0000*I,
  -8.8753 - 0.0000*I,
   8.8758 - 0.0000*I,
  -8.8748 + 0.0000*I,
  -8.8758 + 0.0000*I,
  -8.8757 - 0.0000*I,
   8.8753 - 0.0000*I,
   8.8752 - 0.0000*I,
  -8.8753 + 0.0000*I,
   8.8758 + 0.0000*I,
   8.8756 - 0.0000*I,
   8.8753 - 0.0000*I,
  -8.8754 + 0.0000*I,
   8.8752 + 0.0000*I,
  -8.8749 + 0.0000*I,
   8.8751 + 0.0000*I,
   8.8751 + 0.0000*I,
  -8.8753 + 0.0000*I,
  -8.8756 + 0.0000*I,
  -8.8752 + 0.0000*I,
  -8.8753 + 0.0000*I,
  -8.8748 + 0.0000*I,
   8.8754 + 0.0000*I,
   8.8751 + 0.0000*I,
  -8.8752 - 0.0000*I,
  -8.8753 + 0.0000*I,
  -8.8752 + 0.0000*I,
  -8.8754 + 0.0000*I,
  -8.8745 - 0.0000*I,
   8.8750 + 0.0000*I,
   8.8753 - 0.0000*I,
  -8.8751 + 0.0000*I,
   8.8751 - 0.0000*I,
   8.8753 + 0.0000*I,
  -8.8746 + 0.0000*I,
   8.8752 - 0.0000*I,
   8.8749 + 0.0000*I,
   8.8747 + 0.0000*I,
  -8.8754 - 0.0000*I,
   8.8751 + 0.0000*I,
  -8.8751 + 0.0000*I,
  -8.8751 + 0.0000*I,
  -8.8759 + 0.0000*I,
  -8.8751 - 0.0000*I,
   8.8754 - 0.0000*I,
   0.0002 + 0.0000*I,
   8.8755 + 0.0000*I,
   8.8745 - 0.0000*I,
   8.8755 + 0.0000*I,
  -8.8745 - 0.0000*I,
  -8.8759 + 0.0000*I,
  -8.8748 + 0.0000*I,
  -8.8752 + 0.0000*I,
   8.8752 + 0.0000*I,
  -8.8754 - 0.0000*I,
  -8.8745 + 0.0000*I,
   8.8753 - 0.0000*I,
   8.8748 - 0.0000*I,
  -8.8754 - 0.0000*I,
  -8.8752 + 0.0000*I,
   8.8756 + 0.0000*I,
  -8.8758 + 0.0000*I,
   8.8749 - 0.0000*I,
  -8.8754 + 0.0000*I,
  -8.8755 + 0.0000*I,
  -8.8756 + 0.0000*I,
   8.8745 - 0.0000*I,
  -8.8753 - 0.0000*I,
   8.8756 - 0.0000*I,
   8.8753 + 0.0000*I,
   8.8752 + 0.0000*I,
   8.8753 + 0.0000*I,
  -8.8750 + 0.0000*I,
   8.8756 + 0.0000*I,
  -8.8750 + 0.0000*I,
   8.8759 + 0.0000*I,
  -8.8750 + 0.0000*I,
  -8.8749 + 0.0000*I,
  -8.8750 - 0.0000*I,
  -8.8756 - 0.0000*I,
  -8.8753 + 0.0000*I,
   8.8753 - 0.0000*I,
   8.8745 + 0.0000*I,
   8.8756 - 0.0000*I,
   8.8759 - 0.0000*I,
  -8.8747 + 0.0000*I,
   8.8756 + 0.0000*I,
   8.8749 + 0.0000*I,
  -8.8749 - 0.0000*I,
  -8.8751 - 0.0000*I,
   8.8758 + 0.0000*I,
  -8.8754 + 0.0000*I,
   8.8755 - 0.0000*I,
   8.8755 - 0.0000*I,
   8.8744 + 0.0000*I,
   8.8751 - 0.0000*I,
  -8.8753 - 0.0000*I,
   8.8750 - 0.0000*I,
   0.0005 + 0.0000*I,
   8.8754 - 0.0000*I,
   8.8749 + 0.0000*I,
   8.8753 + 0.0000*I,
  -8.8751 + 0.0000*I,
  -8.8754 - 0.0000*I,
  -8.8757 - 0.0000*I,
  -8.8749 + 0.0000*I,
   8.8756 + 0.0000*I,
   8.8753 + 0.0000*I,
   8.8748 - 0.0000*I,
   8.8758 + 0.0000*I,
  -8.8748 - 0.0000*I,
  -8.8751 + 0.0000*I,
  -8.8753 + 0.0000*I,
   8.8755 + 0.0000*I,
  -8.8751 + 0.0000*I,
  -8.8752 - 0.0000*I,
  -8.8754 - 0.0000*I,
   8.8754 + 0.0000*I,
  -8.8750 + 0.0000*I,
   8.8753 - 0.0000*I,
   8.8750 - 0.0000*I,
   8.8750 + 0.0000*I,
  -8.8760 + 0.0000*I,
   8.8749 - 0.0000*I,
  -8.8758 + 0.0000*I,
   8.8743 - 0.0000*I,
  -8.8752 - 0.0000*I,
  -8.8748 + 0.0000*I,
   8.8751 - 0.0000*I,
   8.8747 - 0.0000*I,
   8.8751 - 0.0000*I,
   8.8756 + 0.0000*I,
  -8.8749 + 0.0000*I,
  -8.8750 + 0.0000*I,
   8.8757 + 0.0000*I,
   8.8754 + 0.0000*I,
  -8.8754 - 0.0000*I,
  -8.8752 + 0.0000*I,
   8.8754 + 0.0000*I,
   8.8754 + 0.0000*I,
  -8.8750 - 0.0000*I,
   8.8751 + 0.0000*I,
   8.8758 + 0.0000*I,
  -8.8748 + 0.0000*I,
   8.8747 - 0.0000*I,
  -8.8748 + 0.0000*I,
  -8.8753 - 0.0000*I,
   8.8752 - 0.0000*I,
  -8.8752 - 0.0000*I,
  -8.8745 - 0.0000*I,
   8.8753 - 0.0000*I,
   0.0002 + 0.0000*I,
  -8.8753 - 0.0000*I,
  -8.8747 - 0.0000*I,
   8.8755 - 0.0000*I,
  -8.8752 - 0.0000*I,
   8.8750 + 0.0000*I,
   8.8754 - 0.0000*I,
   8.8754 + 0.0000*I,
   8.8752 + 0.0000*I,
  -8.8753 + 0.0000*I,
  -8.8758 + 0.0000*I,
   8.8754 + 0.0000*I,
   8.8758 + 0.0000*I,
  -8.8755 + 0.0000*I,
  -8.8751 - 0.0000*I,
   8.8753 - 0.0000*I,
  -8.8750 + 0.0000*I,
  -8.8757 + 0.0000*I,
   8.8757 + 0.0000*I,
   8.8750 + 0.0000*I,
   8.8756 + 0.0000*I,
  -8.8756 - 0.0000*I,
  -8.8756 + 0.0000*I,
  -8.8757 + 0.0000*I,
   8.8752 + 0.0000*I,
  -8.8749 + 0.0000*I,
   8.8750 - 0.0000*I,
  -8.8749 + 0.0000*I,
   8.8754 + 0.0000*I,
   8.8751 + 0.0000*I,
  -8.8741 - 0.0000*I,
   8.8753 - 0.0000*I,
   8.8753 + 0.0000*I,
   8.8751 + 0.0000*I,
  -8.8754 + 0.0000*I,
  -8.8751 + 0.0000*I,
  -8.8753 - 0.0000*I,
   8.8755 + 0.0000*I,
  -8.8755 - 0.0000*I,
   8.8756 + 0.0000*I,
  -8.8750 + 0.0000*I,
  -8.8756 + 0.0000*I,
   8.8755 - 0.0000*I,
   8.8750 - 0.0000*I,
   8.8758 - 0.0000*I,
  -8.8754 + 0.0000*I,
   8.8753 - 0.0000*I,
   8.8749 + 0.0000*I,
   8.8750 + 0.0000*I,
   8.8754 - 0.0000*I,
   8.8750 - 0.0000*I,
   8.8752 - 0.0000*I,
   8.8755 + 0.0000*I,
   0.0001 + 0.0000*I,
   8.8755 + 0.0000*I,
   8.8754 + 0.0000*I,
  -8.8751 + 0.0000*I,
  -8.8750 - 0.0000*I,
   8.8752 + 0.0000*I,
   8.8753 + 0.0000*I,
   8.8753 - 0.0000*I,
  -8.8753 + 0.0000*I,
   8.8752 + 0.0000*I,
  -8.8746 + 0.0000*I,
   8.8749 + 0.0000*I,
   8.8747 + 0.0000*I,
  -8.8752 - 0.0000*I,
   8.8750 + 0.0000*I,
   8.8752 + 0.0000*I,
  -8.8757 + 0.0000*I,
   8.8750 + 0.0000*I,
  -8.8753 + 0.0000*I,
  -8.8745 + 0.0000*I,
   8.8751 + 0.0000*I,
  -8.8754 - 0.0000*I,
   8.8750 - 0.0000*I,
   8.8759 - 0.0000*I,
  -8.8752 + 0.0000*I,
   8.8751 + 0.0000*I,
   8.8749 - 0.0000*I,
   8.8750 - 0.0000*I,
  -8.8759 - 0.0000*I,
  -8.8751 + 0.0000*I,
  -8.8760 - 0.0000*I,
  -8.8752 + 0.0000*I,
  -8.8756 + 0.0000*I,
   8.8754 + 0.0000*I,
   8.8751 - 0.0000*I,
  -8.8754 + 0.0000*I,
  -8.8755 + 0.0000*I,
   8.8753 + 0.0000*I,
  -8.8751 - 0.0000*I,
  -8.8753 + 0.0000*I,
  -8.8752 + 0.0000*I,
   8.8756 + 0.0000*I,
   8.8750 - 0.0000*I,
   8.8748 + 0.0000*I,
   8.8750 + 0.0000*I,
   8.8753 + 0.0000*I,
  -8.8746 + 0.0000*I,
  -8.8746 - 0.0000*I,
  -8.8749 + 0.0000*I,
   8.8754 - 0.0000*I,
   8.8757 - 0.0000*I,
   8.8748 + 0.0000*I,
  -8.8755 - 0.0000*I,
   0.0001 + 0.0000*I,
  -8.8754 + 0.0000*I,
   8.8747 + 0.0000*I,
  -8.8754 - 0.0000*I,
  -8.8750 - 0.0000*I,
   8.8750 - 0.0000*I,
   8.8751 - 0.0000*I,
  -8.8753 - 0.0000*I,
  -8.8751 + 0.0000*I,
   8.8756 + 0.0000*I,
   8.8754 + 0.0000*I,
  -8.8757 - 0.0000*I,
   8.8749 + 0.0000*I,
   8.8755 + 0.0000*I,
   8.8757 + 0.0000*I,
   8.8748 + 0.0000*I,
  -8.8751 + 0.0000*I,
  -8.8754 + 0.0000*I,
   8.8748 + 0.0000*I,
  -8.8754 + 0.0000*I,
   8.8751 + 0.0000*I,
   8.8755 + 0.0000*I,
  -8.8753 + 0.0000*I,
  -8.8758 - 0.0000*I,
   8.8753 + 0.0000*I,
  -8.8755 + 0.0000*I,
  -8.8753 - 0.0000*I
};

double complex rx_symb[SIZESYMBOL] = {
   0.0819 + 0.0044*I,
   0.0837 + 0.0027*I,
   0.0859 + 0.0017*I,
   0.0848 - 0.0028*I,
   0.0817 - 0.0068*I,
   0.0806 - 0.0107*I,
   0.0768 - 0.0164*I,
   0.0743 - 0.0182*I,
   0.0730 - 0.0212*I,
   0.0727 - 0.0223*I,
  -0.0709 + 0.0232*I,
   0.0758 - 0.0227*I,
   0.0768 - 0.0215*I,
   0.0775 - 0.0226*I,
   0.0785 - 0.0231*I,
  -0.0785 + 0.0231*I,
  -0.0786 + 0.0254*I,
  -0.0772 + 0.0285*I,
   0.0786 - 0.0340*I,
   0.0763 - 0.0367*I,
   0.0741 - 0.0408*I,
  -0.0683 + 0.0446*I,
   0.0708 - 0.0473*I,
   0.0702 - 0.0487*I,
  -0.0674 + 0.0521*I,
   0.0690 - 0.0513*I,
   0.0004 + 0.0011*I,
   0.0698 - 0.0520*I,
   0.0700 - 0.0549*I,
   0.0687 - 0.0565*I,
  -0.0679 + 0.0605*I,
  -0.0661 + 0.0639*I,
   0.0624 - 0.0665*I,
   0.0594 - 0.0704*I,
   0.0557 - 0.0749*I,
  -0.0521 + 0.0799*I,
  -0.0488 + 0.0822*I,
  -0.0454 + 0.0850*I,
  -0.0432 + 0.0851*I,
  -0.0417 + 0.0870*I,
  -0.0414 + 0.0870*I,
  -0.0391 + 0.0874*I,
   0.0407 - 0.0858*I,
  -0.0375 + 0.0876*I,
  -0.0336 + 0.0898*I,
  -0.0313 + 0.0926*I,
   0.0255 - 0.0925*I,
  -0.0190 + 0.1005*I,
   0.0118 - 0.1011*I,
   0.0038 - 0.1038*I,
   0.0002 + 0.1112*I,
   0.0044 + 0.1117*I,
  -0.0130 - 0.1065*I,
  -0.0806 - 0.0042*I,
  -0.0840 - 0.0031*I,
  -0.0844 - 0.0021*I,
  -0.0836 + 0.0024*I,
  -0.0813 + 0.0073*I,
   0.0762 - 0.0068*I,
  -0.0746 + 0.0158*I,
   0.0693 - 0.0160*I,
  -0.0721 + 0.0214*I,
  -0.0730 + 0.0224*I,
   0.0714 - 0.0211*I,
   0.0727 - 0.0222*I,
  -0.0755 + 0.0232*I,
  -0.0775 + 0.0223*I,
  -0.0790 + 0.0236*I,
  -0.0796 + 0.0251*I,
   0.0797 - 0.0250*I,
  -0.0787 + 0.0316*I,
  -0.0777 + 0.0347*I,
   0.0722 - 0.0358*I,
  -0.0754 + 0.0438*I,
   0.0686 - 0.0458*I,
  -0.0716 + 0.0478*I,
  -0.0711 + 0.0508*I,
  -0.0699 + 0.0515*I,
  -0.0709 + 0.0514*I,
  -0.0021 - 0.0018*I,
  -0.0708 + 0.0539*I,
  -0.0708 + 0.0545*I,
  -0.0700 + 0.0563*I,
   0.0658 - 0.0611*I,
   0.0647 - 0.0644*I,
   0.0617 - 0.0673*I,
   0.0587 - 0.0731*I,
   0.0552 - 0.0771*I,
  -0.0513 + 0.0789*I,
  -0.0480 + 0.0800*I,
  -0.0458 + 0.0839*I,
  -0.0437 + 0.0853*I,
   0.0422 - 0.0870*I,
  -0.0418 + 0.0860*I,
   0.0396 - 0.0878*I,
  -0.0404 + 0.0853*I,
   0.0371 - 0.0889*I,
  -0.0342 + 0.0875*I,
   0.0306 - 0.0915*I,
   0.0261 - 0.0956*I,
  -0.0193 + 0.0961*I,
   0.0112 - 0.1040*I,
   0.0032 - 0.1091*I,
   0.0013 + 0.1065*I,
   0.0067 + 0.1068*I,
  -0.0088 - 0.1117*I,
   0.0832 + 0.0095*I,
  -0.0840 - 0.0065*I,
   0.0850 + 0.0069*I,
  -0.0817 + 0.0020*I,
  -0.0798 + 0.0065*I,
   0.0760 - 0.0083*I,
   0.0733 - 0.0129*I,
  -0.0721 + 0.0185*I,
   0.0693 - 0.0196*I,
  -0.0725 + 0.0231*I,
   0.0721 - 0.0237*I,
  -0.0746 + 0.0221*I,
   0.0754 - 0.0218*I,
  -0.0787 + 0.0233*I,
   0.0780 - 0.0223*I,
  -0.0795 + 0.0245*I,
   0.0785 - 0.0270*I,
   0.0780 - 0.0288*I,
   0.0769 - 0.0326*I,
   0.0758 - 0.0373*I,
   0.0718 - 0.0413*I,
  -0.0715 + 0.0451*I,
   0.0703 - 0.0481*I,
   0.0672 - 0.0504*I,
  -0.0676 + 0.0520*I,
  -0.0690 + 0.0536*I,
   0.0011 + 0.0005*I,
   0.0686 - 0.0529*I,
  -0.0696 + 0.0558*I,
   0.0676 - 0.0559*I,
  -0.0692 + 0.0602*I,
   0.0637 - 0.0628*I,
  -0.0647 + 0.0663*I,
   0.0565 - 0.0694*I,
  -0.0570 + 0.0739*I,
  -0.0529 + 0.0782*I,
  -0.0490 + 0.0811*I,
  -0.0480 + 0.0831*I,
  -0.0452 + 0.0828*I,
  -0.0413 + 0.0836*I,
   0.0422 - 0.0889*I,
   0.0419 - 0.0876*I,
   0.0413 - 0.0881*I,
  -0.0368 + 0.0850*I,
  -0.0332 + 0.0876*I,
  -0.0291 + 0.0910*I,
   0.0265 - 0.0938*I,
  -0.0190 + 0.0980*I,
   0.0126 - 0.1029*I,
  -0.0048 + 0.1071*I,
  -0.0017 - 0.1086*I,
  -0.0074 - 0.1099*I,
   0.0086 + 0.1087*I,
  -0.0807 - 0.0074*I,
  -0.0810 - 0.0068*I,
   0.0865 + 0.0069*I,
  -0.0808 + 0.0021*I,
  -0.0796 + 0.0066*I,
   0.0766 - 0.0069*I,
   0.0736 - 0.0122*I,
   0.0713 - 0.0161*I,
  -0.0721 + 0.0227*I,
  -0.0717 + 0.0235*I,
   0.0701 - 0.0215*I,
  -0.0746 + 0.0230*I,
   0.0740 - 0.0227*I,
   0.0763 - 0.0225*I,
   0.0785 - 0.0240*I,
   0.0791 - 0.0243*I,
   0.0793 - 0.0273*I,
   0.0774 - 0.0300*I,
   0.0788 - 0.0349*I,
   0.0755 - 0.0394*I,
   0.0737 - 0.0415*I,
  -0.0697 + 0.0450*I,
   0.0699 - 0.0481*I,
  -0.0679 + 0.0497*I,
  -0.0667 + 0.0521*I,
   0.0685 - 0.0525*I,
  -0.0003 + 0.0007*I,
   0.0695 - 0.0538*I,
   0.0686 - 0.0546*I,
   0.0678 - 0.0563*I,
   0.0666 - 0.0600*I,
  -0.0648 + 0.0639*I,
   0.0634 - 0.0672*I,
   0.0587 - 0.0707*I,
   0.0553 - 0.0751*I,
   0.0518 - 0.0804*I,
  -0.0470 + 0.0823*I,
   0.0463 - 0.0851*I,
   0.0452 - 0.0859*I,
  -0.0412 + 0.0879*I,
  -0.0393 + 0.0870*I,
  -0.0399 + 0.0880*I,
   0.0403 - 0.0845*I,
   0.0374 - 0.0848*I,
  -0.0348 + 0.0901*I,
  -0.0320 + 0.0921*I,
   0.0247 - 0.0923*I,
  -0.0209 + 0.1003*I,
  -0.0144 + 0.1036*I,
  -0.0069 + 0.1066*I,
  -0.0030 + 0.1091*I,
   0.0043 + 0.1099*I,
  -0.0124 - 0.1108*I,
  -0.0841 - 0.0062*I,
   0.0811 + 0.0085*I,
   0.0818 + 0.0062*I,
  -0.0849 + 0.0008*I,
  -0.0828 + 0.0050*I,
  -0.0789 + 0.0102*I,
   0.0729 - 0.0158*I,
  -0.0744 + 0.0182*I,
   0.0693 - 0.0213*I,
  -0.0728 + 0.0207*I,
   0.0704 - 0.0230*I,
  -0.0758 + 0.0218*I,
  -0.0776 + 0.0215*I,
  -0.0778 + 0.0217*I,
  -0.0803 + 0.0215*I,
   0.0790 - 0.0265*I,
   0.0784 - 0.0280*I,
   0.0783 - 0.0305*I,
   0.0754 - 0.0343*I,
  -0.0763 + 0.0381*I,
   0.0717 - 0.0425*I,
   0.0709 - 0.0460*I,
  -0.0700 + 0.0477*I,
  -0.0694 + 0.0487*I,
  -0.0680 + 0.0511*I,
  -0.0694 + 0.0509*I,
  -0.0013 - 0.0019*I,
  -0.0685 + 0.0525*I,
   0.0699 - 0.0568*I,
   0.0689 - 0.0593*I,
   0.0676 - 0.0620*I,
   0.0662 - 0.0659*I,
  -0.0610 + 0.0661*I,
  -0.0586 + 0.0706*I,
   0.0568 - 0.0762*I,
  -0.0501 + 0.0786*I,
   0.0491 - 0.0840*I,
  -0.0434 + 0.0837*I,
   0.0454 - 0.0862*I,
  -0.0414 + 0.0861*I,
  -0.0397 + 0.0849*I,
  -0.0395 + 0.0862*I,
   0.0404 - 0.0880*I,
  -0.0353 + 0.0851*I,
  -0.0327 + 0.0872*I,
   0.0329 - 0.0914*I,
   0.0283 - 0.0957*I,
   0.0225 - 0.0991*I,
  -0.0065 + 0.1033*I,
   0.0080 - 0.1057*I,
   0.0020 - 0.1077*I,
  -0.0024 - 0.1085*I,
  -0.0069 - 0.1069*I,
   0.0802 + 0.0092*I,
  -0.0880 - 0.0062*I,
   0.0809 + 0.0041*I,
   0.0805 - 0.0016*I,
   0.0784 - 0.0060*I,
  -0.0774 + 0.0083*I,
   0.0734 - 0.0155*I,
  -0.0727 + 0.0178*I,
   0.0700 - 0.0214*I,
   0.0710 - 0.0228*I,
   0.0719 - 0.0230*I,
   0.0730 - 0.0218*I,
  -0.0762 + 0.0217*I,
   0.0759 - 0.0220*I,
  -0.0798 + 0.0229*I,
  -0.0815 + 0.0252*I,
  -0.0814 + 0.0274*I,
  -0.0791 + 0.0311*I,
  -0.0782 + 0.0336*I,
  -0.0763 + 0.0373*I,
  -0.0748 + 0.0411*I,
  -0.0738 + 0.0445*I,
   0.0668 - 0.0492*I,
  -0.0711 + 0.0487*I,
   0.0669 - 0.0538*I,
  -0.0691 + 0.0505*I,
  -0.0013 - 0.0010*I,
   0.0696 - 0.0566*I,
   0.0689 - 0.0592*I,
   0.0698 - 0.0618*I,
  -0.0668 + 0.0566*I,
  -0.0635 + 0.0594*I,
   0.0631 - 0.0692*I,
  -0.0575 + 0.0697*I,
  -0.0543 + 0.0743*I,
   0.0528 - 0.0803*I,
   0.0495 - 0.0824*I,
   0.0464 - 0.0849*I,
  -0.0444 + 0.0850*I,
  -0.0417 + 0.0864*I,
   0.0411 - 0.0889*I,
  -0.0409 + 0.0840*I,
  -0.0385 + 0.0830*I,
   0.0381 - 0.0902*I,
   0.0368 - 0.0911*I,
  -0.0290 + 0.0879*I,
   0.0288 - 0.0967*I,
   0.0238 - 0.1001*I,
   0.0162 - 0.1033*I,
   0.0100 - 0.1072*I,
   0.0044 - 0.1084*I,
  -0.0000 - 0.1079*I,
  -0.0046 - 0.1045*I,
   0.0813 + 0.0095*I,
   0.0841 + 0.0079*I,
   0.0830 + 0.0049*I,
   0.0818 + 0.0009*I,
  -0.0808 + 0.0050*I,
  -0.0773 + 0.0102*I,
   0.0728 - 0.0151*I,
   0.0714 - 0.0177*I,
   0.0709 - 0.0212*I,
   0.0719 - 0.0227*I,
  -0.0707 + 0.0232*I,
  -0.0739 + 0.0238*I,
   0.0753 - 0.0204*I,
   0.0775 - 0.0211*I,
  -0.0791 + 0.0225*I,
  -0.0800 + 0.0258*I,
  -0.0812 + 0.0275*I,
  -0.0783 + 0.0301*I,
  -0.0777 + 0.0340*I,
  -0.0753 + 0.0378*I,
  -0.0735 + 0.0423*I,
  -0.0720 + 0.0450*I,
   0.0692 - 0.0486*I,
   0.0674 - 0.0506*I,
   0.0679 - 0.0505*I,
  -0.0696 + 0.0525*I,
  -0.0000 - 0.0005*I,
   0.0686 - 0.0552*I,
  -0.0694 + 0.0558*I,
  -0.0678 + 0.0567*I,
  -0.0680 + 0.0591*I,
   0.0651 - 0.0635*I,
  -0.0627 + 0.0666*I,
  -0.0604 + 0.0703*I,
  -0.0554 + 0.0755*I,
  -0.0516 + 0.0793*I,
   0.0472 - 0.0820*I,
  -0.0470 + 0.0835*I,
  -0.0445 + 0.0869*I,
   0.0410 - 0.0867*I,
   0.0406 - 0.0860*I,
  -0.0412 + 0.0864*I,
  -0.0387 + 0.0850*I,
   0.0366 - 0.0885*I,
   0.0335 - 0.0896*I,
   0.0295 - 0.0936*I,
   0.0262 - 0.0959*I,
   0.0200 - 0.1002*I,
  -0.0112 + 0.1014*I,
   0.0045 - 0.1098*I,
   0.0041 + 0.1080*I,
   0.0092 + 0.1090*I,
  -0.0076 - 0.1106*I,
  -0.0792 - 0.0088*I,
  -0.0807 - 0.0078*I,
   0.0884 + 0.0047*I,
  -0.0792 - 0.0001*I,
   0.0835 - 0.0039*I,
   0.0786 - 0.0083*I,
  -0.0726 + 0.0169*I,
   0.0734 - 0.0163*I,
  -0.0696 + 0.0236*I,
  -0.0713 + 0.0244*I,
   0.0721 - 0.0211*I,
   0.0729 - 0.0211*I,
   0.0757 - 0.0206*I,
   0.0769 - 0.0220*I,
   0.0789 - 0.0234*I,
  -0.0790 + 0.0239*I,
   0.0799 - 0.0269*I,
  -0.0780 + 0.0293*I,
  -0.0767 + 0.0349*I,
   0.0748 - 0.0360*I,
  -0.0718 + 0.0438*I,
   0.0708 - 0.0434*I,
  -0.0688 + 0.0492*I,
  -0.0676 + 0.0510*I,
   0.0673 - 0.0518*I,
   0.0686 - 0.0522*I,
  -0.0010 - 0.0002*I,
   0.0680 - 0.0547*I,
  -0.0698 + 0.0563*I,
   0.0673 - 0.0573*I,
   0.0676 - 0.0590*I,
  -0.0651 + 0.0633*I,
   0.0629 - 0.0667*I,
   0.0597 - 0.0709*I,
  -0.0546 + 0.0765*I,
   0.0506 - 0.0783*I,
   0.0473 - 0.0815*I,
   0.0444 - 0.0839*I,
  -0.0437 + 0.0869*I,
  -0.0429 + 0.0874*I,
   0.0404 - 0.0863*I,
   0.0400 - 0.0874*I,
  -0.0401 + 0.0873*I,
  -0.0381 + 0.0873*I,
  -0.0350 + 0.0897*I,
  -0.0307 + 0.0925*I,
   0.0229 - 0.0937*I,
  -0.0211 + 0.0985*I,
   0.0098 - 0.1019*I,
   0.0025 - 0.1065*I,
  -0.0007 + 0.1091*I,
   0.0061 + 0.1093*I,
  -0.0133 - 0.1105*I,
  -0.0811 - 0.0061*I,
  -0.0826 - 0.0044*I,
  -0.0832 - 0.0032*I,
   0.0828 + 0.0017*I,
  -0.0789 + 0.0054*I,
  -0.0760 + 0.0127*I,
   0.0740 - 0.0126*I,
   0.0708 - 0.0156*I,
   0.0709 - 0.0186*I,
  -0.0720 + 0.0239*I,
   0.0703 - 0.0206*I,
   0.0721 - 0.0219*I,
   0.0733 - 0.0219*I,
  -0.0792 + 0.0233*I,
   0.0769 - 0.0237*I,
   0.0780 - 0.0258*I,
  -0.0803 + 0.0265*I,
   0.0765 - 0.0308*I,
   0.0762 - 0.0351*I,
  -0.0747 + 0.0368*I,
   0.0720 - 0.0418*I,
  -0.0718 + 0.0446*I,
   0.0689 - 0.0482*I,
   0.0677 - 0.0504*I,
   0.0677 - 0.0512*I,
   0.0688 - 0.0526*I,
   0.0000 + 0.0001*I,
  -0.0696 + 0.0545*I,
  -0.0700 + 0.0558*I,
  -0.0688 + 0.0571*I,
  -0.0665 + 0.0602*I,
   0.0646 - 0.0652*I,
   0.0632 - 0.0692*I,
  -0.0591 + 0.0716*I,
   0.0556 - 0.0764*I,
   0.0518 - 0.0801*I,
   0.0491 - 0.0821*I,
  -0.0456 + 0.0856*I,
   0.0430 - 0.0861*I,
   0.0414 - 0.0861*I,
  -0.0414 + 0.0883*I,
   0.0386 - 0.0858*I,
  -0.0411 + 0.0874*I,
   0.0350 - 0.0878*I,
   0.0312 - 0.0893*I,
  -0.0322 + 0.0915*I,
   0.0228 - 0.0965*I,
   0.0173 - 0.0997*I,
  -0.0131 + 0.0996*I,
   0.0039 - 0.1100*I,
   0.0022 + 0.1071*I,
  -0.0057 - 0.1134*I,
   0.0125 + 0.1056*I,
   0.0845 + 0.0046*I,
   0.0864 + 0.0041*I,
  -0.0821 - 0.0065*I,
   0.0855 - 0.0025*I,
  -0.0766 + 0.0029*I,
  -0.0738 + 0.0098*I,
   0.0769 - 0.0154*I,
   0.0745 - 0.0186*I,
   0.0740 - 0.0210*I,
   0.0742 - 0.0229*I,
  -0.0697 + 0.0242*I,
   0.0748 - 0.0212*I,
  -0.0747 + 0.0231*I,
   0.0790 - 0.0214*I,
   0.0787 - 0.0234*I,
  -0.0792 + 0.0251*I,
   0.0789 - 0.0257*I,
  -0.0783 + 0.0294*I,
   0.0760 - 0.0344*I,
  -0.0732 + 0.0378*I,
   0.0731 - 0.0409*I,
   0.0714 - 0.0446*I,
  -0.0685 + 0.0490*I,
   0.0691 - 0.0495*I,
  -0.0667 + 0.0538*I,
   0.0697 - 0.0521*I,
  -0.0005 + 0.0014*I,
  -0.0689 + 0.0567*I,
   0.0700 - 0.0537*I,
  -0.0678 + 0.0608*I,
  -0.0672 + 0.0624*I,
  -0.0661 + 0.0658*I,
  -0.0639 + 0.0689*I,
  -0.0604 + 0.0738*I,
  -0.0553 + 0.0769*I,
  -0.0529 + 0.0799*I,
   0.0463 - 0.0829*I,
  -0.0469 + 0.0859*I,
  -0.0443 + 0.0859*I,
  -0.0426 + 0.0857*I,
   0.0404 - 0.0861*I,
   0.0396 - 0.0871*I,
  -0.0389 + 0.0878*I,
   0.0373 - 0.0851*I,
   0.0341 - 0.0875*I,
   0.0299 - 0.0901*I,
  -0.0255 + 0.0955*I,
   0.0182 - 0.0970*I,
  -0.0126 + 0.1044*I,
   0.0016 - 0.1053*I,
  -0.0046 - 0.1088*I,
   0.0046 + 0.1109*I,
   0.0078 + 0.1093*I,
  -0.0823 - 0.0028*I,
  -0.0847 - 0.0033*I,
  -0.0848 - 0.0009*I,
   0.0815 + 0.0036*I,
   0.0781 - 0.0033*I,
  -0.0799 + 0.0120*I,
  -0.0766 + 0.0172*I,
  -0.0743 + 0.0198*I,
  -0.0739 + 0.0228*I,
  -0.0739 + 0.0227*I,
   0.0702 - 0.0230*I,
   0.0712 - 0.0220*I,
  -0.0776 + 0.0226*I,
   0.0769 - 0.0220*I,
   0.0775 - 0.0240*I,
  -0.0808 + 0.0247*I,
   0.0783 - 0.0274*I,
   0.0762 - 0.0293*I,
   0.0749 - 0.0339*I,
  -0.0757 + 0.0380*I,
   0.0716 - 0.0420*I,
  -0.0717 + 0.0461*I,
  -0.0711 + 0.0471*I,
  -0.0695 + 0.0508*I,
  -0.0693 + 0.0522*I,
   0.0660 - 0.0538*I,
  -0.0016 - 0.0014*I,
   0.0679 - 0.0561*I,
   0.0681 - 0.0566*I,
   0.0674 - 0.0577*I,
  -0.0677 + 0.0591*I,
  -0.0653 + 0.0613*I,
  -0.0634 + 0.0664*I,
  -0.0589 + 0.0708*I,
   0.0530 - 0.0773*I,
  -0.0520 + 0.0773*I,
  -0.0490 + 0.0812*I,
   0.0446 - 0.0858*I,
   0.0426 - 0.0880*I,
  -0.0411 + 0.0842*I,
  -0.0397 + 0.0847*I,
   0.0403 - 0.0890*I,
  -0.0376 + 0.0836*I,
   0.0374 - 0.0889*I,
  -0.0337 + 0.0874*I,
  -0.0284 + 0.0888*I,
  -0.0227 + 0.0936*I,
   0.0189 - 0.0994*I,
  -0.0101 + 0.1011*I,
   0.0051 - 0.1104*I,
  -0.0003 - 0.1123*I,
  -0.0049 - 0.1115*I,
  -0.0085 - 0.1107*I,
  -0.0814 - 0.0048*I,
   0.0840 + 0.0073*I,
  -0.0844 - 0.0042*I,
   0.0819 - 0.0001*I,
  -0.0810 + 0.0054*I,
  -0.0776 + 0.0116*I,
  -0.0738 + 0.0142*I,
  -0.0710 + 0.0197*I,
  -0.0717 + 0.0214*I,
   0.0710 - 0.0223*I,
   0.0712 - 0.0226*I,
   0.0743 - 0.0221*I,
   0.0765 - 0.0222*I,
  -0.0773 + 0.0240*I,
   0.0797 - 0.0225*I,
   0.0787 - 0.0249*I,
  -0.0798 + 0.0267*I,
  -0.0777 + 0.0309*I,
   0.0762 - 0.0334*I,
  -0.0756 + 0.0373*I,
   0.0724 - 0.0411*I,
   0.0690 - 0.0459*I,
   0.0697 - 0.0481*I,
   0.0672 - 0.0486*I,
  -0.0690 + 0.0516*I,
   0.0665 - 0.0526*I,
  -0.0015 + 0.0003*I,
   0.0681 - 0.0546*I,
   0.0676 - 0.0558*I,
   0.0679 - 0.0572*I,
  -0.0681 + 0.0606*I,
  -0.0660 + 0.0639*I,
  -0.0632 + 0.0661*I,
  -0.0596 + 0.0708*I,
   0.0532 - 0.0768*I,
   0.0503 - 0.0810*I,
   0.0484 - 0.0843*I,
   0.0443 - 0.0867*I,
  -0.0428 + 0.0858*I,
  -0.0418 + 0.0857*I,
  -0.0395 + 0.0854*I,
   0.0405 - 0.0870*I,
  -0.0366 + 0.0870*I,
  -0.0362 + 0.0875*I,
  -0.0345 + 0.0889*I,
   0.0288 - 0.0900*I,
  -0.0249 + 0.0932*I,
   0.0170 - 0.0984*I,
   0.0098 - 0.1035*I,
   0.0035 - 0.1081*I,
   0.0022 + 0.1088*I,
  -0.0076 - 0.1114*I,
   0.0120 + 0.1061*I,
   0.0788 + 0.0086*I,
  -0.0865 - 0.0063*I,
  -0.0865 - 0.0024*I,
   0.0794 - 0.0006*I,
   0.0764 - 0.0052*I,
   0.0744 - 0.0110*I,
   0.0715 - 0.0164*I,
  -0.0745 + 0.0187*I,
  -0.0719 + 0.0208*I,
   0.0691 - 0.0241*I,
   0.0697 - 0.0238*I,
  -0.0752 + 0.0206*I,
  -0.0767 + 0.0223*I,
   0.0760 - 0.0248*I,
   0.0777 - 0.0224*I,
  -0.0803 + 0.0225*I,
   0.0782 - 0.0285*I,
   0.0776 - 0.0317*I,
  -0.0760 + 0.0324*I,
   0.0750 - 0.0387*I,
  -0.0735 + 0.0409*I,
  -0.0703 + 0.0438*I,
   0.0692 - 0.0490*I,
  -0.0674 + 0.0501*I,
  -0.0685 + 0.0506*I,
   0.0674 - 0.0540*I,
  -0.0003 - 0.0010*I,
  -0.0691 + 0.0534*I,
  -0.0682 + 0.0560*I,
   0.0699 - 0.0599*I,
  -0.0654 + 0.0575*I,
   0.0656 - 0.0654*I,
   0.0629 - 0.0702*I,
   0.0592 - 0.0735*I,
   0.0563 - 0.0780*I,
  -0.0498 + 0.0776*I,
  -0.0458 + 0.0823*I,
   0.0479 - 0.0851*I,
   0.0450 - 0.0872*I,
  -0.0393 + 0.0876*I,
  -0.0391 + 0.0865*I,
   0.0415 - 0.0863*I,
  -0.0359 + 0.0874*I,
  -0.0349 + 0.0859*I,
   0.0364 - 0.0884*I,
   0.0324 - 0.0908*I,
   0.0263 - 0.0935*I,
  -0.0165 + 0.0990*I,
  -0.0088 + 0.1027*I,
  -0.0017 + 0.1080*I,
  -0.0010 - 0.1076*I,
   0.0104 + 0.1118*I,
  -0.0086 - 0.1069*I,
  -0.0834 - 0.0089*I,
   0.0831 + 0.0052*I,
   0.0833 + 0.0027*I,
  -0.0838 - 0.0001*I,
   0.0788 - 0.0061*I,
   0.0756 - 0.0130*I,
   0.0734 - 0.0168*I,
  -0.0719 + 0.0171*I,
  -0.0719 + 0.0196*I,
  -0.0715 + 0.0216*I,
   0.0711 - 0.0253*I,
  -0.0731 + 0.0210*I,
   0.0763 - 0.0266*I,
  -0.0760 + 0.0211*I,
  -0.0786 + 0.0217*I,
   0.0806 - 0.0268*I,
   0.0806 - 0.0295*I,
   0.0801 - 0.0309*I,
  -0.0746 + 0.0331*I,
   0.0767 - 0.0391*I,
   0.0735 - 0.0408*I,
   0.0717 - 0.0443*I,
   0.0687 - 0.0484*I,
   0.0682 - 0.0503*I,
   0.0670 - 0.0520*I,
   0.0682 - 0.0532*I,
   0.0008 + 0.0006*I,
   0.0689 - 0.0563*I,
   0.0696 - 0.0567*I,
  -0.0679 + 0.0575*I,
  -0.0656 + 0.0600*I,
   0.0651 - 0.0642*I,
   0.0647 - 0.0680*I,
   0.0598 - 0.0726*I,
  -0.0527 + 0.0761*I,
   0.0526 - 0.0793*I,
  -0.0469 + 0.0829*I,
   0.0474 - 0.0834*I,
   0.0435 - 0.0842*I,
  -0.0410 + 0.0879*I,
   0.0422 - 0.0847*I,
   0.0394 - 0.0849*I,
  -0.0383 + 0.0879*I,
   0.0373 - 0.0862*I,
  -0.0333 + 0.0883*I,
  -0.0313 + 0.0915*I,
   0.0238 - 0.0933*I,
  -0.0186 + 0.0984*I,
   0.0107 - 0.1023*I,
   0.0040 - 0.1062*I,
   0.0044 + 0.1101*I,
  -0.0080 - 0.1094*I,
  -0.0116 - 0.1078*I,
   0.0846 + 0.0089*I,
  -0.0834 - 0.0046*I,
  -0.0833 - 0.0022*I,
  -0.0819 + 0.0021*I,
  -0.0788 + 0.0074*I,
  -0.0766 + 0.0122*I,
   0.0733 - 0.0140*I,
   0.0710 - 0.0183*I,
  -0.0713 + 0.0228*I,
  -0.0713 + 0.0228*I,
   0.0713 - 0.0225*I,
  -0.0745 + 0.0226*I,
  -0.0765 + 0.0224*I,
  -0.0787 + 0.0233*I,
   0.0786 - 0.0231*I,
   0.0785 - 0.0245*I,
   0.0783 - 0.0267*I,
   0.0782 - 0.0317*I,
   0.0758 - 0.0342*I,
  -0.0749 + 0.0379*I,
  -0.0723 + 0.0422*I,
  -0.0707 + 0.0460*I,
   0.0685 - 0.0482*I,
   0.0679 - 0.0512*I,
   0.0663 - 0.0521*I,
  -0.0693 + 0.0541*I,
  -0.0002 - 0.0004*I,
  -0.0698 + 0.0549*I,
   0.0673 - 0.0562*I,
  -0.0687 + 0.0586*I,
  -0.0665 + 0.0606*I,
   0.0642 - 0.0636*I,
   0.0601 - 0.0675*I,
  -0.0586 + 0.0717*I,
  -0.0555 + 0.0760*I,
   0.0502 - 0.0799*I,
   0.0480 - 0.0827*I,
  -0.0457 + 0.0846*I,
   0.0424 - 0.0856*I,
   0.0420 - 0.0872*I,
   0.0389 - 0.0868*I,
   0.0391 - 0.0870*I,
  -0.0401 + 0.0867*I,
  -0.0376 + 0.0868*I,
   0.0322 - 0.0895*I,
  -0.0318 + 0.0910*I,
   0.0232 - 0.0958*I,
   0.0162 - 0.1004*I,
  -0.0120 + 0.1007*I,
  -0.0060 + 0.1047*I,
  -0.0038 - 0.1108*I,
   0.0078 + 0.1077*I,
   0.0121 + 0.1057*I
};
