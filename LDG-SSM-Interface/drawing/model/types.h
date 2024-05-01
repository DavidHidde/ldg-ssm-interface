#ifndef TYPES_H
#define TYPES_H

/**
 * @brief The DrawType enum Types of drawing that can be done
 */
enum DrawType
{
    IMAGE,
    VOLUME
};

/**
 * @brief The DrawType enum Types of drawing that can be done
 */
enum VolumeRenderingType
{
    ISOSURFACE,
    MAX,
    AVERAGE,
    ACCUMULATE
};

#endif // TYPES_H
