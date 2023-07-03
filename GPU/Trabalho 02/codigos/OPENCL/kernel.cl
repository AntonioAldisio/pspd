__kernel void UmaVida(__global int* tabulIn, __global int* tabulOut, const int tam) {
    size_t i = get_global_id(0) + 1;
    size_t j = get_global_id(1) + 1;
    int vizviv;

    if (i <= tam && j <= tam) {
        vizviv = tabulIn[ind2d(i-1,j-1)] + tabulIn[ind2d(i-1,j)] +
                 tabulIn[ind2d(i-1,j+1)] + tabulIn[ind2d(i,j-1)] +
                 tabulIn[ind2d(i,j+1)] + tabulIn[ind2d(i+1,j-1)] +
                 tabulIn[ind2d(i+1,j)] + tabulIn[ind2d(i+1,j+1)];

        if (tabulIn[ind2d(i,j)] && vizviv < 2)
            tabulOut[ind2d(i,j)] = 0;
        else if (tabulIn[ind2d(i,j)] && vizviv > 3)
            tabulOut[ind2d(i,j)] = 0;
        else if (!tabulIn[ind2d(i,j)] && vizviv == 3)
            tabulOut[ind2d(i,j)] = 1;
        else
            tabulOut[ind2d(i,j)] = tabulIn[ind2d(i,j)];
    }
}
