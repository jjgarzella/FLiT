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
 * -- LICENSE END -- */
#include "Shewchuk.h"

#include <flit.h>

#include <iomanip>
#include <string>
#include <vector>

template <typename T>
class ShewchukSum : public flit::TestBase<T> {
public:
  ShewchukSum(std::string id) : flit::TestBase<T>(std::move(id)) {}

  virtual size_t getInputsPerRun() override { return 1000; }
  virtual std::vector<T> getDefaultInput() override;

protected:
  virtual flit::Variant run_impl(const std::vector<T>& ti) override {
    Shewchuk<T> chuk;
    T naive = 0.0;
    for (auto val : ti) {
      chuk.add(val);
      naive += val;
			flit::info_stream
				<< id << ":   partials now: (" << chuk.partials().size() << ") ";
      for (auto p : chuk.partials()) {
        flit::info_stream << " " << p;
      }
      flit::info_stream << std::endl;
    }
    T sum = chuk.sum();
    flit::info_stream << id << ": naive sum         = " << naive << std::endl;
    flit::info_stream << id << ": shewchuk sum      = " << sum << std::endl;
    flit::info_stream << id << ": shewchuk partials = " << chuk.partials().size() << std::endl;
    return sum;
  }

protected:
  using flit::TestBase<T>::id;
};

namespace {
  template<typename T> std::vector<T> getToRepeat();
  template<> std::vector<float> getToRepeat() { return { 1.0, 1.0e8, 1.0, -1.0e8 }; }
  template<> std::vector<double> getToRepeat() { return { 1.0, 1.0e100, 1.0, -1.0e100 }; }
  template<> std::vector<long double> getToRepeat() { return { 1.0, 1.0e200, 1.0, -1.0e200 }; }
} // end of unnamed namespace

template <typename T>
std::vector<T> ShewchukSum<T>::getDefaultInput() {
  auto dim = getInputsPerRun();
  std::vector<T> ti(dim);
  auto toRepeat = getToRepeat<T>();
  for (decltype(dim) i = 0, j = 0;
       i < dim;
       i++, j = (j+1) % toRepeat.size())
  {
    ti[i] = toRepeat[j];
  }
  return ti;
}

REGISTER_TYPE(ShewchukSum)
