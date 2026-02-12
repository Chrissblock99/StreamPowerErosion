// Vector  

// Self include
#include "evector.h"

/*!
\class Vector2 evector.h
\brief Vectors in two dimensions.

This class implements most operators and member functions as for Vector class.

\ingroup MathGroup
*/

const Vector2 Vector2::Null = Vector2(0.0, 0.0);

const Vector2 Vector2::X = Vector2(1.0, 0.0);
const Vector2 Vector2::Y = Vector2(0.0, 1.0);


/*!
\brief Overloaded output-stream operator.
\param u %Vector.
\param s Stream.
*/
std::ostream& operator<<(std::ostream& s, const Vector2& u)
{
    s << "Vector2(" << u.c[0] << ',' << u.c[1] << ')';
    return s;
}