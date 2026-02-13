#pragma once

#include "box2.h"
#include "ivector.h"

class Array2 : protected Box2
{
protected:
  int nx=0, ny=0; //!< Sizes.
  Vector2 celldiagonal= Vector2::Null; //!< Cell diagonal.
public:
  Array2();
  explicit Array2(const Box2&, int, int);
  explicit Array2(const Box2&, int);

  //! Empty
  ~Array2() {}

  int VertexSize() const;
  int GetSizeX() const;
  int GetSizeY() const;

  Vector2 Size() const;

  // Empty
  bool IsEmpty() const;

  friend std::ostream& operator<<(std::ostream&, const Array2&);

  // Indexes for storing elements at vertices
  constexpr int VertexIndex(int, int) const;
  int VertexIndex(const Vec2I&) const;
};

/*!
\brief Return the size of the array.
*/
inline Vector2 Array2::Size() const
{
  return Box2::Size();
}

/*!
\brief Detect if the array is empty, i.e., any dimension equal to zero.
*/
inline bool Array2::IsEmpty() const
{
  return (nx <= 0) || (ny <= 0);
}

/*!
\brief Get the vertex size of the array for x axis.
*/
inline int Array2::GetSizeX() const
{
  return nx;
}

/*!
\brief Get the vertex size of the array for y axis.
*/
inline int Array2::GetSizeY() const
{
  return ny;
}

/*!
\brief Return the size of the vertex array.
*/
inline int Array2::VertexSize() const
{
  return nx * ny;
}

/*!
\brief Compute the index of a given cell.
\param i,j Integer coordinates of the cell.
*/
inline constexpr int Array2::VertexIndex(int i, int j) const
{
  return i + nx * j;
}
/*!
\brief Compute the index of a given cell.
\param p Point.
*/
inline int Array2::VertexIndex(const Vec2I& p) const
{
  return p[0] + nx * p[1];
}