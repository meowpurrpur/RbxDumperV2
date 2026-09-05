constexpr float floatEpsilon = 0.001f;

inline bool FloatEqual(float a, float b) {
    return std::fabs(a - b) <= floatEpsilon;
}

struct Vector3 final {
    float x, y, z;

    Vector3() = default;
    Vector3(float xVal, float yVal, float zVal) : x(xVal), y(yVal), z(zVal) {}

    bool operator==(const Vector3& other) const {
        return FloatEqual(x, other.x) &&
               FloatEqual(y, other.y) &&
               FloatEqual(z, other.z);
    }

    bool operator!=(const Vector3& other) const {
        return !(*this == other);
    }
};

struct Color3 final {
    float r, g, b;

    Color3() : r(0.f), g(0.f), b(0.f) {}
    Color3(int rVal, int gVal, int bVal) : r(rVal / 255.f), g(gVal / 255.f), b(bVal / 255.f) {}
    Color3(float rVal, float gVal, float bVal) : r(rVal), g(gVal), b(bVal) {}

    bool operator==(const Color3& other) const {
        return FloatEqual(r, other.r) &&
               FloatEqual(g, other.g) &&
               FloatEqual(b, other.b);
    }

    bool operator!=(const Color3& other) const {
        return !(*this == other);
    }
};

#pragma pack(push, 1)
struct ColorUint_8 {
    uint8_t r;
    uint8_t b;
    uint8_t g;

    ColorUint_8() = default;
    ColorUint_8(uint8_t rVal, uint8_t bVal, uint8_t gVal) : r(rVal), b(bVal), g(gVal) {}

    bool operator==(const ColorUint_8& other) const {
        return r == other.r &&
               g == other.g &&
               b == other.b;
    }

    bool operator!=(const ColorUint_8& other) const {
        return !(*this == other);
    }
};
#pragma pack(pop)

struct Vector2 final {
    float x, y;

    Vector2() = default;
    Vector2(float xVal, float yVal) : x(xVal), y(yVal) {}

    bool operator==(const Vector2& other) const {
        return FloatEqual(x, other.x) &&
               FloatEqual(y, other.y);
    }

    bool operator!=(const Vector2& other) const {
        return !(*this == other);
    }
};

struct CFrame {
    float r00, r01, r02;
    float r10, r11, r12;
    float r20, r21, r22;
    float x, y, z;

    CFrame() = default;

    CFrame(float inR00, float inR01, float inR02,
           float inR10, float inR11, float inR12,
           float inR20, float inR21, float inR22,
           float inX, float inY, float inZ)
        : r00(inR00), r01(inR01), r02(inR02),
          r10(inR10), r11(inR11), r12(inR12),
          r20(inR20), r21(inR21), r22(inR22),
          x(inX), y(inY), z(inZ) {}
};

struct Matrix3x3 final {
    float data[9];

    Vector3 MatrixToEulerAngles() const {
        Vector3 angles;

        angles.y = asin(clamp(data[6], -1.0f, 1.0f));

        if (abs(data[6]) < 0.9999f) {
            angles.x = atan2(-data[7], data[8]);
            angles.z = atan2(-data[3], data[0]);
        }
        else {
            angles.x = 0.0f;
            angles.z = atan2(data[1], data[4]);
        }

        angles.x = angles.x * (180.0f / M_PI);
        angles.y = angles.y * (180.0f / M_PI);
        angles.z = angles.z * (180.0f / M_PI);

        return angles;
    }
};

struct ViewMatrix_t
{
    ViewMatrix_t() = default;

    constexpr ViewMatrix_t(
        const float m00, const float m01, const float m02, const float m03,
        const float m10, const float m11, const float m12, const float m13,
        const float m20, const float m21, const float m22, const float m23,
        const float m30, const float m31, const float m32, const float m33)
    {
        arrData[0][0] = m00;
        arrData[0][1] = m01;
        arrData[0][2] = m02;
        arrData[0][3] = m03;
        arrData[1][0] = m10;
        arrData[1][1] = m11;
        arrData[1][2] = m12;
        arrData[1][3] = m13;
        arrData[2][0] = m20;
        arrData[2][1] = m21;
        arrData[2][2] = m22;
        arrData[2][3] = m23;
        arrData[3][0] = m30;
        arrData[3][1] = m31;
        arrData[3][2] = m32;
        arrData[3][3] = m33;
    }

