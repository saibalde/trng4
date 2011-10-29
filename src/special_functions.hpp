#if !(defined TRNG_SPECIAL_FUNCTIONS_HPP)

#define TRNG_SPECIAL_FUNCTIONS_HPP

#include <trng/limits.hpp>
#include <trng/math.hpp>
#include <trng/constants.hpp>
#include <cerrno>

namespace trng {

  namespace math {

    // --- log-Gamma function ------------------------------------------

    namespace detail {
    
      template<typename T>
      struct ln_Gamma_traits;
    
      template<>
      struct ln_Gamma_traits<float> {
	static float one_half() throw() { 
	  return 0.5f;
	}
	static float ln_sqrt_2pi() throw() {
	  return 0.91893853320467274177f;
	}
	static float b(int i) throw() {
	  // B_{2k}/(2k(2k-1))   k=1, 2, ...
	  static const float b_[]={1.0f/12.0f,
				   -1.0f/360.0f,
				   1.0f/1260.0f};
	  return b_[i];
	}
	static const int size_b=3;
      };
    
      template<>
      struct ln_Gamma_traits<double> {
	static double one_half() throw() { 
	  return 0.5;
	}
	static double ln_sqrt_2pi() throw() {
	  return 0.91893853320467274177;
	}
	static double b(int i) throw() {
	  // B_{2k}/(2k(2k-1))   k=1, 2, ...
	  static const double b_[]={1.0/12.0,
				    -1.0/360.0,
				    1.0/1260.0,
				    -1.0/1680.0,
				    1.0/1188.0,
				    -691.0/360360.0,
				    1.0/156.0};
	  return b_[i];
	}
	static const int size_b=7;
      };

      template<>
      struct ln_Gamma_traits<long double> {
	static long double one_half() throw() { 
	  return 0.5l;
	}
	static long double ln_sqrt_2pi() throw() {
	  return 0.91893853320467274177l;
	}
	static long double b(int i) throw() {
	  // B_{2k}/(2k(2k-1))   k=1, 2, ...
	  static const long double b_[]={1.0l/12.0l,
					 -1.0l/360.0l,
					 1.0l/1260.0l,
					 -1.0l/1680.0l,
					 1.0l/1188.0l,
					 -691.0l/360360.0l,
					 1.0l/156.0l,
					 -3617.0l/122400.0l,
					 43867.0l/125400.0l};
	  return b_[i];
	}
	static const int size_b=9;
      };

      // calculate ln(Gamma(x+1)) for x near zero using Legendre's series
      template<typename T>
      T ln_Gamma_1(T x) {
	static const T eps(T(4)*numeric_limits<T>::epsilon());
	if (abs(x)<eps)
	  return T(0);
	T t(x*constants<T>::pi());
	T sum( ln_Gamma_traits<T>::one_half()*
	       ln( t*(T(1)-x)/(sin(t)*(T(1)+x)) )  );
	sum+=ln_Gamma_traits<T>::one_min_gamma()*x;  // (1-gamma)*x
	T x2(x*x);
	int i(0);
	while (i<ln_Gamma_traits<T>::size_a) {
	  x*=x2;
	  t=ln_Gamma_traits<T>::a(i++)*x;
	  if (abs(t)<eps*sum)
	    break;
	  sum+=t;
	}
	return sum;
      }

      // calculate ln(Gamma(x)) by Lanczos' approximation
      template<typename T>
      T ln_Gamma_Lanczos(T x);

      template<>
      float ln_Gamma_Lanczos<float>(float x) {
	return ln(+0.299890266072888E-2f/(x+4.0f)
		  -0.308748865044984E1f/(x+3.0f)
		  +0.6019440944395479E2f/(x+2.0f)
		  -0.2168366808191931E3f/(x+1.0f)
		  +0.190955171863804E3f/x
		  +0.250662827022856E1f)
	  -(x+4.5f)+(x+0.5f)*ln(x+4.5f);
      }
    
      template<>
      double ln_Gamma_Lanczos<double>(double x) {
	return ln(-0.1710538478644311E-5/(x+7.0)
		  +0.8683645856906762E-1/(x+6.0)
		  -0.1567563009175129E2/(x+5.0)
		  +0.3873696975776843E3/(x+4.0)
		  -0.2900131673187631E4/(x+3.0)
		  +0.8679533396416264E4/(x+2.0)
		  -0.1102260304013762E5/(x+1.0)
		  +0.4951528076618453E4/x
		  +0.2506628274630859E1)
	  -(x+7.5)+(x-0.5)*ln(x+7.5);
      }
    
