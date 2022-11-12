# xFD

xFD is a fixed-point library for C++, with support for all arithmetic functions, as well as many scientific functions and constants. Numbers are stored as base-10 in a deque, enabling arbitrary-precision arithmetic. Full support for IEEE-754 special numbers is also available. Say goodbye to [0.1 + 0.2 != 0.3](https://0.30000000000000004.com/) errors - xFD can calculate these values correctly.

The library is desiged for you to use the class as if it was a primitive type, by introducing a user-defined-lieral for constants, and making all math functions static and short to type.

xFD exports the Decimal class, which supports setting the precision and iteration parameters independently from any other Decimal instance. A class for constants, and classes for various sequences, are also exported.

This is a work in progress.

## Example

```
#include <types/Decimal>
#include <iostream>

int main() {
  Decimal a; // `xFD a` also works. There are constructors for all primitive types.
  
  std::cout << a << std::endl; // NaN - decimals are NaN by default
  
  a = 3_D; // Prefix a number string, float, or integer with _D to turn it into a Decimal
  
  int k = a; // 3
  
  1 + a; // Regular numbers are also supported (see below for caveats). Similarly: -, *, /
  
  a / 9_D; // Decimal auto-adjusts the number of decimal places (up to a limit) to capture
           // all decimal places of the quotient.
  
  a / 0 == xFD::Inf(); // You can also get negative infinity using -Inf().
                       // Can also be configured to throw an exception.
   
  xFD::Tan(1_D) == xFDCon::Pi4(); // xFDCon is the constants class. Pi4 generates Pi/4
                                  // Hyperbolic and logarithmic functions are also available.

  xFD::Pow(1) == XFDCon::E(); // Pow with 1 argument evaluates e^x. With two, it computes x^y.
  
  a ^ 3_D == 27_D;  // Expontentation - Pow() also works
 
  xFD::Binomial(10_D, 20_D, 2_D); // (10+20)^2 - Binomial, nCr, nPr, and Factorial are also available.
  
  Decimal x = 3_D, y = 4_D; // Cartesian coordinates
  Decimal r = xFD::Hypot(x, y), theta = xFD::Atan2(x, y); // Convert to polar coordinates
  std::cout "( " << x << ", " << y << ")" << std::endl;
  
  return 0;
}

```

## Building

This is a cross-platform package with no external dependencies. Simply run `make` to compile a shared library of this.

CMake is a work in progress.

## Copyright

xFD is Copyright © 2022 Ali Sherief.

This code is based on [BigFloat](https://github.com/Mariotti94/BigFloat) which is © Gianfranco Mariotti. I fixed some errors in the division method and added an exception type, among other things.

Licensed unther the 3-clause BSD license.
