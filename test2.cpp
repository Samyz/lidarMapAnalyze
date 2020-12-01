#include<stdio.h>
#include<conio.h>
#include<math.h>
#include<string.h>
#include<cstdlib>

#define DeB 0
#define OBJ 50
#define THRES_DIS 100
#define THRES_ANG 5
#define THRES_CHA 1
#define THRES_SQR 150
#define filenum "10"
int polar_dis[360], objs;
struct	st_object{
	//int	data[50];
	int	ang_sta;
	int	ang_end;
	int	dis_sum;
	int count;
	//int	dis_max;
	//int	dis_min;
	int ang[3]; // {min, max, corner}
	float m[5];
	char type;
		/*
			0 : None Class.
			P :	Parking
			B : Box
			W : Wall
			X : not used
			1 : Probably Wall
		*/
};
struct	st_object objects[OBJ];

//float maxmin_cartesian [4]; //{xmax, ymin, xmin, ymax}
int maxmin_obj [4]; //{xmax, ymin, xmin, ymax}

struct car_object{
	int x;
	int y;
};
struct car_object cartesian[360], topLeftParking, box[5];
int boxNum = 0;

struct accu_object{
	int num;
	int deg;
	int length;
	float m,b,mDeg;
};
struct accu_object accu_max;

void findmaxmin(int i, int j);
float slope4polar(int, int);
void find_wall(int, int, int);
void ShowObject (int);
void ShowObjectInfo (int);