    [[nodiscard]] float* operator[](const int nIndex)
    {
        return arrData[nIndex];
    }

    [[nodiscard]] const float* operator[](const int nIndex) const
    {
        return arrData[nIndex];
    }

    [[nodiscard]] const bool operator==(const ViewMatrix_t& viewOther) const
    {
        return (
            arrData[0][0] == viewOther.arrData[0][0] && arrData[0][1] == viewOther.arrData[0][1] && arrData[0][2] == viewOther.arrData[0][2] && arrData[0][3] == viewOther.arrData[0][3] &&
            arrData[1][0] == viewOther.arrData[1][0] && arrData[1][1] == viewOther.arrData[1][1] && arrData[1][2] == viewOther.arrData[1][2] && arrData[1][3] == viewOther.arrData[1][3] &&
            arrData[2][0] == viewOther.arrData[2][0] && arrData[2][1] == viewOther.arrData[2][1] && arrData[2][2] == viewOther.arrData[2][2] && arrData[2][3] == viewOther.arrData[2][3] &&
            arrData[3][0] == viewOther.arrData[3][0] && arrData[3][1] == viewOther.arrData[3][1] && arrData[3][2] == viewOther.arrData[3][2] && arrData[3][3] == viewOther.arrData[3][3]
            );
    }

    constexpr ViewMatrix_t& operator+=(const ViewMatrix_t& matAdd)
    {
        for (std::uint8_t c = 0U; c < 4U; c++)
        {
            for (std::uint8_t r = 0U; r < 4U; r++)
                arrData[c][r] += matAdd[c][r];
        }

        return *this;
    }

    constexpr ViewMatrix_t& operator-=(const ViewMatrix_t& matSubtract)
    {
        for (std::uint8_t c = 0U; c < 4U; c++)
        {
            for (std::uint8_t r = 0U; r < 4U; r++)
                arrData[c][r] -= matSubtract[c][r];
        }

        return *this;
    }

    constexpr void Identity()
    {
        for (std::uint8_t c = 0U; c < 4U; c++)
        {
            for (std::uint8_t r = 0U; r < 4U; r++)
                arrData[c][r] = (c == r) ? 1.0f : 0.0f;
        }
    }

    /// concatenate transformations of two matrices into one
    /// @returns: matrix with concatenated transformations
    [[nodiscard]] constexpr ViewMatrix_t ConcatTransforms(const ViewMatrix_t& matOther) const
    {
        return {
            arrData[0][0] * matOther.arrData[0][0] + arrData[0][1] * matOther.arrData[1][0] + arrData[0][2] * matOther.arrData[2][0] + arrData[0][3] * matOther.arrData[3][0],
            arrData[0][0] * matOther.arrData[0][1] + arrData[0][1] * matOther.arrData[1][1] + arrData[0][2] * matOther.arrData[2][1] + arrData[0][3] * matOther.arrData[3][1],
            arrData[0][0] * matOther.arrData[0][2] + arrData[0][1] * matOther.arrData[1][2] + arrData[0][2] * matOther.arrData[2][2] + arrData[0][3] * matOther.arrData[3][2],
            arrData[0][0] * matOther.arrData[0][3] + arrData[0][1] * matOther.arrData[1][3] + arrData[0][2] * matOther.arrData[2][3] + arrData[0][3] * matOther.arrData[3][3],

            arrData[1][0] * matOther.arrData[0][0] + arrData[1][1] * matOther.arrData[1][0] + arrData[1][2] * matOther.arrData[2][0] + arrData[1][3] * matOther.arrData[3][0],
            arrData[1][0] * matOther.arrData[0][1] + arrData[1][1] * matOther.arrData[1][1] + arrData[1][2] * matOther.arrData[2][1] + arrData[1][3] * matOther.arrData[3][1],
            arrData[1][0] * matOther.arrData[0][2] + arrData[1][1] * matOther.arrData[1][2] + arrData[1][2] * matOther.arrData[2][2] + arrData[1][3] * matOther.arrData[3][2],
            arrData[1][0] * matOther.arrData[0][3] + arrData[1][1] * matOther.arrData[1][3] + arrData[1][2] * matOther.arrData[2][3] + arrData[1][3] * matOther.arrData[3][3],

            arrData[2][0] * matOther.arrData[0][0] + arrData[2][1] * matOther.arrData[1][0] + arrData[2][2] * matOther.arrData[2][0] + arrData[2][3] * matOther.arrData[3][0],
            arrData[2][0] * matOther.arrData[0][1] + arrData[2][1] * matOther.arrData[1][1] + arrData[2][2] * matOther.arrData[2][1] + arrData[2][3] * matOther.arrData[3][1],
            arrData[2][0] * matOther.arrData[0][2] + arrData[2][1] * matOther.arrData[1][2] + arrData[2][2] * matOther.arrData[2][2] + arrData[2][3] * matOther.arrData[3][2],
            arrData[2][0] * matOther.arrData[0][3] + arrData[2][1] * matOther.arrData[1][3] + arrData[2][2] * matOther.arrData[2][3] + arrData[2][3] * matOther.arrData[3][3],

            arrData[3][0] * matOther.arrData[0][0] + arrData[3][1] * matOther.arrData[1][0] + arrData[3][2] * matOther.arrData[2][0] + arrData[3][3] * matOther.arrData[3][0],
            arrData[3][0] * matOther.arrData[0][1] + arrData[3][1] * matOther.arrData[1][1] + arrData[3][2] * matOther.arrData[2][1] + arrData[3][3] * matOther.arrData[3][1],
            arrData[3][0] * matOther.arrData[0][2] + arrData[3][1] * matOther.arrData[1][2] + arrData[3][2] * matOther.arrData[2][2] + arrData[3][3] * matOther.arrData[3][2],
            arrData[3][0] * matOther.arrData[0][3] + arrData[3][1] * matOther.arrData[1][3] + arrData[3][2] * matOther.arrData[2][3] + arrData[3][3] * matOther.arrData[3][3]
        };
    }

