#include "cfortran.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int photoion
(float *ear,int ne,float *param,int ifl,float *photar,float *photer);

FCALLSCSUB6(photoion,PHOTOION,photoion,FLOATV,INT,FLOATV,INT,FLOATV,FLOATV)

#define sqr(X) ((X)*(X))
#define cube(X) ((X)*(X)*(X))
#define SMALL (1.e-6)
#define FINE_STRUCTURE (1./137.0359895)
#define eVtoHartree (1./(2.*13.6056981))
#define a0 (5.29177249e-9)          /* Bohr radius [cm] */
#define ccc (2.99792458e10)         /* speed of light [cm/s] */
#define eee (4.8032068e-10)         /* electron charge [esu] */
#define hhh (6.6260755e-27)         /* Planck's constant [cgs] */
#define H_0 (2.301e-18) /* Hubble constant [1/s]: (WMAP h = 71) 71*1e5/3.085678e18/1e6*/
#define me (9.1093897e-28)          /* electron mass [g] */
#define re (2.81794092e-13)         /* classical electron radius: e^2/m c^2 */
#define ge (2.)                     /* gyromagnetic ratio for the electron */
#define meeV (5.1099906e5)          /* electron mass [eV] */
#define sigmaT (6.6525e-25)    /* Thomson cross-section [cm^2]: 8*Pi/3*re^2 */
#define eVtoergs (1.60217733e-12)   /* convert eV to ergs */
#define ergstoeV (1./eVtoergs)      /* convert ergs to eV */
#define AMconv (4.13413733134e16)   /* convert Einstein A_ij A.U.'s -> s^-1 */
#define AngstromtokeV (12.39841856)    /* lambda=12.39841856/E_keV */
#define FACTOR (66.784)     /* For calculating line center optical depth */
#define parsectocm (3.085678e18)  /* parsecs to cm */
#define TEMPERATURES (11)
#define SMALL (1.e-6)
#define EPS 1.0e-5
#define JMAX 22
#define JMAXP JMAX+1
#define K 5
#define FUNC_pi(x) ((*func)(x))
#define SWAP_pi(a,b) {double temp=(a);(a)=(b);(b)=temp;}
#define PI (3.141592653589793)

/* START */

void nrerror_pi(error_text)
char error_text[];
{
   void exit();

   /*fprintf(stderr,"Numerical Recipes run-time error...\n");
   fprintf(stderr,"%s\n",error_text);
   fprintf(stderr,"...now exiting to system...\n");
   exit(1);*/
}

double *vector_pi(nl,nh)
int nl,nh;
{
   double *v;
   int i,sz;

   sz = nh-nl+1;
   v=(double *)malloc((unsigned) sz*sizeof(double));
   if (!v) nrerror_pi("allocation failure in vector()");
   for (i=0; i<sz; ++i) v[i] = .0; 
   return v-nl;
}

int *ivector_pi(nl,nh)
int nl,nh;
{
   int *v, i, sz;

   sz = nh-nl+1;
   v=(int *)malloc((unsigned) sz*sizeof(int));
   if (!v) nrerror_pi("allocation failure in ivector()");
   for (i=0; i<sz; ++i) v[i] = 0; 
   return v-nl;
}

double *dvector_pi(nl,nh)
int nl,nh;
{
   double *v;
   int i,sz;

   sz = nh-nl+1;
   v=(double *)malloc((unsigned) sz*sizeof(double));
   if (!v) nrerror_pi("allocation failure in dvector()");
   for (i=0; i<sz; ++i) v[i] = .0; 
   return v-nl;
}

double **matrix_pi(nrl,nrh,ncl,nch)
int nrl,nrh,ncl,nch;
{
   int i, j, sz;
   double **m, *m_i;

   m=(double **) malloc((unsigned) (nrh-nrl+1)*sizeof(double*));
   if (!m) nrerror_pi("allocation failure 1 in matrix()");
   m -= nrl;

   sz = nch-ncl+1;
   for(i=nrl;i<=nrh;i++) {
      m[i]=(double *) malloc((unsigned) sz*sizeof(double));
      if (!m[i]) nrerror_pi("allocation failure 2 in matrix()");
      m_i = m[i];
      for (j=0; j<sz; ++j) m_i[j] = .0;
      m[i] -= ncl;
   }
   return m;
}

double **dmatrix_pi(nrl,nrh,ncl,nch)
int nrl,nrh,ncl,nch;
{
   int i ,j, sz;
   double **m, *m_i;

   m=(double **) malloc((unsigned) (nrh-nrl+1)*sizeof(double*));
   if (!m) nrerror_pi("allocation failure 1 in dmatrix()");
   m -= nrl;

   sz = nch-ncl+1;
   for(i=nrl;i<=nrh;i++) {
      m[i]=(double *) malloc((unsigned) sz*sizeof(double));
      if (!m[i]) nrerror_pi("allocation failure 2 in dmatrix()");
      m_i = m[i];
      for (j=0; j<sz; ++j) m_i[j] = .0;
      m[i] -= ncl;
   }
   return m;
}

int **imatrix_pi(nrl,nrh,ncl,nch)
int nrl,nrh,ncl,nch;
{
   int i,**m, *m_i, j, sz;

   m=(int **)malloc((unsigned) (nrh-nrl+1)*sizeof(int*));
   if (!m) nrerror_pi("allocation failure 1 in imatrix()");
   m -= nrl;

   sz = nch-ncl+1;
   for(i=nrl;i<=nrh;i++) {
      m[i]=(int *)malloc((unsigned) sz*sizeof(int));
      if (!m[i]) nrerror_pi("allocation failure 2 in imatrix()");
      m_i = m[i];
      for (j=0; j<sz; ++j) m_i[j] = 0;
      m[i] -= ncl;
   }
   return m;
}

double **submatrix_pi(a,oldrl,oldrh,oldcl,oldch,newrl,newcl)
double **a;
int oldrl,oldrh,oldcl,oldch,newrl,newcl;
{
   int i,j;
   double **m;

   m=(double **) malloc((unsigned) (oldrh-oldrl+1)*sizeof(double*));
   if (!m) nrerror_pi("allocation failure in submatrix()");
   m -= newrl;

   for(i=oldrl,j=newrl;i<=oldrh;i++,j++) m[j]=a[i]+oldcl-newcl;

   return m;
}

void free_vector_pi(v,nl,nh)
double *v;
int nl,nh;
{
   free((char*) (v+nl));
}

void free_ivector_pi(v,nl,nh)
int *v,nl,nh;
{
   free((char*) (v+nl));
}

void free_dvector_pi(v,nl,nh)
double *v;
int nl,nh;
{
   free((char*) (v+nl));
}

void free_matrix_pi(m,nrl,nrh,ncl,nch)
double **m;
int nrl,nrh,ncl,nch;
{
   int i;

   for(i=nrh;i>=nrl;i--) free((char*) (m[i]+ncl));
   free((char*) (m+nrl));
}

void free_dmatrix_pi(m,nrl,nrh,ncl,nch)
double **m;
int nrl,nrh,ncl,nch;
{
   int i;

   for(i=nrh;i>=nrl;i--) free((char*) (m[i]+ncl));
   free((char*) (m+nrl));
}

void free_imatrix_pi(m,nrl,nrh,ncl,nch)
int **m;
int nrl,nrh,ncl,nch;
{
   int i;

   for(i=nrh;i>=nrl;i--) free((char*) (m[i]+ncl));
   free((char*) (m+nrl));
}

void free_submatrix_pi(b,nrl,nrh,ncl,nch)
double **b;
int nrl,nrh,ncl,nch;
{
   free((char*) (b+nrl));
}

double **convert_matrix_pi(a,nrl,nrh,ncl,nch)
double *a;
int nrl,nrh,ncl,nch;
{
   int i,j,nrow,ncol;
   double **m;

   nrow=nrh-nrl+1;
   ncol=nch-ncl+1;
   m = (double **) malloc((unsigned) (nrow)*sizeof(double*));
   if (!m) nrerror_pi("allocation failure in convert_matrix()");
   m -= nrl;
   for(i=0,j=nrl;i<=nrow-1;i++,j++) m[j]=a+ncol*i-ncl;
   return m;
}

void free_convert_matrix_pi(b,nrl,nrh,ncl,nch)
double **b;
int nrl,nrh,ncl,nch;
{
   free((char*) (b+nrl));
}

double qromb_pi(func,a,b)
double a,b;
double (*func)();
{
   double ss,dss,trapzd_pi();
   double s[JMAXP+1],h[JMAXP+1];
   int j;
   void polint_pi(),nrerror_pi();

   h[1]=1.0;
   for (j=1;j<=JMAX;j++) {
      s[j]=trapzd_pi(func,a,b,j);
      if (j >= K) {
         polint_pi(&h[j-K],&s[j-K],K,0.0,&ss,&dss);
         if (fabs(dss) < EPS*fabs(ss)) return ss;
      }
      /*printf("%2d     %e\n",j,ss);*/
      s[j+1]=s[j];
      h[j+1]=0.25*h[j];
   }
   nrerror_pi("Too many steps in routine QROMB");
   /*     printf("not accurate\n");*/
   return ss;
}

double trapzd_pi(func,a,b,n)
double a,b;
double (*func)();                           /* ANSI: double (*func)(double); */
int n;
{
   double x,tnm,sum,del;
   static double s;
   static int it;
   int j;

   if (n == 1) {
      it=1;
      return (s=0.5*(b-a)*(FUNC_pi(a)+FUNC_pi(b)));
   } else {
      tnm=it;
      del=(b-a)/tnm;
      x=a+0.5*del;
      for (sum=0.0,j=1;j<=it;j++,x+=del) sum += FUNC_pi(x);
      it *= 2;
      s=0.5*(s+(b-a)*sum/tnm);
      return s;
   }
}

void polint_pi(xa,ya,n,x,y,dy)
double xa[],ya[],x,*y,*dy;
int n;
{
   int i,m,ns=1;
   double den,dif,dift,ho,hp,w;
   double *c,*d,*vector_pi();
   void nrerror_pi(),free_vector_pi();

   dif=fabs(x-xa[1]);
   c=vector_pi(1,n);
   d=vector_pi(1,n);
   for (i=1;i<=n;i++) {
      if ( (dift=fabs(x-xa[i])) < dif) {
         ns=i;
         dif=dift;
      }
      c[i]=ya[i];
      d[i]=ya[i];
   }
   *y=ya[ns--];
   for (m=1;m<n;m++) {
      for (i=1;i<=n-m;i++) {
         ho=xa[i]-x;
         hp=xa[i+m]-x;
         w=c[i+1]-d[i];
         if ( (den=ho-hp) == 0.0) nrerror_pi("Error in routine POLINT");
         den=w/den;
         d[i]=hp*den;
         c[i]=ho*den;
      }
      *y += (*dy=(2*ns < (n-m) ? c[ns+1] : d[ns--]));
   }
   free_vector_pi(d,1,n);
   free_vector_pi(c,1,n);
}

void spline_pi(x,y,n,yp1,ypn,y2)
double x[],y[],yp1,ypn,y2[];
int n;
{
   int i,k;
   double p,qn,sig,un,*u,*vector_pi();
   void free_vector_pi();

   u=vector_pi(1,n-1);
   if (yp1 > 0.99e30)
      y2[1]=u[1]=0.0;
   else {
      y2[1] = -0.5;
      u[1]=(3.0/(x[2]-x[1]))*((y[2]-y[1])/(x[2]-x[1])-yp1);
   }
   for (i=2;i<=n-1;i++) {
      sig=(x[i]-x[i-1])/(x[i+1]-x[i-1]);
      p=sig*y2[i-1]+2.0;
      y2[i]=(sig-1.0)/p;
      u[i]=(y[i+1]-y[i])/(x[i+1]-x[i]) - (y[i]-y[i-1])/(x[i]-x[i-1]);
      u[i]=(6.0*u[i]/(x[i+1]-x[i-1])-sig*u[i-1])/p;
   }
   if (ypn > 0.99e30)
      qn=un=0.0;
   else {
      qn=0.5;
      un=(3.0/(x[n]-x[n-1]))*(ypn-(y[n]-y[n-1])/(x[n]-x[n-1]));
   }
   y2[n]=(un-qn*u[n-1])/(qn*y2[n-1]+1.0);
   for (k=n-1;k>=1;k--)
      y2[k]=y2[k]*y2[k+1]+u[k];
   free_vector_pi(u,1,n-1);
}

void splint_pi(xa,ya,y2a,n,x,y)
double xa[],ya[],y2a[],x,*y;
int n;
{
   int klo,khi,k;
   double h,b,a;
   void nrerror_pi();

   klo=1;
   khi=n;
   while (khi-klo > 1) {
      k=(khi+klo) >> 1;
      if (xa[k] > x) khi=k;
      else klo=k;
   }
   h=xa[khi]-xa[klo];
   if (h == 0.0) nrerror_pi("Bad XA input to routine SPLINT");
   a=(xa[khi]-x)/h;
   b=(x-xa[klo])/h;
   *y=a*ya[klo]+b*ya[khi]+((a*a*a-a)*y2a[klo]+(b*b*b-b)*y2a[khi])*(h*h)/6.0;
}

void gaussj_pi(a,n,b,m)
double **a,**b;
int n,m;
{
   int *indxc,*indxr,*ipiv;
   int i,icol=1,irow=1,j,k,l,ll,*ivector_pi();
   double big,dum,pivinv;
   void nrerror_pi(),free_ivector_pi();

   indxc=ivector_pi(1,n);
   indxr=ivector_pi(1,n);
   ipiv=ivector_pi(1,n);
   for (j=1;j<=n;j++) ipiv[j]=0;
   for (i=1;i<=n;i++) {
      big=0.0;
      for (j=1;j<=n;j++)
         if (ipiv[j] != 1)
            for (k=1;k<=n;k++) {
               if (ipiv[k] == 0) {
                  if (fabs(a[j][k]) >= big) {
                     big=fabs(a[j][k]);
                     irow=j;
                     icol=k;
                  }
               } else if (ipiv[k] > 1) nrerror_pi("GAUSSJ: Singular Matrix-1");
            }
      ++(ipiv[icol]);
      if (irow != icol) {
         for (l=1;l<=n;l++) SWAP_pi(a[irow][l],a[icol][l])
         for (l=1;l<=m;l++) SWAP_pi(b[irow][l],b[icol][l])
      }
      indxr[i]=irow;
      indxc[i]=icol;
      if (a[icol][icol] == 0.0) nrerror_pi("GAUSSJ: Singular Matrix-2");
      pivinv=1.0/a[icol][icol];
      a[icol][icol]=1.0;
      for (l=1;l<=n;l++) a[icol][l] *= pivinv;
      for (l=1;l<=m;l++) b[icol][l] *= pivinv;
      for (ll=1;ll<=n;ll++)
         if (ll != icol) {
            dum=a[ll][icol];
            a[ll][icol]=0.0;
            for (l=1;l<=n;l++) a[ll][l] -= a[icol][l]*dum;
            for (l=1;l<=m;l++) b[ll][l] -= b[icol][l]*dum;
         }
   }
   for (l=n;l>=1;l--) {
      if (indxr[l] != indxc[l])
         for (k=1;k<=n;k++)
            SWAP_pi(a[k][indxr[l]],a[k][indxc[l]]);
   }
   free_ivector_pi(ipiv,1,n);
   free_ivector_pi(indxr,1,n);
   free_ivector_pi(indxc,1,n);
}

/* FINISH */


struct VERNER_STRUCT {
  double Eth;
  double Emax;
  double Ezero;
  double s0;
  double ya;
  double P;
  double yw;
  double y0;
  double y1;
};

struct VERNER_PARTIAL_STRUCT {
  int electron;
  int principal;
  int angular;
  double Eth;
  double Ezero;
  double s0;
  double ya;
  double P;
  double yw;
};


static double L_X,L_EMAX,L_EMIN,LNORM,LinterpNORM,GAMMA,f_COVERING,D,kT,sigmav_rad,sigmav_trans,v_rad,v_trans;
static double *EGRID,*PIGRID,*PIGRID_2,*RRGRID,*RRGRID_2,ELECTRON_ENERGY,THRESHOLD,ANGULAR;
static int GRIDNUM=20000,RECNUM=10;

static double *LOWE_EGRID,*LOWE_PIGRID,*LOWE_PIGRID_2,*LOWE_RRGRID,*LOWE_RRGRID_2;
static int LOWE_GRIDNUM=6;

static double *E_array,*E_spectrum,*abs_spectrum,*exc_spectrum,*rec_spectrum;
static double *l_array,*l_spectrum;
static double *int_array,*int_array_2;
static double *Tvec,*Yvec,*Yvec2;
static double *z_array,*hubble_array,*hubble_array_2;
static int HUBBLE_BINS=100000;

static double EBIN,EMIN,EMAX; /* range of spectrum in [eV] */

static double voigt_lim; /* Voigt function parameter */
static double tau_lim; /* Voigt function opacity limit */
static double pi_rate_lim; /* Voigt function opacity limit */

static int INPUT, INPUT_SIZE, INPUT_SHIFT;
static double *E_input,*L_input,*L_input_2,*EtimesL_input,*EtimesL_input_2;
static double HALFBIN_SIZE;

static double N_e,**Nion,**Tion,**EMion,**EM;

static double doppler_rad,doppler_trans;

static double **Hionizsigmaconv,**Heionizsigmaconv,*ionizsigmatemp,*spectrumtemp,*tau,*tau_exc,*tau_edge;

static double *RR_line,*RR_line_2,*DR_line,*DR_line_2,*L_kT;
static double *L_RR,*L_RR_2,*L_DR,*L_DR_2,*L_REC,*L_REC_2;
static double L_RR_kT,L_DR_kT,L_REC_kT;
static int SPECBINS;

/* XSPEC subroutines */
char* FGMSTR(char* name);

/* PHOTOION Subroutines */
/* Subroutines from PHOTOION */
double dfdE_pi(double g_i, double p0, double p1, double p2, double p3, double E);
double pisigma_pi(double g_i, double p0, double p1, double p2, double p3, double E);
double pispline_pi(double E);
double lowEpispline_pi(double E);
double vernerph_pi(struct VERNER_STRUCT verner, double E); /* verner photoionization sigma */
double vernerpartialph_pi(struct VERNER_PARTIAL_STRUCT verner, double E); /* verner partial photoionization sigma */
double excitsigma_pi(double E0, double OSCILLATOR,double DELTANUD,double ALPHA, double E);
double gauss_pi(double s,double x);   /* norm. gaussian: 1/sqrt(2*PI)/s e^(-x^2/2*s^2) */
double doppler_pi(double v /*[km/s]*/);
double voigt_pi(double ALPHA,double v);

