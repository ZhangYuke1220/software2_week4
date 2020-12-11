#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char**argv)
{
    if (argc != 2){
        fprintf(stderr,"usage: %s string\n", argv[1]);
        return EXIT_FAILURE;
    }

    unsigned short bufsize = sizeof(char) * strlen(argv[1]);
    float buf[bufsize];

    FILE *fp;
    if ( (fp = fopen("calc_magicnumber.txt","wb")) == NULL ){
        fprintf(stderr,"calc_magicnumber.txt: cannot open file.\n");
        return EXIT_FAILURE;  
    }

    fwrite(argv[1], sizeof(char), strlen(argv[1]), fp);
    fclose(fp);
    fp = NULL;

    if ( (fp = fopen("calc_magicnumber.txt","rb")) == NULL ){
        fprintf(stderr,"calc_magicnumber.txt: cannot open file.\n");
        return EXIT_FAILURE;  
    }

    for (int i=0; fread((char *)&buf[i], sizeof(char), 4, fp); ++i)
        printf("%.150f\n", buf[i]);

    fclose(fp);

    printf("%s\n", (char *)buf);
    return EXIT_SUCCESS;
}