    [[nodiscard]] std::string ToString() const
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(4);
        for (std::uint8_t r = 0U; r < 4U; r++)
        {
            oss << "[ ";
            for (std::uint8_t c = 0U; c < 4U; c++)
            {
                oss << arrData[r][c];
                if (c < 3U)
                    oss << ", ";
            }
            oss << " ]\n";
        }
        return oss.str();
    }

    float arrData[4][4] = {};
};

struct UDim {
    float scale;
    int offset;

    UDim(float inScale = 0.0f, int inOffset = 0)
        : scale(inScale), offset(inOffset) {
    }
};

struct UDim2 {
    UDim x;
    UDim y;

    UDim2()
        : x(0.0f, 0), y(0.0f, 0) {
    }

    UDim2(float xScale, int xOffset, float yScale, int yOffset)
        : x(xScale, xOffset), y(yScale, yOffset) {
    }

    UDim2(const UDim& inX, const UDim& inY)
        : x(inX), y(inY) {
    }

    static UDim2 FromScale(float xScale, float yScale) {
        return UDim2(xScale, 0, yScale, 0);
    }

    static UDim2 FromOffset(int xOffset, int yOffset) {
        return UDim2(0.0f, xOffset, 0.0f, yOffset);
    }

    UDim2 Lerp(const UDim2& goal, float alpha) const {
        float newXScale = x.scale + (goal.x.scale - x.scale) * alpha;
        auto newXOffset = static_cast<int>(x.offset + (goal.x.offset - x.offset) * alpha);
        float newYScale = y.scale + (goal.y.scale - y.scale) * alpha;
        auto newYOffset = static_cast<int>(y.offset + (goal.y.offset - y.offset) * alpha);
        return UDim2(newXScale, newXOffset, newYScale, newYOffset);
    }

    UDim2 operator+(const UDim2& other) const {
        return UDim2(
            x.scale + other.x.scale,
            x.offset + other.x.offset,
            y.scale + other.y.scale,
            y.offset + other.y.offset
        );
    }

    UDim2 operator-(const UDim2& other) const {
        return UDim2(
            x.scale - other.x.scale,
            x.offset - other.x.offset,
            y.scale - other.y.scale,
            y.offset - other.y.offset
        );
    }

    bool operator==(const UDim2& other) const {
        return FloatEqual(x.scale, other.x.scale) &&
            x.offset == other.x.offset &&
            FloatEqual(y.scale, other.y.scale) &&
            y.offset == other.y.offset;
    }

    bool operator!=(const UDim2& other) const {
        return !(*this == other);
    }

    UDim Width() const { return x; }
    UDim Height() const { return y; }
};