/* MIABS Subroutines */
double fac_ionizsigma_pi(double THRESHOLD, double E);
void line_limits_pi(double Nion_column_density,double E0,double OSCILLATOR,double ALPHA,double DELTANUD,double *Elo,double *Ehi,int *SUMlo,int *SUMhi);
void line_opacity_pi(double Nion_column_density,double E0,double OSCILLATOR,double ALPHA,double DELTANUD,double tau_exc_p[]);
void fac_edge_opacity_pi(double Nion_column_density, double THRESHOLD, double tau_edge_p[]) ;
void verner_full_edge_opacity_pi(double Nion_column_density,double THRESHOLD,struct VERNER_STRUCT verner,double tau_edge_p[]);
void verner_partial_edge_opacity_pi(double Nion_column_density,double THRESHOLD,struct VERNER_PARTIAL_STRUCT verner,double tau_edge_p[]);

double rrsigma_pi(double g_i, double g_j, double p0, double p1, double p2, double p3, double E);
double rrspline_pi(double E);
double lowEpispline_pi(double E);
double lowErrspline_pi(double E);
double L_pi(double E);     /* nuclear power-law continuum */
double Linterp_pi(double E);
double EtimesL_pi(double E);
double RR_line_spline_pi(double temp);
double DR_line_spline_pi(double temp);
double REC_spline_pi(double temp);
double L_RR_spline_pi(double temp);
double L_DR_spline_pi(double temp);
double L_REC_spline_pi(double temp);
double integrand_pi(double temp);
double vernerph_pi(struct VERNER_STRUCT verner, double E); /* verner photoionization sigma */
double vernerpartialph_pi(struct VERNER_PARTIAL_STRUCT verner, double E); /* verner partial photoionization sigma */
double excitsigma_pi(double E0, double OSCILLATOR,double DELTANUD,double ALPHA, double E);
double maxwell_pi(double Te, double kT, double NORM);
double verner_recombination_pi(struct VERNER_STRUCT vioniz, double kT, double E);
void HeI_edge_opacity_pi(double Nion_column_density,double THRESHOLD,double tau_edge_p[]);
double fac_recombination_pi(double g_i,double g_j,double p0,double p1,double p2,double p3,double kT, double Te);
double loglinestrength_pi(double logkT);
double gauss_pi(double s,double x);   /* norm. gaussian: 1/sqrt(2*PI)/s e^(-x^2/2*s^2) */
double doppler_pi(double v /*[km/s]*/);

double hubble_integrand_pi(double z);
double hubble_integrate_pi(double z);

double fac_PI_rate_integral_pi(double THRESHOLD,double Labsorb[]);
double voigt_pi(double alpha, double v);

double HeI_edge_pi(double E);

