#ifndef _MATH_H_
#define _MATH_H_


#include <algorithm>

#define _XM_AVX2_INTRINSICS_
#include <../JBFramework/DirectXMath/Inc/DirectXMath.h>


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


namespace JBF{
    namespace Common{
        typedef float Scalar;
        
        typedef DirectX::XMFLOAT2 Vector2;
        typedef DirectX::XMFLOAT3 Vector3;
        typedef DirectX::XMFLOAT4A Vector4;
        typedef DirectX::XMFLOAT3X3 Vector33;
        typedef DirectX::XMFLOAT3X4A Vector34;
        typedef DirectX::XMFLOAT4X4A Vector44;

        typedef DirectX::XMVECTOR RegVector;
        typedef DirectX::XMMATRIX RegVector44;


        inline RegVector Load(const Scalar& V)noexcept{ return DirectX::XMVectorReplicate(V); }
        inline RegVector Load(const Vector2& V)noexcept{ return DirectX::XMLoadFloat2(&V); }
        inline RegVector Load(const Vector3& V)noexcept{ return DirectX::XMLoadFloat3(&V); }
        inline RegVector Load(const Vector4& V)noexcept{ return DirectX::XMLoadFloat4A(&V); }
        inline RegVector44 Load(const Vector33& V)noexcept{ return DirectX::XMLoadFloat3x3(&V); }
        inline RegVector44 Load(const Vector34& V)noexcept{
            RegVector44 Result;
            Result.r[0] = DirectX::XMLoadFloat4A(reinterpret_cast<const Vector4*>(&V.m[0]));
            Result.r[1] = DirectX::XMLoadFloat4A(reinterpret_cast<const Vector4*>(&V.m[1]));
            Result.r[2] = DirectX::XMLoadFloat4A(reinterpret_cast<const Vector4*>(&V.m[2]));
            Result.r[3] = DirectX::g_XMIdentityR3;
            return std::move(Result);
        }
        inline RegVector44 Load(const Vector44& V)noexcept{ return DirectX::XMLoadFloat4x4A(&V); }

        inline Scalar StoreX(const RegVector& V) noexcept{ return DirectX::XMVectorGetX(V); }
        inline Scalar StoreY(const RegVector& V) noexcept{ return DirectX::XMVectorGetY(V); }
        inline Scalar StoreZ(const RegVector& V) noexcept{ return DirectX::XMVectorGetZ(V); }
        inline Scalar StoreW(const RegVector& V) noexcept{ return DirectX::XMVectorGetW(V); }
        inline Vector2 Store2(const RegVector& V)noexcept{
            Vector2 Result;
            DirectX::XMStoreFloat2(&Result, V);
            return std::move(Result);
        }
        inline Vector3 Store3(const RegVector& V)noexcept{
            Vector3 Result;
            DirectX::XMStoreFloat3(&Result, V);
            return std::move(Result);
        }
        inline Vector4 Store4(const RegVector& V)noexcept{
            Vector4 Result;
            DirectX::XMStoreFloat4A(&Result, V);
            return std::move(Result);
        }
        inline Vector33 Store33(const RegVector44& V)noexcept{
            Vector33 Result;
            DirectX::XMStoreFloat3x3(&Result, V);
            return std::move(Result);
        }
        inline Vector34 Store34(const RegVector44& V)noexcept{
            Vector34 Result;
            DirectX::XMStoreFloat4A(reinterpret_cast<Vector4*>(&Result.m[0]), V.r[0]);
            DirectX::XMStoreFloat4A(reinterpret_cast<Vector4*>(&Result.m[1]), V.r[1]);
            DirectX::XMStoreFloat4A(reinterpret_cast<Vector4*>(&Result.m[2]), V.r[2]);
            return std::move(Result);
        }
        inline Vector44 Store44(const RegVector44& V)noexcept{
            Vector44 Result;
            DirectX::XMStoreFloat4x4A(&Result, V);
            return std::move(Result);
        }
    };
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#endif

