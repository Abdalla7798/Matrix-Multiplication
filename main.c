#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

int **arr1 , **arr2 , **arrR1 , **arrR2;
int r1 , c1 , r2 , c2;
int start_row = 0;

void write_file_row_version(int **arr , double time_taken){

  int a,b;
  FILE * fPtr;
   fPtr  = fopen("output.txt", "a");

   if (fPtr == NULL)
    {
        printf("\nUnable to open file.\n");
        exit(0);
    }
     //fprintf(fPtr,"\n");
    for(a = 0 ; a<r1 ; a++){
        for(b = 0 ; b<c2 ; b++){
           fprintf(fPtr,"%d ",arr[a][b]);
        }
        fprintf(fPtr,"\n");
     }

     fprintf(fPtr,"END1     %lf\n",time_taken);

    fclose(fPtr);
}

void write_file_element_version(int **arr , double time_taken){

  int a,b;
  FILE * fPtr;
   fPtr  = fopen("output.txt", "a");

   if (fPtr == NULL)
    {
        printf("\nUnable to open file.\n");
        exit(0);
    }
     //fprintf(fPtr,"\n");
    for(a = 0 ; a<r1 ; a++){
        for(b = 0 ; b<c2 ; b++){
           fprintf(fPtr,"%d ",arr[a][b]);
        }
        fprintf(fPtr,"\n");
     }

     fprintf(fPtr,"END2     %lf\n",time_taken);

    fclose(fPtr);
}

void * start_routine_MUL_for_Each_Elem(void *arg){

  int *row_col = (int *)arg;
    int k , i ,j;

    i = row_col[0];
    j = row_col[1];

    //int x = data[0];
    for (k = 0; k < c1; k++) {

        arrR2[i][j] = arrR2[i][j] + arr1[i][k] * arr2[k][j];
    }

}


void * start_routine_MUL(void *arg){

   int i , j ,k;

   for (i = start_row; i < (start_row + 1); i++){ // one iteration for this loop

        for (j = 0; j < c2; j++) {

             for (k = 0; k < c1; k++) {

                  arrR1[i][j] = arrR1[i][j] + arr1[i][k] * arr2[k][j];
             }
        }

   }

   start_row++;
}

int main()
{

FILE * INFile;
int i , j;

INFile = fopen ("input.txt","r");
if (INFile!=NULL) {

// first array
fscanf(INFile, "%d", &r1);
fscanf(INFile, "%d", &c1);

arr1 = (int **)malloc(r1 * sizeof(int *));
    for (i=0; i < r1; i++)
         arr1[i] = (int *)malloc(c1 * sizeof(int));

 for(i = 0 ; i < r1 ; i++){
        for(j = 0 ; j < c1 ; j++){
           fscanf(INFile, "%d", &arr1[i][j]);
        }
 }

// second array
fscanf(INFile, "%d", &r2);
fscanf(INFile, "%d", &c2);

arr2 = (int **)malloc(r2 * sizeof(int *));
    for (i=0; i < r2; i++)
         arr2[i] = (int *)malloc(c2 * sizeof(int));

 for(i = 0 ; i<r2 ; i++){
        for(j = 0 ; j < c2 ; j++){
           fscanf(INFile, "%d", &arr2[i][j]);
        }
 }

fclose (INFile);
}

// array of result
arrR1 = (int **)malloc(r1 * sizeof(int *));
    for (i=0; i < r1; i++)
         arrR1[i] = (int *)malloc(c2 * sizeof(int));
// initialize ResultArray by 0
    for(i = 0 ; i < r1 ; i++){
        for(j = 0 ; j < c2 ; j++){
            arrR1[i][j] = 0;
        }
 }


pthread_t *threads = (pthread_t*)malloc(r1*sizeof(pthread_t));

clock_t begin = clock();

for (i = 0; i < r1; i++) {
    int* arg;
    pthread_create(&threads[i], NULL, start_routine_MUL, (void*)(arg)); // create threads equal to number of rows of the first matrix
}

for (i = 0; i < r1; i++){
    pthread_join(threads[i], NULL); // wait for each thread until finish its work before main thread exits
}

clock_t end = clock();

double time_taken = ((double) (end - begin) / CLOCKS_PER_SEC);

write_file_row_version(arrR1 , time_taken);


/********************************************/

pthread_t *threadsForeachElem = (pthread_t*)malloc(r1*c2*sizeof(pthread_t));
int *row_col_indexOfcell;
int count = 0;

 arrR2 = (int **)malloc(r1 * sizeof(int *));
    for (i=0; i < r1; i++)
         arrR2[i] = (int *)malloc(c2 * sizeof(int));
// initialize ResultArray by 0
    for(i = 0 ; i < r1 ; i++){
        for(j = 0 ; j < c2 ; j++){
            arrR2[i][j] = 0;
        }
 }

 clock_t begin1 = clock();

     for (i = 0; i < r1; i++) {

         for (j = 0; j < c2; j++){

             row_col_indexOfcell = (int *)malloc((2)*sizeof(int));
             row_col_indexOfcell[0]=i;
             row_col_indexOfcell[1]=j;

             pthread_create(&threadsForeachElem[count++], NULL, start_routine_MUL_for_Each_Elem, (void*)(row_col_indexOfcell));
          }

        }

        for (i = 0; i < (r1*c2); i++){
           pthread_join(threadsForeachElem[i], NULL); // wait for each thread until finish its work before main thread exits
        }

   clock_t end1 = clock();

   double time_taken1 = ((double) (end1 - begin1) / CLOCKS_PER_SEC);

   write_file_element_version(arrR2,time_taken1);


    return 0;
}
