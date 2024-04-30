
/**
 * Accumulation composition
 *
 * @param sample: current sample value.
 * @param samplingRatio: the ratio between current sampling rate and the original. (ray step)
 * @param composedColor: blended color (both input and output)
 */
void accumulation(float value, float sampleRatio, inout vec4 composed_color)
{
    vec4 color = transferFunction(value);
    color.a = opacityCorrection(color.a, sampleRatio);

    // Old accumulation method
    composed_color += (1.0 - composed_color.a) * color * color.a;
    composed_color.a += (1.0 - composed_color.a) * color.a;
}
