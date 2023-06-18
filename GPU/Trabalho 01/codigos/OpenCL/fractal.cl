__kernel void compute_julia_pixel(__global int* input_pixels, __global uchar* output_pixels, const int largura, const int altura) {
    int x = get_global_id(0);
    int y = get_global_id(1);

    // Check coordinates
    if ((x < 0) || (x >= largura) || (y < 0) || (y >= altura)) {
        return;
    }

    // Compute the index of the pixel in the input array
    int index = y * largura + x;

    // Extract the RGB values from the input pixel
    int r = (input_pixels[index] & 0xFF0000) >> 16;
    int g = (input_pixels[index] & 0x00FF00) >> 8;
    int b = (input_pixels[index] & 0x0000FF);

    // Compute the new RGB values for the output pixel using the compute_julia_pixel function
    float tint_bias = 0.5;
    unsigned char rgb[3];
    int result = compute_julia_pixel(x, y, largura, altura, tint_bias, rgb);

    // Write the new RGB values to the output pixel
    output_pixels[index * 3] = rgb[0];
    output_pixels[index * 3 + 1] = rgb[1];
    output_pixels[index * 3 + 2] = rgb[2];
}
