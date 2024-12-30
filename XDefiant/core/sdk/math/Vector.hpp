#pragma once 
#include <cassert>

constexpr auto flt_max = 3.402823466e+38F; // max value
constexpr auto flt_min = 1.175494351e-38F; // min normalized positive value

#include <corecrt_math.h>
#include <stdlib.h>
#include <numbers>

#define DECL_ALIGN(x) __attribute__((aligned(x)))


#define ALIGN16 DECL_ALIGN(16)
#define VALVE_RAND_MAX 0x7fff
#define VectorExpand(v) (v).x, (v).y, (v).z

#define Assert( _exp ) ((void)0)

#define FastSqrt(x)			(sqrt)(x)

#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h

#define M_PI_F		((float)(M_PI))	// Shouldn't collide with anything.

#define M_PHI		1.61803398874989484820 // golden ratio

// NJS: Inlined to prevent floats from being autopromoted to doubles, as with the old system.
#ifndef RAD2DEG
#define RAD2DEG( x  )  ( (float)(x) * (float)(180.f / M_PI_F) )
#endif

#ifndef DEG2RAD
#define DEG2RAD( x  )  ( (float)(x) * (float)(M_PI_F / 180.f) )
#endif


inline bool getKeyDown(int key)
{
	return (GetAsyncKeyState(key) & 0x8000) != 0;
}

namespace uemath
{
	struct matrix3x4_t
	{
		float m[3][4];
	};

	class matrix4x4_t
	{
	public:
		float m[4][4];
	};

	struct vector3
	{
		vector3() : x(0), y(0), z(0) {}
		vector3(float x, float y, float z) : x(x), y(y), z(z) {}

		vector3 operator		+ (const vector3& other) const { return { this->x + other.x, this->y + other.y, this->z + other.z }; }
		vector3 operator		- (const vector3& other) const { return { this->x - other.x, this->y - other.y, this->z - other.z }; }
		vector3 operator		* (float offset) const { return { this->x * offset, this->y * offset, this->z * offset }; }
		vector3 operator		/ (float offset) const { return { this->x / offset, this->y / offset, this->z / offset }; }

		vector3& operator		*= (const float other) { this->x *= other; this->y *= other; this->z *= other; return *this; }
		vector3& operator		/= (const float other) { this->x /= other; this->y /= other; this->z /= other; return *this; }

		vector3& operator		=  (const vector3& other) { this->x = other.x; this->y = other.y; this->z = other.z; return *this; }
		vector3& operator		+= (const vector3& other) { this->x += other.x; this->y += other.y; this->z += other.z; return *this; }
		vector3& operator		-= (const vector3& other) { this->x -= other.x; this->y -= other.y; this->z -= other.z; return *this; }
		vector3& operator		*= (const vector3& other) { this->x *= other.x; this->y *= other.y; this->z *= other.z; return *this; }
		vector3& operator		/= (const vector3& other) { this->x /= other.x; this->y /= other.y; this->z /= other.z; return *this; }

		operator bool() { return bool(this->x || this->y || this->z); }
		friend bool operator	== (const vector3& a, const vector3& b) { return a.x == b.x && a.y == b.y && a.z == b.z; }
		friend bool operator	!= (const vector3& a, const vector3& b) { return !(a == b); }

		inline float Dot(const vector3& vector)
		{
			return x * vector.x + y * vector.y + z * vector.z;
		}
		inline float distance(vector3 v)
		{
			return float(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
		}

		inline bool is_valid() {
			if (x > 0 && y > 0)
				return true;

			return false;
		}

		//inline bool normalize( ) {
		//	while ( this->x > 89.0f )
		//		this->x -= 180.f;

		//	while ( this->x < -89.0f )
		//		this->x += 180.f;

		//	while ( this->y > 180.f )
		//		this->y -= 360.f;

		//	while ( this->y < -180.f )
		//		this->y += 360.f;
		//}

		inline vector3 calculate_viewpoint(const vector3& dst)
		{
			vector3 angle = vector3();
			vector3 delta = vector3((this->x - dst.x), (this->y - dst.y), (this->z - dst.z));

			double hyp = sqrt(delta.x * delta.x + delta.y * delta.y);

			angle.x = atan(delta.z / hyp) * (180.0f / M_PI);
			angle.y = atan(delta.y / delta.x) * (180.0f / M_PI);
			angle.z = 0;

			if (delta.x >= 0.0)
				angle.y += 180.0f;

			return angle;
		}

		inline bool normalize() {
			while (this->x > 180.0f)
			{
				this->x -= 360.0f;
			}
			while (this->x < -180.0f)
			{
				this->x += 360.0f;
			}
			while (this->y > 180.0f)
			{
				this->y -= 360.0f;
			}
			while (this->y < -180.0f)
			{
				this->y += 360.0f;
			}
			return this->x >= -180.0f && this->x <= 180.0f && this->y >= -180.0f && this->y <= 180.0f;
		}

		float x, y, z;
	};

	inline float distance(const vector3 p1, const vector3 p2) {
		float x = p1.x - p2.x;
		float y = p1.y - p2.y;
		float z = p1.z - p2.z;
		return sqrt(x * x + y * y + z * z);
	}

	struct vector2
	{
		vector2() : x(0), y(0) {}
		vector2(float x, float y) : x(x), y(y) {}

