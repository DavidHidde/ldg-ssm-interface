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

Mesh createPlane(QVector3D origin, float side_len, QVector3D projection, unsigned int vertex_offset);

Mesh createCube(QVector3D origin, float side_len, QVector3D projection, unsigned int vertex_offset);

#endif // MESH_H