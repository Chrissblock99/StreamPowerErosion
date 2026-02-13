#pragma once

#include "random.h"
#include "evector.h"
#include <vector>

class Box2
{
protected:
  Vector2 a, b; //!< Lower and upper vertices of the box.
public:
  //! Empty
  Box2() {}
  explicit Box2(const Vector2&, const Vector2&);
  explicit Box2(const Vector2&, const double&);

  Vector2& operator[] (int);
  Vector2 operator[] (int) const;

  // Access to vertices
  Vector2 Vertex(int) const;
  Vector2 Vertex(int, int, int, int) const;
  

  friend std::ostream& operator<<(std::ostream&, const Box2&);

public:
  static const double epsilon;
  static const Box2 Null;
  static const Box2 Unit;
};

/*!
\brief Create a box.

Note that is possible to create a box using Vector as parameters
as the compiler will call the constructor Vector2::Vector2(const Vector&).

\param a,b Points.
*/
inline Box2::Box2(const Vector2& a, const Vector2& b)
{
  Box2::a = a;
  Box2::b = b;
}

/*!
\brief Creates a box.

\param c Center.
\param r Radius.
*/
inline Box2::Box2(const Vector2& c, const double& r)
{
  a = c - Vector2(r);
  b = c + Vector2(r);
}

//! Returns either end vertex of the box.
inline Vector2& Box2::operator[] (int i)
{
  if (i == 0) return a;
  else return b;
}

//! Overloaded.
inline Vector2 Box2::operator[] (int i) const
{
  if (i == 0) return a;
  else return b;
}

/*!
\brief Returns the k-th vertex of the box.

The returned vector is computed by analysing the first two bits of k as follows:
\code
Vector2 vertex=Vector2((k&1)?b[0]:a[0],(k&2)?b[1]:a[1]);
\endcode
*/
inline Vector2 Box2::Vertex(int k) const
{
  return Vector2((k & 1) ? b[0] : a[0], (k & 2) ? b[1] : a[1]);
}