		vector2 operator		+ (const vector2& other) const { return { this->x + other.x, this->y + other.y }; }
		vector2 operator		- (const vector2& other) const { return { this->x - other.x, this->y - other.y }; }
		vector2 operator		* (float offset) const { return { this->x * offset, this->y * offset }; }
		vector2 operator		/ (float offset) const { return { this->x / offset, this->y / offset }; }

		vector2& operator		*= (const float other) { this->x *= other; this->y *= other; return *this; }
		vector2& operator		/= (const float other) { this->x /= other; this->y /= other; return *this; }

		vector2& operator		=  (const vector2& other) { this->x = other.x; this->y = other.y; return *this; }
		vector2& operator		+= (const vector2& other) { this->x += other.x; this->y += other.y; return *this; }
		vector2& operator		-= (const vector2& other) { this->x -= other.x; this->y -= other.y; return *this; }
		vector2& operator		*= (const vector2& other) { this->x *= other.x; this->y *= other.y; return *this; }
		vector2& operator		/= (const vector2& other) { this->x /= other.x; this->y /= other.y; return *this; }

		operator bool() { return bool(this->x || this->y); }
		friend bool operator	==(const vector2& a, const vector2& b) { return a.x == b.x && a.y == b.y; }
		friend bool operator	!=(const vector2& a, const vector2& b) { return !(a == b); }

		float x, y;
	};

	inline float dot_product(const vector3& v1, const float* v2)
	{
		return v1.x * v2[0] + v1.y * v2[1] + v1.z * v2[2];
	}
	inline void vector_transform(const vector3& in1, const matrix3x4_t& in2, vector3& out)
	{
		out.x = dot_product(in1, in2.m[0]) + in2.m[0][3];
		out.y = dot_product(in1, in2.m[1]) + in2.m[1][3];
		out.z = dot_product(in1, in2.m[2]) + in2.m[2][3];
	}

	class frotator
	{
	public:

		frotator() : pitch(), yaw(), roll() { }
		frotator(float Pitch, float Yaw, float Roll) : pitch(Pitch), yaw(Yaw), roll(Roll) { }

		frotator operator + (const frotator& other) const { return { this->pitch + other.pitch, this->yaw + other.yaw, this->roll + other.roll }; }
		frotator operator - (const frotator& other) const { return { this->pitch - other.pitch, this->yaw - other.yaw, this->roll - other.roll }; }
		frotator operator * (float offset) const { return { this->pitch * offset, this->yaw * offset, this->roll * offset }; }
		frotator operator / (float offset) const { return { this->pitch / offset, this->yaw / offset, this->roll / offset }; }

		frotator& operator *= (const float other) { this->pitch *= other; this->yaw *= other; this->roll *= other; return *this; }
		frotator& operator /= (const float other) { this->pitch /= other; this->yaw /= other; this->roll /= other; return *this; }

		frotator& operator = (const frotator& other) { this->pitch = other.pitch; this->yaw = other.yaw; this->roll = other.roll; return *this; }
		frotator& operator += (const frotator& other) { this->pitch += other.pitch; this->yaw += other.yaw; this->roll += other.roll; return *this; }
		frotator& operator -= (const frotator& other) { this->pitch -= other.pitch; this->yaw -= other.yaw; this->roll -= other.roll; return *this; }
		frotator& operator *= (const frotator& other) { this->pitch *= other.pitch; this->yaw *= other.yaw; this->roll *= other.roll; return *this; }
		frotator& operator /= (const frotator& other) { this->pitch /= other.pitch; this->yaw /= other.yaw; this->roll /= other.roll; return *this; }

		operator bool() { return bool(this->pitch || this->yaw || this->roll); }
		friend bool operator == (const frotator& a, const frotator& b) { return a.pitch == b.pitch && a.yaw == b.yaw && a.roll == b.roll; }
		friend bool operator != (const frotator& a, const frotator& b) { return !(a == b); }

		inline frotator get() {
			return frotator(pitch, yaw, roll);
		}

		inline void set(float _Pitch, float _Yaw, float _Roll) {
			pitch = _Pitch;
			yaw = _Yaw;
			roll = _Roll;
		}

		inline frotator Clamp() {
			frotator result = get();
			if (result.pitch > 180)
				result.pitch -= 360;
			else if (result.pitch < -180)
				result.pitch += 360;
			if (result.yaw > 180)
				result.yaw -= 360;
			else if (result.yaw < -180)
				result.yaw += 360;
			if (result.pitch < -89)
				result.pitch = -89;
			if (result.pitch > 89)
				result.pitch = 89;
			while (result.yaw < -180.0f)
				result.yaw += 360.0f;
			while (result.yaw > 180.0f)
				result.yaw -= 360.0f;

			result.roll = 0;
			return result;

		}

		float length() {
			return sqrt(pitch * pitch + yaw * yaw + roll * roll);
		}

		float Dot(const frotator& V) { return pitch * V.pitch + yaw * V.yaw + roll * V.roll; }
		float Distance(frotator V) { return float(sqrtf(powf(V.pitch - this->pitch, 2.0) + powf(V.yaw - this->yaw, 2.0) + powf(V.roll - this->roll, 2.0))); }

		float pitch;
		float yaw;
		float roll;

	};

	struct flinearcolor
	{
		flinearcolor() :a(), r(), g(), b() { }
		flinearcolor(int8_t a, int8_t r, int8_t g, int8_t b) : a(a), r(r), g(g), b(b) { }