int photoion
(float *ear,int ne,float *param,int ifl,float *photar,float *photer)
{
  /*ear[0] -> ear[ne]   in keV
    photar[0] -> photar[ne-1]  
    photar[i] = [photons/cm^2/s] for bin ear[i] -> ear[i+1]
    param[0] -> param[TOTAL-1]   gives XSPEC parameters from 1 to TOTAL*/

  struct VERNER_STRUCT vernerionizsigma[27][27];

  int PARTIAL_NUM=125;
  struct VERNER_PARTIAL_STRUCT partialsigma[29][125];

  struct HYDROGEN_STRUCT {
    double lambda[8];
    double f[8];
    double A[8];
    double b[8]; /* branching ratios? */
  };
  struct HYDROGEN_STRUCT hydrogen[29];
  
  struct HELIUM_STRUCT {
    double lambda[11];
    double f[11];
    double A[11];
    double b[11]; /* branching ratios? */
  };
  struct HELIUM_STRUCT helium[29];
  
  struct H_REC_STRUCT {
    double T[TEMPERATURES+1];
    double lines[6][TEMPERATURES+1];
    double rrc[TEMPERATURES+1];
    double C[TEMPERATURES+1];
  };
  struct H_REC_STRUCT H_rec[29];
  
  struct HE_REC_STRUCT {
    double T[TEMPERATURES+1];
    double lines[9][TEMPERATURES+1];
    double rrc[TEMPERATURES+1];
    double C[TEMPERATURES+1];
  };
  struct HE_REC_STRUCT He_rec[29];
  
  struct HIGHER_ORDER_STRUCT {
    double lambda[101];
    double f[101];
  };
  struct HIGHER_ORDER_STRUCT highn[29][3];

  double *type1_spectrum,*type2_spectrum,*type3_spectrum,*type4_spectrum,*type5_spectrum,*type6_spectrum,*type7_spectrum,*type8_spectrum;

  double int_junk,int_ans;

  int LENGTH=400;

  double Elo,Ehi; /* Voigt function param.'s */
  double Ewidth,earBIN;

  int first=1;

  int typenum;
  double lambda;

  /* junk values for strings, ints, and floats */
  char *sjunk,*line,*sjunk1,*sjunk2,*sjunk3,*sjunk4;
  int ijunk,jjunk;
  double djunk;

  double g_i,g_j;

  double ELO=1.e-3,EHI=1.e6;  /* MAKE SURE THIS RANGE IS OK!!! CHECK HERE!!! */

  double en,Atemp,ga;

  char *IONSTR,*root,*ext,*temp,*element_name;
  int i,j,k,n,itemp,jtemp;
  /* *.rr file */
  double p0,p1,p2,p3;

  double *Labsorb,*specRR,*specDR,*spec,*convert;
  double *rec_spectrum_temp,*specRR_temp,*specDR_temp;
  double ratePE,**rateRR,**rateDR,**ratePI;

  double RECNORM;

  double grounddeg[3],leveldeg[3][11],freedeg[3];

  int verbose;

  double earlo,earhi;
  
  int HIGHN=50;

  int ELEMENTS=12;

  double E0=0.,DELTANUD,ALPHA,OSCILLATOR;

  int element,electron,element_prev=0,principal,angular;
  int LINE;
  double WAVE,AMtemp,fMtemp;
  double Emax,Ezero,Eth,s0,ya,P,yw,y0,y1;
  int type;
  double Etemp,Ttemp,atemp,btemp,ctemp,dtemp,etemp,ftemp,intertemp,rtemp,rrctemp,Ctemp;
  double R,strength;

  double RRtemp,DRtemp;

  int *list;
  double intMIN,intMAX;

  char name[]="PHOTOION_DIR";
  char *DATADIR;

  double **H_excite, **He_excite;

  double redshift;
  int fileincr;

  int LOG;

  int SUMlo,SUMhi;

  double COLNORM,FLUXAVE;

  double *ABUND,*oshe,oscillatornorm=0.;

  int klo,khi,jlow,jhigh;
  double spectemp;

  int DIST,lines=0;

  FILE *vernerphoto,*vernerpartial,*linedat,*highnfile,*H_recfile,*He_recfile,*E_specfile=NULL,*l_specfile=NULL;
  FILE *input,*input2,*E_output=NULL,*l_output=NULL;
  char *vernerphoto_name,*vernerpartial_name,*linedat_name,*highnfile_name,*H_recfile_name,*He_recfile_name,*specfile_name;

  /* initialize photar array */
  for (i=0;i<ne;++i) photar[i]=0.;

  /* FILE NAMES */
  root=malloc(200);
  vernerphoto_name=malloc(200);
  vernerpartial_name=malloc(200);
  linedat_name=malloc(200);
  highnfile_name=malloc(200);
  H_recfile_name=malloc(200);
  He_recfile_name=malloc(200);
  specfile_name=malloc(200);

  IONSTR=malloc(30);
  ext=malloc(30);
  element_name=malloc(2);
  temp=malloc(130);
  sjunk=malloc(50);
  sjunk1=malloc(50);
  sjunk2=malloc(50);
  sjunk3=malloc(50);
  sjunk4=malloc(50);
  line=malloc(400);

  DATADIR=FGMSTR(name);

  ABUND=dvector_pi(1,30);
  for (i=1;i<=30;++i) ABUND[i]=0.;
  sprintf(temp,"%s/photoion_dat/abundance.dat",DATADIR);
  input=fopen(temp,"r");
  if ( input == NULL ) {
    printf("PHOTOION: Failed to open %s\n", temp);
    return 1;
  }
  while (fscanf(input,"%d%lf",&element,&djunk)!=EOF) {
    ABUND[element]=djunk;
  }
  fclose(input);

  oshe=dvector_pi(1,30);
  sprintf(temp,"%s/photoion_dat/oscillator_he.dat",DATADIR);
  input=fopen(temp,"r");
  while (fscanf(input,"%d%lf",&element,&djunk)!=EOF) {
    oshe[element]=cube(10.)*djunk;
  }
  fclose(input);

  /* Initialize the model array */
  for (i=0;i<ne;++i) photar[i] = 0.;

  Nion=dmatrix_pi(1,28,1,28);
  Tion=dmatrix_pi(1,28,1,28);
  EMion=dmatrix_pi(1,28,1,28);
  EM=dmatrix_pi(1,28,1,28);
  ratePI=dmatrix_pi(1,28,1,28);
  rateRR=dmatrix_pi(1,28,1,28);
  rateDR=dmatrix_pi(1,28,1,28);
  for (i=1;i<=28;++i) for (j=1;j<=28;++j) {Nion[i][j]=0.;Tion[i][j]=10.;EMion[i][j]=0.;EM[i][j]=0.;ratePI[i][j]=0.;rateRR[i][j]=0.;rateDR[i][j]=0.;}

  type = (int) param[0];
  redshift = param[1];
  v_rad = 1.e5*param[2];   /* convert [km/s] to [cm/s] */
  v_trans = 1.e5*param[3];   /* convert [km/s] to [cm/s] */
  sigmav_rad = 1.e5*param[4];   /* convert [km/s] to [cm/s] */
    if (sigmav_rad>0.) lines=1; else if (sigmav_rad<=0.) lines=0;
  sigmav_trans = 1.e5*param[5]; /* convert [km/s] to [cm/s] */
  INPUT = param[6];
  INPUT_SHIFT = param[7];
  GAMMA = param[8];
  L_EMIN = 1000.*param[9]; /* from keV to eV */
  L_EMAX = 1000.*param[10]; /* from keV to eV */
  L_X = 1.e30*param[11];  /* convert [1e30 ergs/s] to [ergs/s] */
  FLUXAVE=param[12];
  f_COVERING = param[13];
  D = parsectocm*param[14];    /* convert [pc] to [cm] */
  EMIN=1000.*param[15]; /* from keV to eV */
  EMAX=1000.*param[16]; /* from keV to eV */
  SPECBINS=(int) param[17];
  fileincr = (int) param[18];
  verbose =(int) param[19];
  COLNORM=param[20];

  /* Total electron column density */
  N_e = param[21];

  /* H */
  Nion[1][1] = param[22];

  /* He */
  Nion[2][1] = param[23];
  Nion[2][2] = param[24];

  /* C */
  Nion[6][1] = param[25];
  Nion[6][2] = param[26];
  Nion[6][3] = param[27];
  Nion[6][4] = param[28];
  Nion[6][5] = param[29];
  Nion[6][6] = param[30];

  /* N */
  Nion[7][1] = param[31];
  Nion[7][2] = param[32];
  Nion[7][3] = param[33];
  Nion[7][4] = param[34];
  Nion[7][5] = param[35];
  Nion[7][6] = param[36];
  Nion[7][7] = param[37];

  /* O */
  Nion[8][1] = param[38];
  Nion[8][2] = param[39];
  Nion[8][3] = param[40];
  Nion[8][4] = param[41];
  Nion[8][5] = param[42];
  Nion[8][6] = param[43];
  Nion[8][7] = param[44];
  Nion[8][8] = param[45];

  /* Ne */
  Nion[10][1] = param[46];
  Nion[10][2] = param[47];
  Nion[10][3] = param[48];
  Nion[10][4] = param[49];
  Nion[10][5] = param[50];
  Nion[10][6] = param[51];
  Nion[10][7] = param[52];
  Nion[10][8] = param[53];
  Nion[10][9] = param[54];
  Nion[10][10] = param[55];

  /* Mg */
  Nion[12][1] = param[56];
  Nion[12][2] = param[57];
  Nion[12][3] = param[58];
  Nion[12][4] = param[59];
  Nion[12][5] = param[60];
  Nion[12][6] = param[61];
  Nion[12][7] = param[62];
  Nion[12][8] = param[63];
  Nion[12][9] = param[64];
  Nion[12][10] = param[65];
  Nion[12][11] = param[66];
  Nion[12][12] = param[67];

  /* Al */
  Nion[13][1] = param[68];
  Nion[13][2] = param[69];
  Nion[13][3] = param[70];
  Nion[13][4] = param[71];
  Nion[13][5] = param[72];
  Nion[13][6] = param[73];
  Nion[13][7] = param[74];
  Nion[13][8] = param[75];
  Nion[13][9] = param[76];
  Nion[13][10] = param[77];
  Nion[13][11] = param[78];
  Nion[13][12] = param[79];
  Nion[13][13] = param[80];

  /* Si */
  Nion[14][1] = param[81];
  Nion[14][2] = param[82];
  Nion[14][3] = param[83];
  Nion[14][4] = param[84];
  Nion[14][5] = param[85];
  Nion[14][6] = param[86];
  Nion[14][7] = param[87];
  Nion[14][8] = param[88];
  Nion[14][9] = param[89];
  Nion[14][10] = param[90];
  Nion[14][11] = param[91];
  Nion[14][12] = param[92];
  Nion[14][13] = param[93];
  Nion[14][14] = param[94];

  /* S */
  Nion[16][1] = param[95];
  Nion[16][2] = param[96];
  Nion[16][3] = param[97];
  Nion[16][4] = param[98];
  Nion[16][5] = param[99];
  Nion[16][6] = param[100];
  Nion[16][7] = param[101];
  Nion[16][8] = param[102];
  Nion[16][9] = param[103];
  Nion[16][10] = param[104];
  Nion[16][11] = param[105];
  Nion[16][12] = param[106];
  Nion[16][13] = param[107];
  Nion[16][14] = param[108];
  Nion[16][15] = param[109];
  Nion[16][16] = param[110];

  /* Ar */
  Nion[18][1] = param[111];
  Nion[18][2] = param[112];
  Nion[18][3] = param[113];
  Nion[18][4] = param[114];
  Nion[18][5] = param[115];
  Nion[18][6] = param[116];
  Nion[18][7] = param[117];
  Nion[18][8] = param[118];
  Nion[18][9] = param[119];
  Nion[18][10] = param[120];
  Nion[18][11] = param[121];
  Nion[18][12] = param[122];
  Nion[18][13] = param[123];
  Nion[18][14] = param[124];
  Nion[18][15] = param[125];
  Nion[18][16] = param[126];
  Nion[18][17] = param[127];
  Nion[18][18] = param[128];

  /* Ca */
  Nion[20][1] = param[129];
  Nion[20][2] = param[130];
  Nion[20][3] = param[131];
  Nion[20][4] = param[132];
  Nion[20][5] = param[133];
  Nion[20][6] = param[134];
  Nion[20][7] = param[135];
  Nion[20][8] = param[136];
  Nion[20][9] = param[137];
  Nion[20][10] = param[138];
  Nion[20][11] = param[139];
  Nion[20][12] = param[140];
  Nion[20][13] = param[141];
  Nion[20][14] = param[142];
  Nion[20][15] = param[143];
  Nion[20][16] = param[144];
  Nion[20][17] = param[145];
  Nion[20][18] = param[146];
  Nion[20][19] = param[147];
  Nion[20][20] = param[148];

  /* Fe */
  Nion[26][1] = param[149];
  Nion[26][2] = param[150];
  Nion[26][3] = param[151];
  Nion[26][4] = param[152];
  Nion[26][5] = param[153];
  Nion[26][6] = param[154];
  Nion[26][7] = param[155];
  Nion[26][8] = param[156];
  Nion[26][9] = param[157];
  Nion[26][10] = param[158];
  Nion[26][11] = param[159];
  Nion[26][12] = param[160];
  Nion[26][13] = param[161];
  Nion[26][14] = param[162];
  Nion[26][15] = param[163];
  Nion[26][16] = param[164];
  Nion[26][17] = param[165];
  Nion[26][18] = param[166];
  Nion[26][19] = param[167];
  Nion[26][20] = param[168];
  Nion[26][21] = param[169];
  Nion[26][22] = param[170];
  Nion[26][23] = param[171];
  Nion[26][24] = param[172];
  Nion[26][25] = param[173];
  Nion[26][26] = param[174];

  /* Ni */
  Nion[28][1] = param[175];
  Nion[28][2] = param[176];
  Nion[28][3] = param[177];
  Nion[28][4] = param[178];
  Nion[28][5] = param[179];
  Nion[28][6] = param[180];
  Nion[28][7] = param[181];
  Nion[28][8] = param[182];
  Nion[28][9] = param[183];
  Nion[28][10] = param[184];
  Nion[28][11] = param[185];
  Nion[28][12] = param[186];
  Nion[28][13] = param[187];
  Nion[28][14] = param[188];
  Nion[28][15] = param[189];
  Nion[28][16] = param[190];
  Nion[28][17] = param[191];
  Nion[28][18] = param[192];
  Nion[28][19] = param[193];
  Nion[28][20] = param[194];
  Nion[28][21] = param[195];
  Nion[28][22] = param[196];
  Nion[28][23] = param[197];
  Nion[28][24] = param[198];
  Nion[28][25] = param[199];
  Nion[28][26] = param[200];
  Nion[28][27] = param[201];
  Nion[28][28] = param[202];
  Tion[6][1] = param[203];
  Tion[6][2] = param[204];
  Tion[7][1] = param[205];
  Tion[7][2] = param[206];
  Tion[8][1] = param[207];
  Tion[8][2] = param[208];
  Tion[10][1] = param[209];
  Tion[10][2] = param[210];
  Tion[10][3] = param[211];
  Tion[10][4] = param[212];
  Tion[10][5] = param[213];
  Tion[10][6] = param[214];
  Tion[10][7] = param[215];
  Tion[10][8] = param[216];
  Tion[10][9] = param[217];
  Tion[10][10] = param[218];
  Tion[12][1] = param[219];
  Tion[12][2] = param[220];
  Tion[12][3] = param[221];
  Tion[12][4] = param[222];
  Tion[12][5] = param[223];
  Tion[12][6] = param[224];
  Tion[12][7] = param[225];
  Tion[12][8] = param[226];
  Tion[12][9] = param[227];
  Tion[12][10] = param[228];
  Tion[13][1] = param[229];
  Tion[13][2] = param[230];
  Tion[13][3] = param[231];
  Tion[13][4] = param[232];
  Tion[13][5] = param[233];
  Tion[13][6] = param[234];
  Tion[13][7] = param[235];
  Tion[13][8] = param[236];
  Tion[13][9] = param[237];
  Tion[13][10] = param[238];
  Tion[14][1] = param[239];
  Tion[14][2] = param[240];
  Tion[14][3] = param[241];
  Tion[14][4] = param[242];
  Tion[14][5] = param[243];
  Tion[14][6] = param[244];
  Tion[14][7] = param[245];
  Tion[14][8] = param[246];
  Tion[14][9] = param[247];
  Tion[14][10] = param[248];
  Tion[16][1] = param[249];
  Tion[16][2] = param[250];
  Tion[16][3] = param[251];
  Tion[16][4] = param[252];
  Tion[16][5] = param[253];
  Tion[16][6] = param[254];
  Tion[16][7] = param[255];
  Tion[16][8] = param[256];
  Tion[16][9] = param[257];
  Tion[16][10] = param[258];
  Tion[18][1] = param[259];
  Tion[18][2] = param[260];
  Tion[18][3] = param[261];
  Tion[18][4] = param[262];
  Tion[18][5] = param[263];
  Tion[18][6] = param[264];
  Tion[18][7] = param[265];
  Tion[18][8] = param[266];
  Tion[18][9] = param[267];
  Tion[18][10] = param[268];
  Tion[20][1] = param[269];
  Tion[20][2] = param[270];
  Tion[20][3] = param[271];
  Tion[20][4] = param[272];
  Tion[20][5] = param[273];
  Tion[20][6] = param[274];
  Tion[20][7] = param[275];
  Tion[20][8] = param[276];
  Tion[20][9] = param[277];
  Tion[20][10] = param[278];
  Tion[26][1] = param[279];
  Tion[26][2] = param[280];
  Tion[26][3] = param[281];
  Tion[26][4] = param[282];
  Tion[26][5] = param[283];
  Tion[26][6] = param[284];
  Tion[26][7] = param[285];
  Tion[26][8] = param[286];
  Tion[26][9] = param[287];
  Tion[26][10] = param[288];
  Tion[28][1] = param[289];
  Tion[28][2] = param[290];
  Tion[28][3] = param[291];
  Tion[28][4] = param[292];
  Tion[28][5] = param[293];
  Tion[28][6] = param[294];
  Tion[28][7] = param[295];
  Tion[28][8] = param[296];
  Tion[28][9] = param[297];
  Tion[28][10] = param[298];

  if (type == 4 || type ==5) {v_trans=v_rad; sigmav_trans=sigmav_rad;}

  LOG = 0;
  if (LOG!=0) {
    for (i=1;i<=28;++i) {
      for (j=1;j<=28;++j) { 
	if (Nion[i][j]) {
	  Nion[i][j]=pow(10.,Nion[i][j]);
	}
      }
    }
    if (N_e) N_e=pow(10.,N_e);
  }

  if (COLNORM!=1.) {
    for (i=1;i<=28;++i) for (j=1;j<=28;++j) Nion[i][j]*=COLNORM;
    N_e*=COLNORM;
  }

  /* SAME AS "PHSI" and "PHXI" FROM HERE ON OUT */

  /* To put EMIN and EMAX in source rest-frame energy units
     We convert back at the very end */  
  EMIN=EMIN*(1.+redshift);
  EMAX=EMAX*(1.+redshift);
 
  voigt_lim=1.e-4;
  tau_lim=1.e-4;
  pi_rate_lim=1.e-3;
  /*  if (type<=0) tau_lim=0.;*/

  if (D==0.) {
    if (redshift!=0.) {
      /* hubble equation integral */
      z_array=dvector_pi(1,HUBBLE_BINS);
      hubble_array=dvector_pi(1,HUBBLE_BINS);
      hubble_array_2=dvector_pi(1,HUBBLE_BINS);
      for (k=1;k<=HUBBLE_BINS;++k) {
	z_array[k]=((double) k-1.)/((double) HUBBLE_BINS-1.)*redshift;
	hubble_array[k]=hubble_integrand_pi(z_array[k]);
      }
      spline_pi(z_array,hubble_array,HUBBLE_BINS,1.e40,1.e40,hubble_array_2);
      D=qromb_pi(hubble_integrate_pi,0.,redshift); /* if D=0, use Hubble law */
      if (verbose) printf("redshift = %e   D = %e Mpc  (using H_0=71 km/s/Mpc, Omega_m=0.27, Omega_lambda=0.73)\n",redshift,D/parsectocm/1.e6);
      free_dvector_pi(z_array,1,HUBBLE_BINS);
      free_dvector_pi(hubble_array,1,HUBBLE_BINS);
      free_dvector_pi(hubble_array_2,1,HUBBLE_BINS);
    } else {
      D=1.e6*parsectocm;
      if (verbose) printf("Since redshift=0 and D=0, taking D=1 Mpc\n");
    }
  }

  doppler_rad=doppler_pi(v_rad);
  doppler_trans=doppler_pi(v_trans);

  EGRID=dvector_pi(1,GRIDNUM);  
  PIGRID=dvector_pi(1,GRIDNUM); 
  PIGRID_2=dvector_pi(1,GRIDNUM);
  RRGRID=dvector_pi(1,GRIDNUM);  
  RRGRID_2=dvector_pi(1,GRIDNUM);

  LOWE_EGRID=dvector_pi(1,LOWE_GRIDNUM);  
  LOWE_PIGRID=dvector_pi(1,LOWE_GRIDNUM); 
  LOWE_PIGRID_2=dvector_pi(1,LOWE_GRIDNUM);
  LOWE_RRGRID=dvector_pi(1,LOWE_GRIDNUM); 
  LOWE_RRGRID_2=dvector_pi(1,LOWE_GRIDNUM);

  specRR=dvector_pi(1,SPECBINS);  
  specDR=dvector_pi(1,SPECBINS);  
  specRR_temp=dvector_pi(1,SPECBINS);  
  specDR_temp=dvector_pi(1,SPECBINS);  
  spec=dvector_pi(1,SPECBINS);  

  L_kT=dvector_pi(1,RECNUM);  
  L_RR=dvector_pi(1,RECNUM);  
  L_RR_2=dvector_pi(1,RECNUM);  
  L_DR=dvector_pi(1,RECNUM);  
  L_DR_2=dvector_pi(1,RECNUM);  
  L_REC=dvector_pi(1,RECNUM);  
  L_REC_2=dvector_pi(1,RECNUM);  
  RR_line=dvector_pi(1,RECNUM);  
  RR_line_2=dvector_pi(1,RECNUM);  
  DR_line=dvector_pi(1,RECNUM);  
  DR_line_2=dvector_pi(1,RECNUM);  

  E_array=dvector_pi(1,SPECBINS);       /* energy axis */
  E_spectrum=dvector_pi(1,SPECBINS);    /* spectrum (energy units) */
  type1_spectrum=dvector_pi(1,SPECBINS);    /* spectrum (energy units) */
  type2_spectrum=dvector_pi(1,SPECBINS);    /* spectrum (energy units) */
  type3_spectrum=dvector_pi(1,SPECBINS);    /* spectrum (energy units) */
  type4_spectrum=dvector_pi(1,SPECBINS);    /* spectrum (energy units) */
  type5_spectrum=dvector_pi(1,SPECBINS);    /* spectrum (energy units) */
  type6_spectrum=dvector_pi(1,SPECBINS);    /* spectrum (energy units) */
  type7_spectrum=dvector_pi(1,SPECBINS);    /* spectrum (energy units) */
  type8_spectrum=dvector_pi(1,SPECBINS);    /* spectrum (energy units) */
  l_array=dvector_pi(1,SPECBINS);       /* wavelength axis */
  l_spectrum=dvector_pi(1,SPECBINS);    /* spectrum (wavelength units) */
  convert=dvector_pi(1,SPECBINS);    /* spectrum (wavelength units) */
  abs_spectrum=dvector_pi(1,SPECBINS);    /* pure absorption spectrum */
  exc_spectrum=dvector_pi(1,SPECBINS);    /* photoexcitation spectrum */
  rec_spectrum=dvector_pi(1,SPECBINS);    /* recombination spectrum */
  rec_spectrum_temp=dvector_pi(1,SPECBINS);    /* recombination spectrum */
  tau=dvector_pi(1,SPECBINS);           /* total opacity in all ions */
  tau_edge=dvector_pi(1,SPECBINS);           /* total opacity in all ions */
  tau_exc=dvector_pi(1,SPECBINS);           /* total opacity in all ions */
  Labsorb=dvector_pi(1,SPECBINS);    
  int_array=dvector_pi(1,SPECBINS);  
  int_array_2=dvector_pi(1,SPECBINS); 
  Hionizsigmaconv=dmatrix_pi(1,28,1,SPECBINS);
  Heionizsigmaconv=dmatrix_pi(1,28,1,SPECBINS);
  ionizsigmatemp=dvector_pi(1,SPECBINS);
  spectrumtemp=dvector_pi(1,SPECBINS);
  H_excite=dmatrix_pi(1,28,1,6);
  He_excite=dmatrix_pi(1,28,1,9);
  list=ivector_pi(1,ELEMENTS);
  Tvec=dvector_pi(1,TEMPERATURES);
  Yvec=dvector_pi(1,TEMPERATURES);
  Yvec2=dvector_pi(1,TEMPERATURES);

  /* Atomic number for C,N,O,Ne,Mg,Al,Si,S,Ar,Ca,Fe */
  list[1]=6;
  list[2]=7;
  list[3]=8;
  list[4]=10;
  list[5]=12;
  list[6]=13;
  list[7]=14;
  list[8]=16;
  list[9]=18;
  list[10]=20;
  list[11]=26;
  list[12]=28;

  /* uniformly-spaced energy bin size */
  EBIN=(EMAX-EMIN)/((double) SPECBINS);
  if (verbose) if (EBIN>0.5) {
    printf("****************************************\n");
    printf("* Warning: EBIN = %6.2e eV is big!  *\n",EBIN);
    printf("* You might want to increase SPECBINS. *\n");
    printf("****************************************\n");
  }
  for (k=1;k<=SPECBINS;++k) {
    E_array[k]=((double) k-0.5)*EBIN+EMIN;
    l_array[k]=AngstromtokeV/(E_array[k]/1000.);  /* [Angstrom] */
  }

  /* for velocity convolution out to >= 4 sigma */
  DIST=(int) (4.*sigmav_rad/ccc*EMAX/EBIN); /*  +/-4 sigma */
  /*  if (verbose) printf("4-sigma_v^rad = %d bins\n",DIST);*/
  
  /* logarithmic energy grid for Fe-L RR and PI cross sections*/
  for (k=1;k<=GRIDNUM;++k) {
    /*EGRID=log10(energy)*/
    EGRID[k]=((double) k-1.)/((double) GRIDNUM)*log10(EHI/ELO)+log10(ELO);
  }

  /*  Reading in Photoionization Cross Sections (Verner)  */
  sprintf(vernerphoto_name,"%s/photoion_dat/verner_photo.dat",DATADIR);
  vernerphoto=fopen(vernerphoto_name,"r");
  while (fscanf(vernerphoto,"%d%d%lf%lf%lf%lf%lf%lf%lf%lf%lf",&element,&electron,&Eth,&Emax,&Ezero,&s0,&ya,&P,&yw,&y0,&y1)!=EOF) {
    vernerionizsigma[element][electron].Eth=Eth;
    vernerionizsigma[element][electron].Emax=Emax;
    vernerionizsigma[element][electron].Ezero=Ezero;
    vernerionizsigma[element][electron].s0=s0;
    vernerionizsigma[element][electron].ya=ya;
    vernerionizsigma[element][electron].P=P;
    vernerionizsigma[element][electron].yw=yw;
    vernerionizsigma[element][electron].y0=y0;
    vernerionizsigma[element][electron].y1=y1;
  }
  fclose(vernerphoto);
  free(vernerphoto_name);

  /*  Reading in Photoionization Cross Sections (Verner)  */
  sprintf(vernerpartial_name,"%s/photoion_dat/verner_partial_PIsigmas.dat",DATADIR);
  vernerpartial=fopen(vernerpartial_name,"r");
  k=0;
  while (fscanf(vernerpartial,"%d%d%d%d%lf%lf%lf%lf%lf%lf",&element,&electron,&principal,&angular,&Eth,&Ezero,&s0,&ya,&P,&yw)!=EOF) {
    if (element!=element_prev) k=0;
    partialsigma[element][k].electron=electron;
    partialsigma[element][k].principal=principal;
    partialsigma[element][k].angular=angular;
    partialsigma[element][k].Eth=Eth;
    partialsigma[element][k].Ezero=Ezero;
    partialsigma[element][k].s0=s0;
    partialsigma[element][k].ya=ya;
    partialsigma[element][k].P=P;
    partialsigma[element][k].yw=yw;
    element_prev=element;
    k=k+1;
  }
  fclose(vernerpartial);
  free(vernerpartial_name);

  /* H- and He-like cross sections for C, N, and O */
  if (verbose) printf("H- and He-like edge cross sections for H,He,C to Ni...\n");
  /* Photoionization opacity for H- and He-like */
  for (element=1;element<=28;++element) {
    for (electron=1;electron<=2;++electron) {
      if (Nion[element][electron]) {
	if (element==2 && electron==2) {
	  THRESHOLD=24.58;
	  HeI_edge_opacity_pi(Nion[element][electron],THRESHOLD,tau_edge);
	} else {
	  THRESHOLD=vernerionizsigma[element][electron].Eth;
	  verner_full_edge_opacity_pi(Nion[element][electron],THRESHOLD,vernerionizsigma[element][electron],tau_edge);
	}
      }
    }
  }

  /* From Verner table: L-shell edges for C,N,O */
  for (element=6;element<=8;++element) {
    for (electron=3;electron<=8;++electron) {
      if (Nion[element][electron]) {
	for (j=0;j<=PARTIAL_NUM;++j) {
	  if (partialsigma[element][j].electron==electron && partialsigma[element][j].principal>=2) {
	    THRESHOLD=partialsigma[element][j].Eth;
	    verner_partial_edge_opacity_pi(Nion[element][electron],THRESHOLD,partialsigma[element][j],tau_edge);
  	  }
	}
      }
    }
  }

  /* From Verner table: Get L-shell edges for C,N,O and M-shell edges for M-shell ions */
  for (element=1;element<=28;++element) {
    for (electron=11;electron<=28;++electron) {
      if (Nion[element][electron] && !(electron <=20 && (element == 26 || element == 28))) {
	for (j=0;j<=PARTIAL_NUM;++j) {
	  if (partialsigma[element][j].electron==electron && partialsigma[element][j].principal>=3) {
	    THRESHOLD=partialsigma[element][j].Eth;
	    verner_partial_edge_opacity_pi(Nion[element][electron],THRESHOLD,partialsigma[element][j],tau_edge);
  	  }
	}
      }
    }
  }

  if (verbose) printf("Determining LOW-n Photoexcitation Cross Sections & Opacity for C to Ni...\n");
  grounddeg[1]=2.;
  freedeg[1]=1.;
  for (i=1;i<=6;++i) leveldeg[1][i]=6.;
  grounddeg[2]=1.;
  freedeg[2]=2.;
  for (i=3;i<=8;++i) leveldeg[2][i]=3.;
  sprintf(linedat_name,"%s/photoion_dat/line.dat",DATADIR);
  linedat=fopen(linedat_name,"r");
  while(fscanf(linedat,"%s%d%d%d%s%lf%s%lf%lf",sjunk,&element,&electron,&LINE,sjunk,&WAVE,sjunk,&AMtemp,&fMtemp) != EOF) {
    if (electron == 1) {
      hydrogen[element].lambda[LINE]=WAVE;
      hydrogen[element].A[LINE]=AMtemp;
      hydrogen[element].f[LINE]=fMtemp;
      if (Nion[element][electron] && fMtemp && LINE <= 4) {
	E0=1000.*AngstromtokeV/hydrogen[element].lambda[LINE];
	E0=E0*doppler_rad;
	OSCILLATOR=hydrogen[element].f[LINE];
	g_j=leveldeg[electron][LINE];
	g_i=grounddeg[electron];
	Atemp=hydrogen[element].A[LINE];
	ga=Atemp/*g_j/(3.*g_i*OSCILLATOR)*/;
	DELTANUD=sqrt(2.)*sigmav_rad/ccc*(E0*eVtoergs/hhh);
	ALPHA=ga/(4.*PI*DELTANUD);
	if (lines) line_opacity_pi(Nion[element][electron],E0,OSCILLATOR,ALPHA,DELTANUD,tau_exc);
      }
    } else if (electron == 2) {
      helium[element].lambda[LINE]=WAVE;
      helium[element].A[LINE]=AMtemp;
      helium[element].f[LINE]=fMtemp;
      if (Nion[element][electron] && fMtemp && LINE <= 6) {
	E0=1000.*AngstromtokeV/helium[element].lambda[LINE];
	E0=E0*doppler_rad;
	OSCILLATOR=helium[element].f[LINE];
	g_j=leveldeg[electron][LINE];
	g_i=grounddeg[electron];
	Atemp=helium[element].A[LINE];
	ga=Atemp/*g_j*Atemp/(3.*g_i*OSCILLATOR)*/;
	DELTANUD=sqrt(2.)*sigmav_rad/ccc*(E0*eVtoergs/hhh);
	ALPHA=ga/(4.*PI*DELTANUD);
	if (lines) line_opacity_pi(Nion[element][electron],E0,OSCILLATOR,ALPHA,DELTANUD,tau_exc);
      }
    }
  }
  fclose(linedat);
  free(linedat_name);
  
  if (lines) {
    if (verbose) printf("Determining HIGH-n Photoexcitation Cross Sections & Opacity for C to Fe\n");
    sprintf(highnfile_name,"%s/photoion_dat/highn.dat",DATADIR);
    highnfile=fopen(highnfile_name,"r");
    while (fscanf(highnfile,"%d%d%d%lf%lf",&element,&electron,&n,&Etemp,&ftemp)!=EOF) {
      highn[element][electron].lambda[n]=AngstromtokeV/Etemp*1000.;
      highn[element][electron].f[n]=ftemp;
    }
    fclose(highnfile);
    free(highnfile_name);
    for (i=1;i<=ELEMENTS;++i) {
      element=list[i];
      for (electron=1;electron<=2;++electron) {
	if (Nion[element][electron] && lines) {
	  if (electron==1) oscillatornorm=1.6; /* Bethe-Salpeter p. 265 */
	  if (electron==2) oscillatornorm=oshe[element]; /* defined above */
	  for (n=6;n<=HIGHN;++n) {
	    OSCILLATOR=oscillatornorm/cube((double) n);
	    if (element!=28) {
	      E0=AngstromtokeV/highn[element][electron].lambda[n]*1000.;
	    } else if (electron==1) {/* Use Fe numbers for Ni */
	      E0=AngstromtokeV/(highn[26][electron].lambda[n]/1.1614)*1000.;
	    } else if (electron==2) {/* Use Fe numbers for Ni */
	      E0=AngstromtokeV/(highn[26][electron].lambda[n]/1.165)*1000.;
	    }
	    E0=E0*doppler_rad;
	    DELTANUD=sqrt(2.)*sigmav_rad/ccc*(E0*eVtoergs/hhh);
	    /* taking classical value for "ga=gamma" from p. 112-114 B+D */
	    ga=2.47*pow(10.,-22.)*sqr(E0*eVtoergs/hhh);
	    ALPHA=ga/(4.*PI*DELTANUD);
	    line_opacity_pi(Nion[element][electron],E0,OSCILLATOR,ALPHA,DELTANUD,tau_exc);
	  }
	}
      }
    }
  }

  /* File root for L-shell input files for Ne through Ca */
  if (verbose) printf("L-shell ions:  Ne through Ni\n");
  for (element=10;element<=28;++element) {
    for (electron=1;electron<=10;++electron) {
      if (Nion[element][electron] && (electron>=3 || element==28)) {
	if (element == 10) sprintf(element_name,"Ne");
	if (element == 12) sprintf(element_name,"Mg");
	if (element == 13) sprintf(element_name,"Al");
	if (element == 14) sprintf(element_name,"Si");
	if (element == 16) sprintf(element_name,"S");
	if (element == 18) sprintf(element_name,"Ar");
	if (element == 20) sprintf(element_name,"Ca");
	if (element == 26) sprintf(element_name,"Fe");
	if (element == 28) sprintf(element_name,"Ni");
	sprintf(root,"%s/photoion_dat/L_shell/%s",DATADIR,element_name);
	if (verbose) printf("Z = %2d   z = %2d\n",element,electron);
	/* for photoionization cross-sections */
	ext="pi_short"; 
	if (electron<10) sprintf(temp,"%s0%da.%s\0",root,electron,ext);
	else sprintf(temp,"%s%da.%s\0",root,electron,ext);
	input=fopen(temp,"r");
	while (fscanf(input,"%d%lf%d%lf%lf%lf",&i,&g_i,&j,&g_j,&THRESHOLD,&ANGULAR) != EOF) {
	  g_i=g_i+1.;
	  g_j=g_j+1.;
	  fscanf(input,"%lf%lf%lf%lf",&p0,&p1,&p2,&p3);
	  for (k=1;k<=LOWE_GRIDNUM;++k) {
	    fscanf(input,"%lf%lf%lf%lf",&(LOWE_EGRID[k]),&djunk,&(LOWE_PIGRID[k]),&djunk);
	    LOWE_EGRID[k]=log10(LOWE_EGRID[k]+THRESHOLD);
	    LOWE_PIGRID[k]=log10(1.e-20*LOWE_PIGRID[k]);
	  }
	  spline_pi(LOWE_EGRID,LOWE_PIGRID,LOWE_GRIDNUM,1.e40,1.e40,LOWE_PIGRID_2);	  
	  if (Nion[element][electron]*pisigma_pi(g_i,p0,p1,p2,p3,THRESHOLD) >= tau_lim) {
	    /* PHOTOIONIZATION OUT OF GRD STATE OF n+1 ION */
	    for (k=1;k<=GRIDNUM;++k) {
	      if (EGRID[k]<log10(THRESHOLD)) PIGRID[k]=1.e-90;
	      else if (EGRID[k]>=log10(THRESHOLD) && EGRID[k]<=LOWE_EGRID[LOWE_GRIDNUM]) PIGRID[k]=lowEpispline_pi(pow(10.,EGRID[k]));
	      else PIGRID[k]=pisigma_pi(g_i,p0,p1,p2,p3,pow(10.,EGRID[k]));
	    }
	    for (k=1;k<=GRIDNUM;++k) PIGRID[k]=log10(PIGRID[k]);
	    spline_pi(EGRID,PIGRID,GRIDNUM,1.e40,1.e40,PIGRID_2);
	    
	    /* calculate opacity of given edge and modify "tau" accordingly */
	    fac_edge_opacity_pi(Nion[element][electron],THRESHOLD,tau_edge);
	  }
	}
	fclose(input);
	
	if (lines && electron >= 3) {
	  ext="tr_short"; 
	  if (electron<10) sprintf(temp,"%s0%da.%s\0",root,electron,ext);
	  else sprintf(temp,"%s%da.%s\0",root,electron,ext);
	  input=fopen(temp,"r");
	  while (fscanf(input,"%d%lf%d%lf%lf%lf%lf",&j,&g_j,&i,&g_i,&en,&ftemp,&Atemp) != EOF) {
	    j=j+1;
	    i=i+1;
	    g_i=g_i+1.;
	    g_j=g_j+1.;
	    ftemp=ftemp/g_i;     /* CHECK THIS - VERY IMPORTANT!!! */
	    E0=en;
	    E0=E0*doppler_rad;
	    DELTANUD=sqrt(2.)*sigmav_rad/ccc*(E0*eVtoergs/hhh);
	    if ((E0>=EMIN && E0<=EMAX) && ftemp>tau_lim*FACTOR*DELTANUD/Nion[element][electron]) {
	      OSCILLATOR=ftemp;
	      ga=Atemp/*g_j*Atemp/(3.*g_i*OSCILLATOR)*/;
	      ALPHA=ga/(4.*PI*DELTANUD);
	    line_opacity_pi(Nion[element][electron],E0,OSCILLATOR,ALPHA,DELTANUD,tau_exc);
	    }
	  }
	  fclose(input);
	}
      }
    }
  }


  if (verbose) printf("L-shell ions:  C through O\n");
  for (element=6;element<=8;++element) {
    for (electron=3;electron<=8;++electron) {
      if (Nion[element][electron]) {
	if (element == 6) sprintf(element_name,"C");
	if (element == 7) sprintf(element_name,"N");
	if (element == 8) sprintf(element_name,"O");
	sprintf(root,"%s/photoion_dat/L_shell/%s",DATADIR,element_name);
	if (verbose) printf("Z = %2d   z = %2d\n",element,electron);
	/* for photoionization cross-sections */
	ext="pi_short"; 
	if (electron<10) sprintf(temp,"%s0%da.%s\0",root,electron,ext);
	else sprintf(temp,"%s%da.%s\0",root,electron,ext);
	input=fopen(temp,"r");
	while (fscanf(input,"%d%lf%d%lf%lf%lf",&i,&g_i,&j,&g_j,&THRESHOLD,&ANGULAR) != EOF) {
	  g_i=g_i+1.;
	  g_j=g_j+1.;
	  fscanf(input,"%lf%lf%lf%lf",&p0,&p1,&p2,&p3);
	  for (k=1;k<=LOWE_GRIDNUM;++k) {
	    fscanf(input,"%lf%lf%lf%lf",&(LOWE_EGRID[k]),&djunk,&(LOWE_PIGRID[k]),&djunk);
	    LOWE_EGRID[k]=log10(LOWE_EGRID[k]+THRESHOLD);
	    LOWE_PIGRID[k]=log10(1.e-20*LOWE_PIGRID[k]);
	  }
	  spline_pi(LOWE_EGRID,LOWE_PIGRID,LOWE_GRIDNUM,1.e40,1.e40,LOWE_PIGRID_2);	  
	  if (Nion[element][electron]*pisigma_pi(g_i,p0,p1,p2,p3,THRESHOLD) >= tau_lim) {
	    /* PHOTOIONIZATION OUT OF GRD STATE OF n+1 ION */
	    for (k=1;k<=GRIDNUM;++k) {
	      if (EGRID[k]<log10(THRESHOLD)) PIGRID[k]=1.e-90;
	      else if (EGRID[k]>=log10(THRESHOLD) && EGRID[k]<=LOWE_EGRID[LOWE_GRIDNUM]) PIGRID[k]=lowEpispline_pi(pow(10.,EGRID[k]));
	      else PIGRID[k]=pisigma_pi(g_i,p0,p1,p2,p3,pow(10.,EGRID[k]));
	    }
	    for (k=1;k<=GRIDNUM;++k) PIGRID[k]=log10(PIGRID[k]);
	    spline_pi(EGRID,PIGRID,GRIDNUM,1.e40,1.e40,PIGRID_2);
	    
	    /* calculate opacity of given edge and modify "tau" accordingly */
	    fac_edge_opacity_pi(Nion[element][electron],THRESHOLD,tau_edge);
	  }
	}
	fclose(input);
	
	if (lines) {
	  ext="tr_short"; 
	  if (electron<10) sprintf(temp,"%s0%da.%s\0",root,electron,ext);
	  else sprintf(temp,"%s%da.%s\0",root,electron,ext);
	  input=fopen(temp,"r");
	  while (fscanf(input,"%d%lf%d%lf%lf%lf%lf",&j,&g_j,&i,&g_i,&en,&ftemp,&Atemp) != EOF) {
	    j=j+1;
	    i=i+1;
	    g_i=g_i+1.;
	    g_j=g_j+1.;
	    ftemp=ftemp/g_i;     /* CHECK THIS - VERY IMPORTANT!!! */
	    E0=en;
	    E0=E0*doppler_rad;
	    DELTANUD=sqrt(2.)*sigmav_rad/ccc*(E0*eVtoergs/hhh);
	    if ((E0>=EMIN && E0<=EMAX) && ftemp>tau_lim*FACTOR*DELTANUD/Nion[element][electron]) {
	      OSCILLATOR=ftemp;
	      ga=Atemp/*g_j*Atemp/(3.*g_i*OSCILLATOR)*/;
	      ALPHA=ga/(4.*PI*DELTANUD);
	      line_opacity_pi(Nion[element][electron],E0,OSCILLATOR,ALPHA,DELTANUD,tau_exc);
	    }
	  }
	  fclose(input);
	}
      }
    }
  }


  /* M-shell ions */
  if (verbose) printf("M-shell ions:  Mg through Ni\n");
  for (element=12;element<=28;++element) {
    for (electron=11;electron<=28;++electron) {
      if (Nion[element][electron]) {
	if (element == 12) sprintf(element_name,"Mg");
	if (element == 13) sprintf(element_name,"Al");
	if (element == 14) sprintf(element_name,"Si");
	if (element == 16) sprintf(element_name,"S");
	if (element == 18) sprintf(element_name,"Ar");
	if (element == 20) sprintf(element_name,"Ca");
	if (element == 26) sprintf(element_name,"Fe");
	if (element == 28) sprintf(element_name,"Ni");
	sprintf(root,"%s/photoion_dat/M_shell/%s",DATADIR,element_name);
	if (verbose) printf("Z = %2d   z = %2d\n",element,electron);

	/* Transitions */
	if (lines) {
	  ext="tr_short"; 
	  sprintf(temp,"%s%2da.%s\0",root,electron,ext);
	  input=fopen(temp,"r");
	  while (fscanf(input,"%d%lf%d%lf%lf%lf%lf",&j,&g_j,&i,&g_i,&en,&ftemp,&Atemp) != EOF) {
	    g_j=g_j+1.;
	    g_i=g_i+1.;
	    ftemp=ftemp/g_i;     /* CHECK THIS - VERY IMPORTANT!!! */
	    E0=en;
	    E0=E0*doppler_rad;
	    DELTANUD=sqrt(2.)*sigmav_rad/ccc*(E0*eVtoergs/hhh);
	    if ((E0>=EMIN && E0<=EMAX) && ftemp>tau_lim*FACTOR*DELTANUD/Nion[element][electron]) {
	      OSCILLATOR=ftemp;
	      ga=Atemp/*g_j*Atemp/(3.*g_i*OSCILLATOR)*/;
	      ALPHA=ga/(4.*PI*DELTANUD);
	      line_opacity_pi(Nion[element][electron],E0,OSCILLATOR,ALPHA,DELTANUD,tau_exc);
	    }
	  }
	  fclose(input);
	}

	/* Edges */
	ext="pi_short"; 
	sprintf(temp,"%s%2da.%s\0",root,electron,ext);
	input=fopen(temp,"r");
	while (fscanf(input,"%d%lf%d%lf%lf%lf",&i,&g_i,&j,&g_j,&THRESHOLD,&ANGULAR) != EOF) {
	  g_i=g_i+1.;
	  g_j=g_j+1.;
	  fscanf(input,"%lf%lf%lf%lf",&p0,&p1,&p2,&p3);
	  for (k=1;k<=LOWE_GRIDNUM;++k) {
	    fscanf(input,"%lf%lf%lf%lf",&(LOWE_EGRID[k]),&djunk,&(LOWE_PIGRID[k]),&djunk);
	    LOWE_EGRID[k]=log10(LOWE_EGRID[k]+THRESHOLD);
	    LOWE_PIGRID[k]=log10(1.e-20*LOWE_PIGRID[k]);
	  }
	  spline_pi(LOWE_EGRID,LOWE_PIGRID,LOWE_GRIDNUM,1.e40,1.e40,LOWE_PIGRID_2);	  
	  if (Nion[element][electron]*pisigma_pi(g_i,p0,p1,p2,p3,1.01*THRESHOLD) >= tau_lim) {
	    /* PHOTOIONIZATION OUT OF GRD STATE OF n+1 ION */
	    for (k=1;k<=GRIDNUM;++k) {
	      if (EGRID[k]<log10(THRESHOLD)) PIGRID[k]=1.e-90;
	      else if (EGRID[k]>=log10(THRESHOLD) && EGRID[k]<=LOWE_EGRID[LOWE_GRIDNUM]) PIGRID[k]=lowEpispline_pi(pow(10.,EGRID[k]));
	      else PIGRID[k]=pisigma_pi(g_i,p0,p1,p2,p3,pow(10.,EGRID[k]));
	    }
	    for (k=1;k<=GRIDNUM;++k) PIGRID[k]=log10(PIGRID[k]);
	    spline_pi(EGRID,PIGRID,GRIDNUM,1.e40,1.e40,PIGRID_2);
	    
	    /* calculate opacity of given edge and modify "tau" accordingly */
	    fac_edge_opacity_pi(Nion[element][electron],THRESHOLD,tau_edge);
	  }
	}
	fclose(input);
      }
    }
  }
  

  if (0 && sigmav_rad) {
    if (verbose) printf("Convolving spectrum with appropriate velocity distribution...");
    klo=1;
    khi=SPECBINS;
    for (k=klo;k<=khi;++k) {
      if (k<=DIST) {jlow = 1;      jhigh = k+DIST;} 
      else         {jlow = k-DIST; jhigh = k+DIST;}
      if (jlow<1) jlow=1;
      if (jhigh>SPECBINS) jhigh=SPECBINS;
      for (n=jlow;n<=jhigh;++n) {
	djunk=EBIN/(E_array[k])*gauss_pi(sigmav_rad/ccc,(E_array[n]-E_array[k])/E_array[k])*tau_edge[n];
	tau[k]+=djunk;
      }
    }
    if (verbose) printf("done.\n");
  } else {
    for (k=1;k<=SPECBINS;++k) tau[k]+=tau_edge[k];
  }
  
  for (k=1;k<=SPECBINS;++k) tau[k]+=tau_exc[k];
  
  /**************************************************************************/
  /* Determine integrand term for rate integrals, Labsorb, and abs_spectrum */

  /* Get rid of line excitation depth for high radial velocity width limit */
  if (type==7 || type==8) for (k=1;k<=SPECBINS;++k) tau[k]=tau[k]-tau_exc[k];

  /* Total electron Thomson depth */
  for (k=1;k<=SPECBINS;++k) {
    tau[k]+=N_e*sigmaT;
    tau_exc[k]+=N_e*sigmaT;
  }

  if (INPUT==0) {
    /* Normalize intrinsic power law */
    if (GAMMA != 2.) LNORM=L_X*ergstoeV*(2.-GAMMA)/(pow(L_EMAX,2.-GAMMA)-pow(L_EMIN,2.-GAMMA)); 
    else LNORM=L_X*ergstoeV/log(L_EMAX/L_EMIN);
    for (k=1;k<=SPECBINS;++k) abs_spectrum[k]=L_pi(E_array[k])*exp(-tau[k]);
    if (type==4) { /* Filled cone - rec. cont. (lower limit) */
      for (k=1;k<=SPECBINS;++k)	{
	if (tau[k]>1.e-5) {
	  Labsorb[k]=L_pi(E_array[k])*exp(-tau[k]);
	  abs_spectrum[k]=L_pi(E_array[k])*exp(-tau[k]);
	} else {
	  Labsorb[k]=L_pi(E_array[k]);
	  abs_spectrum[k]=L_pi(E_array[k]);
	}
      }
    } else if (type==5) { /* Filled cone - rec. cont. (upper limit) */
      for (k=1;k<=SPECBINS;++k) {
	if (tau[k]>1.e-5) {
	  Labsorb[k]=L_pi(E_array[k]);
	  abs_spectrum[k]=L_pi(E_array[k])*exp(-tau[k]);
	} else {
	  Labsorb[k]=L_pi(E_array[k]);
	  abs_spectrum[k]=L_pi(E_array[k]);
	}
      }
    } else {
      for (k=1;k<=SPECBINS;++k) {
	if (tau[k]>1.e-5) {
	  Labsorb[k]=L_pi(E_array[k])*(1.-exp(-tau[k]))/tau[k];
	  abs_spectrum[k]=L_pi(E_array[k])*exp(-tau[k]);
	} else {
	  Labsorb[k]=L_pi(E_array[k]);
	  abs_spectrum[k]=L_pi(E_array[k]);
	}
      }
    }
  } else if (INPUT>0) {
    sprintf(temp,"input.qdp");
    input=fopen(temp,"r");
    if (input==NULL) {
      printf("The file 'input.qdp' must exist in this directory.\n");
      return 0;
    }
    for (k=1;k<=3;++k) fgets(line,LENGTH,input);
    /* Get # of data lines in input file */
    INPUT_SIZE=0;
    while (fgets(line,LENGTH,input) != NULL) ++INPUT_SIZE;
    fclose(input);
    E_input=dvector_pi(1,INPUT_SIZE);
    L_input=dvector_pi(1,INPUT_SIZE);
    L_input_2=dvector_pi(1,INPUT_SIZE);
    EtimesL_input=dvector_pi(1,INPUT_SIZE);
    EtimesL_input_2=dvector_pi(1,INPUT_SIZE);
    input=fopen(temp,"r");
    for (k=1;k<=3;++k) fgets(line,LENGTH,input);
    k=1;
    while (fgets(line,LENGTH,input) != NULL) {
      sscanf(line,"%lf%lf%lf",&(E_input[k])/* keV */,&HALFBIN_SIZE/* keV */,&(L_input[k])/* Flux units!!! [ph/cm^2/s/keV] */);
      if (INPUT_SHIFT) {E_input[k]=1000.*(1.+redshift)*E_input[k]; /* to convert from keV to eV */
      } else {E_input[k]=1000.*E_input[k];}
      HALFBIN_SIZE=1000.*HALFBIN_SIZE;
      L_input[k]=4.*PI*sqr(D)*L_input[k]/1000.;
      EtimesL_input[k]=E_input[k]*L_input[k];
      if (k==1) L_EMIN=E_input[1]-HALFBIN_SIZE;
      ++k;
    }
    fclose(input);
    L_EMAX=E_input[INPUT_SIZE]+HALFBIN_SIZE;
    spline_pi(E_input,EtimesL_input,INPUT_SIZE,1.e40,1.e40,EtimesL_input_2);
    LinterpNORM=1.;
    if (L_X>0.) {
      djunk=qromb_pi(EtimesL_pi,1.001*L_EMIN,0.999*L_EMAX);
      LinterpNORM=L_X*ergstoeV/djunk;
      if (verbose) printf("LinterpNORM = %e\n",LinterpNORM);
    }
    spline_pi(E_input,L_input,INPUT_SIZE,1.e40,1.e40,L_input_2);
    for (k=1;k<=SPECBINS;++k) abs_spectrum[k]=Linterp_pi(E_array[k])*exp(-tau[k]);
    if (type==4) { /* Filled cone - rec. cont. (lower limit) */
      for (k=1;k<=SPECBINS;++k)	{
	if (tau[k]>1.e-5) {
	  Labsorb[k]=Linterp_pi(E_array[k])*exp(-tau[k]);
	  abs_spectrum[k]=Linterp_pi(E_array[k])*exp(-tau[k]);
	} else {
	  Labsorb[k]=Linterp_pi(E_array[k]);
	  abs_spectrum[k]=Linterp_pi(E_array[k]);
	}
      }
    } else if (type==5) { /* Filled cone - rec. cont. (upper limit) */
      for (k=1;k<=SPECBINS;++k) {
	if (tau[k]>1.e-5) {
	  Labsorb[k]=Linterp_pi(E_array[k]);
	  abs_spectrum[k]=Linterp_pi(E_array[k])*exp(-tau[k]);
	} else {
	  Labsorb[k]=Linterp_pi(E_array[k]);
	  abs_spectrum[k]=Linterp_pi(E_array[k]);
	}
      }
    } else {
      for (k=1;k<=SPECBINS;++k) {
	if (tau[k]>1.e-5) {
	  Labsorb[k]=Linterp_pi(E_array[k])*(1.-exp(-tau[k]))/tau[k];
	  abs_spectrum[k]=Linterp_pi(E_array[k])*exp(-tau[k]);
	} else {
	  Labsorb[k]=Linterp_pi(E_array[k]);
	  abs_spectrum[k]=Linterp_pi(E_array[k]);
	}
      }
    }
  }

  if (type>1) {
    /**************************************************************************/
    /*                  Determine Emission Line spectrum                      */
    /**************************************************************************/
    if (lines) {
      if (verbose) printf("Determining LOW-n Photoexcitation Rates (n<=5) \n");
      /* hydrogenic */      
      for (i=1;i<=ELEMENTS;++i) {
	electron=1;
	element=list[i];
	if (Nion[element][electron]) {
	  for (LINE=1;LINE<=4;++LINE) {
	    OSCILLATOR=hydrogen[element].f[LINE];
	    if (OSCILLATOR) {
	      E0=1000.*AngstromtokeV/hydrogen[element].lambda[LINE];
	      E0=E0*doppler_rad;
	      ga=hydrogen[element].A[LINE]/*leveldeg[electron][LINE]/(3.*grounddeg[electron]*hydrogen[element].f[LINE])*/;
	      DELTANUD=sqrt(2.)*sigmav_rad/ccc*(E0*eVtoergs/hhh);
	      ALPHA=ga/(4.*PI*DELTANUD);
	      line_limits_pi(Nion[element][electron],E0,OSCILLATOR,ALPHA,DELTANUD,&Elo,&Ehi,&SUMlo,&SUMhi);
	      int_junk=0.;
	      for (k=SUMlo;k<=SUMhi;++k) {
		int_array[k]=excitsigma_pi(E0,OSCILLATOR,DELTANUD,ALPHA,E_array[k])*Labsorb[k];
		int_junk+=EBIN*int_array[k];
	      }
	      /*	    spline_pi(E_array,int_array,SPECBINS,1.e40,1.e40,int_array_2);
			    int_ans=qromb_pi(integrand_pi,1.001*Elo,0.999*Ehi);*/
	      if (1 || int_ans<0.) int_ans=int_junk;
	      H_excite[element][LINE]=f_COVERING*Nion[element][electron]*int_ans;
	      if (H_excite[element][LINE]<0.) H_excite[element][LINE]=f_COVERING*Nion[element][electron]*ratePE;
	      /* add line to Seyfert 2 spectrum */
	      E0=doppler_trans/doppler_rad*E0;
	      k=(int) ((E0-EMIN+0.5*EBIN)/EBIN);
	      if ((E0-EMIN+0.5*EBIN)/EBIN-(double) k >= 0.5) ++k;
	      if (k <= SPECBINS && k >= 1) {
		exc_spectrum[k]+=H_excite[element][LINE]/EBIN;
	      }
	    }
	  }
	}
      }
      /* He-like photoexcitation - low n */
      for (i=1;i<=ELEMENTS;++i) {
	element=list[i];
	electron=2;
	if (Nion[element][electron]) {
	  for (LINE=3;LINE<=6;++LINE) {
	    OSCILLATOR=helium[element].f[LINE];
	    if (OSCILLATOR) {
	      E0=1000.*AngstromtokeV/helium[element].lambda[LINE];
	      E0=E0*doppler_rad;
	      ga=helium[element].A[LINE]/*leveldeg[electron][LINE]/(3.*grounddeg[electron]*helium[element].f[LINE])*/;
	      DELTANUD=sqrt(2.)*sigmav_rad/ccc*(E0*eVtoergs/hhh);
	      ALPHA=ga/(4.*PI*DELTANUD);
	      line_limits_pi(Nion[element][electron],E0,OSCILLATOR,ALPHA,DELTANUD,&Elo,&Ehi,&SUMlo,&SUMhi);
	      int_junk=0.;
	      for (k=SUMlo;k<=SUMhi;++k) {
		int_array[k]=excitsigma_pi(E0,OSCILLATOR,DELTANUD,ALPHA,E_array[k])*Labsorb[k];
		int_junk+=EBIN*int_array[k];
	      }
	      /*	    spline_pi(E_array,int_array,SPECBINS,1.e40,1.e40,int_array_2);
			    int_ans=qromb_pi(integrand_pi,1.001*Elo,0.999*Ehi);*/
	      if (1 || int_ans<0.) int_ans=int_junk;
	      He_excite[element][LINE]=f_COVERING*Nion[element][electron]*int_ans;
	      /* Add line to Seyfert 2 spectrum */
	      E0=doppler_trans/doppler_rad*E0;
	      k=(int) ((E0-EMIN+0.5*EBIN)/EBIN);
	      if ((E0-EMIN+0.5*EBIN)/EBIN-(double) k >= 0.5) ++k;
	      if (k <= SPECBINS && k >= 1) {
		exc_spectrum[k]+=He_excite[element][LINE]/EBIN;
	      }
	    }
	  }
	}
      }  
      
      if (verbose) printf("Determining HIGH-n Photoexcitation Rates (n>5)\n");
      /* Hydrogenic and Heliumlike */
      for (i=1;i<=ELEMENTS;++i) {
	element=list[i];
	for (electron=1;electron<=2;++electron) {
	  if (Nion[element][electron]) {
	    if (electron==1) oscillatornorm=1.6; /* Bethe-Salpeter p. 265 */
	    if (electron==2) oscillatornorm=oshe[element]; /* defined above */
	    for (n=6;n<=HIGHN;++n) {
	      OSCILLATOR=oscillatornorm/cube((double) n);
	      E0=AngstromtokeV/highn[element][electron].lambda[n]*1000.;
	      E0=E0*doppler_rad;
	      if (E0>=EMIN && E0<=EMAX) {
		DELTANUD=sqrt(2.)*sigmav_rad/ccc*(E0*eVtoergs/hhh);
		/* taking classical value for "ga=gamma" from p. 112-114 B+D */
		ga=2.47*pow(10.,-22.)*sqr(E0*eVtoergs/hhh);
		ALPHA=ga/(4.*PI*DELTANUD);
		line_limits_pi(Nion[element][electron],E0,OSCILLATOR,ALPHA,DELTANUD,&Elo,&Ehi,&SUMlo,&SUMhi);
		int_junk=0.;
		for (k=SUMlo;k<=SUMhi;++k) {
		  int_array[k]=excitsigma_pi(E0,OSCILLATOR,DELTANUD,ALPHA,E_array[k])*Labsorb[k];
		  int_junk+=EBIN*int_array[k];
		}
		/*	      spline_pi(E_array,int_array,SPECBINS,1.e40,1.e40,int_array_2);
			      int_ans=qromb_pi(integrand_pi,1.001*Elo,0.999*Ehi);*/
		if (1 || int_ans<0.) int_ans=int_junk;
		strength=f_COVERING*Nion[element][electron]*int_ans;
		/* add line to seyfert 2 like spectrum */
		E0=doppler_trans/doppler_rad*E0;
		k=(int) ((E0-EMIN+0.5*EBIN)/EBIN);
		if ((E0-EMIN+0.5*EBIN)/EBIN-(double) k >= 0.5) ++k;
		if (k <= SPECBINS && k >= 1) {
		  exc_spectrum[k]+=strength/EBIN;
		}
	      }
	    }
	  }
	}
      }
    }    

    /* Calculate Thomson scattering of spectrum */
    if (N_e) {
      for (k=1;k<=SPECBINS;++k) exc_spectrum[k]+=f_COVERING*N_e*sigmaT*Labsorb[k];
    }
    
    
    /* File root for all L-shell input files */
    for (element=10;element<=28;++element) {
      for (electron=3;electron<=10;++electron) {
	/* line contributions */
	if (Nion[element][electron]) {
	  if (verbose) printf("%2d %2d Recombination ...\n",element,electron);
	  if (element == 10) sprintf(element_name,"Ne");
	  if (element == 12) sprintf(element_name,"Mg");
	  if (element == 13) sprintf(element_name,"Al");
	  if (element == 14) sprintf(element_name,"Si");
	  if (element == 16) sprintf(element_name,"S");
	  if (element == 18) sprintf(element_name,"Ar");
	  if (element == 20) sprintf(element_name,"Ca");
	  if (element == 26) sprintf(element_name,"Fe");
	  if (element == 28) sprintf(element_name,"Ni");

	  sprintf(root,"%s/photoion_dat/L_shell/trates%s",DATADIR,element_name);
	  ext="dat";
	  sprintf(temp,"%s.%s\0",root,ext);
	  input=fopen(temp,"r");
	  /* Read in RR and DR contributions */
	  if (verbose) printf("Read in total RR and DR rates for %2d %2d\n",element,electron);
	  for (k=1;k<=12+(electron-3)*10;++k) fgets(line,LENGTH,input);
	  for (k=1;k<=RECNUM;++k) {
	    fscanf(input,"%d%lf%lf%lf",&ijunk,&Ttemp,&RRtemp,&DRtemp);
	    L_kT[k]=Ttemp;
	    L_RR[k]=1.e-10*RRtemp;
	    L_DR[k]=1.e-10*DRtemp;
	    L_REC[k]=L_RR[k]+L_DR[k];
	  }
	  fclose(input);

	  spline_pi(L_kT,L_RR,RECNUM,1.e40,1.e40,L_RR_2);
	  spline_pi(L_kT,L_DR,RECNUM,1.e40,1.e40,L_DR_2);
	  spline_pi(L_kT,L_REC,RECNUM,1.e40,1.e40,L_REC_2);
	  
	  kT=Tion[element][electron];
	  L_RR_kT=L_RR_spline_pi(kT);
	  L_DR_kT=L_DR_spline_pi(kT);
	  L_REC_kT=L_REC_spline_pi(kT);
	  
	  if (verbose) printf("PE rates %2d %2d ...\n",element,electron);
	  sprintf(root,"%s/photoion_dat/L_shell/",DATADIR);
	  ext="tr_shorter"; 
	  if (electron<10) sprintf(temp,"%s%s0%da.%s\0",root,element_name,electron,ext);
	  else sprintf(temp,"%s%s%2da.%s\0",root,element_name,electron,ext);
	  input=fopen(temp,"r");
	  while (fgets(line,LENGTH,input) != NULL) {
	    sprintf(sjunk,"\0");
	    sscanf(line,"%d%d%d%d%lf%lf%lf",&j,&jjunk,&i,&ijunk,&en,&ftemp,&Atemp);
	    /*    sscanf(line,"%d%d%d%d%lf%lf%lf%lf%lf",&j,&jjunk,&i,&ijunk,&en,&ftemp,&Atemp,&DECAYRATE,&AIRATE);*/
	    j=j+1;
	    i=i+1;
	    g_i=((double) i)+1.;  /* deg=2J */
	    g_j=((double) j)+1.;  /* deg=2J */
	    ftemp=ftemp/g_i;     /* CHECK THIS - VERY IMPORTANT!!! */
	    E0=en;
	    E0=E0*doppler_rad;
	    DELTANUD=sqrt(2.)*sigmav_rad/ccc*(E0*eVtoergs/hhh);
	    if ((E0>=EMIN && E0<=EMAX) && ftemp>tau_lim*FACTOR*DELTANUD/Nion[element][electron]) {
	      /*	    printf("%5d %2d %5d %2d %e %e %e\n",j,jjunk,i,ijunk,en,ftemp,Atemp);*/
	      OSCILLATOR=ftemp;
	      ga=Atemp/*g_j*Atemp/(3.*g_i*ftemp)*/;
	      ALPHA=ga/(4.*PI*DELTANUD);
	      line_limits_pi(Nion[element][electron],E0,OSCILLATOR,ALPHA,DELTANUD,&Elo,&Ehi,&SUMlo,&SUMhi);
	      int_junk=0.;
	      for (k=SUMlo;k<=SUMhi;++k) {
		int_array[k]=f_COVERING*Nion[element][electron]*excitsigma_pi(E0,OSCILLATOR,DELTANUD,ALPHA,E_array[k])*Labsorb[k];
		int_junk+=EBIN*int_array[k];
	      }      
	      /*	      spline_pi(E_array,int_array,SPECBINS,1.e40,1.e40,int_array_2);
			      int_ans=qromb_pi(integrand_pi,1.001*Elo,0.999*Ehi);*/
	      if (1 || int_ans<0.) int_ans=int_junk;
	      ratePE=int_ans;
	      /* *Atemp/(DECAYRATE+AIRATE);
	       ratePI[element][electron]+=djunk*AIRATE/(DECAYRATE+AIRATE);*/
	      /*	      printf("line: PI[%2d][%2d] = %e    %e\n",element,electron,djunk*AIRATE/(DECAYRATE+AIRATE),E0);*/
	      /* Need to add line emission to overall spectrum */
	      E0=doppler_trans/doppler_rad*E0;
	      k=(int) ((E0-EMIN+0.5*EBIN)/EBIN);
	      if ((E0-EMIN+0.5*EBIN)/EBIN-(double) k >= 0.5) ++k;
	      if (k <= SPECBINS && k >= 1)
		exc_spectrum[k]+=ratePE/EBIN;
	    }
	  }
	  fclose(input);
	  
	  if (verbose) printf("PI rates %2d %2d ... \n",element,electron);
	  ext="pi_short"; 
	  if (electron<10) sprintf(temp,"%s%s0%da.%s\0",root,element_name,electron,ext);
	  else  sprintf(temp,"%s%s%2da.%s\0",root,element_name,electron,ext);
	  input=fopen(temp,"r");
	  while (fscanf(input,"%d%lf%d%lf%lf%lf",&i,&g_i,&j,&g_j,&THRESHOLD,&ANGULAR) != EOF) {
	    g_i=g_i+1.;
	    g_j=g_j+1.;
	    fscanf(input,"%lf%lf%lf%lf",&p0,&p1,&p2,&p3);
	    for (k=1;k<=LOWE_GRIDNUM;++k) {
	      fscanf(input,"%lf%lf%lf%lf",&(LOWE_EGRID[k]),&djunk,&(LOWE_PIGRID[k]),&djunk);
	      LOWE_EGRID[k]=log10(LOWE_EGRID[k]+THRESHOLD);
	      LOWE_PIGRID[k]=log10(1.e-20*LOWE_PIGRID[k]);
	    }
	    /* PHOTOIONIZATION OUT OF GRD STATE OF n+1 ION */
	    spline_pi(LOWE_EGRID,LOWE_PIGRID,LOWE_GRIDNUM,1.e40,1.e40,LOWE_PIGRID_2);	  
	    if ((THRESHOLD>=EMIN/doppler_rad && THRESHOLD<=EMAX/doppler_rad) && pisigma_pi(g_i,p0,p1,p2,p3,THRESHOLD) >= 0. /* This one should be left at zero????*/) {
	      for (k=1;k<=GRIDNUM;++k) {
		if (EGRID[k]<log10(THRESHOLD)) PIGRID[k]=1.e-90;
		else if (EGRID[k]>=log10(THRESHOLD) && EGRID[k]<=LOWE_EGRID[LOWE_GRIDNUM]) PIGRID[k]=lowEpispline_pi(pow(10.,EGRID[k]));
		else PIGRID[k]=pisigma_pi(g_i,p0,p1,p2,p3,pow(10.,EGRID[k]));
	      }
	      for (k=1;k<=GRIDNUM;++k) PIGRID[k]=log10(PIGRID[k]);
	      spline_pi(EGRID,PIGRID,GRIDNUM,1.e40,1.e40,PIGRID_2);
	      
	      /* calculate PI rate and modify ratePI[element][electron] */
	      djunk=Nion[element][electron]*fac_PI_rate_integral_pi(THRESHOLD,Labsorb);
	      ratePI[element][electron]+=djunk;
	      /*printf("edge: PI[%2d][%2d]=%e    %e\n",element,electron,djunk,THRESHOLD*doppler_rad);*/
	    }
	  }
	  fclose(input);
	  /*	  if (verbose) printf("%e (PI)\n",ratePI[element][electron]);*/

	  EMion[element][electron]=ratePI[element][electron]/L_REC_kT;
	  EM[element][electron]=1.2/ABUND[element]*EMion[element][electron];

	  ext="dat"; 
	  if (electron<10) sprintf(temp,"%s%s0%d.%s\0",root,element_name,electron,ext);
	  else  sprintf(temp,"%s%s%2d.%s\0",root,element_name,electron,ext);
	  input=fopen(temp,"r");
	  ext="rr_short"; 
	  if (electron<10) sprintf(temp,"%s%s0%da.%s\0",root,element_name,electron,ext);
	  else  sprintf(temp,"%s%s%2da.%s\0",root,element_name,electron,ext);
	  /*	  for (k=1;k<=17;++k) fgets(line,LENGTH,input);*/
	  while (fgets(line,LENGTH,input) != NULL) {  
	    sscanf(line,"%d%lf%d%d%d%lf%lf%lf%lf",&ijunk,&(L_kT[1]),&typenum,&itemp,&jtemp,&en,&lambda,&(RR_line[1]),&(DR_line[1]));
	    for (k=2;k<=10;++k) {
	      fgets(line,LENGTH,input);
	      sscanf(line,"%d%lf%d%d%d%lf%lf%lf%lf",&ijunk,&(L_kT[k]),&typenum,&itemp,&jtemp,&en,&lambda,&(RR_line[k]),&(DR_line[k]));
	    }
	    /*    itemp=itemp+1;
		  jtemp=jtemp+1;*/
	    for (k=1;k<=10;++k) {
	      RR_line[k]*=1.e-10;
	      DR_line[k]*=1.e-10;
	    }
	    spline_pi(L_kT,DR_line,RECNUM,1.e40,1.e40,DR_line_2);	
	    spline_pi(L_kT,RR_line,RECNUM,1.e40,1.e40,RR_line_2);	
	    if (typenum<100) {
	      /*read in RRC contributions */
	      input2=fopen(temp,"r");
	      while (fscanf(input2,"%d%lf%d%lf%lf%lf",&i,&g_i,&j,&g_j,&THRESHOLD,&ANGULAR)!=EOF) {
		g_i=g_i+1.;
		g_j=g_j+1.;
		fscanf(input2,"%lf%lf%lf%lf",&p0,&p1,&p2,&p3);
		for (k=1;k<=LOWE_GRIDNUM;++k) {
		  fscanf(input2,"%lf%lf%lf%lf",&(LOWE_EGRID[k]),&(LOWE_RRGRID[k]),&djunk,&djunk);
		  LOWE_EGRID[k]=log10(LOWE_EGRID[k]/doppler_trans);
		  LOWE_RRGRID[k]=log10(1.e-20*LOWE_RRGRID[k]);
		}
		
		if (i==itemp && j==jtemp) {
		  spline_pi(LOWE_EGRID,LOWE_RRGRID,LOWE_GRIDNUM,1.e40,1.e40,LOWE_RRGRID_2);	  
		  /*		  printf("%5d %5d  %5d %5d  %e\n",i,itemp,j,jtemp,rrsigma_pi(g_i,g_j,p0,p1,p2,p3,THRESHOLD)); */
		  if (THRESHOLD>=EMIN/doppler_trans && THRESHOLD<=EMAX/doppler_trans) {
		    for (k=1;k<=GRIDNUM;++k) RRGRID[k]=1.e-90;
		    for (k=1;k<=GRIDNUM;++k) {
		      Etemp=EGRID[k];
		      if (Etemp<log10(THRESHOLD)) RRGRID[k]=1.e-90;
		      else if (Etemp>=log10(THRESHOLD) && Etemp<=LOWE_EGRID[LOWE_GRIDNUM]) RRGRID[k]=lowEpispline_pi(pow(10.,Etemp));
		      else RRGRID[k]=rrsigma_pi(g_i,g_j,p0,p1,p2,p3,pow(10.,Etemp)-THRESHOLD/* electron energy */);
		    }
		    for (k=1;k<=GRIDNUM;++k) RRGRID[k]=log10(RRGRID[k]);
		    spline_pi(EGRID,RRGRID,GRIDNUM,1.e40,1.e40,RRGRID_2);
		    int_junk=0.;
		    for (k=1;k<=SPECBINS;++k) {
		      int_array[k]=fac_recombination_pi(g_i,g_j,p0,p1,p2,p3,kT,E_array[k]/doppler_trans-THRESHOLD);
		      int_junk+=EBIN*int_array[k];
		    }
		    /* CHANGE HERE */
		    intMIN=1.00001*THRESHOLD*doppler_trans;
		    intMAX=100.*THRESHOLD*doppler_trans;
		    if (intMAX>EMAX) intMAX=EMAX;
		    /*		    spline_pi(E_array,int_array,SPECBINS,1.e40,1.e40,int_array_2);
				    int_ans=qromb_pi(integrand_pi,intMIN,intMAX);*/
		    if (1 || int_ans<0.) int_ans=int_junk;
		    RECNORM=int_ans;
		    rateRR[element][electron]=ratePI[element][electron]*RR_line_spline_pi(kT)/L_REC_kT;		  
		    for (k=1;k<=SPECBINS;++k) {
		      djunk=rateRR[element][electron]*fac_recombination_pi(g_i,g_j,p0,p1,p2,p3,kT,E_array[k]/doppler_trans-THRESHOLD)/RECNORM;
		      specRR[k]+=djunk;
		      rec_spectrum[k]+=djunk;
		    }
		  }
		}
	      }
	      fclose(input2);
	    } else if (typenum>=100 && en>=EMIN/doppler_trans && en<=EMAX/doppler_trans) {
	      /*read in line contributions */
	      E0=en;
	      E0=doppler_trans*E0;
	      rateRR[element][electron]=ratePI[element][electron]*RR_line_spline_pi(kT)/L_REC_kT;
	      rateDR[element][electron]=ratePI[element][electron]*DR_line_spline_pi(kT)/L_REC_kT;
	      k=(int) ((E0-EMIN+0.5*EBIN)/EBIN);
	      if ((E0-EMIN+0.5*EBIN)/EBIN-(double) k >= 0.5) ++k;
	      if (k <= SPECBINS && k >= 1) {
		specRR[k]+=rateRR[element][electron]/EBIN;
		specDR[k]+=rateDR[element][electron]/EBIN;
		rec_spectrum[k]+=specRR[k]+specDR[k];
	      }
	    }
	  }
	  fclose(input);
	}
      }
    }

    if (verbose) printf("Determining Photoionization Rates for H- and He-like...\n");
    /* Photoionization Rates */
    for (i=1;i<=ELEMENTS;++i) {
      element=list[i];
      for (electron=1;electron<=2;++electron) {
	if (Nion[element][electron]) {
	  THRESHOLD=vernerionizsigma[element][electron].Eth;
	  int_junk=0.;
	  for (k=1;k<=SPECBINS;++k) {
	    int_array[k]=vernerph_pi(vernerionizsigma[element][electron],E_array[k])*Labsorb[k];
	    int_junk+=EBIN*int_array[k];
	  }
	  /*	  spline_pi(E_array,int_array,SPECBINS,1.e40,1.e40,int_array_2);
		  int_ans=qromb_pi(integrand_pi,1.001*THRESHOLD*doppler_rad,0.999*EMAX);*/
	  if (1 || int_ans<0.) int_ans=int_junk;
	  ratePI[element][electron]=f_COVERING*Nion[element][electron]*int_ans;
	}
      }
    }
    
    if (verbose) printf("Determining Recombination Line/RRC Strengths & EM's ...\n");
    if (verbose) printf("H-like...\n");
    sprintf(H_recfile_name,"%s/photoion_dat/H_recombination.dat",DATADIR);
    H_recfile=fopen(H_recfile_name,"r");
    k=1;
    while (fscanf(H_recfile,"%d%lf%lf%lf%lf%lf%lf%lf%lf",&element,&Ttemp,&atemp,&btemp,&ctemp,&dtemp,&etemp,&rrctemp,&Ctemp)!=EOF) {
      H_rec[element].T[k]=Ttemp;
      H_rec[element].lines[1][k]=atemp;
      H_rec[element].lines[2][k]=btemp;
      H_rec[element].lines[3][k]=ctemp;
      H_rec[element].lines[4][k]=dtemp;
      H_rec[element].lines[5][k]=etemp;
      H_rec[element].rrc[k]=rrctemp;
      H_rec[element].C[k]=1.e-10*Ctemp;
      ++k;
      if (k==TEMPERATURES+1) k=1;
    }
    for (i=1;i<=ELEMENTS;++i) {
      element=list[i];
      electron=1;
      if (Nion[element][electron]) {
	R=ratePI[element][electron];
	for (k=1;k<=TEMPERATURES;++k) {
	  Tvec[k]=log10(H_rec[element].T[k]);
	}
	/* Now calculate each line strength for given ion kT */
	for (LINE=1;LINE<=5;++LINE) {
	  for (k=1;k<=TEMPERATURES;++k) {
	    Yvec[k]=log10(H_rec[element].lines[LINE][k]);
	  }
	  spline_pi(Tvec,Yvec,TEMPERATURES,1.e40,1.e40,Yvec2);
	  strength=pow(10.,loglinestrength_pi(log10(Tion[element][electron])));
	  E0=AngstromtokeV/hydrogen[element].lambda[LINE]*1000.;
	  E0=doppler_trans*E0;
	  k=(int) ((E0-EMIN+0.5*EBIN)/EBIN);
	  if ((E0-EMIN+0.5*EBIN)/EBIN-(double) k >= 0.5) ++k;
	  if (k >= 1 && k <= SPECBINS) rec_spectrum[k]+=R*strength/EBIN;
	}
	/* RRC strength */
	kT=Tion[element][electron];
	for (k=1;k<=TEMPERATURES;++k) Yvec[k]=log10(H_rec[element].rrc[k]);
	spline_pi(Tvec,Yvec,TEMPERATURES,1.e40,1.e40,Yvec2);
	strength=pow(10.,loglinestrength_pi(log10(Tion[element][electron])));
	E0=AngstromtokeV/hydrogen[element].lambda[6/*rrc*/]*1000.;
	E0=doppler_trans*E0;
	/* normalize "recombination" */
	int_junk=0.;
	for (k=1;k<=SPECBINS;++k) {
	  int_array[k]=verner_recombination_pi(vernerionizsigma[element][electron],kT,E_array[k]/doppler_trans);
	  int_junk+=EBIN*int_array[k];
	}
	THRESHOLD=vernerionizsigma[element][electron].Eth;
	/* CHANGE HERE */
	intMIN=1.00001*THRESHOLD*doppler_trans;
	intMAX=100.*THRESHOLD*doppler_trans;
	if (intMAX>EMAX) intMAX=EMAX;
	/*spline_pi(E_array,int_array,SPECBINS,1.e40,1.e40,int_array_2);
		int_ans=qromb_pi(integrand_pi,1.001*EMIN,0.999*EMAX);*/
	if (1 || int_ans<0.) int_ans=int_junk;
	RECNORM=int_ans;
	for (k=1;k<=SPECBINS;++k) {
	  rec_spectrum[k]+=R*strength/RECNORM*verner_recombination_pi(vernerionizsigma[element][electron],kT,E_array[k]/doppler_trans);
	}
	/* C-coefficient magnitude */
	for (k=1;k<=TEMPERATURES;++k) {
	  Yvec[k]=log10(H_rec[element].C[k]);
	}
	spline_pi(Tvec,Yvec,TEMPERATURES,1.e40,1.e40,Yvec2);
	strength=pow(10.,loglinestrength_pi(log10(Tion[element][electron])));

	EMion[element][electron]=R/strength;
	EM[element][electron]=1.2/ABUND[element]*EMion[element][electron];
      }
    }
    fclose(H_recfile); free(H_recfile_name);
    
    /* Heliumlike */
    if (verbose) printf("He-like...\n");
    sprintf(He_recfile_name,"%s/photoion_dat/He_recombination.dat",DATADIR);
    He_recfile=fopen(He_recfile_name,"r");
    k=1;
    while (fscanf(He_recfile,"%d%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf",&element,&Ttemp,&ftemp,&intertemp,&rtemp,&btemp,&ctemp,&dtemp,&etemp,&rrctemp,&Ctemp)!=EOF) {
      He_rec[element].T[k]=Ttemp;
      He_rec[element].lines[1][k]=ftemp;
      He_rec[element].lines[2][k]=intertemp;
      He_rec[element].lines[3][k]=rtemp;
      He_rec[element].lines[4][k]=btemp;
      He_rec[element].lines[5][k]=ctemp;
      He_rec[element].lines[6][k]=dtemp;
      He_rec[element].lines[7][k]=etemp;
      He_rec[element].rrc[k]=rrctemp;
      He_rec[element].C[k]=1.e-10*Ctemp;
      ++k;
      if (k==TEMPERATURES+1) k=1;
    }
    for (i=1;i<=ELEMENTS;++i) {
      element=list[i];
      electron=2;
      if (Nion[element][electron]) {
	R=ratePI[element][electron];
	for (k=1;k<=TEMPERATURES;++k) {
	  Tvec[k]=log10(He_rec[element].T[k]);
	}
	/* Now calculate each line strength for given ion kT */
	for (LINE=1;LINE<=7;++LINE) {
	  for (k=1;k<=TEMPERATURES;++k) {
	    Yvec[k]=log10(He_rec[element].lines[LINE][k]);
	  }
	  spline_pi(Tvec,Yvec,TEMPERATURES,1.e40,1.e40,Yvec2);
	  strength=pow(10.,loglinestrength_pi(log10(Tion[element][electron])));
	  E0=AngstromtokeV/helium[element].lambda[LINE]*1000.;
	  E0=doppler_trans*E0;
	  k=(int) ((E0-EMIN+0.5*EBIN)/EBIN);
	  if ((E0-EMIN+0.5*EBIN)/EBIN-(double) k >= 0.5) ++k;
	  if (k <= SPECBINS && k >= 1) rec_spectrum[k]+=R*strength/EBIN;
	}
	/* RRC strength */
	kT=Tion[element][electron];
	for (k=1;k<=TEMPERATURES;++k) {
	  Yvec[k]=log10(He_rec[element].rrc[k]);
	}
	spline_pi(Tvec,Yvec,TEMPERATURES,1.e40,1.e40,Yvec2);
	strength=pow(10.,loglinestrength_pi(log10(Tion[element][electron])));
	E0=AngstromtokeV/helium[element].lambda[9/*rrc*/]*1000.;
	E0=doppler_trans*E0;
	/* normalize "recombination" */
	int_junk=0.;
	for (k=1;k<=SPECBINS;++k) {
	  int_array[k]=verner_recombination_pi(vernerionizsigma[element][electron],kT,E_array[k]/doppler_trans);
	  int_junk+=EBIN*int_array[k];
	}
	THRESHOLD=vernerionizsigma[element][electron].Eth;
	/* CHANGE HERE */
	intMIN=1.00001*THRESHOLD*doppler_trans;
	intMAX=100.*THRESHOLD*doppler_trans;
	if (intMAX>EMAX) intMAX=EMAX;
	/*	spline_pi(E_array,int_array,SPECBINS,1.e40,1.e40,int_array_2);
		int_ans=qromb_pi(integrand_pi,intMIN,intMAX);*/
	if (1 || int_ans<0.) int_ans=int_junk;
	RECNORM=int_ans;
	for (k=1;k<=SPECBINS;++k) {
	  rec_spectrum[k]+=R*strength/RECNORM*verner_recombination_pi(vernerionizsigma[element][electron],kT,E_array[k]/doppler_trans);
	}
	/* C-coefficient magnitude */
	for (k=1;k<=TEMPERATURES;++k) {
	  Yvec[k]=log10(He_rec[element].C[k]);
	}
	spline_pi(Tvec,Yvec,TEMPERATURES,1.e40,1.e40,Yvec2);
	strength=pow(10.,loglinestrength_pi(log10(Tion[element][electron])));

	EMion[element][electron]=R/strength;
	EM[element][electron]=1.2/ABUND[element]*EMion[element][electron];
      }
    }
    fclose(He_recfile);  free(He_recfile_name);
  }

  if (type>1 && sigmav_trans) {
    if (verbose) printf("Convolving spectrum with appropriate velocity distribution.\n");
    DIST=(int) (4.*sigmav_trans/ccc*EMAX/EBIN); /*  +/-4 sigma */
    for (k=1;k<=SPECBINS;++k) {
    /* radiative decay after photoexcitation */
      spectrumtemp[k]=exc_spectrum[k]; 
      exc_spectrum[k]=0.;
    /* recombination */
      rec_spectrum_temp[k]=rec_spectrum[k];
      rec_spectrum[k]=0.;
      specRR_temp[k]=specRR[k];
      specRR[k]=0.;
      specDR_temp[k]=specDR[k];
      specDR[k]=0.;
    }
    klo=1;
    khi=SPECBINS;
    for (k=klo;k<=khi;++k) {
      if (k<=DIST) {jlow = 1;      jhigh = k+DIST;} 
      else         {jlow = k-DIST; jhigh = k+DIST;}
      if (jlow<1) jlow=1;
      if (jhigh>SPECBINS) jhigh=SPECBINS;
      for (n=jlow;n<=jhigh;++n) {
	/* radiative decay after photoexcitation */
	spectemp=EBIN/(E_array[k])*gauss_pi(sigmav_trans/ccc,(E_array[n]-E_array[k])/E_array[k])*spectrumtemp[n];
	exc_spectrum[k]+=spectemp;
	/* recombination */
	djunk=EBIN/(E_array[k])*gauss_pi(sigmav_trans/ccc,(E_array[n]-E_array[k])/E_array[k]);
	rec_spectrum[k]+=djunk*rec_spectrum_temp[n];
	specRR[k]+=djunk*specRR_temp[n];
	specDR[k]+=djunk*specDR_temp[n];
      }
    }
  }

  if (verbose) printf("Calculating final spectrum...\n");
  /* Output different spectral "types" */
  /* FLUXAVE is the factor times the observed continuum flux that yields the "average flux"; this modifies Sy2-echo contribution accordingly */
  for (k=1;k<=SPECBINS;++k) {
    /* Seyfert 1 - pure absorption */
    type1_spectrum[k]=abs_spectrum[k];
    /* Seyfert 2 */
    type2_spectrum[k]=FLUXAVE*rec_spectrum[k]+FLUXAVE*exc_spectrum[k];
    /* Pure recombination */
    type3_spectrum[k]=FLUXAVE*rec_spectrum[k];
    /* Seyfert 1 + reemission (lower limit = 4, upper limit = 5) */
    if (tau[k]>1.e-6) {
      type4_spectrum[k]=abs_spectrum[k]+f_COVERING*FLUXAVE*abs_spectrum[k]*tau_exc[k]+FLUXAVE*rec_spectrum[k]*(1.-exp(-tau[k]))/tau[k];
      type5_spectrum[k]=abs_spectrum[k]+f_COVERING*FLUXAVE*abs_spectrum[k]*tau_exc[k]+FLUXAVE*rec_spectrum[k]*(1.-exp(-tau[k]))/tau[k];
    } else {
      type4_spectrum[k]=abs_spectrum[k]+f_COVERING*FLUXAVE*abs_spectrum[k]*tau_exc[k]+FLUXAVE*rec_spectrum[k];
      type5_spectrum[k]=abs_spectrum[k]+f_COVERING*FLUXAVE*abs_spectrum[k]*tau_exc[k]+FLUXAVE*rec_spectrum[k];
    }
    if (INPUT==0) {
      type6_spectrum[k]=L_pi(E_array[k])+FLUXAVE*type2_spectrum[k];
    }
    else if (INPUT>0) {
      type6_spectrum[k]=Linterp_pi(E_array[k])+FLUXAVE*type2_spectrum[k];
    }
  }
  /* Sy1 - Pure Absorption */
  if (type<=1) for (k=1;k<=SPECBINS;++k) E_spectrum[k]=type1_spectrum[k];
  /* Sy2 - Photoionization and Photoexcitation */
  if (type==2) for (k=1;k<=SPECBINS;++k) E_spectrum[k]=type2_spectrum[k];
  /* Sy2 - Pure Recombination */
  if (type==3) for (k=1;k<=SPECBINS;++k) E_spectrum[k]=type3_spectrum[k];
  /* Sy1 - Patchy cone */
  if (type==4) for (k=1;k<=SPECBINS;++k) E_spectrum[k]=type4_spectrum[k];
  /* Sy1 - Filled cone */
  if (type==5) for (k=1;k<=SPECBINS;++k) E_spectrum[k]=type5_spectrum[k];
  /* CV - Sy2, except with unobscured intrinsic continuum */
  if (type==6) for (k=1;k<=SPECBINS;++k) E_spectrum[k]=type6_spectrum[k];
  /* CV - Sy2, completely unsaturated lines + unobscured intrinsic continuum */
  if (type==7) for (k=1;k<=SPECBINS;++k) E_spectrum[k]=type6_spectrum[k];
  /* CV - Sy2, completely unsaturated lines + no intrinsic continuum */
  if (type==8) for (k=1;k<=SPECBINS;++k) E_spectrum[k]=type2_spectrum[k];
  if (fileincr >= 0) {
    sprintf(specfile_name,"l_output_%d.qdp\0",fileincr);
    l_output=fopen(specfile_name,"w");
    sprintf(specfile_name,"E_output_%d.qdp\0",fileincr);
    E_output=fopen(specfile_name,"w");
    sprintf(specfile_name,"E_spectrum_%d.qdp\0",fileincr);
    E_specfile=fopen(specfile_name,"w");
    sprintf(specfile_name,"l_spectrum_%d.qdp\0",fileincr);
    l_specfile=fopen(specfile_name,"w");
    fprintf(E_specfile,"\n\n\n");
    fprintf(l_specfile,"\n\n\n");
  }
  for (k=1;k<=SPECBINS;++k)  {
    /* Convert object rest-frame luminosity to observed flux using redshift */
    E_spectrum[k]=E_spectrum[k]/4./PI/sqr(D)/(1.+redshift);
    type1_spectrum[k]=type1_spectrum[k]/4./PI/sqr(D)/(1.+redshift);
    type2_spectrum[k]=type2_spectrum[k]/4./PI/sqr(D)/(1.+redshift);
    type3_spectrum[k]=type3_spectrum[k]/4./PI/sqr(D)/(1.+redshift);
    type4_spectrum[k]=type4_spectrum[k]/4./PI/sqr(D)/(1.+redshift);
    type5_spectrum[k]=type5_spectrum[k]/4./PI/sqr(D)/(1.+redshift);
    type6_spectrum[k]=type6_spectrum[k]/4./PI/sqr(D)/(1.+redshift);
    type7_spectrum[k]=type7_spectrum[k]/4./PI/sqr(D)/(1.+redshift);
    type8_spectrum[k]=type8_spectrum[k]/4./PI/sqr(D)/(1.+redshift);
    exc_spectrum[k]=exc_spectrum[k]/4./PI/sqr(D)/(1.+redshift);
    specRR[k]=specRR[k]/4./PI/sqr(D)/(1.+redshift);
    specDR[k]=specDR[k]/4./PI/sqr(D)/(1.+redshift);
    convert[k]=(hhh*ccc*ergstoeV)/(sqr(l_array[k]))*1.e8;
    l_spectrum[k]=E_spectrum[k]*convert[k];
    if (fileincr >= 0) {
      if (INPUT==0) {
	fprintf(E_output,"%e   %e  %e  %e  %e  %e  %e  %e  %e  %e  %e  %e\n",E_array[k]/(1.+redshift),tau[k],L_pi(E_array[k])/4./PI/sqr(D),type1_spectrum[k],type2_spectrum[k],type3_spectrum[k],type4_spectrum[k],type5_spectrum[k],type6_spectrum[k],exc_spectrum[k],specRR[k],specDR[k]);
	fprintf(l_output,"%e   %e  %e  %e  %e  %e  %e  %e  %e  %e  %e  %e\n",(1.+redshift)*l_array[k],tau[k],L_pi(E_array[k])*convert[k]/4./PI/sqr(D),type1_spectrum[k]*convert[k],type2_spectrum[k]*convert[k],type3_spectrum[k]*convert[k],type4_spectrum[k]*convert[k],type5_spectrum[k]*convert[k],type6_spectrum[k]*convert[k],exc_spectrum[k]*convert[k],specRR[k]*convert[k],specDR[k]*convert[k]);
      } else if (INPUT>0) {
	fprintf(E_output,"%e   %e  %e  %e  %e  %e  %e  %e  %e  %e  %e  %e\n",E_array[k]/(1.+redshift),tau[k],Linterp_pi(E_array[k])/4./PI/sqr(D),type1_spectrum[k],type2_spectrum[k],type3_spectrum[k],type4_spectrum[k],type5_spectrum[k],type6_spectrum[k],exc_spectrum[k]/4./PI/sqr(D),specRR[k],specDR[k]);
	fprintf(l_output,"%e   %e  %e  %e  %e  %e  %e  %e  %e  %e  %e  %e\n",(1.+redshift)*l_array[k],tau[k],Linterp_pi(E_array[k])*convert[k]/4./PI/sqr(D),type1_spectrum[k]*convert[k],type2_spectrum[k]*convert[k],type3_spectrum[k]*convert[k],type4_spectrum[k]*convert[k],type5_spectrum[k]*convert[k],type6_spectrum[k]*convert[k],exc_spectrum[k]*convert[k]/4./PI/sqr(D),specRR[k]*convert[k],specDR[k]*convert[k]);
      }
      fprintf(E_specfile,"%e %e %e\n",E_array[k]/(1.+redshift)/1000.,EBIN/2./1000.,1000.*E_spectrum[k]);
      if (k==1) {    /* to avoid nonsense for first bin size */
	fprintf(l_specfile,"%e %e %e\n",(1.+redshift)*l_array[k],(l_array[k]-l_array[k+1])/2.,l_spectrum[k]);
      } else {
	fprintf(l_specfile,"%e %e %e\n",(1.+redshift)*l_array[k],(l_array[k-1]-l_array[k])/2.,l_spectrum[k]);
      }
    }
  }
  if (fileincr >= 0) {
    fclose(E_output);
    fclose(l_output);
    fclose(E_specfile);
    fclose(l_specfile);
    free(specfile_name);
  }    


  /*    E_redshift=(1.+redshift)*1000.*(ear[i]+ear[i+1])/2.; */
  j=1;
  i=0;
  earlo=1000.*(1.+redshift)*ear[i];
  earhi=1000.*(1.+redshift)*ear[i+1];
  earBIN=earhi-earlo;
  Elo=E_array[j]-EBIN/2.;
  Ehi=E_array[j]+EBIN/2.;
  while (earhi<=Elo && i<ne-1) {
    ++i;
    earlo=1000.*(1.+redshift)*ear[i];
    earhi=1000.*(1.+redshift)*ear[i+1];
    earBIN=earhi-earlo;
  }
  while (Ehi<earlo && j<SPECBINS) {
    ++j;
    Elo=E_array[j]-EBIN/2.;
    Ehi=E_array[j]+EBIN/2.;
  }
  while (j<SPECBINS && i<ne-1) {
    if (Elo<earlo) {
      if (Ehi-earlo<earBIN) Ewidth=Ehi-earlo;
      else Ewidth=earBIN;
    } else {
      if (earhi-Elo<EBIN) Ewidth=earhi-Elo;
      else Ewidth=EBIN;
    }
    if (type==-1) photar[i]+=Ewidth/earBIN*tau[j]*EBIN/1000.*sqr(l_array[j])/AngstromtokeV;/* ph/cm^2/s in bin */
    else if (type==0) photar[i]+=Ewidth/earBIN*tau[j]*EBIN/1000.;/* ph/cm^2/s in bin */
    else photar[i]+=Ewidth*E_spectrum[j]*(1.+redshift);/* ph/cm^2/s in bin */
    if (earhi<Ehi) {
      ++i;
      earlo=1000.*(1.+redshift)*ear[i];
      earhi=1000.*(1.+redshift)*ear[i+1];
      earBIN=earhi-earlo;
    } else {
      ++j;
      Elo=E_array[j]-EBIN/2.;
      Ehi=E_array[j]+EBIN/2.;
    }
  }
  
  first=1;
  if (verbose && type>1) {
    first=1;
    for (element=1;element<=28;++element) {
      for (electron=1;electron<=28;++electron) {
	if (EM[element][electron]) {
	  if (first) { 
	    printf("******************************************************************\n");
	    printf("*  Z  z  T[eV] PI=RR[1/s] EMion[cm^-3] Abundance f_ion*EM[cm^-3] *\n");
	    first=0;
	  }
	  printf("* %2d %2d %6.2lf  %4.2e    %4.2e    %4.2e     %4.2e    *\n",element,electron,Tion[element][electron],ratePI[element][electron],EMion[element][electron],ABUND[element],EM[element][electron]);
	}
      }
    }
    if (first==0) {
      printf("******************************************************************\n");
    }
  }


  if (INPUT==0) {
    for (i=1;i<=SPECBINS;++i) {
      int_array[i]=E_array[i]*eVtoergs/ccc*L_pi(E_array[i])*(1.-exp(-tau[i]));
    }
  } else {
    for (i=1;i<=SPECBINS;++i) {
      int_array[i]=E_array[i]*eVtoergs/ccc*Linterp_pi(E_array[i])*(1.-exp(-tau[i]));
    }
  }
  spline_pi(E_array,int_array,SPECBINS,1.e40,1.e40,int_array_2);
  int_ans=f_COVERING*qromb_pi(integrand_pi,1.0001*EMIN,0.9999*EMAX);
  if (first==1) printf("******************************************************************\n");
  printf("* Radiation Pressure = %4.2e dyne (%7.3lf - %7.3lf keV, rf) *\n",int_ans,EMIN/1000.,EMAX/1000.);
  if (INPUT==0 && verbose) printf("* Power-law Norm at 1 keV: %e [photons/cm^2/s/keV]     *\n", LNORM/(1.+redshift)/1000./(4.*PI*sqr(D))*pow(1000.,2.-GAMMA));
  printf("******************************************************************\n");
  

  /* Free all the memory */
  free(root);
  free(temp);
  free(sjunk);
  free(sjunk1);
  free(sjunk2);
  free(sjunk3);
  free(sjunk4);
  free(line);
  /*  free(IONSTR);
      free(ext);*/
  if (INPUT!=0) {
    free_dvector_pi(E_input,1,INPUT_SIZE);
    free_dvector_pi(L_input,1,INPUT_SIZE);
    free_dvector_pi(L_input_2,1,INPUT_SIZE);
    free_dvector_pi(EtimesL_input,1,INPUT_SIZE);
    free_dvector_pi(EtimesL_input_2,1,INPUT_SIZE);
  }
  free_dmatrix_pi(ratePI,1,28,1,28);
  free_dmatrix_pi(rateRR,1,28,1,28);
  free_dmatrix_pi(rateDR,1,28,1,28);
  free_dvector_pi(LOWE_EGRID,1,LOWE_GRIDNUM);  
  free_dvector_pi(LOWE_PIGRID,1,LOWE_GRIDNUM); 
  free_dvector_pi(LOWE_PIGRID_2,1,LOWE_GRIDNUM);
  free_dvector_pi(LOWE_RRGRID,1,LOWE_GRIDNUM); 
  free_dvector_pi(LOWE_RRGRID_2,1,LOWE_GRIDNUM);
  free_dvector_pi(specRR,1,SPECBINS);  
  free_dvector_pi(specDR,1,SPECBINS);  
  free_dvector_pi(specRR_temp,1,SPECBINS);  
  free_dvector_pi(specDR_temp,1,SPECBINS);  
  free_dvector_pi(spec,1,SPECBINS);  
  free_dvector_pi(L_kT,1,RECNUM);  
  free_dvector_pi(L_RR,1,RECNUM);  
  free_dvector_pi(L_RR_2,1,RECNUM);  
  free_dvector_pi(L_DR,1,RECNUM);  
  free_dvector_pi(L_DR_2,1,RECNUM);  
  free_dvector_pi(L_REC,1,RECNUM);  
  free_dvector_pi(L_REC_2,1,RECNUM);  
  free_dvector_pi(RR_line,1,RECNUM);  
  free_dvector_pi(RR_line_2,1,RECNUM);  
  free_dvector_pi(DR_line,1,RECNUM);  
  free_dvector_pi(DR_line_2,1,RECNUM);  
  free_dvector_pi(ABUND,1,30);
  free_dvector_pi(oshe,1,30);
  free_dvector_pi(E_array,1,SPECBINS);       
  free_dvector_pi(E_spectrum,1,SPECBINS);
  free_dvector_pi(type1_spectrum,1,SPECBINS);
  free_dvector_pi(type2_spectrum,1,SPECBINS);
  free_dvector_pi(type3_spectrum,1,SPECBINS);
  free_dvector_pi(type4_spectrum,1,SPECBINS);
  free_dvector_pi(type5_spectrum,1,SPECBINS);
  free_dvector_pi(type6_spectrum,1,SPECBINS);
  free_dvector_pi(type7_spectrum,1,SPECBINS);
  free_dvector_pi(type8_spectrum,1,SPECBINS);
  free_dvector_pi(l_array,1,SPECBINS);
  free_dvector_pi(l_spectrum,1,SPECBINS);
  free_dvector_pi(convert,1,SPECBINS);
  free_dvector_pi(abs_spectrum,1,SPECBINS);    
  free_dvector_pi(exc_spectrum,1,SPECBINS);    
  free_dvector_pi(rec_spectrum,1,SPECBINS);    
  free_dvector_pi(rec_spectrum_temp,1,SPECBINS);    
  free_dvector_pi(tau,1,SPECBINS);           
  free_dvector_pi(tau_exc,1,SPECBINS);           
  free_dvector_pi(Labsorb,1,SPECBINS);    
  free_dvector_pi(int_array,1,SPECBINS);  
  free_dvector_pi(int_array_2,1,SPECBINS); 
  free_dmatrix_pi(Nion,1,28,1,28);
  free_dmatrix_pi(Tion,1,28,1,28);
  free_dmatrix_pi(EMion,1,28,1,28);
  free_dmatrix_pi(EM,1,28,1,28);
  free_dmatrix_pi(Hionizsigmaconv,1,28,1,SPECBINS);
  free_dmatrix_pi(Heionizsigmaconv,1,28,1,SPECBINS);
  free_dvector_pi(ionizsigmatemp,1,SPECBINS);
  free_dvector_pi(spectrumtemp,1,SPECBINS);
  free_dmatrix_pi(H_excite,1,28,1,6);
  free_dmatrix_pi(He_excite,1,28,1,9);
  free_ivector_pi(list,1,ELEMENTS);
  free_dvector_pi(Tvec,1,TEMPERATURES);
  free_dvector_pi(Yvec,1,TEMPERATURES);
  free_dvector_pi(Yvec2,1,TEMPERATURES);
  free_dvector_pi(EGRID,1,GRIDNUM);  
  free_dvector_pi(PIGRID,1,GRIDNUM); 
  free_dvector_pi(PIGRID_2,1,GRIDNUM);
  free_dvector_pi(RRGRID,1,GRIDNUM);  
  free_dvector_pi(RRGRID_2,1,GRIDNUM);
  
  if (verbose) printf("Done!\n");
  
  return 0.;
}