      template<>
      long double ln_Gamma_Lanczos<long double>(long double x) {
	return ln(-0.1008550193581785E-7l/(x+9.0l)
		  +0.1349602861619936E-6l/(x+8.0l)
		  -0.1860783595745135E-1l/(x+7.0l)
		  +0.6531512536073623E1l/(x+6.0l)
		  -0.2711587882903386E3l/(x+5.0l)
		  +0.3262648132327785E4l/(x+4.0l)
		  -0.1591247789342777E5l/(x+3.0l)
		  +0.3582345988044706E5l/(x+2.0l)
		  -0.3713646057463839E5l/(x+1.0l)
		  +0.1432889034103444E5l/x
		  +0.2506628274631001E1l)
	  -(x+8.5l)+(x-0.5l)*ln(x+8.5l);
      }

      // calculate ln(Gamma(x)) for large x using an asympotic series
      template<typename T>
      T ln_Gamma_infty(T x) {
	static const T eps(T(4)*numeric_limits<T>::epsilon());
	T sum((x-ln_Gamma_traits<T>::one_half())*ln(x)-x+
	      ln_Gamma_traits<T>::ln_sqrt_2pi());
	x=T(1)/x;
	T x2(x*x), t;
	int i(0);
	while (i<ln_Gamma_traits<T>::size_b) {
	  t=ln_Gamma_traits<T>::b(i++)*x;
	  if (abs(t)<eps*sum)
	    break;
	  x*=x2;
	  sum+=t;
	}
	return sum;
      }
    
      // calculate ln(Gamma(x)) for positive x
      template<typename T>
      inline T ln_Gamma(T x) {
	if (x<T(20)) 
	  return ln_Gamma_Lanczos(x);
	return ln_Gamma_infty(x);
      }
    
    }

#if !(defined(HAVE_LGAMMAF) || defined(__ICC) || defined(__ICL) || defined(__ECC) || defined(__ECL))
    inline float ln_Gamma(float x) {
      if (x>0.0f)
	return detail::ln_Gamma(x);
      float t(abs(sin(constants<float>::pi()*x)));
      if (t<4.0f*numeric_limits<float>::epsilon())
	return numeric_limits<float>::infinity();
      return ln(constants<float>::pi()/(-x*t))-detail::ln_Gamma(-x);
    }
#else
    inline float ln_Gamma(float x) {
      return ::lgammaf(x);
    }
#endif
  
#if !(defined(HAVE_LGAMMA) || defined(__ICC) || defined(__ICL) || defined(__ECC) || defined(__ECL))
    inline double ln_Gamma(double x) {
      if (x>0.0)
	return detail::ln_Gamma(x);
      double t(abs(sin(constants<double>::pi()*x)));
      if (t<4.0*numeric_limits<double>::epsilon())
	return numeric_limits<double>::infinity();
      return ln(constants<double>::pi()/(-x*t))-detail::ln_Gamma(-x);
    }
#else
    inline double ln_Gamma(double x) {
      return ::lgamma(x);
    }
#endif
 
#if !(defined(HAVE_LGAMMAL) || defined(__ICC) || defined(__ICL) || defined(__ECC) || defined(__ECL))
    inline long double ln_Gamma(long double x) {
      if (x>0.0l)
	return detail::ln_Gamma(x);
      long double t(abs(sin(constants<long double>::pi()*x)));
      if (t<4.0l*numeric_limits<long double>::epsilon())
	return numeric_limits<long double>::infinity();
      return ln(constants<long double>::pi()/(-x*t))-detail::ln_Gamma(-x);
    }
#else
    inline long double ln_Gamma(long double x) {
      return ::lgammal(x);
    }
#endif

    // --- Gamma function ----------------------------------------------

    namespace detail {

      template<typename T>
      struct Gamma_traits;
    
