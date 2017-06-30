#include <flit.h>

#include <vector>

#include <cstdlib>

template <typename T>
class ReciprocalMath : public flit::TestBase<T> {
public:
  ReciprocalMath(std::string id) : flit::TestBase<T>(std::move(id)) {}

  virtual size_t getInputsPerRun() override { return 5; }

  virtual flit::TestInput<T> getDefaultInput() override {
    flit::TestInput<T> ti;
    ti.vals = { .1, 1.1e3, -.1, -1.1e3, 1/3 };
    return ti;
  }

protected:
  virtual flit::Variant run_impl(const flit::TestInput<T>& ti) override {
    T a = ti.vals[0];
    T b = ti.vals[1];
    T c = ti.vals[2];
    T d = ti.vals[3];
    T m = ti.vals[4];

    a = a/m;
    b = b/m;
    c = c/m;
    d = d/m;

    const T score = a + b + c + d;

    flit::info_stream << id << ": score  = " << score  << std::endl;

    return score;
  }

protected:
  using flit::TestBase<T>::id;
};


REGISTER_TYPE(ReciprocalMath)