double HeI_edge_pi(double E) /* Yan, Sadeghpour, Dalgarno (1998) */
{
  double answer=0.,x;

  x=E/24.58;
  answer+=1.;
  answer+=-4.7416/pow(x,1./2.);
  answer+=14.8200/pow(x,2./2.);
  answer+=-30.8678/pow(x,3./2.);
  answer+=37.3584/pow(x,4./2.);
  answer+=-23.4585/pow(x,5./2.);
  answer+=5.9133/pow(x,6./2.);
  answer*=733.0/pow(E/1000.,3.5)*1.e-24;

  return answer;
}


double hubble_integrand_pi(double z /* redshift */) /* Peacock, Eq. 3.39, p. 76 */
{
  double answer;
  double Omega_m=0.27,Omega_lambda;
  Omega_lambda=1.-Omega_m;
  answer=ccc/H_0/sqrt(Omega_m*cube(1.+z)+Omega_lambda);
  return answer;
}

double hubble_integrate_pi(double z /* redshift */)
{
  double answer;
  splint_pi(z_array,hubble_array,hubble_array_2,HUBBLE_BINS,z,&answer);
  return answer;
}

double L_pi(double E /*[eV]*/) /* [photons/s/eV] */
{
  double answer;

  if (E<=L_EMAX && E>=L_EMIN) answer=LNORM*pow(E,-GAMMA);
  else answer=0.;
  return answer;
}

