#include "procedural.h"
#include <cstdio>
#include <glm/gtc/noise.hpp>
#include <algorithm>

uint8_t* Procedural::generateHeightMap(uint32_t w, uint32_t h) {
    if(w == 0 || h == 0) return nullptr;

    float min = 1e10;
    float max = -1e10;

    float fdata[w * h];
    for(uint32_t x = 0; x < h; ++x) {
        uint32_t offset = x * w;

        for(uint32_t y = 0; y < w; ++y) {
            float& i = fdata[offset + y];
            //i  = glm::simplex(glm::vec2(x, y)) / 2.0f;
            i = 0;
            i += glm::simplex(glm::vec2(x / 8.0f, y / 8.0f)) / 4.0f;
            i += glm::simplex(glm::vec2(x / 4.0f, y / 4.0f)) / 2.0f;
            i += glm::simplex(glm::vec2(x / 2.0f, y / 2.0f));
            i += glm::simplex(glm::vec3(x * 2.0f, y * 2.0f, x * y / 10.0f)) * 4.0f;

            min = std::min(min, i);
            max = std::max(max, i);
        }
    }

    float range = max - min;

    uint8_t* udata =  new uint8_t[w * h];
    for(uint32_t x = 0; x < h; ++x) {
        uint32_t offset = x * w;

        for(uint32_t y = 0; y < w; ++y) {
            uint8_t& i = udata[offset + y];
            float val = fdata[offset + y];
            val += std::abs(min);
            val /= range;

            i = (uint8_t)(255.0f * val);
        }
    }

    return udata;
}

uint8_t* Procedural::generateAlternatingGrid(uint32_t x, uint32_t y) {
    if(x == 0 || y == 0) return nullptr;
    uint32_t size = x * y;
    uint8_t* grid = new uint8_t[size * 4];
    for(uint32_t x = 0; x < size; ++x) {
        if(x % 2 == 0) {
            grid[x*4] = 0x00;
            grid[x*4 + 1] = 0x00;
            grid[x*4 +2] = 0x00;
        }
        else {
            grid[x*4] = 0xFF;
            grid[x*4 + 1] = 0xFF;
            grid[x*4 +2] = 0xFF;
        }
        grid[x*4 +3] = 0xFF;
    }
    return grid;
}

uint8_t* Procedural::heightMapToRGBA(uint32_t x, uint32_t y, const uint8_t* heights) {
    if(heights == nullptr || x == 0 || y == 0) return nullptr;

    uint32_t size = x * y;
    uint8_t* height_map = new uint8_t[size * 4];

    for(uint32_t x = 0; x < size; ++x) {
        const uint8_t& height = heights[x];
        height_map[x*4 + 0] = height;
        height_map[x*4 + 1] = height;
        height_map[x*4 + 2] = height;
        height_map[x*4 + 3] = 0xFF;
    }

    return height_map;
}

uint8_t* Procedural::generateNormalMap(uint32_t w, uint32_t h, const uint8_t* height_map) {
    if(height_map == nullptr || w == 0 || h == 0) return nullptr;

    uint32_t size = w * h;
    uint8_t* normal_map = new uint8_t[size * 4];

    // See response from Adrian McCarthy to:
    // http://stackoverflow.com/questions/5281261/generating-a-normal-map-from-a-height-map
    for(uint32_t x = 0; x < h; ++x) {
        for(uint32_t y = 0; y < w; ++y) {
            glm::vec3 sum(0.0f, 0.0f, 0.0f); //The sum of all the triangle normals

            // Wrap end values xp1 = x plus 1, xm1 = x minus 1
            // This makes the seams much harder to spot
            const uint32_t xp1 = (x + 1) % h;
            const uint32_t yp1 = (y + 1) % w;
            const uint32_t xm1 = ( (x != 0) ? (x - 1) : (h - 1) );
            const uint32_t ym1 = ( (y != 0) ? (y - 1) : (w - 1) );

            const auto getHeight = [&](uint32_t a, uint32_t b)->float {
                return ((float)height_map[a * w + b]) / 255.0f;
            };

            const glm::vec3 center( 0.0f,  0.0f, getHeight(x, y));
            // go counter-clockwise around square starting and right apothem
            const glm::vec3 points[8] = {
                { 1.0f,  0.0f, getHeight(x,   yp1)},
                { 1.0f,  1.0f, getHeight(xm1, yp1)},
                { 0.0f,  1.0f, getHeight(xm1, y  )},
                {-1.0f,  1.0f, getHeight(xm1, ym1)},
                {-1.0f,  0.0f, getHeight(x,   ym1)},
                {-1.0f, -1.0f, getHeight(xp1, ym1)},
                { 0.0f, -1.0f, getHeight(xp1, y  )},
                { 1.0f, -1.0f, getHeight(xp1, yp1)}
            };

            // When p1, p2, p3 are in counter-clockwise order (p1 is 0, 0, h_cc)
            const auto getNorm = [&center](const glm::vec3& p2, const glm::vec3& p3)->glm::vec3 {
                return glm::cross(p2 - center, p3 - center);
            };

            // Now go around the square as if it were a circle
            for(uint8_t x = 0; x < 8; ++x)
                sum += getNorm(points[x], points[(x + 1) % 8]);

            sum = glm::normalize(sum);

            normal_map[x * w * 4 + y * 4 + 0] = ((sum.x + 1.0f) / 2.0f) * 255.0f;
            normal_map[x * w * 4 + y * 4 + 1] = ((sum.y + 1.0f) / 2.0f) * 255.0f;
            normal_map[x * w * 4 + y * 4 + 2] = ((sum.z + 1.0f) / 2.0f) * 255.0f;
            normal_map[x * w * 4 + y * 4 + 3] = 0xff;
        }
    }

    return normal_map;
}
