#pragma once
#include <cstdint>

// bitmask enum class for different collision layers, bitmask allows for multiple layer comparison
enum class CollisionLayer : std::uint32_t {
    kNone = 0,
    kPlayer = 1 << 0,
    kEnemy = 1 << 1,
    kWorld = 1 << 2,
    kTrigger = 1 << 3,
    kProjectile = 1 << 4,
    kAll = 0xFFFFFFFF
};

// Bitwise helpers
inline CollisionLayer operator|(CollisionLayer a, CollisionLayer b) {
    return static_cast<CollisionLayer>(
        static_cast<std::uint32_t>(a) |
        static_cast<std::uint32_t>(b)
        );
}

inline CollisionLayer operator&(CollisionLayer a, CollisionLayer b) {
    return static_cast<CollisionLayer>(
        static_cast<std::uint32_t>(a) &
        static_cast<std::uint32_t>(b)
        );
}