      template<>
      struct Gamma_traits<float> {
	static float one_half() throw() { 
	  return 0.5f;
	}
	static float lim() throw() { 
	  return 20.0f;
	}
	static float sqrt_2pi() throw() {
	  return 2.506628274631000502416f;
	}
	static float a(int i) throw() {
	  static const float a_[]={1.0f,
				   1.0f/12.0f,
				   1.0f/288.0f};
	  return a_[i];
	}
	static const int size_a=3;
      };
    
      template<>
      struct Gamma_traits<double> {
	static double one_half() throw() { 
	  return 0.5;
	}
	static double lim() throw() { 
	  return 20.0;
	}
	static double sqrt_2pi() throw() {
	  return 2.506628274631000502416;
	}
	static double a(int i) throw() {
	  static const double a_[]={1.0,
				    1.0/12.0,
				    1.0/288.0,
				    -139.0/51840.0,
				    -571.0/2488320.0,
				    163879.0/209018880.0,
				    5246819.0/75246796800.0,
				    -534703531.0/902961561600.0,
				    -4483131259.0/86684309913600.0};
	  return a_[i];
	}
	static const int size_a=9;
      };
    
      template<>
      struct Gamma_traits<long double> {
	static long double one_half() throw() { 
	  return 0.5l;
	}
	static long double lim() throw() { 
	  return 20.0l;
	}
	static long double sqrt_2pi() throw() {
	  return 2.506628274631000502416l;
	}
	static long double a(int i) throw() {
	  static const long double a_[]={1.0l,
					 1.0l/12.0l,
					 1.0l/288.0l,
					 -139.0l/51840.0l,
					 -571.0l/2488320.0l,
					 163879.0l/209018880.0l,
					 5246819.0l/75246796800.0l,
					 -534703531.0l/902961561600.0l,
					 -4483131259.0l/86684309913600.0l,
					 432261921612371.0l/514904800886784000.0l,
					 6232523202521089.0l/86504006548979712000.0l};
	  return a_[i];
	}
	static const int size_a=11;
      };

      // calculate Gamma(x) by Lanczos' approximation
      template<typename T>
      T Gamma_Lanczos(T x);

      template<>
      float Gamma_Lanczos<float>(float x) {
	return (+0.299890266072888E-2f/(x+4.0f)
		-0.308748865044984E1f/(x+3.0f)
		+0.6019440944395479E2f/(x+2.0f)
		-0.2168366808191931E3f/(x+1.0f)
		+0.190955171863804E3f/x
		+0.250662827022856E1f)*
	  exp(-(x+4.5f)+(x+0.5f)*ln(x+4.5f));
      }

      template<>
      double Gamma_Lanczos<double>(double x) {
	return (-0.1710538478644311E-5/(x+7.0)
		+0.8683645856906762E-1/(x+6.0)
		-0.1567563009175129E2/(x+5.0)
		+0.3873696975776843E3/(x+4.0)
		-0.2900131673187631E4/(x+3.0)
		+0.8679533396416264E4/(x+2.0)
		-0.1102260304013762E5/(x+1.0)
		+0.4951528076618453E4/x
		+0.2506628274630859E1)*
	  exp(-(x+7.5)+(x-0.5)*ln(x+7.5));
      }
    
      template<>
      long double Gamma_Lanczos<long double>(long double x) {
	return (-0.1008550193581785E-7l/(x+9.0l)
		+0.1349602861619936E-6l/(x+8.0l)
		-0.1860783595745135E-1l/(x+7.0l)
		+0.6531512536073623E1l/(x+6.0l)
		-0.2711587882903386E3l/(x+5.0l)
		+0.3262648132327785E4l/(x+4.0l)
		-0.1591247789342777E5l/(x+3.0l)
		+0.3582345988044706E5l/(x+2.0l)
		-0.3713646057463839E5l/(x+1.0l)
		+0.1432889034103444E5l/x
		+0.2506628274631001E1l)*
	  exp(-(x+8.5l)+(x-0.5l)*ln(x+8.5l));
      }

