#include <cs50.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

const int BLOCK = 512;
bool new_jpeg(uint8_t buffer[BLOCK]);
const int BITS = 9;

int main(int argc, char *argv[])
{
    // check for exactly one command line argument
    if (argc != 2)
    {
        printf("usage: ./recover FILE\n");
        return 1;
    }

    // open memory card
    FILE *card = fopen(argv[1], "r");
    if (card == NULL)
    {
        printf("cannot open %s\n", argv[1]);
        return 2;
    }

    // create buffer file to read into
    uint8_t buffer[BLOCK];
    int k = 0;
    char name[BITS];

    // while there is still data to read from *card
    while (fread(buffer, 1, BLOCK, card) == BLOCK)
    {
        // check for JPEG signature at the beginning of each block (first 4 bytes)
        if (new_jpeg(buffer))
        {   printf("found a new file, %03i.\n", k);

            // create a new .jpeg file, (numbered starting at 000.jpeg), check if null
            sprintf(name, "%03i.jpeg", k);
            FILE *img = fopen(name, "w");
            if (img == NULL)
            {
                printf("cannot open %s.\n", name);
                return 3;
            }

            //copy buffer into image
            fwrite(buffer, sizeof(uint8_t), BLOCK, img);

            //check for next jpeg signature
            while (fread(buffer, 1, BLOCK, card) == BLOCK && !new_jpeg(buffer))
            {
                fwrite(buffer, sizeof(uint8_t), BLOCK, img);
            }
            fclose(img);
            k++;
        }
    }

    fclose(card);
    return 0;
}

bool new_jpeg(uint8_t buffer[BLOCK])
{
    if (buffer[0] == 0xff && buffer[1] == 0xd8 && buffer[2] == 0xff && (buffer[3] & 0xf0) == 0xe0)
    {
        return true;
    }
    return false;
}
