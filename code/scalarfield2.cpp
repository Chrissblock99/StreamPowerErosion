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

std::vector<float> ScalarField2::GetFloatData() const {
    std::vector<float> res(nx * ny, 0);
    for (int i = 0; i < nx * ny; i++) res[i] = float(field[i]);
    return res;
}