// Arrays

#include "array2.h"

/*!
\class Array2 array.h
\brief A base two-dimensional array structure.

\image html array.png

An array is a two-dimensional rectangle virtually decomposed into cells.

\ingroup StructureGroup
*/

/*!
\brief Empty array, with empty box.
*/
Array2::Array2() :Box2(Box2::Null)
{
  nx = 0;
  ny = 0;
  celldiagonal = Vector2::Null;
  inversecelldiagonal = Vector2::Null;
}

/*!
\brief Create the array structure.
\param box The box.
\param x,y Size of the array.
*/
Array2::Array2(const Box2& box, int x, int y) :Box2(box)
{
  nx = x;
  ny = y;
  celldiagonal = Vector2((b[0] - a[0]) / (nx - 1), (b[1] - a[1]) / (ny - 1));
  inversecelldiagonal = celldiagonal.Inverse();
}

/*!
\brief Create the array structure.
\param box The box.
\param n Size of the array.
*/
Array2::Array2(const Box2& box, int n) :Array2(box, n, n)
{
}

/*!
\brief Overloaded.
\param s Stream.
\param a The array.
*/
std::ostream& operator<<(std::ostream& s, const Array2& a)
{
  s << "Array2(" << a.a << ',' << a.b << "," << a.nx << ',' << a.ny << ')';
  return s;
}

/*!
\brief Return the cell diagonal vector.
*/
Vector2 Array2::CellDiagonal() const
{
  return celldiagonal;
}