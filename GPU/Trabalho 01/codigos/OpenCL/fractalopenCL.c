/*
    PSPD 2023 - 1
    Alunos: Antonio Aldisio        202028211
            Fernando Miranda Calil 190106565
            Lorrany Oliveira Souza 180113992

    Como compilar:
    $ gcc fractalopenCL.c -o fractalopencl -lm -lOpenCL -DCL_TARGET_OPENCL_VERSION=300

    Como rodar:
    $ ./fractalopencl 1000
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <CL/cl.h>
#include <time.h>

#define OUTFILE "out_julia_normal_openCL.bmp"
#define MAX_SOURCE_SIZE (0x100000)

int compute_julia_pixel(int x, int y, int largura, int altura, float tint_bias, unsigned char *rgb) {
  // Check coordinates
  if ((x < 0) || (x >= largura) || (y < 0) || (y >= altura)) {
    fprintf(stderr,"Invalid (%d,%d) pixel coordinates in a %d x %d image\n", x, y, largura, altura);
    return -1;
  }
  // "Zoom in" to a pleasing view of the Julia set
  float X_MIN = -1.6, X_MAX = 1.6, Y_MIN = -0.9, Y_MAX = +0.9;
  float float_y = (Y_MAX - Y_MIN) * (float)y / altura + Y_MIN ;
  float float_x = (X_MAX - X_MIN) * (float)x / largura  + X_MIN ;
  // Point that defines the Julia set
  float julia_real = -.79;
  float julia_img = .15;
  // Maximum number of iteration
  int max_iter = 300;
  // Compute the complex series convergence
  float real=float_y, img=float_x;
  int num_iter = max_iter;
  while (( img * img + real * real < 2 * 2 ) && ( num_iter > 0 )) {
    float xtemp = img * img - real * real + julia_real;
    real = 2 * img * real + julia_img;
    img = xtemp;
    num_iter--;
  }

  // Paint pixel based on how many iterations were used, using some funky colors
  float color_bias = (float) num_iter / max_iter;
  rgb[0] = (num_iter == 0 ? 200 : - 500.0 * pow(tint_bias, 1.2) *  pow(color_bias, 1.6));
  rgb[1] = (num_iter == 0 ? 100 : -255.0 *  pow(color_bias, 0.3));
  rgb[2] = (num_iter == 0 ? 100 : 255 - 255.0 * pow(tint_bias, 1.2) * pow(color_bias, 3.0));


  return 0;
} /*fim compute julia pixel */

int write_bmp_header(FILE *f, int largura, int altura) {

  unsigned int row_size_in_bytes = largura * 3 +
      ((largura * 3) % 4 == 0 ? 0 : (4 - (largura * 3) % 4));

  // Define all fields in the bmp header
  char id[2] = "BM";
  unsigned int filesize = 54 + (int)(row_size_in_bytes * altura * sizeof(char));
  short reserved[2] = {0,0};
  unsigned int offset = 54;

  unsigned int size = 40;
  unsigned short planes = 1;
  unsigned short bits = 24;
  unsigned int compression = 0;
  unsigned int image_size = largura * altura * 3 * sizeof(char);
  int x_res = 0;
  int y_res = 0;
  unsigned int ncolors = 0;
  unsigned int importantcolors = 0;

  // Write the bytes to the file, keeping track of the
  // number of written "objects"
  size_t ret = 0;
  ret += fwrite(id, sizeof(char), 2, f);
  ret += fwrite(&filesize, sizeof(int), 1, f);
  ret += fwrite(reserved, sizeof(short), 2, f);
  ret += fwrite(&offset, sizeof(int), 1, f);
  ret += fwrite(&size, sizeof(int), 1, f);
  ret += fwrite(&largura, sizeof(int), 1, f);
  ret += fwrite(&altura, sizeof(int), 1, f);
  ret += fwrite(&planes, sizeof(short), 1, f);
  ret += fwrite(&bits, sizeof(short), 1, f);
  ret += fwrite(&compression, sizeof(int), 1, f);
  ret += fwrite(&image_size, sizeof(int), 1, f);
  ret += fwrite(&x_res, sizeof(int), 1, f);
  ret += fwrite(&y_res, sizeof(int), 1, f);
ret += fwrite(&ncolors, sizeof(int), 1, f);
ret += fwrite(&importantcolors, sizeof(int), 1, f);

// Success means that we wrote 17 "objects" successfully
return (ret != 17);
} /* fim write bmp-header */

