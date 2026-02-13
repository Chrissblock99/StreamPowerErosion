// Vector  

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

#ifndef __Vector__
#define __Vector__

#include <math.h>
#include <iostream>

// Class
class Vector
{
protected:
  double c[3]; //!< Components.
public:
  //! Empty 
  Vector() {}

  explicit Vector(const double&);
  explicit Vector(const double&, const double&, const double&);

  // Access members
  constexpr double& operator[] (int);
  constexpr double operator[] (int) const;

  // Unary operators
  Vector operator+ () const;
  Vector operator- () const;

  // Assignment operators
  Vector& operator+= (const Vector&);
  Vector& operator-= (const Vector&);
  Vector& operator*= (const Vector&);
  Vector& operator/= (const Vector&);
  Vector& operator*= (const double&);
  Vector& operator/= (const double&);

  // Binary operators
  friend bool operator> (const Vector&, const Vector&);
  friend bool operator< (const Vector&, const Vector&);

  friend bool operator>= (const Vector&, const Vector&);
  friend bool operator<= (const Vector&, const Vector&);

  // Binary operators
  friend Vector operator+ (const Vector&, const Vector&);
  friend Vector operator- (const Vector&, const Vector&);

  friend constexpr double operator* (const Vector&, const Vector&);

  friend Vector operator* (const Vector&, double);
  friend Vector operator* (const double&, const Vector&);
  friend Vector operator/ (const Vector&, double);

  friend Vector operator/ (const Vector&, const Vector&);

  // Boolean functions
  friend bool operator==(const Vector&, const Vector&);
  friend bool operator!=(const Vector&, const Vector&);

  friend std::ostream& operator<<(std::ostream&, const Vector&);

public:
  static const Vector Null; //!< Null vector.
  static const Vector X; //!< Vector(1,0,0).
  static const Vector Y; //!< Vector(0,1,0).
  static const Vector Z; //!< Vector(0,0,1).
};

/*!
\brief Create a vector with the same coordinates.
\param a Real.
*/
inline Vector::Vector(const double& a)
{
  c[0] = c[1] = c[2] = a;
}

/*!
\brief Create a vector with argument coordinates.
\param a,b,c Coordinates.
*/
inline Vector::Vector(const double& a, const double& b, const double& c)
{
  Vector::c[0] = a;
  Vector::c[1] = b;
  Vector::c[2] = c;
}

//! Gets the i-th coordinate of vector.
inline constexpr double& Vector::operator[] (int i)
{
  return c[i];
}

//! Returns the i-th coordinate of vector.
inline constexpr double Vector::operator[] (int i) const
{
  return c[i];
}

// Unary operators

//! Overloaded.
inline Vector Vector::operator+ () const
{
  return *this;
}

//! Overloaded.
inline Vector Vector::operator- () const
{
  return Vector(-c[0], -c[1], -c[2]);
}

// Assignment unary operators

//! Destructive addition.
inline Vector& Vector::operator+= (const Vector& u)
{
  c[0] += u.c[0]; c[1] += u.c[1]; c[2] += u.c[2];
  return *this;
}

//! Destructive subtraction.
inline Vector& Vector::operator-= (const Vector& u)
{
  c[0] -= u.c[0]; c[1] -= u.c[1]; c[2] -= u.c[2];
  return *this;
}

//! Destructive scalar multiply.
inline Vector& Vector::operator*= (const double& a)
{
  c[0] *= a; c[1] *= a; c[2] *= a;
  return *this;
}

//! Destructive division by a scalar.
inline Vector& Vector::operator/= (const double& a)
{
  c[0] /= a; c[1] /= a; c[2] /= a;
  return *this;
}

/*!
\brief Destructively scale a vector by another vector.

This is the same as Scale:
\code
Vector u(2.0,-1.0,1.0);
u=u.Scaled(Vector(3.0,1.0,2.0)); // u*=Vector(3.0,1.0,2.0);
\endcode
*/
inline Vector& Vector::operator*= (const Vector& u)
{
  c[0] *= u.c[0]; c[1] *= u.c[1]; c[2] *= u.c[2];
  return *this;
}

//! Destructively divide the components of a vector by another vector.
inline Vector& Vector::operator/= (const Vector& u)
{
  c[0] /= u.c[0]; c[1] /= u.c[1]; c[2] /= u.c[2];
  return *this;
}

//! Compare two vectors.
inline bool operator> (const Vector& u, const Vector& v)
{
  return ((u.c[0] > v.c[0]) && (u.c[1] > v.c[1]) && (u.c[2] > v.c[2]));
}

