#include<stdio.h>
#include<conio.h>
#include<string.h>
#include<cstdlib>

#define DeB 0
#define OBJ 20
#define THRES_DIS 100
#define THRES_ANG 5
#define THRES_CHA 2
#define THRES_SQR 150
int polar_dis[360];
struct	st_object{
	//int	data[50];
	int	ang_sta;
	int ang_cor;
	int	ang_end;
	int	dis_sum;
	int count;
	int	dis_max;
	int	dis_min;
	char type;
		/*
			0 : None Class.
			P :	Parking
			B : Box
			W : Wall
			X : not used
		*/
};
struct	st_object objects[OBJ];

struct car_object{
	int x;
	int y;
};
struct car_object cartesian[360];

void find_wall(int ang_min, int ang_max, int objs)
{
	int	wallmax = 0;
	for (int i=ang_min; i<ang_max; i++)
	{
		if (polar_dis[i]>wallmax)
			wallmax = polar_dis[i];
	}
	for (int j=0; j<objs ; j++)
	{
		if(objects[j].dis_max >= wallmax && objects[j].type == 0 && (objects[j].ang_end>=ang_min && objects[j].ang_end<=ang_max) )
			objects[j].type = 'W';
	}
	if(DeB)
		printf("MAX polar_dis for %d - %d: %d\n",ang_min,ang_max,wallmax);
}



int main()
{
	int i,j,k,objs,tmp_d,tmp_ang;
	char fname[50];

	char	check = 0;
	FILE	*fp;

	/* Read the distance (Polar) data from text file */
	memset(polar_dis, 0, sizeof polar_dis);
	printf("Enter file name : ");
	//scanf ("%s",fname);
	strcpy(fname, "dataraw/9");
	strcat(fname,".txt");
	fp = fopen(fname,"r");
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
	
	/* Object Segmentation */
	memset(objects, 0, sizeof objects);
	for (objs=0; objs<=OBJ; objs++)
		objects[objs].dis_min = 2300;
	objs = 0;
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
				if (polar_dis[i] < objects[objs].dis_min)
					objects[objs].dis_min = polar_dis[i];
				if (polar_dis[i] > objects[objs].dis_max)
					objects[objs].dis_max = polar_dis[i];
			}
			else if(abs(polar_dis[i]-polar_dis[objects[objs].ang_end]) <= THRES_DIS)
			{
				objects[objs].dis_sum += polar_dis[i];
				objects[objs].ang_end = i;
				objects[objs].count++;
				if (polar_dis[i] < objects[objs].dis_min)
					objects[objs].dis_min = polar_dis[i];
				if (polar_dis[i] > objects[objs].dis_max)
					objects[objs].dis_max = polar_dis[i];
			}
			else
			{
				objs++;
				i--;
			}
		}
	}

	/* Object classification */
	for (j=0; j<objs ; j++)
	{
		if(objects[j].count <= THRES_CHA)
			objects[j].type = 'X';
	}
	for (j=0; j<objs ; j++)
	{
		/*
		if(objects[j].dis_max == polar_dis[objects[j].ang_sta] || objects[j].dis_max == polar_dis[objects[j].ang_end])
			for (i=objects[j].ang_sta; i<=polar_dis[objects[j].ang_end; i++)
				if(polar_dis[i] != 0)
		*/
					
			
				
		if (objects[j].dis_max-objects[j].dis_min > THRES_SQR)
			if(objects[j].dis_max == polar_dis[objects[j].ang_sta] || objects[j].dis_max == polar_dis[objects[j].ang_end])
			{
				objects[j].type = 'B';
			}
			else
				objects[j].type = 'P';
	}
	find_wall (0,90,objs);
	find_wall (90,180,objs);
	find_wall (180,270,objs);
	find_wall (270,360,objs);
	/***/

	/*** hough tranform***/
	for (i = 0; i <360 ; i++){
		
	}


	do {
		if(!DeB)
			system("cls");
		//clrscr ();
		printf ("-------------------------------------\n");
		printf ("|      The %s has %2d Object      |\n",fname,objs+1);
		printf ("-------------------------------------\n");
		printf ("| 1. Show Summary Objects           |\n");
		printf ("| 2. Show The Object Dis.           |\n");
		printf ("| 3. Show Many Objects              |\n");
		printf ("| 0. Exit Program                   |\n");
		printf ("-------------------------------------\n\n");
		printf ("Please Enter Choice : ");
		//check = getche();
		fflush(stdout);
		scanf ("%c",&check);
		if (check == '1')
		{
			for (j=0; j<=objs ; j++)
			{
				if (objects[j].type != 'X')
				{
					printf ("Object : %d\n",j+1);
					printf (" Angle        : %d - %d\n",objects[j].ang_sta,objects[j].ang_end);
					printf (" Average Dis. : %d\n",objects[j].dis_sum/objects[j].count);
					printf (" Chain Obj.   : %d\n",objects[j].count);
					printf (" Max/Min Dis. : %d %d\n",objects[j].dis_max,objects[j].dis_min);
					switch (objects[j].type)
					{
						case 0:
							printf (" Obj. Type    : %s\n\n","None Class.");
							break;
						case 'P':
							printf (" Obj. Type    : %s\n\n","Parking");
							break;
						case 'B':
							printf (" Obj. Type    : %s\n\n","Box");
							break;
						case 'W':
							printf (" Obj. Type    : %s\n\n","Wall");
							break;
						case 'X':
							printf (" Obj. Type    : %s\n\n","not used");
							break;
					}
				}
			}
			getch ();
		}
		else if (check == '2')
		{
			printf ("Enter Object ID : ");
			scanf ("%d",&j);
			j--;
			printf ("Object : %d\n",j+1);
			printf (" Angle        : %d - %d\n",objects[j].ang_sta,objects[j].ang_end);
			printf (" Average Dis. : %d\n",objects[j].dis_sum/objects[j].count);
			printf (" Chain Obj.   : %d\n",objects[j].count);
			printf (" Max/Min Dis. : %d %d\n",objects[j].dis_max,objects[j].dis_min);
			for (i=objects[j].ang_sta; i<=objects[j].ang_end; i++)
				printf("%d %d\n",i,polar_dis[i]);
			getch ();
		}
		else if (check == '3')
		{
			printf ("Enter Start Object ID : ");
			scanf ("%d",&j);
			printf ("Enter End Object ID   : ");
			scanf ("%d",&k);
			j--;			
			while (j<=k)
			{
				printf ("# # # # # Object : %2d # # # # #\n",j+1);
				for (i=objects[j].ang_sta; i<=objects[j].ang_end; i++)
					printf("%d %d\n",i,polar_dis[i]);
				j++;
			}	
			getch ();
		}
	}while(check != '0');
	return 0;
}