		int8_t a, r, g, b;
	};

	struct rgb
	{
		float r, g, b;
	};

}

class Vector4 {
public:
	float x, y, z, a;
};
class Vector
{
public:
	float x, y, z;
	Vector(void);
	Vector(float X, float Y, float Z);
	void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f);
	bool IsValid() const;
	float operator[](int i) const;
	float& operator[](int i);
	inline void Zero();
	bool operator==(const Vector& v) const;
	bool operator!=(const Vector& v) const;
	inline Vector& operator+=(const Vector& v);
	inline Vector& operator-=(const Vector& v);
	inline Vector& operator*=(const Vector& v);
	inline Vector& operator*=(float s);
	inline Vector& operator/=(const Vector& v);
	inline Vector& operator/=(float s);
	inline Vector& operator+=(float fl);
	inline Vector& operator-=(float fl);
	inline float	Length() const;

	inline void Rotate(const Vector& vCenter, const Vector& vAngles);

	inline float LengthSqr(void) const
	{
		return (x * x + y * y + z * z);
	}
	bool IsZero(float tolerance = 0.01f) const
	{
		return (x > -tolerance && x < tolerance &&
			y > -tolerance && y < tolerance &&
			z > -tolerance && z < tolerance);
	}
	Vector	Normalize();
	void	NormalizeInPlace();
	inline float	DistTo(const Vector& vOther) const;
	inline float	DistToSqr(const Vector& vOther) const;
	float	Dot(const Vector& vOther) const;
	float	Length2D(void) const;
	float	Length2DSqr(void) const;
	Vector& operator=(const Vector& vOther);
	Vector	operator-(void) const;
	Vector	operator+(const Vector& v) const;
	Vector	operator-(const Vector& v) const;
	Vector	operator*(const Vector& v) const;
	Vector	operator/(const Vector& v) const;
	Vector	operator*(float fl) const;
	Vector	operator/(float fl) const;
	// Base address...
	float* Base();
	float const* Base() const;
	Vector calculate_viewpoint(const Vector& dst);
};

//===============================================
inline void Vector::Init(float ix, float iy, float iz)
{
	x = ix; y = iy; z = iz;
}
//===============================================
inline Vector::Vector(float X, float Y, float Z)
{
	x = X; y = Y; z = Z;
}
//===============================================
inline Vector::Vector(void) { Zero(); }
//===============================================
inline void Vector::Zero()
{
	x = y = z = 0.0f;
}
//===============================================
inline void VectorClear(Vector& a)
{
	a.x = a.y = a.z = 0.0f;
}
//===============================================
inline Vector& Vector::operator=(const Vector& vOther)
{
	x = vOther.x; y = vOther.y; z = vOther.z;
	return *this;
}
//===============================================
inline float& Vector::operator[](int i)
{
	Assert((i >= 0) && (i < 3));
	return ((float*)this)[i];
}
//===============================================
inline float Vector::operator[](int i) const
{
	Assert((i >= 0) && (i < 3));
	return ((float*)this)[i];
}
//===============================================
inline bool Vector::operator==(const Vector& src) const
{
	return (src.x == x) && (src.y == y) && (src.z == z);
}
//===============================================
inline bool Vector::operator!=(const Vector& src) const
{
	return (src.x != x) || (src.y != y) || (src.z != z);
}
//===============================================
inline void VectorCopy(const Vector& src, Vector& dst)
{
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}
//===============================================
inline  Vector& Vector::operator+=(const Vector& v)
{
	x += v.x; y += v.y; z += v.z;
	return *this;
}
//===============================================
inline  Vector& Vector::operator-=(const Vector& v)
{
	x -= v.x; y -= v.y; z -= v.z;
	return *this;
}
//===============================================
inline  Vector& Vector::operator*=(float fl)
{
	x *= fl;
	y *= fl;
	z *= fl;

	return *this;
}
//===============================================
inline  Vector& Vector::operator*=(const Vector& v)
{
	x *= v.x;
	y *= v.y;
	z *= v.z;

	return *this;
}

inline Vector Vector::calculate_viewpoint(const Vector& dst)
{
	Vector angle = Vector();
	Vector delta = Vector((this->x - dst.x), (this->y - dst.y), (this->z - dst.z));

	double hyp = sqrt(delta.x * delta.x + delta.y * delta.y);

	angle.x = atan(delta.z / hyp) * (180.0f / std::numbers::pi);
	angle.y = atan(delta.y / delta.x) * (180.0f / std::numbers::pi);
	angle.z = 0;

	if (delta.x >= 0.0)
		angle.y += 180.0f;

	return angle;
}

//===============================================
inline Vector& Vector::operator+=(float fl)
{
	x += fl;
	y += fl;
	z += fl;

	return *this;
}
//===============================================
inline Vector& Vector::operator-=(float fl)
{
	x -= fl;
	y -= fl;
	z -= fl;

	return *this;
}
//===============================================
inline  Vector& Vector::operator/=(float fl)
{
	Assert(fl != 0.0f);
	float oofl = 1.0f / fl;
	x *= oofl;
	y *= oofl;
	z *= oofl;

	return *this;
}
//===============================================
inline  Vector& Vector::operator/=(const Vector& v)
{
	Assert(v.x != 0.0f && v.y != 0.0f && v.z != 0.0f);
	x /= v.x;
	y /= v.y;
	z /= v.z;

	return *this;
}
//===============================================
inline float Vector::Length(void) const
{


	float root = 0.0f;

	float sqsr = x * x + y * y + z * z;

	root = sqrt(sqsr);

	return root;
}
//===============================================
inline float Vector::Length2D(void) const
{
	float root = 0.0f;

	float sqst = x * x + y * y;

	root = sqrt(sqst);

	return root;
}
//===============================================
inline float Vector::Length2DSqr(void) const
{
	return (x * x + y * y);
}
//===============================================
inline Vector CrossProduct(const Vector& a, const Vector& b)
{
	return Vector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}
