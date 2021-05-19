#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>


double **clusters;
double **centroids; 
double **vector_list;
int* clustersindexes;
int vector_len;
int vector_num;
int clusters_num; 



int find_vector_num(){
    /*FILE *file;*/
    int lines;
    char ch;
    lines = 0;

    /*file = fopen(path, "r");*/
    ch = getc(stdin);
    while (ch != EOF) {
        if (ch == '\n') {
            lines++;
        }
        ch = getc(stdin);
    }
    /*fclose(file);*/
    vector_num = lines;
    return lines;

} /* number of lines*/


int find_vector_len(){ 
    /*FILE *file;*/
    int vectorcomp;
    char ch;
    vectorcomp = 1;
    /*file = fopen(path, "r");*/
    ch = getc(stdin);
    while (ch != '\n') {
        if (ch == ',') {
            vectorcomp++;
        }
        ch = getc(stdin);
    }
    /*fclose(file);*/
    vector_len = vectorcomp;
    return vectorcomp;
}


void readfile() {
    /*FILE *file;*/ 
    char *split_line;
    int i,j;
    char line[1000*sizeof(char)];
    /*file = fopen(path, "r");*/
    find_vector_len();
    rewind(stdin);
    find_vector_num();
    rewind(stdin);
    split_line = (char *)calloc(vector_len, 1000*sizeof(char));
    assert(split_line != NULL);
    vector_list = (double**) malloc(vector_num*sizeof(double));
    assert(vector_list != NULL);
    for ( i=0; i<vector_num; i++) {
        vector_list[i] = (double*) calloc(vector_len,sizeof(double));
        assert(vector_list[i] != NULL);
    }
   
    /*fgets(line,1000, stdin);STDIN*/
     j = 0;
    while(fgets(line,1000, stdin) != NULL){ /* reads line by line as a string*/
        split_line = strtok(line, ","); /* split line by commas*/
        for ( i=0 ; i<vector_len ; i++) {
            vector_list[j][i] = atof(split_line); /* convert str to float*/
            if (j>780) {}
            split_line = strtok(NULL, ",");
        }
        j++;
    }
    free(split_line);
}
void init_cendroids(int clusters_num) {
    int i,j,x;
    assert(clusters_num<vector_num);
    centroids = (double**) calloc(clusters_num, vector_len*sizeof(double));
    assert(centroids != NULL);
    for ( i=0; i<clusters_num; i++) {
        centroids[i] = (double*) calloc(vector_len,sizeof(double));
        assert(centroids[i] != NULL);
    }
    for ( x=0; x<clusters_num; x++) {
        for ( j=0; j<vector_len; j++) {
            centroids[x][j]=vector_list[x][j];
        }
    }
    clusters = (double**) calloc(clusters_num, sizeof(double*));
}

double distance(double *v1, double *v2) {
    double res;
    int length;
    int i;
    res = 0;
    length = vector_len;
    for ( i=0; i<length; i++) {
        res += (v1[i]-v2[i])*(v1[i]-v2[i]);
    }
    return res;
}

int min_dist_centroid(double *v){
    double minimum;
    int ind;
    double dist;
    int i;
    minimum = distance(v, centroids[0]);
    ind = 0;
    for ( i=0; i<clusters_num; i++){
        dist = distance(v, centroids[i]);
        if (dist < minimum){
            minimum = dist;
            ind = i;
        }
    }  
    return ind;
}

void vector_to_cluster(int clusters_num) {
    int* clusterssizes; /*for realloc*/
    int ind;
    int i;
    clustersindexes = (int *) calloc(clusters_num, sizeof(int));
    assert(clustersindexes != NULL);
    clusterssizes = (int *) calloc(clusters_num, sizeof(int));
    assert(clusterssizes != NULL);
    for ( i=0;i<clusters_num;i++) { /*initialize each cluster's size to 100*/
        clusterssizes[i] = 100;
    }
    for ( i=0; i<clusters_num; i++) {
        clusters[i] = (double*) calloc(100,sizeof(double));
        assert(clusters[i]!=NULL);
        }/*PICK OTHER NUMBER OR VARIABLE*/
    for ( i=0; i<vector_num; i++) {/*COMM:might need to change size */
        ind = min_dist_centroid(vector_list[i]);
        if (clustersindexes[ind]>((clusterssizes[ind])/2)) {/*Increase if necessary*/
            clusters[ind] = (double*) realloc(clusters[ind], 2*clusterssizes[ind]*sizeof(double));
            clusterssizes[ind]*=2;
        }
        clusters[ind][clustersindexes[ind]] = i;
        clustersindexes[ind]++; /*increase number of vectors in specified cluster*/

    }
}

double* cluster_to_centroid(int index){
    int i;
    int j;
    int vector_index;
    int num = clustersindexes[index]; /* number of vectors in given cluster */
    double * res = (double *)calloc(vector_len, sizeof(double));
    assert(res != NULL);
    for (i = 0; i < vector_len; i++) {
        for (j = 0; j < num; j++) {
            vector_index = clusters[index][j];
            res[i] += vector_list[vector_index][i]; /*relevant cluster*/
        }
    }
    for (i = 0; i < vector_len; i++) {
        res[i] = res[i]/(num); 
    }
    return res;

}

int areequal(double* arr1, double* arr2) {
    int length;
    int y;
    length = vector_len; 
    for ( y =0; y<length; y++) {
        if (arr1[y]!=arr2[y]) {
            return 0;
        }
    }
    return 1;
}

int update_centroids() {
    int changed = 0;
    int x,i,j;
    for ( i=0; i<clusters_num; i++) {
        double* newcentroid;
        newcentroid = (double*) calloc(vector_len,sizeof(double));
        assert(newcentroid != NULL);
        for ( j=0; j<vector_len; j++) {
            newcentroid[j] = cluster_to_centroid(i)[j];
        }
        if (areequal(centroids[i], newcentroid)==0) {
            changed++;
        }
        for ( x=0; x<vector_len; x++) {
            centroids[i][x] = newcentroid[x];
        }
    }
    return (changed!=0);
}


int main(int argc, char *argv[]) {
    int counter;
    int isequal;
    int i,j;
    int max_iter;
    assert(argc == 3 || argc == 2 );
    clusters_num = atoi(argv[1]); 
    assert(clusters_num == 0 || clusters_num > 0);
    max_iter = 200;
    if (argc == 3) {
        max_iter = atoi(argv[2]);
        assert(max_iter >0);
    }
    readfile();
    init_cendroids(clusters_num);
    counter = 0;
    isequal = 1;
    while(counter<max_iter && isequal==1) {
        vector_to_cluster(clusters_num);
        isequal = update_centroids();
        counter++;
    }
    free(vector_list);
    free(clusters);
    free(clustersindexes);
    for ( i=0; i<clusters_num;i++) { 
        for ( j=0; j<vector_len;j++) {
            if (j==vector_len-1) {
                printf("%0.4f \n", centroids[i][j]);
            }
            else {
                printf ("%0.4f,", centroids[i][j]);
            }
        }
    }
    return 0;
}