      // calculate Gamma(x) for positive x
      template<typename T>
      T Gamma(T x) {
	static const T eps(T(4)*numeric_limits<T>::epsilon());
	if (x<Gamma_traits<T>::lim())
	  return Gamma_Lanczos(x);
	// use Stirling series
	T x1(T(1)/x), x2(T(1)), sum(0), t;
	int i(0);
	while (i<Gamma_traits<T>::size_a) {
	  t=Gamma_traits<T>::a(i++)*x2;
	  if (abs(t)<eps*sum)
	    break;
	  x2*=x1;
	  sum+=t;
	}
	return sum*Gamma_traits<T>::sqrt_2pi()*
	  pow(x1, Gamma_traits<T>::one_half()-x)*exp(-x);
      }

    }

#if !(defined(HAVE_TGAMMAF) || defined(__ICC) || defined(__ICL) || defined(__ECC) || defined(__ECL))
    inline float Gamma(float x) {
      if (x>0.0f)
	return detail::Gamma(x);
      float t(sin(x*constants<float>::pi()));
      if (abs(t)<4.0f*numeric_limits<float>::epsilon())
	return (t>=0.0f ? 1.0f : -1.0f)*numeric_limits<float>::infinity();
      return constants<float>::pi()/(-x*detail::Gamma(-x)*t);
    }
#else
    inline float Gamma(float x) {
      return ::tgammaf(x);
    }
#endif

#if !(defined(HAVE_TGAMMA) || defined(__ICC) || defined(__ICL) || defined(__ECC) || defined(__ECL))
    inline double Gamma(double x) {
      if (x>0.0)
	return detail::Gamma(x);
      double t(sin(x*constants<double>::pi()));
      if (abs(t)<4.0*numeric_limits<double>::epsilon())
	return (t>=0.0 ? 1.0 : -1.0)*numeric_limits<double>::infinity();
      return constants<double>::pi()/(-x*detail::Gamma(-x)*t);
    }
#else
    inline double Gamma(double x) {
      return ::tgamma(x);
    }
#endif

#if !(defined(HAVE_TGAMMAL) || defined(__ICC) || defined(__ICL) || defined(__ECC) || defined(__ECL))
    inline long double Gamma(long double x) {
      if (x>0.0l)
	return detail::Gamma(x);
      long double t(sin(x*constants<long double>::pi()));
      if (abs(t)<4.0l*numeric_limits<long double>::epsilon())
	return (t>=0.0l ? 1.0l : -1.0l)*numeric_limits<long double>::infinity();
      return constants<long double>::pi()/(-x*detail::Gamma(-x)*t);
    }
#else
    inline long double Gamma(long double x) {
      return ::tgammal(x);
    }
#endif

    // --- Beta function -----------------------------------------------

    inline float Beta(float x, float y) {
      return exp(ln_Gamma(x)+ln_Gamma(x)-ln_Gamma(x+y));
    }

    inline double Beta(double x, double y) {
      return exp(ln_Gamma(x)+ln_Gamma(x)-ln_Gamma(x+y));
    }
  
    inline double Beta(long double x, long double y) {
      return exp(ln_Gamma(x)+ln_Gamma(x)-ln_Gamma(x+y));
    }

    // --- Pochhammer function -----------------------------------------

    inline float Pochhammer(float x, float a) {
      return exp(ln_Gamma(x+a)-ln_Gamma(x));
    }

    inline double Pochhammer(double x, double a) {
      return exp(ln_Gamma(x+a)-ln_Gamma(x));
    }
  
    inline double Pochhammer(long double x, long double a) {
      return exp(ln_Gamma(x+a)-ln_Gamma(x));
    }

    // --- incomplete Gamma functions ----------------------------------

    namespace detail {

      // compute incomplete Gamma function 
      //
      //  gamma(a, x) = Int(exp(-t)*t^(a-1), t=0..x)
      //
      // or
      //
      //  P(a, x) = gamma(a, x) / Gamma(a)
      // 
      // by series expansion
      template<typename T>
      T GammaP_ser(T a, T x, bool by_Gamma_a) {
	static const int itmax=32;
	static const T eps=T(4)*numeric_limits<T>::epsilon();
	if (x<eps)
	  return T(0);
	T xx(T(1)/a), n(a), sum(xx);
	int i(0);
	do {
	  ++n;
	  ++i;
	  xx*=x/n;
	  sum+=xx;
	} while (abs(xx)>eps*abs(sum) && i<itmax);
	if (by_Gamma_a)
	  return exp(-x+a*ln(x)-ln_Gamma(a))*sum;
	return exp(-x+a*ln(x))*sum;
      }

