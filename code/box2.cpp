// Box
#include "box2.h"

/*!
\class Box2 box.h
\brief %Axis aligned box in the plane.

\sa Box
\ingroup PlanarGroup
*/

const double Box2::epsilon = 1.0e-5; //!< Epsilon value used to check intersections and some round off errors.
const Box2 Box2::Null = Box2();                     //!< Null box, equivalent to \code Box2(Vector2(0.0)); \endcode
const Box2 Box2::Unit(Vector2(0.0), Vector2(1.0));  //!< Unit box, defined as \code Box2(Vector2(0.0),Vector2(1.0)); \endcode


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
\brief Compute the coordinates of a grid aligned point.

This function computes the coordinates of a point inside the box as if the box was decomposed into a regular grid.

\param i,j Integer coordinates.
\param x,y Virtual grid size.
*/
Vector2 Box2::Vertex(int i, int j, int x, int y) const
{
  return Vector2(a[0] + i * (b[0] - a[0]) / (x - 1), a[1] + j * (b[1] - a[1]) / (y - 1));
}