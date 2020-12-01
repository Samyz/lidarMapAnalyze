#include<stdio.h>
#include<conio.h>
#include<string.h>
#include<cstdlib>

#define DeB 1
#define OBJ 50
#define THRES_DIS 100
#define THRES_ANG 5
int main()
{
	int i,j,objs,tmp_d,tmp_ang;

	int polar_dis[360];	
	struct	st_object{
		//int	data[50];
		int	ang_sta;
		int	ang_end;
		int	dis_sum;
		int count;
	};
	struct	st_object objects[OBJ];

	char	check = 0;
	FILE	*fp;

	memset(polar_dis, 0, sizeof polar_dis);
	fp = fopen("./dataraw/19.txt","r");
	if (fp == NULL)
	{
		printf("Cannot open file\n");
		getch();
		return 0;
	}
	else
	{
		if(DeB)
			printf("Can open file\n");
		for(i=0; i<360 ; i++)
		{
			fscanf(fp,"%d",&tmp_d);
			fscanf(fp,"%d",&tmp_ang);
			polar_dis[tmp_ang] = tmp_d;
			if(DeB)
				printf("read data : %d %d\n",tmp_ang,tmp_d);
		}
		if(DeB)
			for(i=0; i<360 ; i++)
				printf("%d %d\n",i,polar_dis[i]);
		if(DeB)
			if (!fclose(fp))
				printf("\nClose file\n");
		//getch();
	}
	
	memset(objects, 0, sizeof objects);
	objs = 0;
	//printf ("%d %d %d %d\n",objects[objs].count,objects[objs].ang_sta,objects[objs].ang_end,objects[objs].dis_sum);
	//objects[objs].dis_sum++;
	//printf ("%d %d %d %d\n",objects[objs].count,objects[objs].ang_sta,objects[objs].ang_end,objects[objs].dis_sum);
	for (i=0; i<360; i++)
	{
		if(polar_dis[i] != 0)
		{
			if (objects[objs].count == 0)
			{
				objects[objs].ang_sta = i;
				objects[objs].dis_sum += polar_dis[i];
				objects[objs].ang_end = i;
				objects[objs].count++;
			}
			else if(abs(polar_dis[i]-polar_dis[objects[objs].ang_end]) <= THRES_DIS)
			{
				objects[objs].dis_sum += polar_dis[i];
				objects[objs].ang_end = i;
				objects[objs].count++;
			}
			else
			{
				objs++;
				i--;
			}
		}
	}

	do {
		if(!DeB)
			system("cls");
		//clrscr ();
		printf ("-------------------------------------\n");
		printf ("|            The Object (%2d)        |\n",objs + 1);
		printf ("-------------------------------------\n");
		printf ("| 1. Show all Objects               |\n");
		printf ("| 2. Show The Object                |\n");
		printf ("| 0. Exit Program                   |\n");
		printf ("-------------------------------------\n\n");
		printf ("Please Enter Choice : ");
		//check = getche();
		fflush(stdout);
		scanf ("%c",&check);
		if (check == '1')
		{
			for (j=0; j<objs ; j++)
			{
				printf ("Object : %d\n",j+1);
				printf (" Start ANG    : %d\n",objects[j].ang_sta);
				printf (" End ANG      : %d\n",objects[j].ang_end);
				printf (" Average Dis. : %d\n\n",objects[j].dis_sum/objects[j].count);
			}
			getch ();
		}
		else if (check == '2')
		{
			printf ("Enter Object ID : ");
			scanf ("%d",&j);
			j--;
			printf ("Object : %d\n",j+1);
			printf (" Start ANG    : %d\n",objects[j].ang_sta);
			printf (" End ANG      : %d\n",objects[j].ang_end);
			printf (" Average Dis. : %d\n\n",objects[j].dis_sum/objects[j].count);
			for (i=objects[j].ang_sta; i<=objects[j].ang_end; i++)
				printf("%d %d\n",i,polar_dis[i]);
			getch ();
		}
	}while(check != '0');
	return 0;
}
