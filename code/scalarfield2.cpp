#include <immintrin.h>
#include "scalarfield2.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

/*!
\class ScalarField2 scalarfield.h
\brief A base two-dimensional field of real values.

\ingroup Structure
*/

/*!
\brief Create the field structure.
\param a Array representing the grid domain.
\param v Constant value of field.
*/
ScalarField2::ScalarField2(const Array2& a, const double& v) :Array2(a)
{
  field.resize(nx * ny, v);
}

/*!
\brief Create the field structure.
\param box The box.
\param x,y Size of the array.
\param v Constant value of field.
*/
ScalarField2::ScalarField2(const Box2& box, int x, int y, const double& v) :ScalarField2(Array2(box, x, y), v)
{
}

/*!
\brief Create the field structure.
\param box The box.
\param x,y Size of the array.
\param v Array of scalar values.
*/
ScalarField2::ScalarField2(const Box2& box, int x, int y, const std::vector<double>& v) :Array2(box, x, y)
{
  field = v;
}

/*!
\brief Create the field structure.
\param box The box.
\param name filename to read
\param minV, maxV min/max range of values
*/
ScalarField2::ScalarField2(const Box2& box, const char* filename, double minV, double maxV) 
{
    a = box[0];
    b = box[1];
    int n;
    unsigned char* rawData = stbi_load(filename, &nx, &ny, &n, 1);
    field.resize(nx * ny);
    for (int i = 0; i < nx * ny; i++)
    {
        double t = double(rawData[i]) / 255.0;
        field[i] = minV * (1.0f - t) + t * maxV;
    }
    stbi_image_free(rawData);

    celldiagonal = Vector2((b[0] - a[0]) / (nx - 1), (b[1] - a[1]) / (ny - 1));
    inversecelldiagonal = celldiagonal.Inverse();
}

/*!
\brief Empty.
*/
ScalarField2::~ScalarField2()
{
}

/*!
\brief Compute the gradient at a given array vertex.

\param i,j Integer coordinates of the array vertex.
*/
Vector2 ScalarField2::Gradient(int i, int j) const
{
  Vector2 n;

  // Gradient along x axis
  if (i == 0)
  {
    n[0] = (at(i + 1, j) - at(i, j)) * inversecelldiagonal[0];
  }
  else if (i == nx - 1)
  {
    n[0] = (at(i, j) - at(i - 1, j)) * inversecelldiagonal[0];
  }
  else
  {
    n[0] = (at(i + 1, j) - at(i - 1, j)) * 0.5 * inversecelldiagonal[0];
  }

  // Gradient along y axis
  if (j == 0)
  {
    n[1] = (at(i, j + 1) - at(i, j)) * inversecelldiagonal[1];
  }
  else if (j == ny - 1)
  {
    n[1] = (at(i, j) - at(i, j - 1)) * inversecelldiagonal[1];
  }
  else
  {
    n[1] = (at(i, j + 1) - at(i, j - 1)) * 0.5 * inversecelldiagonal[1];
  }

  return n;
}

/*!
\brief Compute the Lipschitz constant of the elevation function.

Note that this is equivalent to the following code, with the
difference that this function does not store the norm of the gradient in memory.
\code
ScalarField2 n=f.GradientNorm();
double k=0.0;
for (int i=0;i<f.VertexSize();i++)
{
k=Math::Max(k,f.at(i));
}
\endcode
*/
double ScalarField2::K() const
{
  double k = 0.0;

  for (int i = 0; i < nx; i++)
  {
    for (int j = 0; j < ny; j++)
    {
      k = Math::Max(k, Norm(Gradient(i, j)));
    }
  }
  return k;
}


/*!
\brief Add material with gaussian distribution.
\param center Center of the distribution.
\param radius Radius
\param height Maximum height of the distribution.
*/
void ScalarField2::Gaussian(const Vector2& center, const double& radius, const double& height)
{
    // Compute modification Area
    Vec2I pa, pb;
    VertexIntegerArea(Box2(center, radius), pa, pb);

    // Compute thickness
    for (int y = pa[1]; y <= pb[1]; y++)
    {
        for (int x = pa[0]; x <= pb[0]; x++)
        {
            // Distance between central point and current point
            double u = SquaredNorm(center - ArrayVertex(x, y));
            if (u < radius * radius)
                field[VertexIndex(x, y)] += height * Math::CubicSmooth(u, radius * radius);
        }
    }
}

/*!
\brief Overloaded.
\param s Stream.
\param scalar The scalar field.
*/
std::ostream& operator<<(std::ostream& s, const ScalarField2& scalar)
{
  s << Array2(scalar) << std::endl;

  for (int j = scalar.ny - 1; j > -1; j--)
  {
    for (int i = 0; i < scalar.nx; i++)
    {
      s << scalar.at(i, j) << ' ';
    }
    s << std::endl;
  }
  return s;
}

std::vector<float> ScalarField2::GetFloatData() const {
    std::vector<float> res(nx * ny, 0);
    for (int i = 0; i < nx * ny; i++) res[i] = float(field[i]);
    return res;
}