#ifndef MESH_H
#define MESH_H

#include <QList>
#include <QVector3D>

/**
 * @brief The Mesh struct A simple mesh struct for holding vertices and their assigned indices.
 */
struct Mesh
{
    QList<QVector3D> vertices;
    QList<unsigned int> indices;
};

Mesh createPlane(QVector3D origin, float side_len);

Mesh createCube(QVector3D origin, float side_len, float depth_factor);

#endif // MESH_H