      // compute complementary incomplete Gamma function 
      // 
      //  Gamma(a, x) = Int(exp(-t)*t^(a-1), t=x..oo)
      //
      // or
      // 
      //  Q(a, x) = Gamma(a, x) / Gamma(a) = 1 - P(a, x)
      //
      // by continued fraction
      template<typename T>
      T GammaQ_cf(T a, T x, bool by_Gamma_a) {
	static const T itmax=T(32);
	static const T eps=T(4)*numeric_limits<T>::epsilon();
	static const T min=T(4)*numeric_limits<T>::min();
	// set up for evaluating continued fraction by modied Lentz's method
	T del, ai, bi(x+T(1)-a), ci(T(1)/min), di(T(1)/bi), h(di), i(T(0));
	do { // iterate 
	  ++i;
	  ai=-i*(i-a);
	  bi+=T(2);
	  di=ai*di+bi;
	  if (abs(di)<min) 
	    di=min; 
	  ci=bi+ai/ci; 
	  if (abs(ci)<min)
	    ci=min;
	  di=T(1)/di;
	  del=di*ci;
	  h*=del;
	} while ((abs(del-T(1))>eps) && i<itmax);
	if (by_Gamma_a)
	  return exp(-x+a*ln(x)-ln_Gamma(a))*h;
	return exp(-x+a*ln(x))*h;
      }
    
      // P(a, x) and gamma(a, x)
      template<typename T>
      T GammaP(T a, T x, bool by_Gamma_a) {
	if (x<T(0) || a<=T(0))
	  return numeric_limits<T>::signaling_NaN();
	if (by_Gamma_a) {
	  if (x<a+T(1))
	    return GammaP_ser(a, x, true);
	  return T(1)-GammaQ_cf(a, x, true);
	}
	if (x<a+T(1))
	  return GammaP_ser(a, x, false);
	return Gamma(a)-GammaQ_cf(a, x, false);
      }

      // Q(a, x) and Gamma(a, x)
      template<typename T>
      T GammaQ(T a, T x, bool by_Gamma_a) {
	if (x<T(0) || a<=T(0))
	  return numeric_limits<T>::signaling_NaN();
	if (by_Gamma_a) {
	  if (x<a+T(1))
	    return T(1)-GammaP_ser(a, x, true);
	  return GammaQ_cf(a, x, true);
	}
	if (x<a+T(1))
	  return Gamma(a)-GammaP_ser(a, x, false);
	return GammaQ_cf(a, x, false);
      }
    
    }
  
    // P(x, a)
    inline float GammaP(float a, float x) {
      return detail::GammaP(a, x, true);
    }

    inline double GammaP(double a, double x) {
      return detail::GammaP(a, x, true);
    }

    inline long double GammaP(long double a, long double x) {
      return detail::GammaP(a, x, true);
    }

    // Q(x, a)
    inline float GammaQ(float a, float x) {
      return detail::GammaQ(a, x, true);
    }

    inline double GammaQ(double a, double x) {
      return detail::GammaQ(a, x, true);
    }

    inline long double GammaQ(long double a, long double x) {
      return detail::GammaQ(a, x, true);
    }

    // gamma(x, a)
    inline float inc_gamma(float a, float x) {
      return detail::GammaP(a, x, false);
    }

    inline double inc_gamma(double a, double x) {
      return detail::GammaP(a, x, false);
    }
  
    inline long double inc_gamma(long double a, long double x) {
      return detail::GammaP(a, x, false);
    }

    // Gamma(x, a)
    inline float cinc_gamma(float a, float x) {
      return detail::GammaQ(a, x, false);
    }
  
    inline double cinc_gamma(double a, double x) {
      return detail::GammaQ(a, x, false);
    }
  
    inline long double cinc_gamma(long double a, long double x) {
      return detail::GammaQ(a, x, false);
    }

