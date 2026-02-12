// Mathematics
#include <ostream>
#include <limits>
#include "mathematics.h"

/*!
\defgroup MathGroup Core math classes.

\brief Core math classes include several classes such as Vector, Quadric, Cubic and higher order
polynomials and many others that are useful in many graphic applications.
*/

/*!
\class Math mathematics.h
\brief Core class implementing some useful functions and constants.

<P><I>How can I use the constant Pi?</I>
<BR>Simply use the static constant Math::Pi as follows:
\code
double v = 4.0*Math::Pi*r*r*r/3.0; // Volume of a sphere.
\endcode
Note that in this case, you could also have used:
\code
double v = Sphere::Volume(r);
\endcode

<P><I>How many min/max functions have been implemented?</I>
<BR>Up to four arguments are supported; for a larger number of
arguments, a specific routine operating on an array should be written.

<P><I>Is there a function to compute the square of a real sumber?</I>
<BR>Use the following:
\code
double s = Math::Sqr((1.0-sqrt(5.0))/2.0); // Square of golden ratio
\endcode

<P><I>Are there predefined square roots constants?</I>
<BR>The sqrt function used not te be constexpr, so square roots of reals are not computed at compilation time.
Some constants are provided, such as the following one:
\code
double s = Math::Sqrt3; // Square root of 3
\endcode

<P><I>What is the relative performance of mathematical functions such as square root or cosine?</I>
<BR>The relative cost of commonly used mathematical functions is summarized in the following table.

<table>
<caption>Cost</caption>
<tr><th>Function<th>Cost
<tr><td>+ - * fabs<td>1
<tr><td>/ sqrt modf <td>~4.8
<tr><td>/ sin cos <td>~8.9
<tr><td>/ exp log <td>~10.5
</table>

Math provides some functions such as Math::Sqrt32 or Math::Sqrt4 to avoid computationally intensive standard functions like pow:
\code
double y = Math::Sqrt32(x); // Faster than double y = pow(x,1.5);
\endcode

\ingroup MathGroup

<P><I>How are implemented the step and smooth-step functions that are often used in procedural modeling?</I>
Different smoothing kernels, such as Cubic::Smooth(), are implented in odd-degree polynomials Cubic, Quintic and Septic.
The corresponding step functions, such as Cubic::SmoothStep(), are also implemented.
\sa Linear::Step, Cubic::Smooth, Quintic::Smooth, Cubic::SmoothStep, Quintic::SmoothStep, Septic::SmoothStep.

*/

const double Math::Pi = 3.14159265358979323846;

const double Math::TwoPi = 6.28318530717958647693;

const double Math::HalfPi = Math::Pi / 2.0;

const double Math::e = 2.7182818284590452354;

const double Math::TwoPiOverThree = 2.0943951023931954923084;

const double Math::FourPiOverThree = 4.1887902047863909846168;

const double Math::Infinity = std::numeric_limits<double>::max();

const double Math::Large = 1.0e20;

const double Math::Sqrt5 = sqrt(5.0);

const double Math::Sqrt3 = sqrt(3.0);

const double Math::Sqrt2 = sqrt(2.0);

const double Math::Golden = (sqrt(5.0) + 1.0) / 2.0;


/*!
\brief Sine wave over unit interval.
\param x Real value.
*/
double Math::Cycloidal(const double& x)
{
  return sin(x - floor(x) * 2.0 * Math::Pi);
}

/*!
\brief %Triangle wave over unit interval.
\param x Real value.
*/
double Math::Triangle(const double& x)
{
  double offset;

  if (x >= 0.0)
  {
    offset = x - floor(x);
  }
  else
  {
    offset = x - (-1.0 - floor(fabs(x)));
  }
  if (offset >= 0.5)
  {
    return (2.0 * (1.0 - offset));
  }
  else
  {
    return (2.0 * offset);
  }
}