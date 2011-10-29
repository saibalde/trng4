// Copyright (c) 2000-2010, Heiko Bauke
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 
//   * Redistributions of source code must retain the above copyright
//     notice, this list of conditions and the following disclaimer.  
// 
//   * Redistributions in binary form must reproduce the above
//     copyright notice, this list of conditions and the following
//     disclaimer in the documentation and/or other materials provided
//     with the distribution.  
// 
//   * Neither the name of the copyright holder nor the names of its
//     contributors may be used to endorse or promote products derived
//     from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.

#if !(defined TRNG_POWERLAW_DIST_HPP)

#define TRNG_POWERLAW_DIST_HPP

#include <trng/limits.hpp>
#include <trng/utility.hpp>
#include <trng/math.hpp>
#include <ostream>
#include <istream>
#include <iomanip>
#include <cerrno>

namespace trng {

  // uniform random number generator class
  template<typename float_t=double>
  class powerlaw_dist {
  public:
    typedef float_t result_type;
    class param_type;
    
    class param_type {
    private:
      result_type gamma_, theta_;
    public:
      result_type gamma() const { return gamma_; }
      void gamma(result_type gamma_new) { gamma_=gamma_new; }
      result_type theta() const { return theta_; }
      void theta(result_type theta_new) { theta_=theta_new; }
      param_type() : gamma_(1), theta_(1) {
      }
      param_type(result_type gamma, result_type theta) : gamma_(gamma), theta_(theta) {
      }

      friend class powerlaw_dist;

      // Streamable concept
      template<typename char_t, typename traits_t>
      friend std::basic_ostream<char_t, traits_t> &
      operator<<(std::basic_ostream<char_t, traits_t> &out,
                 const param_type &p) {
        std::ios_base::fmtflags flags(out.flags());
        out.flags(std::ios_base::dec | std::ios_base::fixed |
                  std::ios_base::left);
        out << '('
            << std::setprecision(math::numeric_limits<float_t>::digits10+1) 
            << p.gamma() << ' ' << p.theta() 
            << ')';
        out.flags(flags);
        return out;
      }
      
      template<typename char_t, typename traits_t>
      friend std::basic_istream<char_t, traits_t> &
      operator>>(std::basic_istream<char_t, traits_t> &in,
                 param_type &p) {
        float_t gamma, theta;
        std::ios_base::fmtflags flags(in.flags());
        in.flags(std::ios_base::dec | std::ios_base::fixed |
                 std::ios_base::left);
        in >> utility::delim('(')
	   >> gamma >> utility::delim(' ')
	   >> theta >> utility::delim(')');
        if (in)
          p=param_type(gamma, theta);
        in.flags(flags);
        return in;
      }

    };
    
  private:
    param_type p;
    
  public:
    // constructor
    powerlaw_dist(result_type gamma, result_type theta) : p(gamma, theta) {
    }
    explicit powerlaw_dist(const param_type &p) : p(p) {
    }
    // reset internal state
    void reset() { }
    // random numbers
    template<typename R>
    result_type operator()(R &r) {
      return p.theta()*math::pow(utility::uniformoc<result_type>(r), -1/p.gamma());
    }
    template<typename R>
    result_type operator()(R &r, const param_type &p) {
      powerlaw_dist g(p);
      return g(r);
    }
    // property methods
    result_type min() const { return p.theta(); }
    result_type max() const { return math::numeric_limits<result_type>::infinity(); }
    param_type param() const { return p; }
    void param(const param_type &p_new) { p=p_new; }
    result_type gamma() const { return p.gamma(); }
    void gamma(result_type gamma_new) { p.gamma(gamma_new); }
    result_type theta() const { return p.theta(); }
    void theta(result_type theta_new) { p.theta(theta_new); }
    // probability density function  
    result_type pdf(result_type x) const {
      if (x<p.theta())
	return 0;
      else
	return p.gamma()/p.theta()*
	  math::pow(x/p.theta(), -p.gamma()-1);
    }
    // cumulative density function 
    result_type cdf(result_type x) const {
      if (x<=0)
	return 0;
      else
	return 1-math::pow(x/p.theta(), -p.gamma());
    }
    // inverse cumulative density function 
    result_type icdf(result_type x) const {
      if (x<=0 or x>=1) {
	errno=EDOM;
	return math::numeric_limits<result_type>::quiet_NaN();
      }
      if (x==0)
	return p.theta();
      if (x==1)
	return math::numeric_limits<result_type>::infinity();
      return p.theta()*math::pow(1-x, -1/p.gamma());
    }
  };
  
  // -------------------------------------------------------------------

  // EqualityComparable concept
  template<typename float_t>
  inline bool operator==(const typename powerlaw_dist<float_t>::param_type &p1, 
			 const typename powerlaw_dist<float_t>::param_type &p2) {
    return p1.gamma()==p2.gamma() and p1.theta()==p2.theta();
  }

  template<typename float_t>
  inline bool operator!=(const typename powerlaw_dist<float_t>::param_type &p1, 
			 const typename powerlaw_dist<float_t>::param_type &p2) {
    return not (p1==p2);
  }
    
  // -------------------------------------------------------------------

  // EqualityComparable concept
  template<typename float_t>
  inline bool operator==(const powerlaw_dist<float_t> &g1, 
			 const powerlaw_dist<float_t> &g2) {
    return g1.param()==g2.param();
  }

  template<typename float_t>
  inline bool operator!=(const powerlaw_dist<float_t> &g1, 
			 const powerlaw_dist<float_t> &g2) {
    return g1.param()!=g2.param();
  }
  
  // Streamable concept
  template<typename char_t, typename traits_t, typename float_t>
  std::basic_ostream<char_t, traits_t> &
  operator<<(std::basic_ostream<char_t, traits_t> &out,
	     const powerlaw_dist<float_t> &g) {
    std::ios_base::fmtflags flags(out.flags());
    out.flags(std::ios_base::dec | std::ios_base::fixed |
	      std::ios_base::left);
    out << "[powerlaw " << g.param() << ']';
    out.flags(flags);
    return out;
  }
  
  template<typename char_t, typename traits_t, typename float_t>
  std::basic_istream<char_t, traits_t> &
  operator>>(std::basic_istream<char_t, traits_t> &in,
	     powerlaw_dist<float_t> &g) {
    typename powerlaw_dist<float_t>::param_type p;
    std::ios_base::fmtflags flags(in.flags());
    in.flags(std::ios_base::dec | std::ios_base::fixed |
	     std::ios_base::left);
    in >> utility::ignore_spaces()
       >> utility::delim("[powerlaw ") >> p >> utility::delim(']');
    if (in)
      g.param(p);
    in.flags(flags);
    return in;
  }
  
}

#endif
