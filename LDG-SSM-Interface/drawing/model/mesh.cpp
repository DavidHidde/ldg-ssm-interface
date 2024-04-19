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
Mesh createCube(QVector3D origin, float side_len, QVector3D projection, unsigned int vertex_offset)
{
    return {
        {
            project(origin, projection),                                                // Front top left - 0
            project(origin + QVector3D{ side_len, 0., 0. }, projection),                // Front top right - 1
            project(origin + QVector3D{ 0., side_len, 0. }, projection),                // Front bot left - 2
            project(origin + QVector3D{ side_len, side_len, 0. }, projection),          // Front bot right - 3
            project(origin + QVector3D{ 0., 0., side_len }, projection),                // Back top left - 4
            project(origin + QVector3D{ side_len, 0., -side_len }, projection),         // Back top right - 5
            project(origin + QVector3D{ 0., side_len, -side_len }, projection),         // Back bot left - 6
            project(origin + QVector3D{ side_len, side_len, -side_len }, projection)    // Back bot right - 7
        },
        {
            // Front
            vertex_offset, vertex_offset + 2, vertex_offset + 3,
            vertex_offset, vertex_offset + 3, vertex_offset + 1,
            // Back
            vertex_offset + 4, vertex_offset + 7, vertex_offset + 6,
            vertex_offset + 4, vertex_offset + 5, vertex_offset + 7,
            // Left
            vertex_offset + 4, vertex_offset + 6, vertex_offset + 2,
            vertex_offset + 4, vertex_offset + 2, vertex_offset + 0,
            // Right
            vertex_offset + 1, vertex_offset + 3, vertex_offset + 7,
            vertex_offset + 4, vertex_offset + 7, vertex_offset + 5
        }
    };
}