    // --- error function ----------------------------------------------

#if !(defined(HAVE_ERFF) || defined(__ICC) || defined(__ICL) || defined(__ECC) || defined(__ECL))
    inline float erf(float x) {
      return x<0.0f ? -GammaP(0.5f, x*x) : GammaP(0.5f, x*x);
    }
#else
    inline float erf(float x) {
      return ::erff(x);
    }
#endif

#if !(defined(HAVE_ERF) || defined(__ICC) || defined(__ICL) || defined(__ECC) || defined(__ECL))
    inline double erf(double x) {
      return x<0.0 ? -GammaP(0.5, x*x) : GammaP(0.5, x*x);
    }
#else
    inline double erf(double x) {
      return ::erf(x);
    }
#endif
  
#if !(defined(HAVE_ERFL) || defined(__ICC) || defined(__ICL) || defined(__ECC) || defined(__ECL))
    inline long double erf(long double x) {
      return x<0.0l ? -GammaP(0.5l, x*x) : GammaP(0.5l, x*x);
    }
#else
    inline long double erf(long double x) {
      return ::erfl(x);
    }
#endif

    // --- complementary error function --------------------------------

#if !(defined(HAVE_ERFCF) || defined(__ICC) || defined(__ICL) || defined(__ECC) || defined(__ECL))
    inline float erfc(float x) {
      return x<0.0f ? 1.0f+GammaP(0.5f, x*x) : GammaQ(0.5f, x*x);
    }
#else
    inline float erfc(float x) {
      return ::erfcf(x);
    }
#endif

#if !(defined(HAVE_ERFC) || defined(__ICC) || defined(__ICL) || defined(__ECC) || defined(__ECL))
    inline double erfc(double x) {
      return x<0.0 ? 1.0+GammaP(0.5, x*x) : GammaQ(0.5, x*x);
    }
#else
    inline double erfc(double x) {
      return ::erfc(x);
    }
#endif
  
#if !(defined(HAVE_ERFCL) || defined(__ICC) || defined(__ICL) || defined(__ECC) || defined(__ECL))
    inline long double erfc(long double x) {
      return x<0.0l ? 1.0l+GammaP(0.5l, x*x) : GammaQ(0.5l, x*x);
    }
#else
    inline long double erfc(long double x) {
      return ::erfcl(x);
    }
#endif

    // --- normal distribution function  -------------------------------

    inline float Phi(float x) {
      return 0.5f+0.5f*erf(constants<float>::one_over_sqrt_2()*x);
    }

    inline double Phi(double x) {
      return 0.5+0.5*erf(constants<double>::one_over_sqrt_2()*x);
    }

    inline long double Phi(long double x) {
      return 0.5l+0.5l*erf(constants<long double>::one_over_sqrt_2()*x);
    }

    // --- inverse of normal distribution function  --------------------

    // this function is based on an approximation by Peter J. Acklam 
    // see http://home.online.no/~pjacklam/notes/invnorm/ for details

    namespace detail {

      template<typename T>
      struct inv_Phi_traits;
    
      template<>
      struct inv_Phi_traits<float> {
	static float a(int i) throw() {
	  static const float a_[]={
	    -3.969683028665376e+01f,   2.209460984245205e+02f,
	    -2.759285104469687e+02f,   1.383577518672690e+02f,
	    -3.066479806614716e+01f,   2.506628277459239e+00f};
	  return a_[i];
	}
	static float b(int i) throw() {
	  static const float b_[]={
	    -5.447609879822406e+01f,   1.615858368580409e+02f,
	    -1.556989798598866e+02f,   6.680131188771972e+01f,
	    -1.328068155288572e+01f};
	  return b_[i];
	}
	static float c(int i) throw() {
	  static const float c_[]={
	    -7.784894002430293e-03f,  -3.223964580411365e-01f,
	    -2.400758277161838e+00f,  -2.549732539343734e+00f,
	    4.374664141464968e+00f,    2.938163982698783e+00f};
	  return c_[i];
	}
	static float d(int i) throw() {
	  static const float d_[]={
	    7.784695709041462e-03f,    3.224671290700398e-01f,
	    2.445134137142996e+00f,    3.754408661907416e+00f};
	  return d_[i];
	}
	static const float x_low=0.02425f;
	static const float x_high=1.0f-0.02425f;
	static const float zero=0.0f;
	static const float one=1.0f;
	static const float one_half=0.5f;
	static const float minus_two=-2.0f;
      };

