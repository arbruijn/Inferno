#pragma once

#include <DirectXTK12/SimpleMath.h>
#include <fmt/core.h>

template<>
struct fmt::formatter<DirectX::SimpleMath::Vector3> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(DirectX::SimpleMath::Vector3 const& v, FormatContext& ctx) {
        return fmt::format_to(ctx.out(), "{0},{1},{2}", v.x, v.y, v.z);
    }
};

template<>
struct fmt::formatter<DirectX::SimpleMath::Vector4> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(DirectX::SimpleMath::Vector4 const& v, FormatContext& ctx) {
        return fmt::format_to(ctx.out(), "{0},{1},{2},{3}", v.x, v.y, v.z, v.w);
    }
};

template<>
struct fmt::formatter<DirectX::SimpleMath::Matrix> {
    template<typename ParseContext>
    constexpr auto parse(ParseContext& ctx) {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(DirectX::SimpleMath::Matrix const& m, FormatContext& ctx) {
        return fmt::format_to(ctx.out(), "{0} {1} {2} {3}",
        	DirectX::SimpleMath::Vector4(m.m[0]), DirectX::SimpleMath::Vector4(m.m[1]),
        	DirectX::SimpleMath::Vector4(m.m[2]), DirectX::SimpleMath::Vector4(m.m[3]));
    }
};
