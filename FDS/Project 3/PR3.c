#include <stdio.h>
#include <stdlib.h>
#define INFTY 50000 //much bigger than the length of the sum of the paths

typedef struct Graph{
    int num; //to save the numbers of vertices
    int* name; //old map to new map
    int* index; //new map to old map
    int** matrix; //adjacent matrix
}graph;

typedef struct Table{
    int* dist; //to save the distance from source
    int* state;  //to save the state that whether a vertex has found its shortest path
}table;

typedef struct Path{
    int state; //whether the number of the shortest ways to the destination has been cnfirmed
    int ways; //the number of the shortest ways to the destination
    int times;  //times appearing on the shortest paths from source to destination
}path;

graph* init_graph(int vert_num); //init a graph with vert_num vertices
void free_graph(graph* map); //free the space of the graph

table* init_table(int ver_num); //init a table with ver_num vertices
void free_table(table* T); //free the space of the table

int** init_matrix(int length); //init an int matrix length times length
void free_matrix(int** matrix,int length); //free the matrix

void update_path(graph* map,int path_num); //to update the length of the path in adjacent matrix

graph* simplify_map(graph* map,table* T,int des); //simplify the graph
int find_Tmin(table* T,int num); //find the vertex which has smallest diistance and hasn't been confirmed 

table* Dijkstra(graph* map,int src); //Dijkstra Algorithm

void find_mindist(graph* map,int** T_matrix); 
//to update the matrix that save the shortest distance between any two vertices 

int count_ways(graph* map,int** T_matrix,path** count,int src,int des,int dist);
//count the number of shortest paths to the destination 
void count_times(graph* map,int** T_matrix,path** count,int src,int des,int dist);
//count the times appearing on the shortest paths from source to destination
void find_tph(graph* map,int src,int des,int threshold);
//find eligible transportation hubs

int main(){
    int city_num,path_num,threshold; //def the variables
    int source,destination,pair_num;
    int i,j;
    scanf("%d %d %d",&city_num,&path_num,&threshold); //read-in
    graph* map=init_graph(city_num); //init graph
    update_path(map,path_num); //update graph
    scanf("%d",&pair_num);
    for (i=0;i<pair_num;i++){
        scanf("%d %d",&source,&destination); //for each pair, find transportation hubs 
        find_tph(map,source,destination,threshold);
    }
    free_graph(map); //free the map
}

graph* init_graph(int vert_num){
    graph* new_graph=(graph*)malloc(sizeof(graph));
    if (!new_graph){
        printf("The initialization failed.");
        exit(-1);
    }
    int i;
    new_graph->num=vert_num;
    new_graph->name=(int *)malloc(sizeof(int)*vert_num);
    new_graph->index=(int *)malloc(sizeof(int)*vert_num);
    for (i=0;i<vert_num;i++){
        new_graph->name[i]=i; //default: old map and new map are as the same
        new_graph->index[i]=i;
    }
    new_graph->matrix=init_matrix(vert_num); //init the adjacent matrix
    return new_graph;
}

void free_graph(graph* map){
    free_matrix(map->matrix,map->num);
    free(map->name);
    free(map->index);
    free(map);
    return;
}

table* init_table(int ver_num){
    int i;
    table* T=(table*)malloc(sizeof(table));
    T->dist=(int*)malloc(sizeof(int)*ver_num);
    T->state=(int*)malloc(sizeof(int)*ver_num);
    for (i=0;i<ver_num;i++){
        T->dist[i]=INFTY; //default: All is infty
        T->state[i]=0; //default:hasn't been checked
    }
    return T;
}

void free_table(table* T){
    free(T->dist);
    free(T->state);
    free(T);
}

int** init_matrix(int length){
    int i,j;
    int** matrix=(int**)malloc(sizeof(int*)*length);
    for (i=0;i<length;i++){
        matrix[i]=(int*)malloc(sizeof(int)*length);
        for (j=0;j<length;j++){
            if (i==j){ //default: to itself is 0, to others is infty
                matrix[i][j]=0; 
            }
            else{
                matrix[i][j]=INFTY;
            }
        }
    }
    return matrix;
}

void free_matrix(int** matrix,int length){
    int i;
    for (i=0;i<length;i++){
        free(matrix[i]);
    }
    free(matrix);
}

path** init_path(int num){
    int i;
    path** new_path=(path**)malloc(sizeof(path*)*num);
    for (i=0;i<num;i++){
        new_path[i]=(path*)malloc(sizeof(path));
        new_path[i]->state=0; //default:hasn't been confirmed, both ways and times are 0
        new_path[i]->ways=0;
        new_path[i]->times=0;
    }
    return new_path;
}

void free_path(path** tar_path,int num){
    int i;
    for (i=0;i<num;i++){
        free(tar_path[i]);
    }
    free(tar_path);
}

void update_path(graph* map,int path_num){
    int i;
    int src,des,length;
    for (i=0;i<path_num;i++){
        scanf("%d %d %d",&src,&des,&length); //read in the length from src to des
        map->matrix[src][des]=length; //update the distance between src & des
        map->matrix[des][src]=length;
    }
    return;
}