      template<>
      struct inv_Phi_traits<double> {
	static double a(int i) throw() {
	  static const double a_[]={
	    -3.969683028665376e+01,   2.209460984245205e+02,
	    -2.759285104469687e+02,   1.383577518672690e+02,
	    -3.066479806614716e+01,   2.506628277459239e+00};
	  return a_[i];
	}
	static double b(int i) throw() {
	  static const double b_[]={
	    -5.447609879822406e+01,   1.615858368580409e+02,
	    -1.556989798598866e+02,   6.680131188771972e+01,
	    -1.328068155288572e+01};
	  return b_[i];
	}
	static double c(int i) throw() {
	  static const double c_[]={
	    -7.784894002430293e-03,  -3.223964580411365e-01,
	    -2.400758277161838e+00,  -2.549732539343734e+00,
	    4.374664141464968e+00,    2.938163982698783e+00};
	  return c_[i];
	}
	static double d(int i) throw() {
	  static const double d_[]={
	    7.784695709041462e-03,    3.224671290700398e-01,
	    2.445134137142996e+00,    3.754408661907416e+00};
	  return d_[i];
	}
	static const double x_low=0.02425;
	static const double x_high=1.0-0.02425;
	static const double zero=0.0;
	static const double one=1.0;
	static const double one_half=0.5;
	static const double minus_two=-2.0;
      };

      template<>
      struct inv_Phi_traits<long double> {
	static long double a(int i) throw() {
	  static const long double a_[]={
	    -3.969683028665376e+01l,   2.209460984245205e+02l,
	    -2.759285104469687e+02l,   1.383577518672690e+02l,
	    -3.066479806614716e+01l,   2.506628277459239e+00l};
	  return a_[i];
	}
	static long double b(int i) throw() {
	  static const long double b_[]={
	    -5.447609879822406e+01l,   1.615858368580409e+02l,
	    -1.556989798598866e+02l,   6.680131188771972e+01l,
	    -1.328068155288572e+01l};
	  return b_[i];
	}
	static long double c(int i) throw() {
	  static const long double c_[]={
	    -7.784894002430293e-03l,  -3.223964580411365e-01l,
	    -2.400758277161838e+00l,  -2.549732539343734e+00l,
	    4.374664141464968e+00l,    2.938163982698783e+00l};
	  return c_[i];
	}
	static long double d(int i) throw() {
	  static const long double d_[]={
	    7.784695709041462e-03l,    3.224671290700398e-01l,
	    2.445134137142996e+00l,    3.754408661907416e+00l};
	  return d_[i];
	}
	static const long double x_low=0.02425l;
	static const long double x_high=1.0l-0.02425l;
	static const long double zero=0.0l;
	static const long double one=1.0l;
	static const long double one_half=0.5l;
	static const long double minus_two=-2.0l;
      };

      template<typename T>
      T inv_Phi(T x) {
	if (x<inv_Phi_traits<T>::zero && x>inv_Phi_traits<T>::one) {
	  errno=EDOM;
	  return numeric_limits<T>::quiet_NaN();
	} 
	if (x==inv_Phi_traits<T>::zero)
	  return -numeric_limits<T>::infinity();
	if (x==inv_Phi_traits<T>::one)
	  return numeric_limits<T>::infinity();
	T t, q;
	if (x<inv_Phi_traits<T>::x_low) {
	  // Rational approximation for lower region
	  q=sqrt(inv_Phi_traits<T>::minus_two*ln(x));
	  t=(((((inv_Phi_traits<T>::c(0)*q + inv_Phi_traits<T>::c(1))*q +
		inv_Phi_traits<T>::c(2))*q + inv_Phi_traits<T>::c(3))*q +
	      inv_Phi_traits<T>::c(4))*q + inv_Phi_traits<T>::c(5)) /
	    ((((inv_Phi_traits<T>::d(0)*q + inv_Phi_traits<T>::d(1))*q +
	       inv_Phi_traits<T>::d(2))*q + inv_Phi_traits<T>::d(3))*q +
	     inv_Phi_traits<T>::one);
	} else if (x<inv_Phi_traits<T>::x_high) {
	  // Rational approximation for central region
	  q=x-inv_Phi_traits<T>::one_half;
	  T r=q*q;
	  t=(((((inv_Phi_traits<T>::a(0)*r + inv_Phi_traits<T>::a(1))*r + 
		inv_Phi_traits<T>::a(2))*r + inv_Phi_traits<T>::a(3))*r + 
	      inv_Phi_traits<T>::a(4))*r + inv_Phi_traits<T>::a(5))*q /
	    (((((inv_Phi_traits<T>::b(0)*r + inv_Phi_traits<T>::b(1))*r +
		inv_Phi_traits<T>::b(2))*r + inv_Phi_traits<T>::b(3))*r +
	      inv_Phi_traits<T>::b(4))*r + inv_Phi_traits<T>::one);
	} else {
	  // Rational approximation for upper region
	  q=sqrt(inv_Phi_traits<T>::minus_two*ln(1.0-x));
	  t=-(((((inv_Phi_traits<T>::c(0)*q + inv_Phi_traits<T>::c(1))*q +
		 inv_Phi_traits<T>::c(2))*q + inv_Phi_traits<T>::c(3))*q +
	       inv_Phi_traits<T>::c(4))*q + inv_Phi_traits<T>::c(5)) /
	    ((((inv_Phi_traits<T>::d(0)*q + inv_Phi_traits<T>::d(1))*q +
	       inv_Phi_traits<T>::d(2))*q + inv_Phi_traits<T>::d(3))*q +
	     inv_Phi_traits<T>::one);	
	}
	// refinement by Halley rational method
	if (numeric_limits<T>::epsilon()<1e-9) {
	  T e(Phi(t)-x);
	  T u(e*constants<T>::sqrt_2pi()*exp(t*t*inv_Phi_traits<T>::one_half));
	  t-=u/(inv_Phi_traits<T>::one+t*u*inv_Phi_traits<T>::one_half);
	}
	return t;
      }
      
    }