//===============================================
float Vector::DistTo(const Vector& vOther) const
{
	Vector delta;

	delta.x = x - vOther.x;
	delta.y = y - vOther.y;
	delta.z = z - vOther.z;

	return delta.Length();
}
float Vector::DistToSqr(const Vector& vOther) const
{
	Vector delta;

	delta.x = x - vOther.x;
	delta.y = y - vOther.y;
	delta.z = z - vOther.z;

	return delta.LengthSqr();
}
//===============================================
inline Vector Vector::Normalize()
{
	Vector vector;
	float length = this->Length();

	if (length != 0)
	{
		vector.x = x / length;
		vector.y = y / length;
		vector.z = z / length;
	}
	else
	{
		vector.x = vector.y = 0.0f; vector.z = 1.0f;
	}

	return vector;
}
//===============================================
inline void Vector::NormalizeInPlace()
{
	Vector& v = *this;

	float iradius = 1.f / (this->Length() + 1.192092896e-07F); //FLT_EPSILON

	v.x *= iradius;
	v.y *= iradius;
	v.z *= iradius;
}
//===============================================
inline float VectorNormalize(Vector& v)
{
	Assert(v.IsValid());
	float l = v.Length();
	if (l != 0.0f)
	{
		v /= l;
	}
	else
	{
		// FIXME:
		// Just copying the existing implemenation; shouldn't res.z == 0?
		v.x = v.y = 0.0f; v.z = 1.0f;
	}
	return l;
}
//===============================================
inline float VectorNormalize(float* v)
{
	return VectorNormalize(*(reinterpret_cast<Vector*>(v)));
}
//===============================================
inline Vector Vector::operator+(const Vector& v) const
{
	Vector res;
	res.x = x + v.x;
	res.y = y + v.y;
	res.z = z + v.z;
	return res;
}

//===============================================
inline Vector Vector::operator-(const Vector& v) const
{
	Vector res;
	res.x = x - v.x;
	res.y = y - v.y;
	res.z = z - v.z;
	return res;
}
//===============================================
inline Vector Vector::operator*(float fl) const
{
	Vector res;
	res.x = x * fl;
	res.y = y * fl;
	res.z = z * fl;
	return res;
}
//===============================================
inline Vector Vector::operator*(const Vector& v) const
{
	Vector res;
	res.x = x * v.x;
	res.y = y * v.y;
	res.z = z * v.z;
	return res;
}
//===============================================
inline Vector Vector::operator/(float fl) const
{
	Vector res;
	res.x = x / fl;
	res.y = y / fl;
	res.z = z / fl;
	return res;
}
//===============================================
inline Vector Vector::operator/(const Vector& v) const
{
	Vector res;
	res.x = x / v.x;
	res.y = y / v.y;
	res.z = z / v.z;
	return res;
}
inline float Vector::Dot(const Vector& vOther) const
{
	const Vector& a = *this;

	return(a.x * vOther.x + a.y * vOther.y + a.z * vOther.z);
}

//-----------------------------------------------------------------------------
// length
//-----------------------------------------------------------------------------

