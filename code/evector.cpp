
// Vector  

#include "random.h"

// Self include
#include "evector.h"

/*!
\class Vector evector.h
\brief Vectors in three dimensions.

Most binary operators have been overloaded as expected,
destructive operators, such as addition and subtraction
have been implemented and behave as one could expect.

<P><I>How do I compute the cross product of two vectors?</I>
<BR>Simply use the overloaded Vector::operator/, for instance
\code
Vector c=a/b; // Cross product
\endcode
computes the cross product of a and b.
<P><I>How can I get access to the x, y and z components of a vector?</I>
<BR>Use v[0], v[1] and v[2] to get access to the x, y and z components of a vector v respectively.
<P><I>How do I compute the normal of a triangle?</I>
<BR>Let a,b,c the vertices of the triangle, simply compute the cross product
\code
Vector n=(a-b)/(a-c);  // Cross product
\endcode
or use the member function of the Triangle class:
\code
Vector n=Triangle(a,b,c).Normal(); // Compute the normal
\endcode

\ingroup MathGroup
*/

const Vector Vector::Null = Vector(0.0, 0.0, 0.0);
const Vector Vector::X = Vector(1.0, 0.0, 0.0);
const Vector Vector::Y = Vector(0.0, 1.0, 0.0);
const Vector Vector::Z = Vector(0.0, 0.0, 1.0);


/*!
\brief Overloaded output-stream operator.
\param u Vector.
\param s Stream.
*/
std::ostream& operator<<(std::ostream& s, const Vector& u)
{
  s << "Vector(" << u.c[0] << ',' << u.c[1] << ',' << u.c[2] << ')';
  return s;
}