int main()
{
	int i,j,k;
	int tmp_d,tmp_ang,tmp_max,tmp_min;
	char fname[50];
	int cartesianNum = 0, MaxX = 0, MaxY = 0, MinX = 0, MinY = 0, h, w;

	char	check = 0;
	FILE	*fp, *saveF;

	/* Read the distance (Polar) data from text file */
	memset(polar_dis, 0, sizeof polar_dis);
	// printf("Enter file name : ");
	//scanf ("%s",fname);
	// strcpy(fname, "dataraw/");
	strcpy(fname, "dataDynamic/");
	strcat(fname,filenum);
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
			/*
			if(DeB)
				printf("read data : %d %d\n",tmp_ang,tmp_d);
			*/
		}		
		if(DeB)
			printf("Read Text file Completed\n");
			/*
			for(i=0; i<360 ; i++)
				printf("%d %d\n",i,polar_dis[i]);
			*/
		if(DeB)
			if (!fclose(fp))
				printf("Close file\n");
		//getch();
		fclose(fp);
	}

	/*** Save map for static ***/
	// saveF = fopen("map.txt", "w");
	// fprintf(saveF, "{");
	// for (i = 90; i < 270; i++){
	// 	if (i != 269)
	// 		fprintf(saveF, "%d, ", polar_dis[i]);
	// 	else
	// 		fprintf(saveF, "%d}", polar_dis[i]);
	// }
	// fclose(saveF);
	
	/* Object Segmentation */
	memset(objects, 0, sizeof objects);
	for (objs=0; objs<=OBJ; objs++)
	{
		objects[objs].m[0] = 999;
		objects[objs].m[1] = 999;
	}
	objs = 0;
	int ZeroCount = 0;
	for (i=0; i<360; i++)
	{
		if(polar_dis[i] != 0)
		{
			if (objects[objs].count == 0)
			{
				objects[objs].ang_sta = i;
				objects[objs].dis_sum += polar_dis[i];
				objects[objs].count++;
				objects[objs].ang_end = i;
				objects[objs].ang[0] = i;	//Angle for MIN distance
				objects[objs].ang[1] = i;	//Angle for MAX distance
			}
			else if(abs(polar_dis[i]-polar_dis[objects[objs].ang_end]) <= THRES_DIS && ZeroCount <= THRES_ANG)
			{
				objects[objs].dis_sum += polar_dis[i];
				objects[objs].count++;
				objects[objs].ang_end = i;
				if (polar_dis[i] < polar_dis[objects[objs].ang[0]])
					objects[objs].ang[0] = i;
				if (polar_dis[i] > polar_dis[objects[objs].ang[1]])
					objects[objs].ang[1] = i;
			}
			else
			{
				objs++;
				i--;
			}
			ZeroCount = 0;
		}
		else
		{
			ZeroCount++;
		}
		
	}

	/* Object classification */
	for (j=0; j<=objs ; j++)
	{
		if(objects[j].count <= THRES_CHA)
			objects[j].type = 'X';
		if(objects[j].ang_sta >= 0 && objects[j].ang_sta <= 25 || objects[j].ang_end >= 0 && objects[j].ang_end <= 25 || objects[j].ang_sta >= 335 && objects[j].ang_sta <= 360 || objects[j].ang_end >= 335 && objects[j].ang_end <= 360)
			objects[j].type = 'P';
		if(objects[j].ang_sta > 165 && objects[j].ang_sta < 195 || objects[j].ang_end > 165 && objects[j].ang_end < 195 || objects[j].ang_end > 195 && objects[j].ang_sta < 165)
			objects[j].type = 'W';
		if(objects[j].type != 'X')
		{
			objects[j].m[0] = slope4polar(objects[j].ang_sta, objects[j].ang_end);
			objects[j].m[1] = slope4polar(objects[j].ang_sta, objects[j].ang[0]);
			objects[j].m[2] = slope4polar(objects[j].ang_sta, objects[j].ang[1]);
			objects[j].m[3] = slope4polar(objects[j].ang_end, objects[j].ang[0]);
			objects[j].m[4] = slope4polar(objects[j].ang_end, objects[j].ang[1]);
		}
		
		/*
		if(polar_dis[objects[j].ang[1]] == polar_dis[objects[j].ang_sta] || polar_dis[objects[j].ang[1]] == polar_dis[objects[j].ang_end])
		{
			if(DeB)
				printf("Cals slope object : %d\n",j+1);
			objects[j].m[0] = slope4polar(objects[j].ang_sta, polar_dis[objects[j].ang[0]]);
			objects[j].m[1] = slope4polar(objects[j].ang_end, polar_dis[objects[j].ang[0]]);
		}
		*/
	}
	/*** merge Parking ***/
	// if (objects[objs].type == 'P')
	// {
	// 	objects[0].ang_sta = objects[objs].ang_sta;
	// 	objs--;
	// }

	for (j=0; j<objs ; j++)
	{
		/*
		if (objects[j].dis_max-objects[j].dis_min > THRES_SQR)
			if(objects[j].dis_max == polar_dis[objects[j].ang_sta] || objects[j].dis_max == polar_dis[objects[j].ang_end])
			{
				objects[j].type = 'B';
			}
			else
				objects[j].type = 'P';
		*/
	}
	/*
	find_wall (0,90,objs);
	find_wall (90,180,objs);
	find_wall (180,270,objs);
	find_wall (270,360,objs);
	/***/


	/*** hough tranform***/
	for (j = 0; j <= objs ; j++){
		if(objects[j].type != 'X' && objects[j].type != 'P')
		for (i = objects[j].ang_sta; i <= objects[j].ang_end ; i++){
			if(polar_dis[i] != 0){
				cartesian[cartesianNum].x = (int)floor((int)polar_dis[i] * sin(i * (22.0/7) / 180)/32);
				cartesian[cartesianNum].y = (int)floor((int)polar_dis[i] * cos(i * (22.0/7) / 180)/32);
				if(MaxX < cartesian[cartesianNum].x)
					MaxX = cartesian[cartesianNum].x;
				if(MaxY < cartesian[cartesianNum].y)
					MaxY = cartesian[cartesianNum].y;
				if(MinX > cartesian[cartesianNum].x)
					MinX = cartesian[cartesianNum].x;
				if(MinY > cartesian[cartesianNum].y)
					MinY = cartesian[cartesianNum].y;
				
				cartesianNum++;
			}
		}
	}
	for (i = 0; i < cartesianNum; i++){
		cartesian[i].x += abs(MinX);
		cartesian[i].y += abs(MinY);
	}
	// printf("%d %d %d %d",MinX,MinY,MaxX,MaxY);
	w = MaxX - MinX;
	h = MaxY - MinY;
	double hough_h = ((sqrt(2.0) * (double)(h>w?h:w)) / 2.0);
	double _accu_h = hough_h * 2.0; // -r -> +r  
	double _accu_w = 90;
	unsigned int *_accu = (unsigned int*)calloc(_accu_h * _accu_w, sizeof(unsigned int));  
	double center_x = (MaxX - MinX)/2;//abs(MinX);  
	double center_y = (MaxY - MinY)/2;//abs(MinY);
	// printf("%d\n", cartesianNum); 
	printf("%d %d %d\n",(int)_accu_h,(int)_accu_w, (int)center_y);
	for (i = 0; i <cartesianNum; i++){
		for(int t=0;t<180;t+=2)  
		{  
			double r = ( ((double)cartesian[i].x - center_x) * cos((double)t * (22.0/7) / 180)) + (((double)cartesian[i].y - center_y) * sin((double)t * (22.0/7) / 180));
			// printf("%d\n",(int)((round(r + hough_h) * 180.0)) + t);  
			_accu[ (int)((round(r + hough_h) * 180.0)/2) + t/2] += 1;
			// printf("%d",_accu[ (int)((round(r + hough_h) * 180.0)) + t]);
		} 
	}
	accu_max.num = 0;
	printf("%d %d",(int)_accu_h,(int)_accu_w);
	for (i = 0; i < (int)_accu_h; i++){
		for(j = 0; j <(int) _accu_w; j++){

			int max = _accu[(i*(int)_accu_w) + j];  
			for(int ly=-4;ly<=4;ly++){  
				for(int lx=-8;lx<=8;lx++){  
					if( (ly+i>=0 && ly+i<_accu_h) && (lx+j>=0 && lx+j<_accu_w) ){
						if( (int)_accu[( (i+ly)*(int)_accu_w) + (j+lx)] > max ){  
							max = _accu[( (i+ly)*(int)_accu_w) + (j+lx)];  
							ly = lx = 5;  
						}  
					}  
				}  
			}  
			if(max > (int)_accu[(i*(int)_accu_w) + j])  
				continue;
			if ((int)_accu[(i*(int)_accu_w) + j] == accu_max.num && i >= accu_max.length - 4 && i <= accu_max.length + 4 && j >= accu_max.deg - 4 && j <= accu_max.deg + 4)	  
				continue;
			if (abs(180 - j*2) >= accu_max.deg - 2 && abs(180 - j*2) <= accu_max.deg + 2)
				if (abs(i - _accu_h/2) >= abs(accu_max.length - _accu_h/2) - 2 && abs(i - _accu_h/2) <= abs(accu_max.length - _accu_h/2) + 2)
					continue;

			if (accu_max.num < _accu[i*(int)_accu_w + j]){
				accu_max.num = _accu[i*(int)_accu_w + j];
				accu_max.deg = j*2;
				accu_max.length = i;
			}
		}
	}
	int x1, y1, x2, y2;  
	x1 = y1 = x2 = y2 = 0;  
	if(accu_max.deg >= 45 && accu_max.deg <= 135)  
	{  
		//y = (r - x cos(t)) / sin(t)  
		x1 = 0;  
		y1 = ((double)(accu_max.length-(_accu_h/2)) - ((x1 - (center_x) ) * cos(accu_max.deg * (22.0/7) / 180))) / sin(accu_max.deg * (22.0/7) / 180) + (center_y);  
		x2 = center_x - 0;  
		y2 = ((double)(accu_max.length-(_accu_h/2)) - ((x2 - (center_x) ) * cos(accu_max.deg * (22.0/7) / 180))) / sin(accu_max.deg * (22.0/7) / 180) + (center_y);  
	}  
	else  
	{  
		//x = (r - y sin(t)) / cos(t);  
		y1 = 0;  
		x1 = ((double)(accu_max.length-(_accu_h/2)) - ((y1 - (center_y) ) * sin(accu_max.deg * (22.0/7) / 180))) / cos(accu_max.deg * (22.0/7) / 180) + (center_x);  
		y2 = center_y - 0;  
		x2 = ((double)(accu_max.length-(_accu_h/2)) - ((y2 - (center_y) ) * sin(accu_max.deg * (22.0/7) / 180))) / cos(accu_max.deg * (22.0/7) / 180) + (center_x);  
	}
	x1 = x1*32 - abs(MinX*32);
	x2 = x2*32 - abs(MinX*32);
	y1 = y1*32 - abs(MinY*32);
	y2 = y2*32 - abs(MinY*32);
	accu_max.m = ((float)y2 - (float)y1)/((float)x2 - (float)x1);
	accu_max.b = -accu_max.m*x1 + (float)y1;
	accu_max.mDeg = atan(accu_max.m) * 180/(22.0/7);

	/*** rotate map ***/
	if (accu_max.mDeg < 0)
		accu_max.mDeg += 90;
	if (accu_max.mDeg <= 9){
		//clockwise
		int *temp = (int*)malloc((int)accu_max.mDeg * sizeof(int));
		for (i = 0; i < (int)accu_max.mDeg; i++)
			temp[i] = polar_dis[360 - (int)accu_max.mDeg + i];
		for (i = 359 - (int)accu_max.mDeg; i >= 0; i--)
			polar_dis[i + (int)accu_max.mDeg] = polar_dis[i];
		for (i = 0; i < (int)accu_max.mDeg; i++)
			polar_dis[i] = temp[i];
		for (i = 0; i <= objs ; i++){
			objects[i].ang_sta = (objects[i].ang_sta + (int)accu_max.mDeg) % 360;
			objects[i].ang_end = (objects[i].ang_end + (int)accu_max.mDeg) % 360;
			objects[i].ang[0] = (objects[i].ang[0] + (int)accu_max.mDeg) % 360;
			objects[i].ang[1] = (objects[i].ang[1] + (int)accu_max.mDeg) % 360;
		}
		free(temp);
	}
	else{
		//counter clockwise
		int *temp = (int*)malloc((90 - (int)accu_max.mDeg) * sizeof(int));
		for (i = 0; i < (90 - (int)accu_max.mDeg); i++)
			temp[i] = polar_dis[i];
		for (i = (90 - (int)accu_max.mDeg); i < 360; i++)
			polar_dis[i - (90 - (int)accu_max.mDeg)] = polar_dis[i];
		for (i = 0; i < (90 - (int)accu_max.mDeg); i++)
			polar_dis[i + 360 - (90 - (int)accu_max.mDeg)] = temp[i];
		for (i = 0; i <= objs ; i++){
			// printf("%d %d %d %d %d\n", objects[i].ang_sta, objects[i].ang_end, objects[i].ang[0], objects[i].ang[1], (90 - (int)accu_max.mDeg));
			objects[i].ang_sta = (objects[i].ang_sta - (90 - (int)accu_max.mDeg)) > 0 ? (objects[i].ang_sta - (90 - (int)accu_max.mDeg)) : ((objects[i].ang_sta - (90 - (int)accu_max.mDeg)) + 360) % 360;
			objects[i].ang_end = (objects[i].ang_end - (90 - (int)accu_max.mDeg)) > 0 ? (objects[i].ang_end - (90 - (int)accu_max.mDeg)) : ((objects[i].ang_end - (90 - (int)accu_max.mDeg)) + 360) % 360;
			objects[i].ang[0] = (objects[i].ang[0] - (90 - (int)accu_max.mDeg)) > 0 ? (objects[i].ang[0] - (90 - (int)accu_max.mDeg)) : ((objects[i].ang[0] - (90 - (int)accu_max.mDeg)) + 360) % 360;
			objects[i].ang[1] = (objects[i].ang[1] - (90 - (int)accu_max.mDeg)) > 0 ? (objects[i].ang[1] - (90 - (int)accu_max.mDeg)) : ((objects[i].ang[1] - (90 - (int)accu_max.mDeg)) + 360) % 360;
		}
		free(temp);
	}

	/*** find wall value ***/
	for (i = 0; i < 4; i++)
		maxmin_obj [i] = objs+3;
	int wallDeg[2] = {-1,-1};
	int wallValue[4] = {0,0,0,0};
	for (i = 0, j = 0; i <= objs; i++){
		if (objects[i].type == 'W' && objects[i].count >= 2){
			wallDeg[j] = (objects[i].ang[1] - 45) < 0 ? ((objects[i].ang[1] - 45) + 360) / 90 : (objects[i].ang[1] - 45) / 90;
			maxmin_obj[wallDeg[j]] = i;
			printf("\n%d\n",wallDeg[j]);
			j++;
		}
	}

	/*** find other wall ***/
	int wallCheck = 0;
	if(wallDeg[1] == -1)
		if(wallDeg[0] == 0)
			wallCheck = 1;
		else
			wallCheck = 2;
	else
		wallCheck = 3;
	// printf("%d\n", objs);
	for (i = 0; i <= objs; i++){
		if (objects[i].type != 'P' && objects[i].type != 'W'){
			if (objects[i].ang_sta >=0 && objects[i].ang_sta <180 && !(wallCheck&1))
				// find obj xmax maxmin_obj[0]
				findmaxmin(i, 0);
			if (objects[i].ang_sta >=90 && objects[i].ang_sta <270 && !(wallCheck >> 1)&1)
				findmaxmin(i, 1);
			if (objects[i].ang_sta >=180 && objects[i].ang_sta <360)
				findmaxmin(i, 2);
			if (objects[i].ang_sta >=270 || objects[i].ang_sta <90)
				findmaxmin(i, 3);
			// printf("%d ",i);
		}
	}
	for (i = 0; i <4; i++){
		printf("%d  ", maxmin_obj[i]);
		if (objects[maxmin_obj[i]].type != 'W')
			objects[maxmin_obj[i]].type = 1;
	}

	int tmp_a,tmp_b;
	printf("\n%d\n", wallCheck);
	tmp_a = polar_dis[objects[maxmin_obj[0]].ang[1]] * sin(objects[maxmin_obj[0]].ang[1] * (22.0/7)/180);
	tmp_b = polar_dis[objects[maxmin_obj[2]].ang[1]] * sin(objects[maxmin_obj[2]].ang[1] * (22.0/7)/180);
	printf("\n%d", tmp_a - tmp_b);
	if (tmp_a - tmp_b < 2500 && tmp_a - tmp_b > 2200){
		objects[maxmin_obj[2]].type = 'W';
		wallValue[2] = tmp_b;
		wallValue[0] = tmp_a;
		if (wallCheck == 2)
			objects[maxmin_obj[0]].type = 'W';
	}
	else{
		objects[maxmin_obj[2]].type = 0;
		wallValue[2] = tmp_a - 2300;
		wallValue[0] = tmp_a;
		if (wallCheck == 2)
			objects[maxmin_obj[0]].type = 0;
	}
	tmp_a = polar_dis[objects[maxmin_obj[3]].ang[1]] * cos(objects[maxmin_obj[3]].ang[1] * (22.0/7)/180);
	tmp_b = polar_dis[objects[maxmin_obj[1]].ang[1]] * cos(objects[maxmin_obj[1]].ang[1] * (22.0/7)/180);
	printf("\n%d\n", tmp_a - tmp_b);
	if (tmp_a - tmp_b < 2500 && tmp_a - tmp_b > 2200){
		objects[maxmin_obj[3]].type = 'W';
		wallValue[1] = tmp_b;
		wallValue[3] = tmp_a;
		if (wallCheck == 1)
			objects[maxmin_obj[1]].type = 'W';
	}
	else{
		objects[maxmin_obj[3]].type = 0;
		wallValue[1] = tmp_b - 2300;
		wallValue[3] = tmp_b;
		if (wallCheck == 1)
			objects[maxmin_obj[1]].type = 0;
	}

	for (i = 0; i < 4 ; i++){
		printf("\n %d", wallValue[i]);
	}

	/*** find top left parking ***/
	topLeftParking.x = polar_dis[objects[objs].ang[1]] * sin(objects[objs].ang[1] * (22.0/7) / 180) - wallValue[2];
	topLeftParking.y = -(polar_dis[objects[objs].ang[1]] * cos(objects[objs].ang[1] * (22.0/7) / 180) - wallValue[3]);
	topLeftParking.x = topLeftParking.x / 115 + ((topLeftParking.x % 115) > 57 ? 1 : 0);
	topLeftParking.y = topLeftParking.y / 115 + ((topLeftParking.y % 115) > 57 ? 1 : 0);
	printf("\n topLeft %d %d\n", topLeftParking.x, topLeftParking.y);
	int ParkingAngle = 0;
	if (accu_max.mDeg > 9 && accu_max.mDeg <= 24)
		ParkingAngle = 72;
	else if (accu_max.mDeg > 24 && accu_max.mDeg <= 35)
		ParkingAngle = 63;
	else if (accu_max.mDeg > 35 && accu_max.mDeg <= 55)
		ParkingAngle = 45;
	else if (accu_max.mDeg > 55 && accu_max.mDeg <= 65)
		ParkingAngle = 26;
	else if (accu_max.mDeg > 65 && accu_max.mDeg <= 81){
		ParkingAngle = 18;
	}
	printf("\n ParkingAngle %d ", ParkingAngle);

	/*** wall out ***/
	for (i = 0; i < 360; i++){
		tmp_a = polar_dis[i] * sin(i * (22.0/7)/180);
		tmp_b = polar_dis[i] * cos(i * (22.0/7)/180);
		if (tmp_a > wallValue[0] - 160 || tmp_b < wallValue[1] + 160 || tmp_a < wallValue[2] + 160 || tmp_b > wallValue[3] - 160){
			polar_dis[i] = 0;
		}
	}

	/*** parking out ***/
	for (i = 0; i <= objs; i++){
		if(objects[i].type == 'P'){
			if (objects[i].ang_sta < objects[i].ang_end)
				for (j = objects[i].ang_sta; j <= objects[i].ang_end; j++){
					polar_dis[j] = 0;
				}
			else{
				for (j = objects[i].ang_sta; j <= 359; j++){
					polar_dis[j] = 0;
				}
				for (j = 0; j <= objects[i].ang_end; j++){
					polar_dis[j] = 0;
				}
			}
		}
	}

	/* New Object Segmentation */
	memset(objects, 0, sizeof objects);
	for (objs=0; objs<=OBJ; objs++)
	{
		objects[objs].m[0] = 999;
		objects[objs].m[1] = 999;
	}
	objs = 0;
	ZeroCount = 0;
	for (i=0; i<360; i++)
	{
		if(polar_dis[i] != 0)
		{
			if (objects[objs].count == 0)
			{
				objects[objs].ang_sta = i;
				objects[objs].dis_sum += polar_dis[i];
				objects[objs].count++;
				objects[objs].ang_end = i;
				objects[objs].ang[0] = i;	//Angle for MIN distance
				objects[objs].ang[1] = i;	//Angle for MAX distance
			}
			else if(abs(polar_dis[i]-polar_dis[objects[objs].ang_end]) <= 150 || ZeroCount <= 2)
			{
				if (ZeroCount == 2 && objects[objs].count >= 5 &&  polar_dis[objects[objs].ang_end] - polar_dis[i] >= 150){
					objs++;
					i--;
				}
				else{
					objects[objs].dis_sum += polar_dis[i];
					objects[objs].count++;
					objects[objs].ang_end = i;
					if (polar_dis[i] < polar_dis[objects[objs].ang[0]])
						objects[objs].ang[0] = i;
					if (polar_dis[i] > polar_dis[objects[objs].ang[1]])
						objects[objs].ang[1] = i;
				}
			}
			else
			{
				objs++;
				i--;
			}
			ZeroCount = 0;
		}
		else
		{
			ZeroCount++;
		}
		if (ZeroCount == 4 && objects[objs].count != 0){
			objs++;
			ZeroCount = 0;
		}
	}
	if (objects[objs].count == 0)
		objs--;
	if (objects[objs].ang_end > 350 && objects[0].ang_sta < 10){
		objects[0].ang_sta = objects[objs].ang_sta;
		objects[0].count += objects[objs].count;
		objs--;
	}

	/* New Object classification */
	for (j=0; j<=objs ; j++)
	{
		// if(objects[j].count <= THRES_CHA)
		// 	objects[j].type = 'X';
		if(objects[j].type != 'X')
		{
			objects[j].m[0] = slope4polar(objects[j].ang_sta, objects[j].ang_end);
			objects[j].m[1] = slope4polar(objects[j].ang_sta, objects[j].ang[0]);
			objects[j].m[2] = slope4polar(objects[j].ang_sta, objects[j].ang[1]);
			objects[j].m[3] = slope4polar(objects[j].ang_end, objects[j].ang[0]);
			objects[j].m[4] = slope4polar(objects[j].ang_end, objects[j].ang[1]);
		}
	}

	for (i = 0; i <= objs ; i++){
		if(objects[i].count <= 3){
			//dot case
			int rotate = 0;
			tmp_a = (polar_dis[objects[i].ang_sta] * sin(objects[i].ang_sta * (22.0/7)/180) - wallValue[2]);
			tmp_b = -(polar_dis[objects[i].ang_sta] * cos(objects[i].ang_sta * (22.0/7)/180) - wallValue[3]);
			box[boxNum].x = tmp_a / 115 + ((tmp_a % 115) > 57 ? 1 : 0);
			box[boxNum].y = tmp_b / 115 + ((tmp_b % 115) > 57 ? 1 : 0);
			if (accu_max.mDeg > 9 && accu_max.mDeg < 81){
				rotate = 90 - accu_max.mDeg;
			}
			if (objects[i].ang_sta < 360 - rotate){
				box[boxNum].y -= 2;
			}
			else{
				tmp_a = (polar_dis[objects[i].ang_end] * sin(objects[i].ang_end * (22.0/7)/180) - wallValue[2]);
				tmp_b = -(polar_dis[objects[i].ang_end] * cos(objects[i].ang_end * (22.0/7)/180) - wallValue[3]);
				box[boxNum].x = tmp_a / 115 + ((tmp_a % 115) > 57 ? 1 : 0);
				box[boxNum].y = tmp_b / 115 + ((tmp_b % 115) > 57 ? 1 : 0);
				box[boxNum].y -= 2;
				box[boxNum].x -= 2;
			}
		}
		else{
			if(abs(objects[i].m[0]) < 0.1 ){
				//x-axis case
				tmp_a = (polar_dis[objects[i].ang_sta] * sin(objects[i].ang_sta * (22.0/7)/180) - wallValue[2]);
				tmp_b = -(polar_dis[objects[i].ang_sta] * cos(objects[i].ang_sta * (22.0/7)/180) - wallValue[3]);
				if (objects[i].ang_sta >= 90 && objects[i].ang_sta < +270){
					box[boxNum].x = tmp_a / 115 + ((tmp_a % 115) > 57 ? 1 : 0);
					box[boxNum].y = tmp_b / 115 + ((tmp_b % 115) > 57 ? 1 : 0);
					box[boxNum].x -= 2;
				}
				else{
					box[boxNum].x = tmp_a / 115 + ((tmp_a % 115) > 57 ? 1 : 0);
					box[boxNum].y = tmp_b / 115 + ((tmp_b % 115) > 57 ? 1 : 0);
					box[boxNum].y -= 2;
				}
			}
			else if(abs(objects[i].m[0]) > 10){
				//y-axis case
				tmp_a = (polar_dis[objects[i].ang_sta] * sin(objects[i].ang_sta * (22.0/7)/180) - wallValue[2]);
				tmp_b = -(polar_dis[objects[i].ang_sta] * cos(objects[i].ang_sta * (22.0/7)/180) - wallValue[3]);
				if (objects[i].ang_sta >= 0 && objects[i].ang_sta < 180){
					box[boxNum].x = tmp_a / 115 + ((tmp_a % 115) > 57 ? 1 : 0);
					box[boxNum].y = tmp_b / 115 + ((tmp_b % 115) > 57 ? 1 : 0);
				}
				else{
					box[boxNum].x = tmp_a / 115 + ((tmp_a % 115) > 57 ? 1 : 0);
					box[boxNum].y = tmp_b / 115 + ((tmp_b % 115) > 57 ? 1 : 0);
					box[boxNum].y -= 2;
					box[boxNum].x -= 2;
				}
			}
			else{
				//normal case 
				tmp_a = (polar_dis[objects[i].ang[0]] * sin(objects[i].ang[0] * (22.0/7)/180) - wallValue[2]);
				tmp_b = -(polar_dis[objects[i].ang[0]] * cos(objects[i].ang[0] * (22.0/7)/180) - wallValue[3]);
				box[boxNum].x = tmp_a / 115 + ((tmp_a % 115) > 57 ? 1 : 0);
				box[boxNum].y = tmp_b / 115 + ((tmp_b % 115) > 57 ? 1 : 0);
				if (objects[i].ang[0] >= 0 && objects[i].ang[0] < 90)
					box[boxNum].y -= 2;
				else if (objects[i].ang[0] >= 180 && objects[i].ang[0] < 270)
					box[boxNum].x -= 2;
				else if (objects[i].ang[0] >= 270){
					box[boxNum].y -= 2;
					box[boxNum].x -= 2;
				}
				// printf("\n%d", tmp_b%115);
			}
		}
		boxNum++;
	}
	for (i = 0; i < boxNum; i++){
		printf("box %d x = %d y = %d\n",i,box[i].x,box[i].y);
	}


	/*** file for plot ***/
	saveF = fopen("out.txt","w");
	for (i = 0; i < 360; i++)
		if (polar_dis[i] != 0)
			fprintf(saveF,"%d %d\n\n", polar_dis[i], i);
	fclose(saveF);

	do {
		if(!DeB)
			// system("cls");
		//clrscr ();
		printf("No.%d accuDeg = %d accuLenght = %d num = %d m = %f b = %f arctan = %f\n", i + 1, accu_max.deg, accu_max.length, accu_max.num, accu_max.m, accu_max.b, accu_max.mDeg);
		if (accu_max.b < 0)
			printf("y = %.2fx %.2f\n", accu_max.m, accu_max.b);
		else
			printf("y = %.2fx + %.2f\n", accu_max.m, accu_max.b);
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
				// if (objects[j].type != 'X')
					ShowObject(j);
			getch ();
		}
		else if (check == '2')
		{
			printf ("Enter Object ID : ");
			scanf ("%d",&j);
			j--;
			ShowObject(j);
			ShowObjectInfo(j);
			getch ();
		}
		else if (check == '3')
		{
			printf ("Enter Start Object ID : ");
			scanf ("%d",&j);
			printf ("Enter End Object ID   : ");
			scanf ("%d",&k);
			for (j--; j < k; j++)
				ShowObjectInfo(j);
			getch ();
		}
	}while(check != '0');
	return 0;
}