int main(int argc, char *argv[]) {
    int n;
    int area=0, largura = 0, altura = 0, local_i= 0;
    FILE *output_file;
    unsigned char *pixel_array, *rgb;
    cl_device_id device_id = NULL;
    cl_context context = NULL;
    cl_command_queue command_queue = NULL;
    cl_mem input_buffer = NULL, output_buffer = NULL;
    cl_program program = NULL;
    cl_kernel kernel = NULL;
    cl_int ret;

    if ((argc <= 1) | (atoi(argv[1]) < 1)) {
        fprintf(stderr, "Entre 'N' como um inteiro positivo! \n");
        return -1;
    }
    n = atoi(argv[1]);
    altura = n; largura = 2 * n; area = altura * largura * 3;
    //Allocate mem for the pixels array
    pixel_array = calloc(area, sizeof(unsigned char));
    rgb = calloc(3, sizeof(unsigned char));
    printf("Computando linhas de pixel %d até %d, para uma área total de %d\n", 0, n - 1, area);

    clock_t start_time, end_time;
    double total_time;

    // Get platform and device information
    cl_platform_id platform_id = NULL;
    cl_uint num_platforms, num_devices;
    ret = clGetPlatformIDs(1, &platform_id, &num_platforms);
    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &num_devices);

    // Create OpenCL context
    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);

    // Create command queue
    command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

    // Load kernel source code
    FILE *kernel_file;
    char *kernel_source;
    size_t kernel_size;

    kernel_file = fopen("fractal.cl", "r");
    if (kernel_file == NULL) {
        fprintf(stderr, "Failed to open kernel file\n");
        return -1;
    }
    kernel_source = (char*)malloc(MAX_SOURCE_SIZE);
    kernel_size = fread(kernel_source, 1, MAX_SOURCE_SIZE, kernel_file);
    fclose(kernel_file);

    // Create program object from kernel source
    program = clCreateProgramWithSource(context, 1, (const char**)&kernel_source, (const size_t*)&kernel_size, &ret);

    // Build the program executable
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

    // Create a kernel object
    kernel = clCreateKernel(program, "compute_julia_pixel", &ret);

    // Create input and output buffers on device
    input_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY, altura * largura * sizeof(int), NULL, &ret);
    output_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, altura * largura * sizeof(char), NULL, &ret);

    // Copy input data to input buffer
    ret = clEnqueueWriteBuffer(command_queue, input_buffer, CL_TRUE, 0, altura * largura * sizeof(int), pixel_array, 0, NULL, NULL);

    // Set kernel arguments
    ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&input_buffer);
    ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void*)&output_buffer);
    ret = clSetKernelArg(kernel, 2, sizeof(int), (void*)&largura);
    ret = clSetKernelArg(kernel, 3, sizeof(int), (void*)&altura);

    // Execute the kernel
    size_t global_item_size[2] = { 4, 1 }; // 4 threads
    size_t local_item_size[2] = { 1, 1 };
    
    start_time = clock();

    ret = clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, global_item_size, local_item_size, 0, NULL, NULL);

    // Wait for kernel to complete
    clFinish(command_queue);

    end_time = clock();
    total_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;

    // Read output data from output buffer
    ret = clEnqueueReadBuffer(command_queue, output_buffer, CL_TRUE, 0, altura * largura * sizeof(char), pixel_array, 0, NULL, NULL);

    // Release OpenCL objects
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(input_buffer);
    ret = clReleaseMemObject(output_buffer);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);

    // Write bitmap header
    output_file = fopen(OUTFILE, "wb");
    if (output_file == NULL) {
        fprintf(stderr, "Unable to open %s for writing!\n", OUTFILE);
        return -1;
    }
    write_bmp_header(output_file, largura, altura);

    // Compute pixel values and write to file
    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {
            compute_julia_pixel(j, i, largura, altura, 0.5, rgb);
            fwrite(rgb, sizeof(unsigned char), 3, output_file);
        }
    }

    // Clean up
    fclose(output_file);
    free(pixel_array);
    free(rgb);

    printf("Tempo de execução: %f segundos\n", total_time);

    return 0;
}