//! Compare two vectors.
inline bool operator< (const Vector& u, const Vector& v)
{
  return ((u.c[0] < v.c[0]) && (u.c[1] < v.c[1]) && (u.c[2] < v.c[2]));
}

//! Overloaded
inline bool operator>= (const Vector& u, const Vector& v)
{
  return ((u.c[0] >= v.c[0]) && (u.c[1] >= v.c[1]) && (u.c[2] >= v.c[2]));
}

//! Overloaded
inline bool operator<= (const Vector& u, const Vector& v)
{
  return ((u.c[0] <= v.c[0]) && (u.c[1] <= v.c[1]) && (u.c[2] <= v.c[2]));
}

//! Adds up two vectors.
inline Vector operator+ (const Vector& u, const Vector& v)
{
  return Vector(u.c[0] + v.c[0], u.c[1] + v.c[1], u.c[2] + v.c[2]);
}

//! Difference between two vectors.
inline Vector operator- (const Vector& u, const Vector& v)
{
  return Vector(u.c[0] - v.c[0], u.c[1] - v.c[1], u.c[2] - v.c[2]);
}

//! Scalar product.
inline constexpr double operator* (const Vector& u, const Vector& v)
{
  return (u.c[0] * v.c[0] + u.c[1] * v.c[1] + u.c[2] * v.c[2]);
}

//! Right multiply by a scalar.
inline Vector operator* (const Vector& u, double a)
{
  return Vector(u.c[0] * a, u.c[1] * a, u.c[2] * a);
}

//! Left multiply by a scalar.
inline Vector operator* (const double& a, const Vector& v)
{
  return v * a;
}

//! Cross product.
inline Vector operator/ (const Vector& u, const Vector& v)
{
  return Vector(u.c[1] * v.c[2] - u.c[2] * v.c[1], u.c[2] * v.c[0] - u.c[0] * v.c[2], u.c[0] * v.c[1] - u.c[1] * v.c[0]);
}

//! Left multiply by a scalar
inline Vector operator/ (const Vector& u, double a)
{
  return Vector(u.c[0] / a, u.c[1] / a, u.c[2] / a);
}

// Boolean functions

//! Strong equality test.
inline bool operator== (const Vector& u, const Vector& v)
{
  return ((u.c[0] == v.c[0]) && (u.c[1] == v.c[1]) && (u.c[2] == v.c[2]));
}

//! Strong difference test.
inline bool operator!= (const Vector& u, const Vector& v)
{
  return (!(u == v));
}

// Class
class Vector2
{
protected:
  double c[2]; //!< Components.
public:
  //! Empty.
  Vector2() {}

  explicit Vector2(const double&);
  explicit Vector2(const double&, const double&);
  Vector2(const Vector&);

  double& operator[] (int);
  constexpr double operator[] (int) const;

  // Unary operators
  Vector2 operator+ () const;
  Vector2 operator- () const;

  // Assignment operators
  Vector2& operator+= (const Vector2&);
  Vector2& operator-= (const Vector2&);
  Vector2& operator*= (const Vector2&);
  Vector2& operator/= (const Vector2&);
  Vector2& operator*= (double);
  Vector2& operator/= (double);

  // Binary operators
  friend bool operator> (const Vector2&, const Vector2&);
  friend bool operator< (const Vector2&, const Vector2&);

  friend bool operator>= (const Vector2&, const Vector2&);
  friend bool operator<= (const Vector2&, const Vector2&);

  // Binary operators
  friend Vector2 operator+ (const Vector2&, const Vector2&);
  friend Vector2 operator- (const Vector2&, const Vector2&);

  friend double operator* (const Vector2&, const Vector2&);

  friend Vector2 operator* (const Vector2&, double);
  friend Vector2 operator* (double, const Vector2&);
  friend Vector2 operator/ (const Vector2&, double);

  friend double operator/ (const Vector2&, const Vector2&);

  // Boolean functions
  friend bool operator==(const Vector2&, const Vector2&);
  friend bool operator!=(const Vector2&, const Vector2&);


  // Conversion
  Vector ToVector(const double& = 0.0) const;

  friend std::ostream& operator<<(std::ostream&, const Vector2&);

public:
  static const Vector2 Null; //!< Null vector.
  static const Vector2 X; //!< Vector2(1,0).
  static const Vector2 Y; //!< Vector2(0,1).
};

/*!
\brief Create a vector with the same real coordinates.
\param a Real.
*/
inline Vector2::Vector2(const double& a)
{
  c[0] = c[1] = a;
}

