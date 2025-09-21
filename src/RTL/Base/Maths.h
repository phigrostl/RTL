#pragma once

#include "RTL/Base/Base.h"

#include <cmath>
#include <string>
#include <iostream>
#include <algorithm>
#include <vector>

namespace RTL {

    constexpr float PI = 3.14159265359f;
    constexpr float EPSILON = 1e-5f;

    struct Vec2 {
        float X, Y;

        constexpr Vec2()
            : X(0.0f), Y(0.0f) {}
        constexpr Vec2(float x, float y)
            : X(x), Y(y) {}
    };

    struct Vec3 {
        float X, Y, Z;

        constexpr Vec3()
            : X(0.0f), Y(0.0f), Z(0.0f) {}
        constexpr Vec3(float val)
            : X(val), Y(val), Z(val) {}
        constexpr Vec3(Vec2 v, float z)
            : X(v.X), Y(v.Y), Z(z) {}
        constexpr Vec3(float x, float y, float z)
            : X(x), Y(y), Z(z) {}

        operator Vec2() const { return { X, Y }; }

    };

    struct Vec4 {
        float X, Y, Z, W;

        constexpr Vec4()
            : X(0.0f), Y(0.0f), Z(0.0f), W(0.0f) {}
        constexpr Vec4(float val)
            : X(val), Y(val), Z(val), W(val) {}
        constexpr Vec4(float x, float y, float z, float w)
            : X(x), Y(y), Z(z), W(w) {}
        constexpr Vec4(const Vec2& vec2, float z, float w)
            : X(vec2.X), Y(vec2.Y), Z(z), W(w) {}
        constexpr Vec4(const Vec3& vec3, float w)
            : X(vec3.X), Y(vec3.Y), Z(vec3.Z), W(w) {}

        operator Vec2() const { return { X, Y }; }
        operator Vec3() const { return { X, Y, Z }; }
    };

    struct Mat4 {
        float M[4][4];

        constexpr Mat4() : M{ {0.0f, 0.0f, 0.0f, 0.0f},
                              {0.0f, 0.0f, 0.0f, 0.0f},
                              {0.0f, 0.0f, 0.0f, 0.0f},
                              {0.0f, 0.0f, 0.0f, 0.0f} } {}

        Mat4(const Vec4& v0, const Vec4& v1, const Vec4& v2, const Vec4& v3);
    };

    Vec2 operator+ (const Vec2& left, const Vec2& right);
    Vec2 operator- (const Vec2& left, const Vec2& right);

    Vec3 operator+ (const Vec3& left, const Vec3& right);
    Vec3 operator- (const Vec3& left, const Vec3& right);
    Vec3 operator* (const float left, const Vec3& right);
    Vec3 operator* (const Vec3& left, const float right);
    Vec3 operator* (const Vec3& left, const Vec3& right);
    Vec3 operator/ (const Vec3& left, const float right);
    Vec3 operator/ (const Vec3& left, const Vec3& right);
    Vec3& operator*= (Vec3& left, const float right);
    Vec3& operator/= (Vec3& left, const float right);
    Vec3& operator+= (Vec3& left, const Vec3& right);

    float Dot(const Vec3& left, const Vec3& right);
    Vec3 Cross(const Vec3& left, const Vec3& right);
    Vec3 Normalize(const Vec3&);
    Vec3 Reflect(const Vec3& in, const Vec3& normal);

    Vec4 operator+ (const Vec4& left, const Vec4& right);
    Vec4 operator- (const Vec4& left, const Vec4& right);
    Vec4 operator* (const float left, const Vec4& right);
    Vec4 operator* (const Vec4& left, const float right);
    Vec4 operator/ (const Vec4& left, const float right);

    Vec4& operator+= (Vec4& left, const Vec4& right);
    Vec4& operator-= (Vec4& left, const Vec4& right);

    Vec4 operator* (const Mat4& mat4, const Vec4& vec4);
    Mat4 operator* (const Mat4& left, const Mat4& right);
    Mat4& operator*= (Mat4& left, const Mat4& right);

    Mat4 Mat4Scale(const float sx, const float sy, const float sz);
    Mat4 Mat4RotateX(const float angle);
    Mat4 Mat4RotateY(const float angle);
    Mat4 Mat4RotateZ(const float angle);
    Mat4 Mat4Translate(const float tx, const float ty, const float tz);
    constexpr Mat4 Mat4Identity() {
        Mat4 m;
        m.M[0][0] = 1.0f;
        m.M[1][1] = 1.0f;
        m.M[2][2] = 1.0f;
        m.M[3][3] = 1.0f;
        return m;
    }
    Mat4 Mat4Perspective(const float fovy, const float aspect, const float near, const float far);
    Mat4 Mat4LookAt(const Vec3& eye, const Vec3& target, const Vec3& up);

    float Lerp(const float start, const float end, const float t);
    Vec3 Lerp(const Vec3& start, const Vec3& end, const float t);
    Vec4 Lerp(const Vec4& start, const Vec4& end, const float t);

    float Clamp(const float in, const float min, const float max);
    Vec3 Clamp(const Vec3& vec, const float min, const float max);
    Vec4 Clamp(const Vec4& vec, const float min, const float max);

    Vec3 Pow(const Vec3& vec, const float exponent);

    float Length(const Vec3& v);

    unsigned char Float2UChar(const float f);
    float UChar2Float(const unsigned char c);

    float Max(const float right, const float left);
    float Min(const float right, const float left);

    std::vector<size_t> GetNumbersFromString(std::string str);
}