double Linterp_pi(double E /*[eV]*/)   /* [photons/s/eV] */
{
  double answer;

  if (E>=L_EMIN && E<=L_EMAX) splint_pi(E_input,L_input,L_input_2,INPUT_SIZE,E,&answer);
  else answer=0.;
  return LinterpNORM*answer;
}

double EtimesL_pi(double E /*[eV]*/)   /* [photon/s] */
{
  double answer;

  if (E>=L_EMIN && E<=L_EMAX) splint_pi(E_input,EtimesL_input,EtimesL_input_2,INPUT_SIZE,E,&answer);
  else answer=0.;
  return LinterpNORM*answer;
}

/* Maxwellian electron distribution */
double maxwell_pi(double Te, double kT, double NORM)
{
  double answer;

  answer=NORM*sqrt(Te)*exp(-Te/kT);
  return answer;
}

/* Recombination-to-ground distribution (needs to be normalized) */
double verner_recombination_pi(struct VERNER_STRUCT vioniz, double kT, double E)
{
  double Te,THRESHOLD,answer;
  
  THRESHOLD=vioniz.Eth;
  Te=E-THRESHOLD;
  if (Te > 0.)   answer=/*grounddeg[electron]/(ge*freedeg[electron])*ccc*/sqrt(2.*Te/meeV)*maxwell_pi(Te,kT,1.)*sqr(E)/(meeV*Te)*vernerph_pi(vioniz,E);
  else answer=0.;
  return answer;
}