//! Create a vector with argument coordinates.
inline Vector2::Vector2(const double& a, const double& b)
{
  Vector2::c[0] = a;
  Vector2::c[1] = b;
}

//! Create a two dimension vector from another three dimension vector.
inline Vector2::Vector2(const Vector& v)
{
  c[0] = v[0];
  c[1] = v[1];
}

//! Gets the i-th coordinate of vector.
inline double& Vector2::operator[] (int i)
{
  return c[i];
}

//! Returns the i-th coordinate of vector.
inline constexpr double Vector2::operator[] (int i) const
{
  return c[i];
}

/*!
\brief Convert a Vector2 to a Vector.
\param z Extra coordinate.
*/
inline Vector Vector2::ToVector(const double& z) const
{
  return Vector(c[0], c[1], z);
}

// Unary operators

//! Overloaded.
inline Vector2 Vector2::operator+ () const
{
  return *this;
}

//! Overloaded.
inline Vector2 Vector2::operator- () const
{
  return Vector2(-c[0], -c[1]);
}

// Assignment unary operators

//! Destructive addition.
inline Vector2& Vector2::operator+= (const Vector2& u)
{
  c[0] += u.c[0]; c[1] += u.c[1];
  return *this;
}

//! Destructive subtraction.
inline Vector2& Vector2::operator-= (const Vector2& u)
{
  c[0] -= u.c[0]; c[1] -= u.c[1];
  return *this;
}

//! Destructive scalar multiply.
inline Vector2& Vector2::operator*= (double a)
{
  c[0] *= a; c[1] *= a;
  return *this;
}

//! Destructive division by a scalar.
inline Vector2& Vector2::operator/= (double a)
{
  c[0] /= a; c[1] /= a;
  return *this;
}

//! Destructively scale a vector by another vector.
inline Vector2& Vector2::operator*= (const Vector2& u)
{
  c[0] *= u.c[0]; c[1] *= u.c[1];
  return *this;
}

//! Destructively divide the components of a vector by another vector.
inline Vector2& Vector2::operator/= (const Vector2& u)
{
  c[0] /= u.c[0]; c[1] /= u.c[1];
  return *this;
}

//! Compare two vectors.
inline bool operator> (const Vector2& u, const Vector2& v)
{
  return ((u.c[0] > v.c[0]) && (u.c[1] > v.c[1]));
}

//! Compare two vectors.
inline bool operator< (const Vector2& u, const Vector2& v)
{
  return ((u.c[0] < v.c[0]) && (u.c[1] < v.c[1]));
}

//! Overloaded
inline bool operator>= (const Vector2& u, const Vector2& v)
{
  return ((u.c[0] >= v.c[0]) && (u.c[1] >= v.c[1]));
}

//! Overloaded
inline bool operator<= (const Vector2& u, const Vector2& v)
{
  return ((u.c[0] <= v.c[0]) && (u.c[1] <= v.c[1]));
}

//! Adds up two vectors.
inline Vector2 operator+ (const Vector2& u, const Vector2& v)
{
  return Vector2(u.c[0] + v.c[0], u.c[1] + v.c[1]);
}

//! Difference between two vectors.
inline Vector2 operator- (const Vector2& u, const Vector2& v)
{
  return Vector2(u.c[0] - v.c[0], u.c[1] - v.c[1]);
}

/*!
\brief Dot product between two vectors.
\param u,v Argument vectors.
*/
inline double operator* (const Vector2& u, const Vector2& v)
{
  return u.c[0] * v.c[0] + u.c[1] * v.c[1];
}

//! Right multiply by a scalar.
inline Vector2 operator* (const Vector2& u, double a)
{
  return Vector2(u.c[0] * a, u.c[1] * a);
}

//! Left multiply by a scalar.
inline Vector2 operator* (double a, const Vector2& v)
{
  return v * a;
}

/*!
\brief Cross productof two vectors.
Note that the derminant of a 2-square matrix is the cross product of its two colum vectors.
*/
inline double operator/ (const Vector2& u, const Vector2& v)
{
  return u.c[0] * v.c[1] - u.c[1] * v.c[0];
}

//! Left divide by a scalar
inline Vector2 operator/ (const Vector2& u, double a)
{
  return Vector2(u.c[0] / a, u.c[1] / a);
}

// Boolean functions

/*!
\brief Strong equality test.
*/
inline bool operator== (const Vector2& u, const Vector2& v)
{
  return ((u.c[0] == v.c[0]) && (u.c[1] == v.c[1]));
}

/*!
\brief Strong difference test.
*/
inline bool operator!= (const Vector2& u, const Vector2& v)
{
  return (!(u == v));
}

#endif