graph* simplify_map(graph* map,table* T,int des){
    int i,j,new_num=0;
    int* new_name=(int*)malloc(map->num*sizeof(int));
    int* new_index=(int*)malloc(map->num*sizeof(int));
    //ready to build the new grapgh
    for (i=0;i<map->num;i++){
        if (T->dist[i]>=T->dist[des]&&i!=des){
            T->state[i]=0;
        }
        if (T->state[i]){
            new_name[i]=new_num; //i-th vertex in name:old[i]->new[new_num]
            new_index[new_num]=i; //Reversely,new[new_num]->old[i]
            new_num++; //count the number of vertices in the new graph
        }
    }
    graph* new_map=init_graph(new_num); //make a new graph,avoid destructing the old graph
    free(new_map->index); //to clear the default case
    free(new_map->name);
    new_map->name=new_name; //to save the correct case
    new_map->index=new_index;
    for (i=0;i<new_num;i++){
        for (j=0;j<new_num;j++){
            //copy the effective path from the old to the new
            new_map->matrix[i][j]=map->matrix[new_index[i]][new_index[j]];
            new_map->matrix[j][i]=map->matrix[new_index[j]][new_index[i]];
        }
    }
    return new_map;
}


int find_Tmin(table* T,int num){
    int i;
    int dist_min=INFTY;
    int index_min=-1;
    for (i=0;i<num;i++){
        //scan the table to find the one having the samllest distance
        if (T->dist[i]<dist_min&&T->state[i]==0){ //ensure that the vertex hasn't been checked
            index_min=i;
            dist_min=T->dist[i];
        }
    }
    return index_min;
}

table* Dijkstra(graph* map,int src){
    int i,u,v;
    table* T=init_table(map->num); //init a table to save the result
    T->dist[src]=0;
    for (i=0;i<map->num;i++){
        u=find_Tmin(T,map->num);
        if (u==-1){ //means there are only infty remains, none of them can be connected
            printf("The map is not connected.Results are as followed.\n");
            return T; //just return because it can't continue anymore, it is exactly the result
        }
        T->state[u]=1;
        for (v=0;v<map->num;v++){ //traverse all the nodes
            if ((map->matrix[u][v]<INFTY)&&(T->state[v]==0)){ //u,v are connected & v hasn't been confirmed
                if (T->dist[u]+map->matrix[u][v]<T->dist[v]){ //find a shorter path
                    T->dist[v]=T->dist[u]+map->matrix[u][v]; //update a shorter path
                }
            }
        }
    }
    return T;
}

void find_mindist(graph* map,int** T_matrix){
    int i,j;
    table* T=NULL;
    for (i=0;i<map->num;i++){ //traverse all vertices
        T=Dijkstra(map,i); //set i to be the source, find the shortest path to the left each
        for (j=0;j<map->num;j++){
            T_matrix[i][j]=T->dist[j]; //update the result in to the shortest path matrix
        }
        free_table(T);
    }
}

int count_ways(graph* map,int** T_matrix,path** count,int src,int des,int dist){
    if (src==des){ //find the destination->possible paths +1
        return 1;
    }
    int i;
    for (i=0;i<map->num;i++){
        if (map->matrix[src][i]!=INFTY&&i!=src&&count[src]->state==0){
            //only when src and i are adjacent and i!=src and src hasn't been checked,we update it
            if (T_matrix[i][des]+map->matrix[src][i]==dist){
                //lemma 2, it shows i must be on one of the shortest paths form src to des
                count[src]->ways+=count_ways(map,T_matrix,count,i,des,dist-map->matrix[src][i]);
                //lemma 3,recursively calculate the number of shortest paths to des
            }
        }
    }
    count[src]->state=1; //after checked ,update the state
    return count[src]->ways; //when it is called, return the ways
}

void count_times(graph* map,int** T_matrix,path** count,int src,int des,int dist){
    if (src==des){
        return; //meet the des means exit the recursion
    }
    int i;
    for (i=0;i<map->num;i++){
        if (map->matrix[src][i]!=INFTY&&i!=src){ //the same condition
            if (T_matrix[i][des]+map->matrix[src][i]==dist){
                count_times(map,T_matrix,count,i,des,dist-map->matrix[src][i]);
                //recursively call the adjacent vertex
            }
        }
    }
    count[src]->times+=count[src]->ways;
    //when one vertex is called, the times should plus its ways.Explanation can be seen in pdf
    return;
}

void find_tph(graph* map,int src,int des,int threshold){
    int i,flag=1;
    table* T=Dijkstra(map,src);
    graph* sf_map=simplify_map(map,T,des);
    free_table(T);
    path** count=init_path(sf_map->num);
    int** T_matrix=init_matrix(sf_map->num);
    find_mindist(sf_map,T_matrix);
    count[sf_map->name[src]]->ways=count_ways(sf_map,T_matrix,count,sf_map->name[src],sf_map->name[des],T_matrix[sf_map->name[src]][sf_map->name[des]]);
    count_times(sf_map,T_matrix,count,sf_map->name[src],sf_map->name[des],T_matrix[sf_map->name[src]][sf_map->name[des]]);
    for (i=0;i<sf_map->num;i++){
        //find the eligible transportation hubs
        if (count[i]->times>=threshold&&sf_map->index[i]!=src&&sf_map->index[i]!=des){
            if (flag){
                printf("%d",sf_map->index[i]);
                flag=0;
            }
            else{
                printf(" %d",sf_map->index[i]);
            }
        }
    }
    if (flag){
        printf("None\n");
    }
    else{
        printf("\n");
    }
    free_matrix(T_matrix,sf_map->num); //free the space
    free_path(count,sf_map->num);
    free_graph(sf_map);
}