void findmaxmin(int i, int j)
{
	int x1 = polar_dis[objects[i].ang[1]] * sin(objects[i].ang[1] * (22.0/7)/180);
	int y1 = polar_dis[objects[i].ang[1]] * cos(objects[i].ang[1] * (22.0/7)/180);
	int x2,y2;
	if (maxmin_obj[j] > objs){
		x2 = 0 * sin(objects[maxmin_obj [j]].ang[1] * (22.0/7)/180);
		y2 = 0 * cos(objects[maxmin_obj [j]].ang[1] * (22.0/7)/180);
	}
	else{
		x2 = polar_dis[objects[maxmin_obj [j]].ang[1]] * sin(objects[maxmin_obj [j]].ang[1] * (22.0/7)/180);
		y2 = polar_dis[objects[maxmin_obj [j]].ang[1]] * cos(objects[maxmin_obj [j]].ang[1] * (22.0/7)/180);
	}
	if (j == 0 && x1 > x2)
		maxmin_obj [j] = i;
	if (j == 1 && y1 < y2)
		maxmin_obj [j] = i;
	if (j == 2 && x1 < x2)
		maxmin_obj [j] = i;
	if (j == 3 && y1 > y2)
		maxmin_obj [j] = i;
}

float slope4polar (int a1, int a2)
{
	/*
	x = dis * sin(ang * (22/7) / 180);
	y = dis * cos(ang * (22/7) / 180);
	*/
	float m;
	if(DeB)
	{
		printf ("%d %d\t%d %d\n",a1,a2,polar_dis[a1],polar_dis[a2]);
		printf ("%f %f\t%f %f\n\n",polar_dis[a2]*cos(a2*(22/7)/180),polar_dis[a1]*cos(a1*(22/7)/180),polar_dis[a2]*sin(a2*(22.0/7)/180),polar_dis[a1]*sin(a1*(22.0/7)/180));
	}
	m = (polar_dis[a2]*cos(a2*(22.0/7)/180) - polar_dis[a1]*cos(a1*(22.0/7)/180)) / (polar_dis[a2]*sin(a2*(22.0/7)/180) - polar_dis[a1]*sin(a1*(22.0/7)/180));
	return m;
}

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
		if(polar_dis[objects[j].ang[1]] >= wallmax && objects[j].type == 0 && (objects[j].ang_end>=ang_min && objects[j].ang_end<=ang_max) )
			objects[j].type = 'W';
	}
	if(DeB)
		printf("MAX polar_dis for %d - %d: %d\n",ang_min,ang_max,wallmax);
}