/* Recombination-to-ground distribution (needs to be normalized) */
double fac_recombination_pi(double g_i,double g_j,double p0,double p1,double p2,double p3,double kT, double Te)
{
  double E,answer;
  
  E=Te-THRESHOLD;
  if (Te > 0.)   answer=sqrt(2.*Te/meeV)*maxwell_pi(Te,kT,1.)*sqr(E)/(meeV*Te)*rrsigma_pi(g_i,g_j,p0,p1,p2,p3,Te);
  else answer=0.;
  return answer;
}

/* For calculating recombination line strengths */
double loglinestrength_pi(double logkT)
{
  double answer;

  splint_pi(Tvec,Yvec,Yvec2,TEMPERATURES,logkT,&answer);
  return answer;
}

/* integrand for photoionization rate or photoexcitation rate */
double integrand_pi(double temp)
{
  double answer;

  splint_pi(E_array,int_array,int_array_2,SPECBINS,temp,&answer);
  return answer;
}

/* Recombination cross-section in atomic units (a0^2 units) */
double rrsigma_pi(double g_i, double g_j, double p0, double p1, double p2, double p3, double Te /*electron energy?*/)
{
  double answer,E;

  E=Te+THRESHOLD;
  answer=sqr(FINE_STRUCTURE)/2.*g_i/g_j*(sqr(E)/Te*(eVtoHartree))*pisigma_pi(g_i,p0,p1,p2,p3,E);
  return answer;
}

