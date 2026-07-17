#ifndef REALLIB_VEC_HPP
#define REALLIB_VEC_HPP

namespace real {

struct Vec3Int {
public:
    int x;
    int y;
    int z;
};

struct Vec2Int {
public:
    union {
        struct {
            int x;
            int y;
        };

        struct {
            int w;
            int h;
        };

        struct {
            int u;
            int v;
        };
    };
};

}

#endif
