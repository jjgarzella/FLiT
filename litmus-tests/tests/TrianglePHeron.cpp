/* -- LICENSE BEGIN --
 *
 * Copyright (c) 2015-2018, Lawrence Livermore National Security, LLC.
 *
 * Produced at the Lawrence Livermore National Laboratory
 *
 * Written by
 *   Michael Bentley (mikebentley15@gmail.com),
 *   Geof Sawaya (fredricflinstone@gmail.com),
 *   and Ian Briggs (ian.briggs@utah.edu)
 * under the direction of
 *   Ganesh Gopalakrishnan
 *   and Dong H. Ahn.
 *
 * LLNL-CODE-743137
 *
 * All rights reserved.
 *
 * This file is part of FLiT. For details, see
 *   https://pruners.github.io/flit
 * Please also read
 *   https://github.com/PRUNERS/FLiT/blob/master/LICENSE
 *
 * Redistribution and use in source and binary forms, with or
 * without modification, are permitted provided that the following
 * conditions are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the disclaimer below.
 *
 * - Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the disclaimer
 *   (as noted below) in the documentation and/or other materials
 *   provided with the distribution.
 *
 * - Neither the name of the LLNS/LLNL nor the names of its
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL LAWRENCE LIVERMORE NATIONAL
 * SECURITY, LLC, THE U.S. DEPARTMENT OF ENERGY OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Additional BSD Notice
 *
 * 1. This notice is required to be provided under our contract
 *    with the U.S. Department of Energy (DOE). This work was
 *    produced at Lawrence Livermore National Laboratory under
 *    Contract No. DE-AC52-07NA27344 with the DOE.
 *
 * 2. Neither the United States Government nor Lawrence Livermore
 *    National Security, LLC nor any of their employees, makes any
 *    warranty, express or implied, or assumes any liability or
 *    responsibility for the accuracy, completeness, or usefulness of
 *    any information, apparatus, product, or process disclosed, or
 *    represents that its use would not infringe privately-owned
 *    rights.
 *
 * 3. Also, reference herein to any specific commercial products,
 *    process, or services by trade name, trademark, manufacturer or
 *    otherwise does not necessarily constitute or imply its
 *    endorsement, recommendation, or favoring by the United States
 *    Government or Lawrence Livermore National Security, LLC. The
 *    views and opinions of authors expressed herein do not
 *    necessarily state or reflect those of the United States
 *    Government or Lawrence Livermore National Security, LLC, and
 *    shall not be used for advertising or product endorsement
 *    purposes.
 *
 * -- LICENSE END --
 */

#include <flit.h>

#include <typeinfo>

#include <cmath>

namespace {
  int g_iters = 200;
}

template <typename T>
DEVICE
T getCArea(const T a,
          const T b,
          const T c){
    T s = (a + b + c ) / 2;
    return flit::csqrt((T)s * (s-a) * (s-b) * (s-c));
}

template <typename T>
T getArea(const T a,
	  const T b,
	  const T c){
  T s = (a + b + c ) / 2;
  return sqrt(s * (s-a) * (s-b) * (s-c));
}

template <typename T>
GLOBAL
void
TrianglePHKern(const T* tiList, size_t n, double* results) {
#ifdef __CUDA__
  auto idx = blockIdx.x * blockDim.x + threadIdx.x;
#else
  auto idx = 0;
#endif
  auto start = tiList + (idx*n);
  T maxval = start[0];
  T a = maxval;
  T b = maxval;
  T c = maxval * flit::csqrt((T)2.0);
  const T delta = maxval / T(g_iters);
  const T checkVal = (T)0.5 * b * a;

  double score = 0.0;

  for(T pos = 0; pos <= a; pos += delta){
    b = flit::csqrt(flit::cpow(pos, (T)2.0) +
	      flit::cpow(maxval, (T)2.0));
    c = flit::csqrt(flit::cpow(a - pos, (T)2.0) +
	      flit::cpow(maxval, (T)2.0));
    auto crit = getCArea(a,b,c);
    score += std::abs(crit - checkVal);
  }
  results[idx] = score;
}

template <typename T>
class TrianglePHeron: public flit::TestBase<T> {
public:
  TrianglePHeron(std::string id) : flit::TestBase<T>(std::move(id)) {}

  virtual size_t getInputsPerRun() override { return 1; }
  virtual std::vector<T> getDefaultInput() override {
    return { 6.0 };
  }

protected:
  virtual flit::KernelFunction<T>* getKernel() override {return TrianglePHKern; }

  virtual flit::Variant run_impl(const std::vector<T>& ti) override {
    T maxval = ti[0];
    // start as a right triangle
    T a = maxval;
    T b = maxval;
    T c = maxval * std::sqrt(2);
    const T delta = maxval / T(g_iters);

    // 1/2 b*h = A
    // all perturbations will have the same base and height (plus some FP noise)
    const T checkVal = 0.5 * b * a;

    long double score = 0;

    for(T pos = 0; pos <= a; pos += delta){
      b = std::sqrt(std::pow(pos, 2) +
                    std::pow(maxval, 2));
      c = std::sqrt(std::pow(a - pos, 2) +
                    std::pow(maxval, 2));
      auto crit = getArea(a,b,c);
      score += std::abs(crit - checkVal);
    }
    return score;
  }

protected:
  using flit::TestBase<T>::id;
};

REGISTER_TYPE(TrianglePHeron)