double rrspline_pi(double E)
{
  double answer;

  E=log10(E);
  splint_pi(EGRID,RRGRID,RRGRID_2,GRIDNUM,E,&answer);
  return pow(10.,answer);
}

double lowErrspline_pi(double E)
{
  double answer;
  
  E=log10(E);
  splint_pi(LOWE_EGRID,LOWE_RRGRID,LOWE_RRGRID_2,LOWE_GRIDNUM,E,&answer);
  return pow(10.,answer);
}

double RR_line_spline_pi(double temp)
{
  double answer;
  
  splint_pi(L_kT,RR_line,RR_line_2,RECNUM,temp,&answer);
  return answer;
}

double DR_line_spline_pi(double temp)
{
  double answer;
  
  splint_pi(L_kT,DR_line,DR_line_2,RECNUM,temp,&answer);
  return answer;
}

double L_RR_spline_pi(double temp)
{
  double answer;
  
  splint_pi(L_kT,L_RR,L_RR_2,RECNUM,temp,&answer);
  return answer;
}

double L_DR_spline_pi(double temp)
{
  double answer;
  
  splint_pi(L_kT,L_DR,L_DR_2,RECNUM,temp,&answer);
  return answer;
}

double L_REC_spline_pi(double temp)
{
  double answer;
  
  splint_pi(L_kT,L_REC,L_REC_2,RECNUM,temp,&answer);
  return answer;
}

