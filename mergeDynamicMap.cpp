#include <stdio.h>
#include<string.h>

int main(){
    FILE * fp;
    int tmp_d, tmp_ang,polar_dis[360];
    char fname[255], tmp[2];
    tmp[1] = '\0';
    for (int i = 0; i < 10; i++){
        memset(polar_dis, 0, sizeof polar_dis);
        for (int j = 0; j < 2; j++){
            strcpy(fname, "dataDynamic/");
            if (i != 9)
            {
                tmp[0] = (char)(i + 1 + 0x30);
                strcat(fname, tmp);
            }
            else
                strcat(fname, "10");
            strcat(fname, "_");
            tmp[0] = (char)(j + 1 + 0x30);
            strcat(fname, tmp);
            strcat(fname,".txt");
            fp = fopen(fname, "r");
            while (!feof(fp)){
                fscanf(fp,"%d",&tmp_d);
                fscanf(fp,"%d",&tmp_ang);
                if (tmp_ang >= 90 && tmp_ang < 270){
                    if (j == 0)
                        polar_dis[tmp_ang + 90] = tmp_d;
                    else
                        polar_dis[tmp_ang - 90] = tmp_d;
                    // printf("%d %d\n", tmp_d, tmp_ang);
                }
            }
            fclose(fp);
        }
        // for (int j = 0; j < 360; j++) {
        //     printf("%d %d\n", polar_dis[j], j);
        // }
        // scanf("%d", tmp_d);
        strcpy(fname, "dataDynamic/");
        tmp[0] = (char)(i + 1 + 0x30);
        if (i != 9)
            strcat(fname, tmp);
        else
            strcat(fname, "10");
        strcat(fname,".txt");
        fp = fopen(fname, "w");
        for (int j = 0; j < 360; j++){
            fprintf(fp, "%d %d\n\n", polar_dis[j], j);
        }
        fclose(fp);
    }
    // scanf("%d", &tmp_d);
    return 0;
}