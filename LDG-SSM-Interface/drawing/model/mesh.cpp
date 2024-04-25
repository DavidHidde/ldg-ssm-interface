#include "mesh.h"

/**
 * @brief project Project from screen space to world space
 * @param source
 * @param projection
 * @return
 */
QVector3D project(QVector3D source, QVector3D projection)
{
    return source * projection - QVector3D{ 1., 1., 0.};
}

/**
 * @brief createPlane Create a square 2D plane mesh.
 * @param origin
 * @param side_len
 * @param projection
 * @return
 */
Mesh createPlane(QVector3D origin, float side_len, QVector3D projection, unsigned int vertex_offset)
{
    return {
        {
            project(origin, projection),                                        // Top left - 0
            project(origin + QVector3D{ side_len, 0., 0. }, projection),        // Top right - 1
            project(origin + QVector3D{ 0., side_len, 0. }, projection),        // Bot left - 2
            project(origin + QVector3D{ side_len, side_len, 0. }, projection)   // Bot right - 3
        },
        {
            // Top left - Bot left - Bot right
            vertex_offset,
            vertex_offset + 2,
            vertex_offset + 3,
            // Top left - Bot right - Top right
            vertex_offset,
            vertex_offset + 3,
            vertex_offset + 1
        }
    };
}

/**
 * @brief createCube Create a square cube mesh.
 * @param origin
 * @param side_len
 * @param projection
 * @return
 */
Mesh createCube(QVector3D origin, float side_len, float depth_factor = -1)
{
    return {
        {
            origin,                                                             // Front top left - 0
            origin + QVector3D{ side_len, 0., 0. },                             // Front top right - 1
            origin + QVector3D{ 0., side_len, 0. },                             // Front bot left - 2
            origin + QVector3D{ side_len, side_len, 0. },                       // Front bot right - 3
            origin + QVector3D{ 0., 0., side_len },                             // Back top left - 4
            origin + QVector3D{ side_len, 0., depth_factor * side_len },        // Back top right - 5
            origin + QVector3D{ 0., side_len, depth_factor * side_len },        // Back bot left - 6
            origin + QVector3D{ side_len, side_len, depth_factor * side_len }   // Back bot right - 7
        },
        {
            // Front
            0, 2, 3,
            0, 3, 1,
            // Back
            4, 7, 6,
            4, 5, 7,
            // Left
            4, 6, 2,
            4, 2, 0,
            // Right
            1, 3, 7,
            4, 7, 5
        }
    };
}
