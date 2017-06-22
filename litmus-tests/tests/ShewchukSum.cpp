#include "Shewchuk.h"

#include <flit.h>

#include <iomanip>
#include <string>
#include <vector>

template <typename T>
class ShewchukSum : public flit::TestBase<T> {
public:
  ShewchukSum(std::string id) : flit::TestBase<T>(std::move(id)) {}
  
  virtual size_t getInputsPerRun() { return 1000; }
  virtual flit::TestInput<T> getDefaultInput();

protected:
  virtual long double run_impl(const flit::TestInput<T>& ti) {
    Shewchuk<T> chuk;
    T naive = 0.0;
    for (auto val : ti.vals) {
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
#ifndef __CUDA__
  template<> std::vector<long double> getToRepeat() { return { 1.0, 1.0e200, 1.0, -1.0e200 }; }
#endif
} // end of unnamed namespace

template <typename T>
flit::TestInput<T> ShewchukSum<T>::getDefaultInput() {
  flit::TestInput<T> ti;
  auto dim = getInputsPerRun();
  ti.highestDim = dim;
  ti.vals = std::vector<T>(dim);
  auto toRepeat = getToRepeat<T>();
  for (decltype(dim) i = 0, j = 0;
       i < dim;
       i++, j = (j+1) % toRepeat.size()) {
    ti.vals[i] = toRepeat[j];
  }
  return ti;
}

REGISTER_TYPE(ShewchukSum)