inline float VectorLength(const Vector& v)
{

	return (float)FastSqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

//VECTOR SUBTRAC
inline void VectorSubtract(const Vector& a, const Vector& b, Vector& c)
{


	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
}

//VECTORADD
inline void VectorAdd(const Vector& a, const Vector& b, Vector& c)
{


	c.x = a.x + b.x;
	c.y = a.y + b.y;
	c.z = a.z + b.z;
}

//-----------------------------------------------------------------------------
// Base address...
//-----------------------------------------------------------------------------
inline float* Vector::Base()
{
	return (float*)this;
}

inline float const* Vector::Base() const
{
	return (float const*)this;
}

inline void VectorMAInline(const float* start, float scale, const float* direction, float* dest)
{
	dest[0] = start[0] + direction[0] * scale;
	dest[1] = start[1] + direction[1] * scale;
	dest[2] = start[2] + direction[2] * scale;
}

inline void VectorMAInline(const Vector& start, float scale, const Vector& direction, Vector& dest)
{
	dest.x = start.x + direction.x * scale;
	dest.y = start.y + direction.y * scale;
	dest.z = start.z + direction.z * scale;
}

inline void VectorMA(const Vector& start, float scale, const Vector& direction, Vector& dest)
{
	VectorMAInline(start, scale, direction, dest);
}

inline void VectorMA(const float* start, float scale, const float* direction, float* dest)
{
	VectorMAInline(start, scale, direction, dest);
}


inline unsigned long& FloatBits(float& f)
{
	return *reinterpret_cast<unsigned long*>(&f);
}

inline bool IsFinite(float f)
{
	return ((FloatBits(f) & 0x7F800000) != 0x7F800000);
}

//=========================================================
// 2D Vector2D
//=========================================================

class Vector2D
{
public:
	// Members
	float x, y;

	// Construction/destruction
	Vector2D(void);
	Vector2D(float X, float Y);
	Vector2D(const float* pFloat);

	// Initialization
	void Init(float ix = 0.0f, float iy = 0.0f);

	// Got any nasty NAN's?
	bool IsValid() const;

	// array access...
	float operator[](int i) const;
	float& operator[](int i);

	// Base address...
	float* Base();
	float const* Base() const;

	// Initialization methods
	void Random(float minVal, float maxVal);

	// equality
	bool operator==(const Vector2D& v) const;
	bool operator!=(const Vector2D& v) const;

	// arithmetic operations
	Vector2D& operator+=(const Vector2D& v);
	Vector2D& operator-=(const Vector2D& v);
	Vector2D& operator*=(const Vector2D& v);
	Vector2D& operator*=(float s);
	Vector2D& operator/=(const Vector2D& v);
	Vector2D& operator/=(float s);

	// negate the Vector2D components
	void	Negate();

	// Get the Vector2D's magnitude.
	float	Length() const;

	// Get the Vector2D's magnitude squared.
	float	LengthSqr(void) const;

	// return true if this vector is (0,0) within tolerance
	bool IsZero(float tolerance = 0.01f) const
	{
		return (x > -tolerance && x < tolerance &&
			y > -tolerance && y < tolerance);
	}

	float	Normalize();

	// Normalize in place and return the old length.
	float	NormalizeInPlace();

	// Compare length.
	bool	IsLengthGreaterThan(float val) const;
	bool	IsLengthLessThan(float val) const;

	// Get the distance from this Vector2D to the other one.
	float	DistTo(const Vector2D& vOther) const;

	// Get the distance from this Vector2D to the other one squared.
	float	DistToSqr(const Vector2D& vOther) const;

	// Copy
	void	CopyToArray(float* rgfl) const;

	// Multiply, add, and assign to this (ie: *this = a + b * scalar). This
	// is about 12% faster than the actual Vector2D equation (because it's done per-component
	// rather than per-Vector2D).
	void	MulAdd(const Vector2D& a, const Vector2D& b, float scalar);

	// Dot product.
	float	Dot(const Vector2D& vOther) const;

	// assignment
	Vector2D& operator=(const Vector2D& vOther);

#ifndef VECTOR_NO_SLOW_OPERATIONS
	// copy constructors
	Vector2D(const Vector2D& vOther);

	// arithmetic operations
	Vector2D	operator-(void) const;

	Vector2D	operator+(const Vector2D& v) const;
	Vector2D	operator-(const Vector2D& v) const;
	Vector2D	operator*(const Vector2D& v) const;
	Vector2D	operator/(const Vector2D& v) const;
	Vector2D	operator*(float fl) const;
	Vector2D	operator/(float fl) const;

	// Cross product between two vectors.
	Vector2D	Cross(const Vector2D& vOther) const;

	// Returns a Vector2D with the min or max in X, Y, and Z.
	Vector2D	Min(const Vector2D& vOther) const;
	Vector2D	Max(const Vector2D& vOther) const;

#else

private:
	// No copy constructors allowed if we're in optimal mode
	Vector2D(const Vector2D& vOther);
#endif
};

//-----------------------------------------------------------------------------

const Vector2D vec2_origin(0, 0);
//const Vector2D vec2_invalid(3.40282347E+38F, 3.40282347E+38F);

//-----------------------------------------------------------------------------
// Vector2D related operations
//-----------------------------------------------------------------------------

// Vector2D clear
void Vector2DClear(Vector2D& a);

// Copy
void Vector2DCopy(const Vector2D& src, Vector2D& dst);

// Vector2D arithmetic
void Vector2DAdd(const Vector2D& a, const Vector2D& b, Vector2D& result);
void Vector2DSubtract(const Vector2D& a, const Vector2D& b, Vector2D& result);
void Vector2DMultiply(const Vector2D& a, float b, Vector2D& result);
void Vector2DMultiply(const Vector2D& a, const Vector2D& b, Vector2D& result);
void Vector2DDivide(const Vector2D& a, float b, Vector2D& result);
void Vector2DDivide(const Vector2D& a, const Vector2D& b, Vector2D& result);
void Vector2DMA(const Vector2D& start, float s, const Vector2D& dir, Vector2D& result);

// Store the min or max of each of x, y, and z into the result.
void Vector2DMin(const Vector2D& a, const Vector2D& b, Vector2D& result);
void Vector2DMax(const Vector2D& a, const Vector2D& b, Vector2D& result);

#define Vector2DExpand( v ) (v).x, (v).y

// Normalization
float Vector2DNormalize(Vector2D& v);

// Length
float Vector2DLength(const Vector2D& v);

// Dot Product
float DotProduct2D(const Vector2D& a, const Vector2D& b);

// Linearly interpolate between two vectors
void Vector2DLerp(const Vector2D& src1, const Vector2D& src2, float t, Vector2D& dest);


//-----------------------------------------------------------------------------
//
// Inlined Vector2D methods
//
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// constructors
//-----------------------------------------------------------------------------

inline Vector2D::Vector2D(void)
{
#ifdef _DEBUG
	// Initialize to NAN to catch errors
	//x = y = float_NAN;
#endif
}

inline Vector2D::Vector2D(float X, float Y)
{
	x = X; y = Y;
	Assert(IsValid());
}

inline Vector2D::Vector2D(const float* pFloat)
{
	Assert(pFloat);
	x = pFloat[0]; y = pFloat[1];
	Assert(IsValid());
}


//-----------------------------------------------------------------------------
// copy constructor
//-----------------------------------------------------------------------------

inline Vector2D::Vector2D(const Vector2D& vOther)
{
	Assert(vOther.IsValid());
	x = vOther.x; y = vOther.y;
}

//-----------------------------------------------------------------------------
// initialization
//-----------------------------------------------------------------------------

inline void Vector2D::Init(float ix, float iy)
{
	x = ix; y = iy;
	Assert(IsValid());
}

inline void Vector2D::Random(float minVal, float maxVal)
{
	x = minVal + ((float)rand() / VALVE_RAND_MAX) * (maxVal - minVal);
	y = minVal + ((float)rand() / VALVE_RAND_MAX) * (maxVal - minVal);
}

inline void Vector2DClear(Vector2D& a)
{
	a.x = a.y = 0.0f;
}

//-----------------------------------------------------------------------------
// assignment
//-----------------------------------------------------------------------------

inline Vector2D& Vector2D::operator=(const Vector2D& vOther)
{
	Assert(vOther.IsValid());
	x = vOther.x; y = vOther.y;
	return *this;
}

//-----------------------------------------------------------------------------
// Array access
//-----------------------------------------------------------------------------

inline float& Vector2D::operator[](int i)
{
	Assert((i >= 0) && (i < 2));
	return ((float*)this)[i];
}

inline float Vector2D::operator[](int i) const
{
	Assert((i >= 0) && (i < 2));
	return ((float*)this)[i];
}

//-----------------------------------------------------------------------------
// Base address...
//-----------------------------------------------------------------------------

inline float* Vector2D::Base()
{
	return (float*)this;
}

inline float const* Vector2D::Base() const
{
	return (float const*)this;
}

//-----------------------------------------------------------------------------
// IsValid?
//-----------------------------------------------------------------------------

inline bool Vector2D::IsValid() const
{
	return IsFinite(x) && IsFinite(y);
}

//-----------------------------------------------------------------------------
// comparison
//-----------------------------------------------------------------------------

inline bool Vector2D::operator==(const Vector2D& src) const
{
	Assert(src.IsValid() && IsValid());
	return (src.x == x) && (src.y == y);
}

inline bool Vector2D::operator!=(const Vector2D& src) const
{
	Assert(src.IsValid() && IsValid());
	return (src.x != x) || (src.y != y);
}


//-----------------------------------------------------------------------------
// Copy
//-----------------------------------------------------------------------------

inline void Vector2DCopy(const Vector2D& src, Vector2D& dst)
{
	Assert(src.IsValid());
	dst.x = src.x;
	dst.y = src.y;
}

inline void	Vector2D::CopyToArray(float* rgfl) const
{
	Assert(IsValid());
	Assert(rgfl);
	rgfl[0] = x; rgfl[1] = y;
}

//-----------------------------------------------------------------------------
// standard math operations
//-----------------------------------------------------------------------------

inline void Vector2D::Negate()
{
	Assert(IsValid());
	x = -x; y = -y;
}

inline Vector2D& Vector2D::operator+=(const Vector2D& v)
{
	Assert(IsValid() && v.IsValid());
	x += v.x; y += v.y;
	return *this;
}

inline Vector2D& Vector2D::operator-=(const Vector2D& v)
{
	Assert(IsValid() && v.IsValid());
	x -= v.x; y -= v.y;
	return *this;
}

inline Vector2D& Vector2D::operator*=(float fl)
{
	x *= fl;
	y *= fl;
	Assert(IsValid());
	return *this;
}

inline Vector2D& Vector2D::operator*=(const Vector2D& v)
{
	x *= v.x;
	y *= v.y;
	Assert(IsValid());
	return *this;
}

inline Vector2D& Vector2D::operator/=(float fl)
{
	Assert(fl != 0.0f);
	float oofl = 1.0f / fl;
	x *= oofl;
	y *= oofl;
	Assert(IsValid());
	return *this;
}

inline Vector2D& Vector2D::operator/=(const Vector2D& v)
{
	Assert(v.x != 0.0f && v.y != 0.0f);
	x /= v.x;
	y /= v.y;
	Assert(IsValid());
	return *this;
}

inline void Vector2DAdd(const Vector2D& a, const Vector2D& b, Vector2D& c)
{
	Assert(a.IsValid() && b.IsValid());
	c.x = a.x + b.x;
	c.y = a.y + b.y;
}

inline void Vector2DSubtract(const Vector2D& a, const Vector2D& b, Vector2D& c)
{
	Assert(a.IsValid() && b.IsValid());
	c.x = a.x - b.x;
	c.y = a.y - b.y;
}

inline void Vector2DMultiply(const Vector2D& a, float b, Vector2D& c)
{
	Assert(a.IsValid() && IsFinite(b));
	c.x = a.x * b;
	c.y = a.y * b;
}

inline void Vector2DMultiply(const Vector2D& a, const Vector2D& b, Vector2D& c)
{
	Assert(a.IsValid() && b.IsValid());
	c.x = a.x * b.x;
	c.y = a.y * b.y;
}


inline void Vector2DDivide(const Vector2D& a, float b, Vector2D& c)
{
	Assert(a.IsValid());
	Assert(b != 0.0f);
	float oob = 1.0f / b;
	c.x = a.x * oob;
	c.y = a.y * oob;
}

inline void Vector2DDivide(const Vector2D& a, const Vector2D& b, Vector2D& c)
{
	Assert(a.IsValid());
	Assert((b.x != 0.0f) && (b.y != 0.0f));
	c.x = a.x / b.x;
	c.y = a.y / b.y;
}

inline void Vector2DMA(const Vector2D& start, float s, const Vector2D& dir, Vector2D& result)
{
	Assert(start.IsValid() && IsFinite(s) && dir.IsValid());
	result.x = start.x + s * dir.x;
	result.y = start.y + s * dir.y;
}

// FIXME: Remove
// For backwards compatability
inline void	Vector2D::MulAdd(const Vector2D& a, const Vector2D& b, float scalar)
{
	x = a.x + b.x * scalar;
	y = a.y + b.y * scalar;
}

inline void Vector2DLerp(const Vector2D& src1, const Vector2D& src2, float t, Vector2D& dest)
{
	dest[0] = src1[0] + (src2[0] - src1[0]) * t;
	dest[1] = src1[1] + (src2[1] - src1[1]) * t;
}

//-----------------------------------------------------------------------------
// dot, cross
//-----------------------------------------------------------------------------
inline float DotProduct2D(const Vector2D& a, const Vector2D& b)
{
	Assert(a.IsValid() && b.IsValid());
	return(a.x * b.x + a.y * b.y);
}

// for backwards compatability
inline float Vector2D::Dot(const Vector2D& vOther) const
{
	return DotProduct2D(*this, vOther);
}


//-----------------------------------------------------------------------------
// length
//-----------------------------------------------------------------------------
inline float Vector2DLength(const Vector2D& v)
{
	Assert(v.IsValid());
	return (float)FastSqrt(v.x * v.x + v.y * v.y);
}

inline float Vector2D::LengthSqr(void) const
{
	Assert(IsValid());
	return (x * x + y * y);
}

inline float Vector2D::NormalizeInPlace()
{
	return Vector2DNormalize(*this);
}

inline bool Vector2D::IsLengthGreaterThan(float val) const
{
	return LengthSqr() > val * val;
}

inline bool Vector2D::IsLengthLessThan(float val) const
{
	return LengthSqr() < val * val;
}

inline float Vector2D::Length(void) const
{
	return Vector2DLength(*this);
}


inline void Vector2DMin(const Vector2D& a, const Vector2D& b, Vector2D& result)
{
	result.x = (a.x < b.x) ? a.x : b.x;
	result.y = (a.y < b.y) ? a.y : b.y;
}


inline void Vector2DMax(const Vector2D& a, const Vector2D& b, Vector2D& result)
{
	result.x = (a.x > b.x) ? a.x : b.x;
	result.y = (a.y > b.y) ? a.y : b.y;
}


//-----------------------------------------------------------------------------
// Normalization
//-----------------------------------------------------------------------------
inline float Vector2DNormalize(Vector2D& v)
{
	Assert(v.IsValid());
	float l = v.Length();
	if (l != 0.0f)
	{
		v /= l;
	}
	else
	{
		v.x = v.y = 0.0f;
	}
	return l;
}


//-----------------------------------------------------------------------------
// Get the distance from this Vector2D to the other one
//-----------------------------------------------------------------------------
inline float Vector2D::DistTo(const Vector2D& vOther) const
{
	Vector2D delta;
	Vector2DSubtract(*this, vOther, delta);
	return delta.Length();
}

inline float Vector2D::DistToSqr(const Vector2D& vOther) const
{
	Vector2D delta;
	Vector2DSubtract(*this, vOther, delta);
	return delta.LengthSqr();
}


//-----------------------------------------------------------------------------
// Computes the closest point to vecTarget no farther than flMaxDist from vecStart
//-----------------------------------------------------------------------------
inline void ComputeClosestPoint2D(const Vector2D& vecStart, float flMaxDist, const Vector2D& vecTarget, Vector2D* pResult)
{
	Vector2D vecDelta;
	Vector2DSubtract(vecTarget, vecStart, vecDelta);
	float flDistSqr = vecDelta.LengthSqr();
	if (flDistSqr <= flMaxDist * flMaxDist)
	{
		*pResult = vecTarget;
	}
	else
	{
		vecDelta /= FastSqrt(flDistSqr);
		Vector2DMA(vecStart, flMaxDist, vecDelta, *pResult);
	}
}



//-----------------------------------------------------------------------------
//
// Slow methods
//
//-----------------------------------------------------------------------------

#ifndef VECTOR_NO_SLOW_OPERATIONS
#endif
//-----------------------------------------------------------------------------
// Returns a Vector2D with the min or max in X, Y, and Z.
//-----------------------------------------------------------------------------

inline Vector2D Vector2D::Min(const Vector2D& vOther) const
{
	return Vector2D(x < vOther.x ? x : vOther.x,
		y < vOther.y ? y : vOther.y);
}

inline Vector2D Vector2D::Max(const Vector2D& vOther) const
{
	return Vector2D(x > vOther.x ? x : vOther.x,
		y > vOther.y ? y : vOther.y);
}


//-----------------------------------------------------------------------------
// arithmetic operations
//-----------------------------------------------------------------------------

inline Vector2D Vector2D::operator-(void) const
{
	return Vector2D(-x, -y);
}

inline Vector2D Vector2D::operator+(const Vector2D& v) const
{
	Vector2D res;
	Vector2DAdd(*this, v, res);
	return res;
}

inline Vector2D Vector2D::operator-(const Vector2D& v) const
{
	Vector2D res;
	Vector2DSubtract(*this, v, res);
	return res;
}

inline Vector2D Vector2D::operator*(float fl) const
{
	Vector2D res;
	Vector2DMultiply(*this, fl, res);
	return res;
}

inline Vector2D Vector2D::operator*(const Vector2D& v) const
{
	Vector2D res;
	Vector2DMultiply(*this, v, res);
	return res;
}

inline Vector2D Vector2D::operator/(float fl) const
{
	Vector2D res;
	Vector2DDivide(*this, fl, res);
	return res;
}

inline Vector2D Vector2D::operator/(const Vector2D& v) const
{
	Vector2D res;
	Vector2DDivide(*this, v, res);
	return res;
}

inline Vector2D operator*(float fl, const Vector2D& v)
{
	return v * fl;
}


struct SVector
{
	float x;
	float y;
	float z;
	SVector(float x1, float y1, float z1)
	{
		x = x1;
		y = y1;
		z = z1;
	}
};



inline float ToMeters(float x)
{
	return x / 39.62f;
}


namespace Utils
{
	static inline  float sqrtf_(float x)
	{
		union { float f; uint32_t i; } z = { x };
		z.i = 0x5f3759df - (z.i >> 1);
		z.f *= (1.5f - (x * 0.5f * z.f * z.f));
		z.i = 0x7EEEEEEE - z.i;
		return z.f;
	}


	static inline double powf_(double x, int y)
	{
		double temp;
		if (y == 0)
			return 1;
		temp = powf_(x, y / 2);
		if ((y % 2) == 0) {
			return temp * temp;
		}
		else {
			if (y > 0)
				return x * temp * temp;
			else
				return (temp * temp) / x;
		}
	}
	static inline double GetCrossDistance(double x1, double y1, double x2, double y2)
	{
		return sqrtf_(powf_((float)(x1 - x2), (float)2) + powf_((float)(y1 - y2), (float)2));
	}
}
static inline bool check_in_fov(Vector2D screen_body, float FOVmax,int width, int hight)
{

	float Dist = Utils::GetCrossDistance(screen_body.x, screen_body.y, (width / 2), (hight / 2));

	if (Dist < 90.f)//wtf is this?
	{
		float Radius = FOVmax;

		if (screen_body.x <= ((width / 2) + Radius) &&
			screen_body.x >= ((width / 2) - Radius) &&
			screen_body.y <= ((hight / 2) + Radius) &&
			screen_body.y >= ((hight / 2) - Radius))
		{
			FOVmax = Dist;
			return true;
		}
		return false;
	}
}

static inline Vector rotateVectorAroundOrigin(const Vector& vec, float yaw) {
	// Normalize yaw angle to be within [0, 360) degrees
	while (yaw < 0.0f) {
		yaw += 360.0f;
	}
	while (yaw >= 360.0f) {
		yaw -= 360.0f;
	}

	// Convert yaw to radians
	float yawRad = yaw * (M_PI / 180.0);

	// Compute sine and cosine of yaw
	float sinYaw = sin(yawRad);
	float cosYaw = cos(yawRad);

	// Perform rotation around the y-axis (upward direction)
	Vector rotatedVec;
	rotatedVec.x = vec.x * cosYaw - vec.z * sinYaw;
	rotatedVec.y = vec.y;
	rotatedVec.z = vec.x * sinYaw + vec.z * cosYaw;

	return rotatedVec;
}

inline void Vector::Rotate(const Vector& vCenter, const Vector& vAngles)
{
	Vector vDelta = *this - vCenter;
	Vector vNewDelta;

	float fSinYaw = sin(DEG2RAD(vAngles.y));
	float fCosYaw = cos(DEG2RAD(vAngles.y));
	float fSinPitch = sin(DEG2RAD(vAngles.x));
	float fCosPitch = cos(DEG2RAD(vAngles.x));
	float fSinRoll = sin(DEG2RAD(vAngles.z));
	float fCosRoll = cos(DEG2RAD(vAngles.z));

	vNewDelta.x = vDelta.x * (fCosYaw * fCosPitch) + vDelta.y * (fCosYaw * fSinPitch * fSinRoll - fSinYaw * fCosRoll) + vDelta.z * (fCosYaw * fSinPitch * fCosRoll + fSinYaw * fSinRoll);
	vNewDelta.y = vDelta.x * (fSinYaw * fCosPitch) + vDelta.y * (fSinYaw * fSinPitch * fSinRoll + fCosYaw * fCosRoll) + vDelta.z * (fSinYaw * fSinPitch * fCosRoll - fCosYaw * fSinRoll);
	vNewDelta.z = vDelta.x * (-fSinPitch) + vDelta.y * (fCosPitch * fSinRoll) + vDelta.z * (fCosPitch * fCosRoll);

	*this = vNewDelta + vCenter;
}