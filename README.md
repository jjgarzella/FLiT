# FLiT

[![FLiT Bird](/images/flit-small.png)](https://github.com/PRUNERS/FLiT "FLiT")

Floating-point Litmus Tests (FLiT) is a test infrastructure for detecting varibility
in floating-point code caused by variations in compiler code generation,
hardware and execution environments.

FLiT works by compiling many versions of user-provided test cases, using multiple c++
compilers, floating-point related compiler options (i.e. flags) and optimization
levels.  These tests are then executed on target platforms, where a
representative "score" is collected into a database, along with the other
parameters relevant to the execution, such as host, compiler configuration and
compiler vendor.  In addition to the user-configured test output, FLiT can collect
counts of each assembly opcode executed (currently, this works with Intel
architectures only, using their PIN dynamic binary instrumentation tool).

After executing the test cases and collecting the data, the test results can be
mined to find and demonstrate divergent behavior due to factors other than the
source code.  This information can be used by the developer to understand and
configure their software compilations so that they can obtain consistent and
reproducible floating-point results.

Additionally, we've provided the capability to __time__ the test suite, generating
a time plot per test, comparing both the result divergence and the timing for
each compiler flag.

It consists of the following components:

* a c++ reproducibility test infrastructure
* a dynamic make system to generate diverse compilations
* _(currently broken)_ an execution disbursement system 
* _(currently broken)_ a SQL database for collecting results
  * a collection of queries to help the user understand results
  * some data analysis tools, providing visualization of results

Contents:

* [Installation](documentation/installation.md)
* [Litmus Tests](documentation/litmus-tests.md)
* [FLiT Command-Line](documentation/flit-command-line.md)
* [FLiT Configuration File](documentation/flit-configuration-file.md)
  * [Available Compiler Flags](documentation/available-compiler-flags.md)
* [Writing Test Cases](documentation/writing-test-cases.md)
* [Database Structure](documentation/database-structure.md)
* [Analyze Results](documentation/analyze-results.md)
* **Extra Tools**
  * [Autogenerated Tests](documentation/autogenerated-tests.md)
  * [Test Input Generator](documentation/test-input-generator.md)