double fac_PI_rate_integral_pi(double THRESHOLD,double Labsorb[])
{
  int k,klo,khi,kth;
  double strength,pitemp=0.;
  double int_junk,int_ans;

  klo=(int) ((0.5*THRESHOLD*doppler_rad-EMIN+0.5*EBIN)/EBIN);
  kth=(int) ((1.1*THRESHOLD*doppler_rad-EMIN+0.5*EBIN)/EBIN);
  if (klo < 1) klo=1;
  if (klo > SPECBINS) klo=SPECBINS;
  k=klo;
  pi_rate_lim=fac_ionizsigma_pi(THRESHOLD,E_array[kth]/doppler_rad)*Labsorb[kth];
  /*  k=1;
  while (k<SPECBINS) {*/
  int_junk=0.;
  while ((k<=kth || pitemp>=1.e-4*pi_rate_lim) && k < SPECBINS) {
    k=k+1;
    pitemp=fac_ionizsigma_pi(THRESHOLD,E_array[k]/doppler_rad)*Labsorb[k];
    int_array[k]=pitemp;
    int_junk+=EBIN*pitemp;
  }
  khi=(int) ((double) k/2.);
  if (khi>SPECBINS) khi=SPECBINS;
  /*  spline_pi(E_array,int_array,SPECBINS,1.e40,1.e40,int_array_2);
      int_ans=qromb_pi(integrand_pi,(1.001)*THRESHOLD*doppler_rad,E_array[khi]);*/
  if (1 || int_ans<0.) int_ans=int_junk;
  strength=f_COVERING*int_ans;
  return strength;
}

double doppler_pi(double v /*[cm/s]*/) /*[unitless] red: v>0.,d(v)>1, blue: v<0.,d(v)<1*/
{
  double answer;

  answer=sqrt((1.-v/ccc)/(1.+v/ccc));
  return answer;
}

/* For convolution: Normalized gaussian profile centered at 0. with s=sigma */
double gauss_pi(double s,double x) 
{
  double answer=0.;

  answer=1./sqrt(2.*PI)/s*exp(-sqr((x)/s)/2.);  
  return answer;
}

/* Photoexcitation cross section [cm^2] from ground.
   global: E0, DELTANUD, OSCILLATOR, ALPHA */
double excitsigma_pi(double E0, double OSCILLATOR,double DELTANUD,double ALPHA, double E)
{
  double answer;
  double V;

  V=(E-E0)/(hhh*DELTANUD*ergstoeV);  
  answer=PI*re*ccc*OSCILLATOR/sqrt(PI)/DELTANUD*voigt_pi(ALPHA,V)*doppler_rad;
  return answer;
}

/* Differential oscillator strength in atomic units: 1/Hartree = 1/(2.*13.6 eV) */
double dfdE_pi(double g_i,double p0, double p1, double p2, double p3, double Te)
{
  double x,y,answer,E;

  E=Te+THRESHOLD;
  x=(Te+p3)/p3;
  y=(1.+p2)/(sqrt(x)+p2);
  answer=E/(Te+p3)*p0*pow(x,-3.5-ANGULAR+0.5*p1)*pow(y,p1)/g_i;
  return answer;
}

/* Photoionization cross-section in atomic units */
double pisigma_pi(double g_i, double p0, double p1, double p2, double p3, double E)
{
  double answer;

  answer=sqr(a0)*2.*PI*FINE_STRUCTURE*dfdE_pi(g_i,p0,p1,p2,p3,E-THRESHOLD);
  return answer;
}

double pispline_pi(double E)
{
  double answer;
  
  E=log10(E);
  splint_pi(EGRID,PIGRID,PIGRID_2,GRIDNUM,E,&answer);
  return pow(10.,answer);
}

double lowEpispline_pi(double E)
{
  double answer;
  
  E=log10(E);
  splint_pi(LOWE_EGRID,LOWE_PIGRID,LOWE_PIGRID_2,LOWE_GRIDNUM,E,&answer);
  return pow(10.,answer);
}

void line_limits_pi(double Nion_column_density,double E0,double OSCILLATOR,double ALPHA,double DELTANUD,double *Elo,double *Ehi,int *SUMlo,int *SUMhi)
{
  double Vlim;
  
  /*tau_lim=Nion_column_density*excitsigma=Nion_column_density*PI*re*ccc*OSCILLATOR/sqrt(PI)/DELTANUD*voigt_pi(ALPHA,V);*/
  
  Vlim=sqrt(ALPHA*Nion_column_density*re*ccc*OSCILLATOR/DELTANUD/tau_lim);
  if (voigt_pi(ALPHA,Vlim)>voigt_lim) {
    Vlim=sqrt(ALPHA/sqrt(PI)/(voigt_lim*voigt_pi(ALPHA,0.)));
  }
  if (E0-Vlim*(hhh*DELTANUD*ergstoeV)>EMIN) {
    *Elo=E0-Vlim*(hhh*DELTANUD*ergstoeV);
  } else *Elo=(1.+SMALL)*EMIN;
  if (E0+Vlim*(hhh*DELTANUD*ergstoeV)<EMAX) {
    *Ehi=E0+Vlim*(hhh*DELTANUD*ergstoeV);
  } else *Ehi=(1.-SMALL)*EMAX;

  *SUMlo=(int) ((0.9*(*Elo)-EMIN+0.5*EBIN)/EBIN);
  if ((0.9*(*Elo)-EMIN+0.5*EBIN)/EBIN-(double) *SUMlo >= 0.5) ++(*SUMlo);
  if (*SUMlo > SPECBINS) *SUMlo=SPECBINS;
  else if (*SUMlo < 1) *SUMlo=1;

  *SUMhi=(int) ((1.1*(*Ehi)-EMIN+0.5*EBIN)/EBIN);
  if ((1.1*(*Ehi)-EMIN+0.5*EBIN)/EBIN-(double) *SUMlo >= 0.5) ++(*SUMhi);
  if (*SUMhi > SPECBINS) *SUMhi=SPECBINS;
  else if (*SUMhi < 1) *SUMhi=1;
}

void line_opacity_pi(double Nion_column_density,double E0,double OSCILLATOR,double ALPHA,double DELTANUD,double tau_exc_p[])
{
  double Ehi,Elo;
  int k,SUMlo,SUMhi;

  line_limits_pi(Nion_column_density,E0,OSCILLATOR,ALPHA,DELTANUD,&Elo,&Ehi,&SUMlo,&SUMhi);

  for (k=SUMlo;k<=SUMhi;++k) {
    tau_exc_p[k]+=Nion_column_density*excitsigma_pi(E0,OSCILLATOR,DELTANUD,ALPHA,E_array[k]);
  }
}

/* ground-state photoionization cross-section from Verner */
double vernerph_pi(struct VERNER_STRUCT verner, double E)
{
  double answer=0.;
  double Eth,Emax,Ezero,s0,ya,P,yw,y0,y1;
  double F,x,y;

  Eth=verner.Eth;
  Emax=verner.Emax;
  if (E>=Eth && E<=Emax) {
    Ezero=verner.Ezero;
    s0=verner.s0;
    ya=verner.ya;
    P=verner.P;
    yw=verner.yw;
    y0=verner.y0;
    y1=verner.y1;

    x=E/Ezero-y0;
    y=sqrt(sqr(x)+sqr(y1));
    F=(sqr(x-1.)+sqr(yw))*pow(y,0.5*P-5.5)*pow(1.+sqrt(y/ya),-P);
    answer=1.e-18*s0*F;
  }
  return answer;
}

/* ground-state photoionization cross-section from Verner */
double vernerpartialph_pi(struct VERNER_PARTIAL_STRUCT verner, double E)
{
  double answer=0.;
  double Eth,Ezero,s0,ya,P,yw,l;
  int angular;
  double F,y;

  Eth=verner.Eth;
  if (E>=Eth) {
    Ezero=verner.Ezero;
    s0=verner.s0;
    ya=verner.ya;
    P=verner.P;
    yw=verner.yw;
    angular=verner.angular;
    l=(double) angular;

    y=E/Ezero;
    F=(sqr(y-1.)+sqr(yw))*pow(y,-(5.5+l-0.5*P))*pow(1.+sqrt(y/ya),-P);
    answer=1.e-18*s0*F;
  }
  return answer;
}

double fac_ionizsigma_pi(double THRESHOLD, double E)
{
  double answer=0.;
  
  if (E>=THRESHOLD) {
    if (E<=pow(10.,LOWE_EGRID[1])) answer=lowEpispline_pi(E);
    else answer=pispline_pi(E);
  }
  return answer;
}

void HeI_edge_opacity_pi(double Nion_column_density, double THRESHOLD, double tau_p[])
{
  int k,klo,kth;
  double taujunk;
  
  klo=(int) ((0.95*THRESHOLD*doppler_rad-EMIN+0.5*EBIN)/EBIN);
  kth=(int) ((1.05*THRESHOLD*doppler_rad-EMIN+0.5*EBIN)/EBIN);
  if (klo < 1) klo=1;
  if (klo > SPECBINS) klo=SPECBINS;
  k=klo;
  taujunk=Nion_column_density*HeI_edge_pi(E_array[k]/doppler_rad);
  while ((taujunk > tau_lim || k <= kth) && k < SPECBINS) {
    k=k+1;
    taujunk=Nion_column_density*HeI_edge_pi(E_array[k]/doppler_rad);
    tau_p[k]+=taujunk;
  }
}

void fac_edge_opacity_pi(double Nion_column_density, double THRESHOLD, double tau_p[]) 
{
  int k,klo,kth;
  double taujunk;
  
  klo=(int) ((0.95*THRESHOLD*doppler_rad-EMIN+0.5*EBIN)/EBIN);
  kth=(int) ((1.05*THRESHOLD*doppler_rad-EMIN+0.5*EBIN)/EBIN);
  if (klo < 1) klo=1;
  if (klo > SPECBINS) klo=SPECBINS;
  k=klo;
  taujunk=Nion_column_density*fac_ionizsigma_pi(THRESHOLD,E_array[k]/doppler_rad);
  while ((taujunk > tau_lim || k <= kth) && k < SPECBINS) {
    k=k+1;
    taujunk=Nion_column_density*fac_ionizsigma_pi(THRESHOLD,E_array[k]/doppler_rad);
    tau_p[k]+=taujunk;
  }
}

void verner_full_edge_opacity_pi(double Nion_column_density, double THRESHOLD, struct VERNER_STRUCT verner, double tau_p[]) 
{
  int k,klo,kth;
  double taujunk;
  
  klo=(int) ((0.95*THRESHOLD*doppler_rad-EMIN+0.5*EBIN)/EBIN);
  kth=(int) ((1.05*THRESHOLD*doppler_rad-EMIN+0.5*EBIN)/EBIN);
  if (klo < 1) klo=1;
  if (klo > SPECBINS) klo=SPECBINS;
  k=klo;
  taujunk=Nion_column_density*vernerph_pi(verner, E_array[k]/doppler_rad);
  while ((taujunk > tau_lim || k <= kth) && k < SPECBINS) {
    k=k+1;
    taujunk=Nion_column_density*vernerph_pi(verner, E_array[k]/doppler_rad);
    tau_p[k]+=taujunk;
  }
}

void verner_partial_edge_opacity_pi(double Nion_column_density, double THRESHOLD, struct VERNER_PARTIAL_STRUCT verner, double tau_p[]) 
{
  int k,klo,kth;
  double taujunk;
  
  klo=(int) ((0.95*THRESHOLD*doppler_rad-EMIN+0.5*EBIN)/EBIN);
  kth=(int) ((1.05*THRESHOLD*doppler_rad-EMIN+0.5*EBIN)/EBIN);
  if (klo < 1) klo=1;
  if (klo > SPECBINS) klo=SPECBINS;
  k=klo;
  taujunk=Nion_column_density*vernerpartialph_pi(verner, E_array[k]/doppler_rad);
  while ((taujunk > tau_lim || k <= kth) && k < SPECBINS) {
    k=k+1;
    taujunk=Nion_column_density*vernerpartialph_pi(verner, E_array[k]/doppler_rad);
    tau_p[k]+=taujunk;
  }
}

double voigt_pi(double alpha,double v)
{
  int i;
  double *a,*b,*c;
  double v2,v3,fac1,fac2;
  double p1,p2,p3,p4,p5,p6,p7;
  double o1,o2,o3,o4,o5,o6,o7;
  double q1,q2;
  double r1,r2;
  double H;
 
  a=dvector_pi(1,7);
  b=dvector_pi(1,7);
  c=dvector_pi(1,7);

  a[1]=122.607931777104326;
  a[2]=214.382388694706425;
  a[3]=181.928533092181549;
  a[4]=93.155580458134410;
  a[5]=30.180142196210589;   
  a[6]=5.912626209773153;
  a[7]=0.564189583562615;

  b[1]=122.607931773875350;
  b[2]=352.730625110963558;
  b[3]=457.334478783897737;
  b[4]=348.703917719495792;
  b[5]=170.354001821091472;
  b[6]=53.992906912940207;
  b[7]=10.479857114260399;
  
  c[1]=0.5641641;
  c[2]=0.8718681;
  c[3]=1.474395;
  c[4]=-19.57862;
  c[5]=802.4513;
  c[6]=-4850.316;
  c[7]=8031.468;
  
  if (alpha <= .001 && v >= 2.5) {
    v2   = v * v;
    v3   = 1.0;
    fac1 = c[1];
    fac2 = c[1] * (v2 - 1.0);
      
    for (i=1;i<=7;++i) {
      v3     = v3 * v2;
      fac1 = fac1 + c[i] / v3;
      fac2 = fac2 + c[i] / v3 * (v2 - (double) i);
    }
    
    H = exp(-v2) * (1. + sqr(alpha) * (1. - 2.*v2)) + fac1 * (alpha/v2);
    
  } else { 
    p1 = alpha;
    o1 = -v;
    p2 = (p1 * alpha + o1 * v);
    o2 = (o1 * alpha - p1 * v);
    p3 = (p2 * alpha + o2 * v);
    o3 = (o2 * alpha - p2 * v);
    p4 = (p3 * alpha + o3 * v);
    o4 = (o3 * alpha - p3 * v);
    p5 = (p4 * alpha + o4 * v);
    o5 = (o4 * alpha - p4 * v);
    p6 = (p5 * alpha + o5 * v);
    o6 = (o5 * alpha - p5 * v);
    p7 = (p6 * alpha + o6 * v);
    o7 = (o6 * alpha - p6 * v);
    
    q1 = a[1] + p1 * a[2] + p2 * a[3] + p3 * a[4] +
      p4 * a[5] + p5 * a[6] + p6 * a[7];
    r1 =        o1 * a[2] + o2 * a[3] + o3 * a[4] +
      o4 * a[5] + o5 * a[6] + o6 * a[7];
    q2 = b[1] + p1 * b[2] + p2 * b[3] + p3 * b[4] +
      p4 * b[5] + p5 * b[6] + p6 * b[7] + p7;
    r2 =        o1 * b[2] + o2 * b[3] + o3 * b[4] +
      o4 * b[5] + o5 * b[6] + o6 * b[7] + o7;
    
    H = (q1 * q2 + r1 * r2) / (q2 * q2 + r2 * r2);
  }

  free_dvector_pi(a,1,7);
  free_dvector_pi(b,1,7);
  free_dvector_pi(c,1,7);
  
  return H;
}


