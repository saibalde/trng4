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

#include <trng/yarn3s.hpp>

namespace trng {

  // Uniform random number generator concept

  // Parameter and status classes

  // EqualityComparable concept
  bool operator==(const yarn3s::parameter_type &P1,
                  const yarn3s::parameter_type &P2) {
    return P1.a1==P2.a1 and P1.a2==P2.a2 and P1.a3==P2.a3;
  }

  bool operator!=(const yarn3s::parameter_type &P1,
                  const yarn3s::parameter_type &P2) {
    return not (P1==P2);
  }

  // Equality comparable concept
  bool operator==(const yarn3s::status_type &S1,
                  const yarn3s::status_type &S2) {
    return S1.r1==S2.r1 and S1.r2==S2.r2 and S1.r3==S2.r3;
  }

  bool operator!=(const yarn3s::status_type &S1,
                  const yarn3s::status_type &S2) {
    return not (S1==S2);
  }

  const yarn3s::parameter_type
  yarn3s::trng0=parameter_type(2025213985l, 1112953677l, 2038969601l);
  const yarn3s::parameter_type
  yarn3s::trng1=parameter_type(1287767370l, 1045931779l, 58150106l);
  
  // Random number engine concept
  yarn3s::yarn3s(yarn3s::parameter_type P) :
    P(P), S() { }

  yarn3s::yarn3s(unsigned long s, yarn3s::parameter_type P) :
    P(P), S() {
    seed(s);
  }

  void yarn3s::seed() {
    (*this)=yarn3s();
  }

  void yarn3s::seed(unsigned long s) {
    long long t=s;
    t%=modulus;
    if (t<0)
      t+=modulus;
    S.r1=static_cast<result_type>(t);
    S.r2=1;
    S.r3=1;
  }

  void yarn3s::seed(yarn3s::result_type s1, yarn3s::result_type s2,
		   yarn3s::result_type s3) {
    S.r1=s1%modulus;
    if (S.r1<0)
      S.r1+=modulus;
    S.r2=s2%modulus;
    if (S.r2<0)
      S.r2+=modulus;
    S.r3=s3%modulus;
    if (S.r3<0)
      S.r3+=modulus;
  }

  // Equality comparable concept
  bool operator==(const yarn3s &R1, const yarn3s &R2) {
    return R1.P==R2.P and R1.S==R2.S;
  }

  bool operator!=(const yarn3s &R1, const yarn3s &R2) {
    return not (R1==R2);
  }

  // Parallel random number generator concept
  void yarn3s::split(unsigned int s, unsigned int n) {
    if (s<1 or n>=s)
      throw std::invalid_argument("invalid argument for trng::yarn3s::split");
    if (s>1) {
      jump(n+1);  long q0=S.r1;
      jump(s);    long q1=S.r1;
      jump(s);    long q2=S.r1;
      jump(s);    long q3=S.r1;
      jump(s);    long q4=S.r1;
      jump(s);    long q5=S.r1;
      std::vector<long> a(3), b(9);
      a[0]=q3;  b[0]=q2;  b[1]=q1;  b[2]=q0;
      a[1]=q4;  b[3]=q3;  b[4]=q2;  b[5]=q1;
      a[2]=q5;  b[6]=q4;  b[7]=q3;  b[8]=q2;
      utility::gauss(b, a, modulus);
      P.a1=a[0];  P.a2=a[1];  P.a3=a[2];
      S.r1=q2;    S.r2=q1;    S.r3=q0;
      backward();
      backward();
      backward();
    }
  }
  
  void yarn3s::jump2(unsigned int s) {
    std::vector<long> b(9), c(9), d(3), r(3);
    long t1(P.a1), t2(P.a2), t3(P.a3);
    b[0]=P.a1;  b[1]=P.a2;  b[2]=P.a3;
    b[3]=1l;    b[4]=0l;    b[5]=0l;
    b[6]=0l;    b[7]=1l;    b[8]=0l;
    for (unsigned int i(0); i<s; ++i)
      if ((i&1)==0)
	utility::matrix_mult(b, b, c, modulus);
      else
	utility::matrix_mult(c, c, b, modulus);
    r[0]=S.r1;  r[1]=S.r2;  r[2]=S.r3;
    if ((s&1)==0)
      utility::matrix_vec_mult(b, r, d, modulus);
    else
      utility::matrix_vec_mult(c, r, d, modulus);
    S.r1=d[0];  S.r2=d[1];  S.r3=d[2];
    P.a1=t1;    P.a2=t2;    P.a3=t3;
  }

  void yarn3s::jump(unsigned long long s) {
    if (s<16) {
      for (unsigned int i(0); i<s; ++i) 
	step();
    } else {
      unsigned int i(0);
      while (s>0) {
	if (s%2==1)
	  jump2(i);
	++i;
	s>>=1;
      }
    }
  }

  // other useful methods
  const char * const yarn3s::name_str="yarn3s";
  
  const char * yarn3s::name() {
    return name_str;
  }

  void yarn3s::backward() {
    long t;
    if (P.a3!=0l) {
      t=S.r1;
      t-=static_cast<long>((static_cast<long long>(P.a1)*
			    static_cast<long long>(S.r2))%modulus);
      if (t<0l)
	t+=modulus;
      t-=static_cast<long>((static_cast<long long>(P.a2)*
			    static_cast<long long>(S.r3))%modulus);
    if (t<0l)
      t+=modulus;
    t=static_cast<long>((static_cast<long long>(t)*
                         static_cast<long long>
                         (utility::modulo_invers(P.a3, modulus)))%modulus);
    } else if (P.a2!=0l) {
      t=S.r2;
      t-=static_cast<long>((static_cast<long long>(P.a1)*
			    static_cast<long long>(S.r3))%modulus);
      if (t<0l)
	t+=modulus;
      t=static_cast<long>((static_cast<long long>(t)*
			   static_cast<long long>
			   (utility::modulo_invers(P.a2, modulus)))%modulus);
    } else if (P.a1!=0l) {
      t=S.r3;
      t=static_cast<long>((static_cast<long long>(t)*
			   static_cast<long long>
			   (utility::modulo_invers(P.a1, modulus)))%modulus);
    } else
      t=0l;
    S.r1=S.r2;  S.r2=S.r3;  S.r3=t;
  }

  utility::power<yarn3s::modulus, yarn3s::gen> yarn3s::parameter_type::g;

}
