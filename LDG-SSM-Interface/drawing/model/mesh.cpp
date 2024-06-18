#include "mesh.h"

/**
 * @brief createPlane Create a square 2D plane mesh.
 * @param origin
 * @param side_len
 * @return
 */
Mesh createPlane(QVector3D origin, float side_len)
{
    return {
        {
            origin,                                         // Top left - 0
            origin + QVector3D{ side_len, 0., 0. },         // Top right - 1
            origin + QVector3D{ 0., side_len, 0. },         // Bot left - 2
            origin + QVector3D{ side_len, side_len, 0. }    // Bot right - 3
        },
        {
            // Top left - Bot left - Bot right
            0, 2, 3,
            // Top left - Bot right - Top right
            0, 3, 1
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
