// Box
#include "box2.h"

/*!
\class Box2 box.h
\brief %Axis aligned box in the plane.

\sa Box
\ingroup PlanarGroup
*/

const double Box2::epsilon = 1.0e-5; //!< Epsilon value used to check intersections and some round off errors.
const Box2 Box2::Infinity(Vector2(-Math::Infinity), Vector2(Math::Infinity)); //!< Huge bounding box, which should enclose any other 
const Box2 Box2::Null = Box2();                     //!< Null box, equivalent to \code Box2(Vector2(0.0)); \endcode
const Box2 Box2::Unit(Vector2(0.0), Vector2(1.0));  //!< Unit box, defined as \code Box2(Vector2(0.0),Vector2(1.0)); \endcode

/*!
\brief Extend the limits of the box by a given distance.

Note that this is the same as performing the Minkowski sum with a cubic box of size r.
\param r Range.
*/
void Box2::Extend(const double& r)
{
  a -= Vector2(r);
  b += Vector2(r);
}

/*!
\brief Extend the limits of the box by a given distance.

Note that this is the same as performing the Minkowski sum with a cubic box of size r.
\param r Range.
*/
Box2 Box2::Extended(const double& r) const
{
  return Box2(a - Vector2(r), b + Vector2(r));
}

/*!
\brief Extend the limits of the box given a point.

If the point lies inside the box, the vertices of the box are unchanged.
\param p Point.
*/
void Box2::Extend(const Vector2& p)
{
  a = Vector2::Min(a, p);
  b = Vector2::Max(b, p);
}

/*!
\brief Overloaded.
\param s Stream.
\param box The box.
*/
std::ostream& operator<<(std::ostream& s, const Box2& box)
{
  s << "Box2(" << box.a << ',' << box.b << ")";
  return s;
}

/*!
\brief Inflates a box so that its dimensions should be a fraction of its maximum side length.

\param n Fraction.
\param x,y Two integers.
*/
void Box2::SetParallelepipedic(int n, int& x, int& y)
{
  // Diagonal
  Vector2 d = (b - a);

  // Maximum side length
  double e = Math::Max(d[0], d[1]);

  double size = e / n;

  SetParallelepipedic(size, x, y);
}

/*!
\brief Creates a parallelepipedic box whose dimensions are integer
multiples of a given input reference size.

\param size Reference size, the dimension of the box will be a multiple of this size.
\param x,y Two integers.
*/
void Box2::SetParallelepipedic(const double& size, int& x, int& y)
{
  // Diagonal
  Vector2 d = (b - a);

  // Integer sizes
  // Bug tracking: adding 0.99 avoids keeping track of which indexes are the maxima 
  x = int(d[0] / size + 0.99);
  y = int(d[1] / size + 0.99);

  // Expand if necessary
  if (x == 0) { x++; }
  if (y == 0) { y++; }

  // Center
  Vector2 c = 0.5 * (a + b);

  // Diagonal
  Vector2 e = Vector2(x, y) * size / 2.0;
  a = c - e;
  b = c + e;
}

/*!
\brief Compute the coordinates of a grid aligned point.

This function computes the coordinates of a point inside the box as if the box was decomposed into a regular grid.

\param i,j Integer coordinates.
\param x,y Virtual grid size.
*/
Vector2 Box2::Vertex(int i, int j, int x, int y) const
{
  return Vector2(a[0] + i * (b[0] - a[0]) / (x - 1), a[1] + j * (b[1] - a[1]) / (y - 1));
}