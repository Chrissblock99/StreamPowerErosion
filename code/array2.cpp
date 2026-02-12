// Arrays

#include "array2.h"

/*!
\class Array2 array.h
\brief A base two-dimensional array structure.

\image html array.png

An array is a two-dimensional rectangle virtually decomposed into cells.

\ingroup StructureGroup
*/

const Vec2I Array2::next[8] = { Vec2I(1, 0), Vec2I(1, 1), Vec2I(0, 1), Vec2I(-1, 1), Vec2I(-1, 0), Vec2I(-1, -1), Vec2I(0, -1), Vec2I(1, -1) };
const double Array2::length[8] = { 1.0, sqrt(2.0), 1.0, sqrt(2.0), 1.0, sqrt(2.0), 1.0, sqrt(2.0) };
const double Array2::inverselength[8] = { 1.0, 1.0 / sqrt(2.0), 1.0, 1.0 / sqrt(2.0), 1.0, 1.0 / sqrt(2.0), 1.0, 1.0 / sqrt(2.0) };

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

/*
int NeighborCodeBIS(int a, int b)
{
  static const int x[9] = { 32,64,128,16,0,1,8,4,2 };
  return x[(b + 1) * 3 + a + 1];
}
*/

/*!
\brief Convert an 8 bit integer code direction into a direction vector.
\param c Direction code.
*/
Vec2I Array2::codeToDir(int c)
{
  int a = 0;
  int b = 0;

  if (c == 1) { a = 1; b = 0; }
  if (c == 2) { a = 1; b = -1; }
  if (c == 4) { a = 0; b = -1; }
  if (c == 8) { a = -1; b = -1; }
  if (c == 16) { a = -1; b = 0; }
  if (c == 32) { a = -1; b = 1; }
  if (c == 64) { a = 0; b = 1; }
  if (c == 128) { a = 1; b = 1; }
  return Vec2I(a, b);
}

/*!
\brief Change the resolution.
 
Increases the resolution by two.
*/
void Array2::Subdivide()
{   
  // Change resolution
  nx = nx * 2 - 1;
  ny = ny * 2 - 1;

  // Change diagonals
  celldiagonal = Vector2((b[0] - a[0]) / (nx - 1), (b[1] - a[1]) / (ny - 1));
  inversecelldiagonal = celldiagonal.Inverse();
}