void ShowObject (int j)
{
	printf ("Object : %d\n",j+1);
	printf (" Angle        : %d - %d\n",objects[j].ang_sta,objects[j].ang_end);
	printf (" Average Dis. : %d\n",objects[j].dis_sum/objects[j].count);
	printf (" Chain Obj.   : %d\n",objects[j].count);
	printf (" Max/Min Dis. : %d (%d) / %d (%d)\n",polar_dis[objects[j].ang[1]],objects[j].ang[1],polar_dis[objects[j].ang[0]],objects[j].ang[0]);
	printf (" Slope        : %f %f %f %f %f\n",objects[j].m[0],objects[j].m[1],objects[j].m[2],objects[j].m[3],objects[j].m[4]);
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
		case 1:
			printf (" Obj. Type    : %s\n\n","Probably Wall X max");
			break;
		case 2:
			printf (" Obj. Type    : %s\n\n","Probably Wall Y min");
			break;
		case 3:
			printf (" Obj. Type    : %s\n\n","Probably Wall X min");
			break;
		case 4:
			printf (" Obj. Type    : %s\n\n","Probably Wall Y max");
			break;
	}
}
void ShowObjectInfo (int j)
{
	printf ("# # # # # Object : %2d Information # # # # #\n",j+1);
	if (objects[j].ang_sta > objects[j].ang_end){
		for (int i=objects[j].ang_sta; i<=359; i++)
			printf("%d %d\n",i,polar_dis[i]);
		for (int i=0; i<=objects[j].ang_end; i++)
			printf("%d %d\n",i,polar_dis[i]);
	}
	else
	for (int i=objects[j].ang_sta; i<=objects[j].ang_end; i++)
		printf("%d %d\n",i,polar_dis[i]);
}
