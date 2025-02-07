#pragma once

#include <math.h>
#include <iostream>
#include <sstream>

template <typename T>
class Vec3 {
public:
    T x, y, z;

    Vec3(T x = 0, T y = 0, T z = 0) : x(x), y(y), z(z) {}

    Vec3(const Vec3<T>& other) : x(other.x), y(other.y), z(other.z) {}

    Vec3(const Vec3<T>& start, const Vec3<T>& end)
        : x(end.x - start.x), y(end.y - start.y), z(end.z - start.z) {}

    void normalize() {
        T len = this->length();
        if (len != 0 && len != 1) {
            *this /= len;
        }
    }

    T length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    T angleWith(const Vec3<T>& other) const {
        T len1 = this->length();
        T len2 = other.length();
        if (len1 == 0 || len2 == 0) {
            return 0;  
        }
        return std::acos((*this * other) / (len1 * len2));
    }

    T distanceFrom(const Vec3<T>& other) const {
        return std::sqrt(
            (other.x - x) * (other.x - x) +
            (other.y - y) * (other.y - y) +
            (other.z - z) * (other.z - z)
        );
    }

    Vec3<T>& operator=(const Vec3<T>& other) {
        if (this != &other) { 
            this->x = other.x;
            this->y = other.y;
            this->z = other.z;
        }
        return *this;
    }

    Vec3<T> operator+(const Vec3<T>& other) const {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }

