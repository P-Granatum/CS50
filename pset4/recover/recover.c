//recovers jpegs from formatted memory card
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./recover image\n");
        return 1;
    }

    // open input file
    FILE *file = fopen(argv[1], "r");
    if (file == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", argv[1]);
        return 2;
    }

    // search for first JPEG (check first 4 bites of every block)
    uint8_t buffer[512];
    bool found = false;
    do
    {
        fread(buffer, 1, 512, file);

        if (buffer[0] == 0xff &&
            buffer[1] == 0xd8 &&
            buffer[2] == 0xff &&
            (buffer[3] & 0xf0) == 0xe0)
        {
            found = true;
        }
    }
    while (found != true);

    //file counter
    int i = 0;
    //bytes written
    int a = 512;
    bool end;
    do
    {
        //open new jpeg
        char name[8];
        sprintf(name, "%03i.jpg", (char)i);
        FILE *img = fopen(name, "w");

        //write first block of file
        fwrite(buffer, 1, 512, img);

        end = false;
        //keep on writing till another jpeg is found
        do
        {
            //save portion of file in buffer
            a = fread(buffer, 1, 512, file);
            //check if this is the start of a new file
            if (buffer[0] == 0xff &&
                buffer[1] == 0xd8 &&
                buffer[2] == 0xff &&
                (buffer[3] & 0xf0) == 0xe0)
            {
                end = true;
                break;
            }
            //check if memory card finished
            if (a != 512)
            {
                break;
            }
            //write to image
            else
            {
                fwrite(buffer, 1, 512, img);
            }
        }
        while (end == false);
        //close jpeg
        fclose(img);
        i++;
    }
    while (a == 512);

    fclose(file);

    //show result
    //printf("%i images were found!\n", i);
    // Success
    return 0;
}
