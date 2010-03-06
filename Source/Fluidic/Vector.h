/*
Copyright (c) 2010 Steven Leigh

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#pragma once

#include "Math.h"

namespace Fluidic
{	
	/**
	 * 3 dimensional float vector
	 */
	class Vector
	{
	public:
		float x, y, z;
		int dim;
		inline int xi() const { return (int)x; }
		inline int yi() const { return (int)y; }
		inline int zi() const { return (int)z; }

		Vector():x(0),y(0),z(0), dim(0){}

		Vector(int _dim): dim(_dim){}
		Vector(float _x, float _y):x(_x),y(_y), z(0), dim(2){}
		Vector(float _x, float _y, float _z):x(_x),y(_y), z(_z), dim(3){}
		Vector(float _x, float _y, float _z, int _dim):x(_x),y(_y), z(_z), dim(_dim){}

		Vector & operator+=(const Vector &v2) 
		{
			x += v2.x;
			y += v2.y;
			if (dim == 3) z += v2.z;
			return *this;
		};

		Vector & operator-=(const Vector &v2) 
		{
			x -= v2.x;
			y -= v2.y;
			if (dim == 3) z -= v2.z;
			return *this;
		};
		Vector operator-() const
		{
			return Vector(-x, -y, -z, dim);
		}
		Vector operator-(const Vector &v2) const
		{
			return Vector(x - v2.x, y - v2.y, z - v2.z, dim);
		};
		
		Vector & operator*=(const float &f) 
		{
			x *= f;
			y *= f;
			if (dim == 3) z *= f;
			return *this;
		};
		Vector operator*(const float &f) const
		{
			return Vector(x*f, y*f, z*f, dim);
		};
		Vector operator*(const Vector &v) const
		{
			return Vector(x*v.x, y*v.y, z*v.z, dim);
		};
		
		Vector & operator/=(const float &f) 
		{
			x /= f;
			y /= f;
			z /= f;
			return *this;
		};
		Vector operator/(const Vector &v) const
		{
			if (dim == 2) return Vector(x/v.x, y/v.y);
			else return Vector(x/v.x, y/v.y, z/v.z, dim);
		};
		Vector operator/(const float &f) const
		{
			float fi = 1.f/f;
			return Vector(x*fi, y*fi, z*fi, dim);
		};

		float Dot(const Vector &other) 
		{
			return x*other.x + y*other.y + z*other.z;
		}

		float Length2()
		{
			return Dot(*this);
		}

		float Length() 
		{
			return sqrt(Length2());
		}
		Vector & Normalize()
		{
			float len = Length();
			if (len == 0) return *this;
			x /= len;
			y /= len;
			if (dim == 3) z /= len;
			return *this;
		}
		Vector Normalized()
		{
			float len = Length();
			if (len == 0) return *this;
			return Vector(x/len,y/len,z/len, dim);
		}

		static Vector Zero2D() { return Vector(0,0); }
		static Vector Zero() { return Vector(0,0,0); }


		//Output operators
		friend std::ostream& operator<<(std::ostream &os, const Vector &v) 
		{
			if (v.dim == 2) os << "(" << v.x << "," << v.y << ")";
			else os << "(" << v.x << "," << v.y << "," << v.z << ")";
			return os;
		}
	};
}