    Vec3<T>& operator+=(const Vec3<T>& other) {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    Vec3<T> operator-(const Vec3<T>& other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    Vec3<T>& operator-=(const Vec3<T>& other) {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    Vec3<T> operator*(const T& scalar) const {
        return Vec3(x * scalar, y * scalar, z * scalar);
    }

    Vec3<T>& operator*=(const T& scalar) {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    Vec3<T> operator/(const T& scalar) const {
        if (scalar == 0) {
            std::stringstream os;
            os << "Vector division by zero error at: (" << x << ", " << y << ", " << z << ")";
            throw std::runtime_error(os.str());
        }
        return Vec3(x / scalar, y / scalar, z / scalar);
    }

    Vec3<T>& operator/=(const T& scalar) {
        if (scalar == 0) {
            std::stringstream os;
            os << "Vector division by zero error at: (" << x << ", " << y << ", " << z << ")";
            throw std::runtime_error(os.str());
        }
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    // Dot product 
    T operator*(const Vec3<T>& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    // Cross product 
    Vec3<T> operator^(const Vec3<T>& other) const {
        return Vec3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x);
    }

    Vec3<T>& operator^=(const Vec3<T>& other) {
        T newX = y * other.z - z * other.y;
        T newY = z * other.x - x * other.z;
        T newZ = x * other.y - y * other.x;
        x = newX;
        y = newY;
        z = newZ;
        return *this;
    }

    void rotate(float thetaX, float thetaY, float thetaZ) {
        float sinX = sin(thetaX), cosX = cos(thetaX);
        float sinY = sin(thetaY), cosY = cos(thetaY);
        float sinZ = sin(thetaZ), cosZ = cos(thetaZ);

        
        float newX = (cosY*cosZ)*x + (cosZ*sinY*sinX - sinZ*cosX)*y + (cosZ*sinY*cosX + sinZ*sinX)*z;
        float newY = (cosY*sinZ)*x + (sinZ*sinY*sinX + cosZ*cosX)*y + (sinZ*sinY*cosX - cosZ*sinX)*z;
        float newZ = (-sinY)*x + (cosY*sinX)*y + (cosY*cosX)*z;

        x = newX;
        y = newY;
        z = newZ;
    }

    template <typename U>
    friend Vec3<U> operator+(const U& scalar, const Vec3<U>& vec);

    template <typename U>
    friend Vec3<U> operator-(const U& scalar, const Vec3<U>& vec);

    template <typename U>
    friend Vec3<U> operator*(const U& scalar, const Vec3<U>& vec);

    template <typename U>
    friend Vec3<U> operator/(const U& scalar, const Vec3<U>& vec);

    template <typename U>
    friend std::ostream& operator<<(std::ostream& os, const Vec3<U>& vec3);
};

// --- Friend Function Definitions ---
template <typename U>
Vec3<U> operator+(const U& scalar, const Vec3<U>& vec) {
    return Vec3<U>(scalar + vec.x, scalar + vec.y, scalar + vec.z);
}

template <typename U>
Vec3<U> operator-(const U& scalar, const Vec3<U>& vec) {
    return Vec3<U>(scalar - vec.x, scalar - vec.y, scalar - vec.z);
}

template <typename U>
Vec3<U> operator*(const U& scalar, const Vec3<U>& vec) {
    return Vec3<U>(scalar * vec.x, scalar * vec.y, scalar * vec.z);
}

template <typename U>
Vec3<U> operator/(const U& scalar, const Vec3<U>& vec) {
    if (vec.x == 0 || vec.y == 0 || vec.z == 0) {
        throw std::runtime_error("Division by zero in vector operation.");
    }
    return Vec3<U>(scalar / vec.x, scalar / vec.y, scalar / vec.z);
}

template <typename U>
std::ostream& operator<<(std::ostream& os, const Vec3<U>& vec3) {
    os << "(" << vec3.x << ", " << vec3.y << ", " << vec3.z << ")";
    return os;
}

template <typename T>
class Triangle {
public:
    Vec3<T> A, B, C;
    Vec3<T> AB, AC, BC;
    Vec3<T> normal;
    float s;
    T D;

    Triangle(const Vec3<T>& A, const Vec3<T>& B, const Vec3<T>& C) {
        this->A = A;
        this->B = B;
        this->C = C;
        AB = Vec3<T>(A, B); 
        AC = Vec3<T>(A, C);
        BC = Vec3<T>(B, C);
        normal = (AB ^ BC);
        s = 0.5 * normal.length();
        D = -normal.x * A.x - normal.y * A.y - normal.z * A.z;
    }

    Triangle(const Triangle<T>& other) {
        A = other.A;
        B = other.B;
        C = other.C;
        AB = Vec3<T>(other.A, other.B); 
        AC = Vec3<T>(other.A, other.C);
        BC = Vec3<T>(other.B, other.C);
        normal = (other.AB ^ other.AC);
        s = 0.5f * normal.length();
        D = -normal.x * A.x - normal.y * A.y - normal.z * A.z;
    }

    ~Triangle() {}

    float getXFrom(float y, float z) {
        if (normal.x == 0) {
            return std::max(A.x, std::max(B.x, C.x));
        } else {
            return (-D - normal.y * y - normal.z * z) / normal.x;
        }
    }

    float getYFrom(float x, float z) {
        if (normal.y == 0) {
            return std::max(A.y, std::max(B.y, C.y));
        } else {
            return (-D - normal.x * x - normal.z * z) / normal.y;
        }
    }

    float getZFrom(float x, float y) {
        if (normal.z == 0) {
            return std::max(A.z, std::max(B.z, C.z));
        } else {
            return (-D - normal.x * x - normal.y * y) / normal.z;
        }
    }

    float sign(const Vec3<float>& v0, const Vec3<float>& v1, Vec3<float>& v2) {
        return (v0.x - v2.x) * (v1.y - v2.y) - (v1.x - v2.x) * (v0.y - v2.y);
    }

    bool containsVertex(const Vec3<float>& vertex) {

        float d0, d1, d2;
        bool hasNeg, hadPos;

        d0 = sign(vertex, A, B);
        d1 = sign(vertex, B, C);
        d2 = sign(vertex, C, A);

        hasNeg = (d0 < 0) || (d1 < 0) || (d2 < 0);
        hadPos = (d0 > 0) || (d1 > 0) || (d2 > 0);

        return !(hasNeg && hadPos);
    }

    template <typename u>
    friend std::ostream& operator<<(std::ostream& os, const Triangle<T>& Triangle);
};

template <typename T>
std::ostream& operator<<(std::ostream& os, Triangle<T>& Triangle) {
    os << "P(x): " << Triangle.normal.x << "x + " << Triangle.normal.y << "y + " << Triangle.normal.z << "z + " << Triangle.D;
    return os;
}