#pragma once
#include <cstdint>

namespace Procedural {

    /**
     * This uses the hill algorithm defined here:
     * http://www.stuffwithstuff.com/robot-frog/3d/hills/hill.html
     *
     * @param  x width (number of columns)
     * @param  y height (number of rows)
     * @return   Returns data[row][column] as a value between 0 255
     */
    uint8_t* generateHeightMap(uint32_t x, uint32_t y);

    /**
     * Creates alternating back/white grid.
     * If x and y are even, it is grid lines, and if odd a checkerboard pattern.
     *
     * @note This is mostly for debugging.
     *
     * @param  x width
     * @param  y height
     * @return   data[row][column][0=r, 1=g, 2=b, 3=a]
     */
    uint8_t* generateAlternatingGrid(uint32_t x, uint32_t y);

    /**
     * Represents a height map as grayscale color
     *
     * @note This is mostly for debugging.
     *
     * @param  x          width
     * @param  y          height
     * @param height_map A map of data[row][colum] height values
     * @return           data[row][column][0=r, 1=g, 2=b, 3=a]
     */
    uint8_t* heightMapToRGBA(uint32_t x, uint32_t y, const uint8_t* height_map);

    /**
     * @param x          width (number of columns)
     * @param y          height (number of rows)
     * @param height_map A map of data[row][colum] height values
     * @return           data[row][column][0=r, 1=g, 2=b, 3=a]
     */
    uint8_t* generateNormalMap(uint32_t x, uint32_t y, const uint8_t* height_map);
}
