#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>
#include <iostream>
#include <sys/time.h>                
using namespace std;



typedef struct electron 
{
	double x, y, f_x, f_y, a_x, a_y, v_x, v_y;
} electron;

typedef struct ion
{
	float xpos, ypos;
} ion;

#define MAX_Y 1000
#define N 5000
#define CHARGE 1.0
#define MASS 1.0
#define TIME_STEP 1.0
#define STEPS 100
#define WIDTH 50000000
#define HEIGHT 10
#define FORCE 0.5
#define ICOUNT 1000


void change (ion ions[], electron electrons[], FILE *fp)
{
/* loop for each time step*/
	int threads = 8;
	#pragma omp parallel for default(none) shared(electrons, ions) num_threads(threads)
	// {
		for (int i=0; i<N; i++)
		{

			electron e_i = electrons[i];
			e_i.f_x = 0.0;
			e_i.f_y = FORCE;
			//printf("%f %f %f %f %f %f\n", e_i.f_x, e_i.f_y, e_i.v_x, e_i.v_y, e_i.x, e_i.y);
			double iondist;
			double idist;
			ion i_k;
			for (int k=0; k<ICOUNT; k++)
			{
				i_k = ions[k];
				iondist = pow((e_i.x - i_k.xpos), 2) +pow((e_i.y - i_k.ypos),2);
				idist = sqrt(iondist);
				if(iondist < MAX_Y)
				{
					e_i.f_x += (-pow(CHARGE,2)/(iondist))*(-(pow((e_i.x - i_k.xpos),1)/idist));
					e_i.f_y += (-pow(CHARGE,2)/(iondist))*(-(pow((e_i.y - i_k.ypos),1)/idist));
				}
			}
			//printf("%f %f %f %f %f %f %f %f\n.", e_i.f_x, e_i.f_y, e_i.v_x, e_i.v_y, e_i.x, e_i.y, idist, iondist);
			for (int j=0; j<N; j++)
			{
				if (i != j)
				{
					electron e_j = electrons[j];
					double distsqr =pow((e_i.x - e_j.x), 2) + pow((e_i.y - e_j.y), 2);
					double dist = sqrt(distsqr);

					// force calculations
					e_i.f_x += (pow(CHARGE,2)/(distsqr))*((pow((e_i.x - e_j.x),1)/dist));
					e_i.f_y += (pow(CHARGE,2)/(distsqr))*((pow((e_i.y - e_j.y),1)/dist));
				}	 
							
			}
			//acceleration calculations
			e_i.a_x = e_i.f_x/MASS;
			e_i.a_y = e_i.f_y/MASS;
			//velocity calculations
			e_i.v_x = e_i.v_x + e_i.a_x*TIME_STEP;
			e_i.v_y = e_i.v_y + e_i.a_y*TIME_STEP;


			electrons[i] = e_i;
		}
	// }	
		
	#pragma omp parallel for num_threads(threads)
	// {	
		for (int i=0; i<N; i++)
		{	
			electron e_i = electrons[i];
			if (e_i.y < MAX_Y)
			{
				
				e_i.y = e_i.y + e_i.v_y*TIME_STEP;
				e_i.x = e_i.x + e_i.v_x*TIME_STEP;
				
				fprintf(fp, "%f %f\n", e_i.x, e_i.y);
				
				if (e_i.y >= MAX_Y)
				{
					e_i.y = MAX_Y;
					
				}	
				electrons[i] = e_i;

			}
			//fprintf(fp, "%f %f", e.x, e.y);
			//if (e_i.x > 1000)
			//{
			//	printf("%f %f %f %f\n", e_i.x, e_i.v_x, e_i.a_x, e_i.f_x);
			//}
		}
			fprintf(fp,"\n");
	// }
}    
void iterations (int steps, ion ions[], electron electrons[], FILE *fp)
{
	//printf("Here\n");
	for (int i = 0; i < steps; i++)
	{
		printf("Step %d\n", i);
		change(ions, electrons, fp);
	}
		

}

int main(int argc, char *argv[])
{

	int id, numOfTask;

	FILE *f = fopen("Eresult.txt", "w");
	FILE *ionf = fopen("Iresult.txt", "w");
	
	
	electron elist[N];
	srand(time(NULL));
	//declaring each electron's initial position and putting in elist 
	for (int i=0; i<N; i++)
	{	
		printf("Placing Electron[%d]", i);
		double mindist;
		electron e = elist[i];
		do
		{
			//srand(mindist);
			e.x = ((double)(rand())/(double)RAND_MAX)*(double)WIDTH;
			e.y = ((double)(rand())/(double)RAND_MAX)*(double)HEIGHT;
			mindist = (double)WIDTH * (double)HEIGHT;
			for (int j = 0; j<i; j++)
			{
				double distsqr =pow((elist[j].x - elist[i].x), 2) + pow((elist[j].y - elist[i].y), 2);
				if (distsqr < mindist)
				{
					mindist = distsqr;
				}
			}
			printf(" Min Dist:%f\n", mindist);
		}
		while(mindist < 1.0);

		//printf("%f %f\n", e.x, e.y);
		e.v_x = 0;
		e.v_y = 0;
		elist[i] = e;
	}

	ion ilist[ICOUNT];

	//---------------------------


	//---------------------------

	for (int i=0; i<ICOUNT; i++)
	{
		ion particle = ilist[i];
		particle.xpos = ((float)(rand())/(float)RAND_MAX)*WIDTH;
		particle.ypos = ((float)(rand())/(float)RAND_MAX)*(MAX_Y-HEIGHT) + HEIGHT;
		//printf("%f %f\n", particle.xpos, particle.ypos); 
		fprintf(ionf, "%f %f\n", particle.xpos, particle.ypos);
	}

	
	timeval t1, t2;
    double elapsedTime;

    // start timer
    gettimeofday(&t1, NULL);


	fclose(ionf);
	printf("START ITERATIONS\n");
	//clock_t start = clock(), diff;
	//timing start
	iterations(STEPS, ilist, elist, f);
	//timing end
	//diff = clock() - start;
	printf("END ITERATIONS\n");
	fclose(f);

	gettimeofday(&t2, NULL);

    // compute and print the elapsed time in millisec
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0;      // sec to ms
    elapsedTime += (t2.tv_usec - t1.tv_usec) / 1000.0;   // us to ms
    cout << elapsedTime << " ms.\n";
	

	

	//int msec = diff * 1000 / CLOCKS_PER_SEC;
	//printf("Time taken %d seconds %d milliseconds", msec/1000, msec%1000);
	return 0;
}	
