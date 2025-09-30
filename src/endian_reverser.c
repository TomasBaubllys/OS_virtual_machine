#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *in = fopen("__IMAGINE_DRAGONS.bin", "rb");
    if (!in) {
        perror("Input file");
        return 1;
    }

    FILE *out = fopen("output_reversed.bin", "wb");
    if (!out) {
        perror("Output file");
        fclose(in);
        return 1;
    }

    unsigned char buf[4];
    size_t n;

    while ((n = fread(buf, 1, 4, in)) > 0) {
        // Reverse the bytes we actually read
        for (size_t i = 0; i < n / 2; i++) {
            unsigned char tmp = buf[i];
            buf[i] = buf[n - 1 - i];
            buf[n - 1 - i] = tmp;
        }
        fwrite(buf, 1, n, out);
    }

    fclose(in);
    fclose(out);
    return 0;
}