    inline float inv_Phi(float x) {
      return detail::inv_Phi<float>(x);
    }
    
    inline double inv_Phi(double x) {
      return detail::inv_Phi<double>(x);
    }
    
    inline long double inv_Phi(long double x) {
      return detail::inv_Phi<long double>(x);
    }

    // --- inverse of error function  ----------------------------------
    
    // see http://mathworld.wolfram.com/InverseErf.html
    // see The On-Line Encyclopedia of Integer Sequences! 
    // http://www.research.att.com/~njas/sequences/A007019
    // http://www.research.att.com/~njas/sequences/A092676

    inline float inv_erf(float x) {
      if (abs(x)<1.0f/8.0f) {
	x*=0.886226925452758013649085f;  // sqrt(pi)/2
	float x2=x*x, x3=x2*x, x4=x2*x2;
	return x + (1.0f/3.0f + 7.0f/30.0f*x2 + 127.0f/630.0f*x4)*x3;
      }
      return inv_Phi(0.5f*(x+1.0f))*constants<float>::one_over_sqrt_2();
    }
    
    inline double inv_erf(double x) {
      if (abs(x)<1.0/20.0) {
	x*=0.886226925452758013649085;  // sqrt(pi)/2
	double x2=x*x, x3=x2*x, x4=x2*x2, x5=x3*x2;
	return x + (1.0/3.0 + 127.0/630.0*x4)*x3 +
	  (7.0/30.0 + 4369.0/22680.0*x4)*x5;
      }
      return inv_Phi(0.5*(x+1.0))*constants<double>::one_over_sqrt_2();
    }
    
    inline long double inv_erf(long double x) {
      if (abs(x)<1.0l/24.0l) {
	x*=0.886226925452758013649085l;  // sqrt(pi)/2
	long double x2=x*x, x3=x2*x, x4=x2*x2, x5=x3*x2, x7=x3*x4;
	return x + 1.0l/3.0l*x3 + 7.0l/30.0l*x5 + 
	  127.0l/630.0l*x7 + 4369.0l/22680.0l*x4*x5 + 
	  34807.0l/178200.0l*x4*x7;
      }
      return inv_Phi(0.5l*(x+1.0l))*constants<long double>::one_over_sqrt_2();
    }

    // --- inverse of complementary error function  --------------------
    
    inline float inv_erfc(float x) {
      return -inv_Phi(0.5f*x)*constants<float>::one_over_sqrt_2();
    }
    
    inline double inv_erfc(double x) {
      return -inv_Phi(0.5*x)*constants<double>::one_over_sqrt_2();
    }
    
    inline long double inv_erfc(long double x) {
      return -inv_Phi(0.5l*x)*constants<long double>::one_over_sqrt_2();
    }
    
  }

}

#endif
