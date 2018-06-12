# FLiT

[![FLiT Bird](/images/flit-small.png)](https://github.com/PRUNERS/FLiT "FLiT")

Floating-point Litmus Tests (FLiT) is a C++ test infrastructure for detecting
variability in floating-point code caused by variations in compiler code
generation, hardware and execution environments.

Originally, FLiT stood for "Floating-point Litmus Tests", but has grown into a
tool with much more flexability than to study simple litmus tests.  However, it
has always been the focus of FLiT to study the variability caused by compilers.
That brings us to the other reason for the name, "flit" is defined by the
Merriam Webster dictionary as "to pass quickly or abruptly from one place or
condition to another".  This fits in well with testing for various sources of
variability.

Compilers are primarily focused on optimizing the speed of your code.  However,
when it comes to floating-point, compilers go a little further than some might
want, to the point that you may not get the same result from your
floating-point algorithms.  For many applications, this may not matter as the
difference is typically very small.  But there are situations where

1. The difference is not so small
2. Your application cares even about small differences, or
3. Your application is so large (such as a weather simulation) that a small
   change may propagate into very large result variability.

FLiT helps developers determine where reproducibility problems may occur due to
compilers.  The developer creates reproducibility tests with their code using
the FLiT testing framework.  Then FLiT takes those reproducibility tests and
compiles them under a set of configured compilers and a large range of compiler
flags.  The results from the tests under different compilations are then compared
against the results from a "ground truth" compilation (e.g. a completely
unoptimized compilation).

More than simply comparing against a "ground truth" test result, the FLiT
framework also measures runtime of your tests.  Using this information, you can
not only determine which compilations of your code are safe for your specific
application, but you can also determine the fastest safe compilation.  This
ability helps the developer navigate the tradeoff between reproducibility and
performance.

It consists of the following components:

* a C++ reproducibility test infrastructure
* a dynamic make system to generate diverse compilations
* an SQLite database containing results
* tools to help analyze test results
* a bisection tool that can isolate the file(s) and function(s) where
  variability was introduced by the compiler.

Contents:

* [Installation](documentation/installation.md)
* [Litmus Tests](documentation/litmus-tests.md)
* [FLiT Command-Line](documentation/flit-command-line.md)
* [FLiT Configuration File](documentation/flit-configuration-file.md)
  * [Available Compiler Flags](documentation/available-compiler-flags.md)
* [Writing Test Cases](documentation/writing-test-cases.md)
* [Test Executable](documentation/test-executable.md)
* [Benchmarks](documentation/benchmarks.md)
* [Database Structure](documentation/database-structure.md)
* [Analyze Results](documentation/analyze-results.md)
* **Extra Tools**
  * [Autogenerated Tests](documentation/autogenerated-tests.md)
  * [Test Input Generator](documentation/test-input-generator.md)

