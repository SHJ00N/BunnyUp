#pragma once

#include <cfloat>
#include <DirectXMath.h>

namespace Engine
{
	constexpr float PI = 3.141592654f;
	
	struct Vector2
	{
		float x, y;
		constexpr Vector2() noexcept : x(0), y(0) {}
		constexpr Vector2(float x, float y) noexcept : x(x), y(y) {}

		// convert to XMVECTOR
		DirectX::XMVECTOR ToSIMD() const noexcept
		{
			return DirectX::XMLoadFloat2(reinterpret_cast<const DirectX::XMFLOAT2*>(&x));
		}
		// convert from XMVECTOR
		static Vector2 FromSIMD(DirectX::XMVECTOR v) noexcept
		{
			Vector2 result;
			DirectX::XMStoreFloat2(reinterpret_cast<DirectX::XMFLOAT2*>(&result.x), v);
			return result;
		}
	};

	struct Vector3
	{
		float x, y, z;
		constexpr Vector3() noexcept : x(0), y(0), z(0) {}
		constexpr Vector3(float x, float y, float z) noexcept : x(x), y(y), z(z) {}
		
		// convert to XMVECTOR
		DirectX::XMVECTOR ToSIMD() const noexcept
		{
			return DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(&x));
		}
		// convert from XMVECTOR
		static Vector3 FromSIMD(DirectX::XMVECTOR v) noexcept
		{
			Vector3 result;
			DirectX::XMStoreFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&result.x), v);
			return result;
		}
	};

	struct Vector4
	{
		float x, y, z, w;
		constexpr Vector4() noexcept : x(0), y(0), z(0), w(0) {}
		constexpr Vector4(float x, float y, float z, float w) noexcept : x(x), y(y), z(z), w(w) {}
		
		// convert to XMVECTOR
		DirectX::XMVECTOR ToSIMD() const noexcept
		{
			return DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&x));
		}
		// convert from XMVECTOR
		static Vector4 FromSIMD(DirectX::XMVECTOR v) noexcept
		{
			Vector4 result;
			DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&result.x), v);
			return result;
		}
	};

	struct Quaternion
	{
		float x, y, z, w;

		constexpr Quaternion() noexcept : x(0), y(0), z(0), w(1.0f) {}
		constexpr Quaternion(float x, float y, float z, float w) noexcept : x(x), y(y), z(z), w(w) {}

		static Quaternion Identity() noexcept
		{
			return Quaternion(0, 0, 0, 1.0f);
		}

		// convert to XMVECTOR
		DirectX::XMVECTOR ToSIMD() const noexcept
		{
			return DirectX::XMLoadFloat4(reinterpret_cast<const DirectX::XMFLOAT4*>(&x));
		}
		// convert from XMVECTOR
		static Quaternion FromSIMD(DirectX::XMVECTOR v) noexcept
		{
			Quaternion result;
			DirectX::XMStoreFloat4(reinterpret_cast<DirectX::XMFLOAT4*>(&result.x), v);
			return result;
		}
	};

	struct Matrix4x4
	{
		union
		{
			struct
			{
				float m00, m01, m02, m03;
				float m10, m11, m12, m13;
				float m20, m21, m22, m23;
				float m30, m31, m32, m33;
			};
			float m[4][4];
		};

		constexpr Matrix4x4() noexcept
			: m00(0), m01(0), m02(0), m03(0),
			  m10(0), m11(0), m12(0), m13(0),
			  m20(0), m21(0), m22(0), m23(0),
			  m30(0), m31(0), m32(0), m33(0) {
		}

		constexpr Matrix4x4(float n) noexcept
			: m00(n), m01(0), m02(0), m03(0),
			m10(0), m11(n), m12(0), m13(0),
			m20(0), m21(0), m22(n), m23(0),
			m30(0), m31(0), m32(0), m33(n) {
		}

		Matrix4x4(const DirectX::XMFLOAT4X4& m) noexcept
			: m00(m._11), m01(m._12), m02(m._13), m03(m._14),
			m10(m._21), m11(m._22), m12(m._23), m13(m._24),
			m20(m._31), m21(m._32), m22(m._33), m23(m._34),
			m30(m._41), m31(m._42), m32(m._43), m33(m._44) {
		}

		// identity matrix
		static Matrix4x4 Identity() noexcept
		{
			return Matrix4x4(1.0f);
		}

		// convert to XMMATRIX
		DirectX::XMMATRIX ToSIMD() const noexcept
		{
			return DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(&m00));
		}
		// convert from XMMATRIX
		static Matrix4x4 FromSIMD(DirectX::XMMATRIX m) noexcept
		{
			Matrix4x4 result;
			DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&result.m00), m);
			return result;
		}
	};

	// operator overloads
	// Vector2
	inline Vector2 operator+(const Vector2& a, const Vector2& b) noexcept { return Vector2::FromSIMD(DirectX::XMVectorAdd(a.ToSIMD(), b.ToSIMD())); }
	inline Vector2 operator-(const Vector2& a, const Vector2& b) noexcept { return Vector2::FromSIMD(DirectX::XMVectorSubtract(a.ToSIMD(), b.ToSIMD())); }
	inline Vector2 operator*(const Vector2& a, const Vector2& b) noexcept { return Vector2::FromSIMD(DirectX::XMVectorMultiply(a.ToSIMD(), b.ToSIMD())); }
	inline Vector2 operator/(const Vector2& a, const Vector2& b) noexcept { return Vector2::FromSIMD(DirectX::XMVectorDivide(a.ToSIMD(), b.ToSIMD())); }

	inline Vector2 operator*(const Vector2& v, float scalar) noexcept { return Vector2::FromSIMD(DirectX::XMVectorScale(v.ToSIMD(), scalar)); }
	inline Vector2 operator*(float scalar, const Vector2& v) noexcept { return Vector2::FromSIMD(DirectX::XMVectorScale(v.ToSIMD(), scalar)); }
	inline Vector2 operator/(const Vector2& v, float scalar) noexcept { assert(scalar != 0.0f); return Vector2::FromSIMD(DirectX::XMVectorScale(v.ToSIMD(), 1.0f / scalar)); }

	inline Vector2& operator+=(Vector2& a, const Vector2& b) noexcept { a = a + b; return a; }
	inline Vector2& operator-=(Vector2& a, const Vector2& b) noexcept { a = a - b; return a; }
	inline Vector2& operator*=(Vector2& a, const Vector2& b) noexcept { a = a * b; return a; }
	inline Vector2& operator/=(Vector2& a, const Vector2& b) noexcept { a = a / b; return a; }

	inline Vector2& operator*=(Vector2& v, float scalar) noexcept { v = v * scalar; return v; }
	inline Vector2& operator/=(Vector2& v, float scalar) noexcept { assert(scalar != 0.0f); v = v / scalar; return v; }

	// Vector3
	inline Vector3 operator+(const Vector3& a, const Vector3& b) noexcept { return Vector3::FromSIMD(DirectX::XMVectorAdd(a.ToSIMD(), b.ToSIMD())); }
	inline Vector3 operator-(const Vector3& a, const Vector3& b) noexcept { return Vector3::FromSIMD(DirectX::XMVectorSubtract(a.ToSIMD(), b.ToSIMD())); }
	inline Vector3 operator*(const Vector3& a, const Vector3& b) noexcept { return Vector3::FromSIMD(DirectX::XMVectorMultiply(a.ToSIMD(), b.ToSIMD())); }
	inline Vector3 operator/(const Vector3& a, const Vector3& b) noexcept { return Vector3::FromSIMD(DirectX::XMVectorDivide(a.ToSIMD(), b.ToSIMD())); }

	inline Vector3 operator*(const Vector3& v, float scalar) noexcept { return Vector3::FromSIMD(DirectX::XMVectorScale(v.ToSIMD(), scalar)); }
	inline Vector3 operator*(float scalar, const Vector3& v) noexcept { return Vector3::FromSIMD(DirectX::XMVectorScale(v.ToSIMD(), scalar)); }
	inline Vector3 operator/(const Vector3& v, float scalar) noexcept { assert(scalar != 0.0f); return Vector3::FromSIMD(DirectX::XMVectorScale(v.ToSIMD(), 1.0f / scalar)); }

	inline Vector3& operator+=(Vector3& a, const Vector3& b) noexcept { a = a + b; return a; }
	inline Vector3& operator-=(Vector3& a, const Vector3& b) noexcept { a = a - b; return a; }
	inline Vector3& operator*=(Vector3& a, const Vector3& b) noexcept { a = a * b; return a; }
	inline Vector3& operator/=(Vector3& a, const Vector3& b) noexcept { a = a / b; return a; }

	inline Vector3& operator*=(Vector3& v, float scalar) noexcept { v = v * scalar; return v; }
	inline Vector3& operator/=(Vector3& v, float scalar) noexcept { assert(scalar != 0.0f); v = v / scalar; return v; }

	// Vector4
	inline Vector4 operator+(const Vector4& a, const Vector4& b) noexcept { return Vector4::FromSIMD(DirectX::XMVectorAdd(a.ToSIMD(), b.ToSIMD())); }
	inline Vector4 operator-(const Vector4& a, const Vector4& b) noexcept { return Vector4::FromSIMD(DirectX::XMVectorSubtract(a.ToSIMD(), b.ToSIMD())); }
	inline Vector4 operator*(const Vector4& a, const Vector4& b) noexcept { return Vector4::FromSIMD(DirectX::XMVectorMultiply(a.ToSIMD(), b.ToSIMD())); }
	inline Vector4 operator/(const Vector4& a, const Vector4& b) noexcept { return Vector4::FromSIMD(DirectX::XMVectorDivide(a.ToSIMD(), b.ToSIMD())); }

	inline Vector4 operator*(const Vector4& v, float scalar) noexcept { return Vector4::FromSIMD(DirectX::XMVectorScale(v.ToSIMD(), scalar)); }
	inline Vector4 operator*(float scalar, const Vector4& v) noexcept { return Vector4::FromSIMD(DirectX::XMVectorScale(v.ToSIMD(), scalar)); }
	inline Vector4 operator/(const Vector4& v, float scalar) noexcept { assert(scalar != 0.0f); return Vector4::FromSIMD(DirectX::XMVectorScale(v.ToSIMD(), 1.0f / scalar)); }

	inline Vector4& operator+=(Vector4& a, const Vector4& b) noexcept { a = a + b; return a; }
	inline Vector4& operator-=(Vector4& a, const Vector4& b) noexcept { a = a - b; return a; }
	inline Vector4& operator*=(Vector4& a, const Vector4& b) noexcept { a = a * b; return a; }
	inline Vector4& operator/=(Vector4& a, const Vector4& b) noexcept { a = a / b; return a; }

	inline Vector4& operator*=(Vector4& v, float scalar) noexcept { v = v * scalar; return v; }
	inline Vector4& operator/=(Vector4& v, float scalar) noexcept { assert(scalar != 0.0f); v = v / scalar; return v; }

	// common
	inline float Length(const Vector2& v) noexcept { return DirectX::XMVectorGetX(DirectX::XMVector2Length(v.ToSIMD())); }
	inline float Length(const Vector3& v) noexcept { return DirectX::XMVectorGetX(DirectX::XMVector3Length(v.ToSIMD())); }
	inline float Length(const Vector4& v) noexcept { return DirectX::XMVectorGetX(DirectX::XMVector4Length(v.ToSIMD())); }

	inline float LengthSq(const Vector2& v) noexcept { return DirectX::XMVectorGetX(DirectX::XMVector2LengthSq(v.ToSIMD())); }
	inline float LengthSq(const Vector3& v) noexcept { return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(v.ToSIMD())); }
	inline float LengthSq(const Vector4& v) noexcept { return DirectX::XMVectorGetX(DirectX::XMVector4LengthSq(v.ToSIMD())); }

	inline float Distance(const Vector2& a, const Vector2& b) noexcept { return Length(a - b); }
	inline float Distance(const Vector3& a, const Vector3& b) noexcept { return Length(a - b); }
	inline float Distance(const Vector4& a, const Vector4& b) noexcept { return Length(a - b); }
	
	inline float Dot(const Vector2& a, const Vector2& b) noexcept { return DirectX::XMVectorGetX(DirectX::XMVector2Dot(a.ToSIMD(), b.ToSIMD())); }
	inline float Dot(const Vector3& a, const Vector3& b) noexcept { return DirectX::XMVectorGetX(DirectX::XMVector3Dot(a.ToSIMD(), b.ToSIMD())); }

	inline Vector2 Normalize(const Vector2& v) noexcept
	{ 
		auto simd = v.ToSIMD();
		auto lenSq = DirectX::XMVector2LengthSq(simd);
		if (DirectX::XMVectorGetX(lenSq) <= FLT_EPSILON) return Vector2(); // Avoid division by zero
		return Vector2::FromSIMD(DirectX::XMVectorMultiply(simd, DirectX::XMVectorReciprocalSqrt(lenSq)));
	}
	inline Vector3 Normalize(const Vector3& v) noexcept
	{ 
		auto simd = v.ToSIMD();
		auto lenSq = DirectX::XMVector3LengthSq(simd);
		if (DirectX::XMVectorGetX(lenSq) <= FLT_EPSILON) return Vector3(); // Avoid division by zero
		return Vector3::FromSIMD(DirectX::XMVectorMultiply(simd, DirectX::XMVectorReciprocalSqrt(lenSq)));
	}
	inline Vector4 Normalize(const Vector4& v) noexcept 
	{ 
		auto simd = v.ToSIMD();
		auto lenSq = DirectX::XMVector4LengthSq(simd);
		if (DirectX::XMVectorGetX(lenSq) <= FLT_EPSILON) return Vector4(); // Avoid division by zero
		return Vector4::FromSIMD(DirectX::XMVectorMultiply(simd, DirectX::XMVectorReciprocalSqrt(lenSq)));
	}
	
	inline Vector2 Lerp(const Vector2& a, const Vector2& b, float t) noexcept { return a + (b - a) * t; }
	inline Vector3 Lerp(const Vector3& a, const Vector3& b, float t) noexcept { return a + (b - a) * t; }
	inline Vector4 Lerp(const Vector4& a, const Vector4& b, float t) noexcept { return a + (b - a) * t; }

	inline Vector3 Cross(const Vector3& a, const Vector3& b) noexcept { return Vector3::FromSIMD(DirectX::XMVector3Cross(a.ToSIMD(), b.ToSIMD())); }

	// Quaternion
	inline float Radians(float degrees) noexcept { return degrees * (PI / 180.0f); }
	inline float Degrees(float radians) noexcept { return radians * (180.0f / PI); }
	inline Quaternion operator*(const Quaternion& a, const Quaternion& b) noexcept { return Quaternion::FromSIMD(DirectX::XMQuaternionMultiply(a.ToSIMD(), b.ToSIMD())); }

	inline Quaternion AngleAxis(float angle, const Vector3& axis) noexcept { return Quaternion::FromSIMD(DirectX::XMQuaternionRotationAxis(axis.ToSIMD(), Radians(angle))); }
	inline Quaternion RotationPitchYawRoll(float pitch, float yaw, float roll) noexcept { return Quaternion::FromSIMD(DirectX::XMQuaternionRotationRollPitchYaw(Radians(pitch), Radians(yaw), Radians(roll))); }

	inline Quaternion Normalize(const Quaternion& q) noexcept
	{
		auto simd = q.ToSIMD();
		auto lenSq = DirectX::XMVector4LengthSq(simd);
		if (DirectX::XMVectorGetX(lenSq) <= FLT_EPSILON) return Quaternion::Identity(); // Avoid division by zero
		return Quaternion::FromSIMD(DirectX::XMVectorMultiply(simd, DirectX::XMVectorReciprocalSqrt(lenSq)));
	}
	inline Quaternion Slerp(const Quaternion& a, const Quaternion& b, float t) noexcept { return Quaternion::FromSIMD(DirectX::XMQuaternionSlerp(a.ToSIMD(), b.ToSIMD(), t)); }
	inline Quaternion Inverse(const Quaternion& q) noexcept { return Quaternion::FromSIMD(DirectX::XMQuaternionInverse(q.ToSIMD())); }

	// Matrix4x4
	inline Vector3 operator*(const Matrix4x4& m, const Vector3& v) noexcept { return Vector3::FromSIMD(DirectX::XMVector3Transform(v.ToSIMD(), m.ToSIMD())); }
	inline Vector4 operator*(const Matrix4x4& m, const Vector4& v) noexcept { return Vector4::FromSIMD(DirectX::XMVector4Transform(v.ToSIMD(), m.ToSIMD())); }
	inline Matrix4x4 operator*(const Matrix4x4& m, float scalar) noexcept { return Matrix4x4::FromSIMD(m.ToSIMD() * scalar); }
	inline Matrix4x4 operator*(const Matrix4x4& a, const Matrix4x4& b) noexcept { return Matrix4x4::FromSIMD(DirectX::XMMatrixMultiply(a.ToSIMD(), b.ToSIMD())); }

	inline Matrix4x4 Transpose(const Matrix4x4& m) noexcept { return Matrix4x4::FromSIMD(DirectX::XMMatrixTranspose(m.ToSIMD())); }
	inline Matrix4x4 Inverse(const Matrix4x4& m) noexcept { return Matrix4x4::FromSIMD(DirectX::XMMatrixInverse(nullptr, m.ToSIMD())); }

	// transformation matrices
	inline Matrix4x4 Translation(const Vector3& position) noexcept { return Matrix4x4::FromSIMD(DirectX::XMMatrixTranslation(position.x, position.y, position.z)); }
	inline Matrix4x4 Scaling(const Vector3& scale) noexcept { return Matrix4x4::FromSIMD(DirectX::XMMatrixScaling(scale.x, scale.y, scale.z)); }
	inline Matrix4x4 Rotation(const Quaternion& rotation) noexcept { return Matrix4x4::FromSIMD(DirectX::XMMatrixRotationQuaternion(rotation.ToSIMD())); }
	inline Matrix4x4 TransformMatrix(const Vector3& position, const Quaternion& rotation, const Vector3& scale) noexcept
	{
		return Matrix4x4::FromSIMD(
			DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) *
			DirectX::XMMatrixRotationQuaternion(rotation.ToSIMD()) *
			DirectX::XMMatrixTranslation(position.x, position.y, position.z)
		);
	}

	// view matrix
	inline Matrix4x4 LookAtLH(const Vector3& eye, const Vector3& at, const Vector3& up) noexcept { return Matrix4x4::FromSIMD(DirectX::XMMatrixLookAtLH(eye.ToSIMD(), at.ToSIMD(), up.ToSIMD())); }

	// projection matrices
	inline Matrix4x4 PerspectiveFovLH(float fovAngleY, float aspectRatio, float nearZ, float farZ) { return Matrix4x4::FromSIMD(DirectX::XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ)); }
	inline Matrix4x4 OrthographicLH(float viewWidth, float viewHeight, float nearZ, float farZ) { return Matrix4x4::FromSIMD(DirectX::XMMatrixOrthographicLH(viewWidth, viewHeight, nearZ, farZ)); }
}