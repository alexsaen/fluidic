/*
Copyright (c) 2010, Steven Leigh
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name "Fluidic" nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Steven Leigh BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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