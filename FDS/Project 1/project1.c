#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int cmp_int(const void*a,const void*b);     //The comparing function provided for function "qsort()"

long* initial(long* a,long length,long sup);    //To generate the original random collection

long random_long(long sup);     //This function can produce an integer in [1,V] regardless of RAND_MAX

double test(long N,long V,long times,void (*solve)(long*,long,long,long));      //Test each solves and return the time costs

void solve_1(long* collection,long N,long V,long target);
void solve_2(long* collection,long N,long V,long target);

int main(){
    srand((unsigned int)time(NULL));    //initialize for function "rand()"
    long N=100000;   //sizeof the collection
    long V=100;   //upper bound  is of the numbers in collection
    long K=1000;     //times that the codes will run for
    double duration;    // tp save the time that the project run K times costs
    duration=test(N,V*N,K,solve_2);
    printf("%lf %lf\n",duration,duration/K);
}
    
int cmp_int(const void*a,const void*b){
    return *((int *)a)-*((int *)b);
}

long random_long(long sup){     //random number is generated digit by digit,sup is the upper bound 
    long rdm=0;
    long mul=1;
    while(sup>10){      //to guarantee the number of digits of the random number is less than sup
        rdm=rdm+rand()%10*mul;
        mul=mul*10;
        sup=sup/10;
    }
    rdm=rdm+rand()%(sup)*mul;   //to guarantee the highest digit of rdm is less than sup
    return rdm+1;   //"+1"to keep the returned number positive
}

long *initial(long *a,long length,long sup){    //generate N randomly one by one
    long i=0;
    while (i<length){
        a[i++]=random_long(sup);
    }
    return a;
}

double test(long N,long V,long times,void (*solve)(long*,long,long,long)){
    clock_t start,stop;     //initial the clock module
    double duration;    //save the total time
    long k=0,target;    //k is just for traversing,while target is the "c" in the problem
    long *collection=(long *)malloc(sizeof(long)*N);    //distribute the space for collection
    start=clock();  //start to count the time 
    for (k=0;k<times;k++){
        initial(collection,N,V);    //every test uses a new random collection 
        target=random_long(2*V);    //every test uses a new target
        solve(collection,N,V,target);     //choose a solve to test
    }
    stop=clock();   //stop to count the time
    duration=((double)(stop-start))/CLK_TCK; 
    free(collection);
    return duration;
}

void solve_1(long* collection,long N,long V,long target){
    long i,j,temp;      //i,j are indexes, while temp saves temporary results
    i=0;j=N-1;  //i starts from zero and move form left to right and J move inversely 
    qsort(collection,N,sizeof(long),cmp_int);   
        //"qsort()" is a built-in function declared in <stdlib.h>
        //It uses quick sort to sort the collection
        //It guarantees that the collection is non-descending
    while ((temp=collection[i]+collection[j])!=target&&i<=j){
        if (temp<target)
            i++;    //if temp is less than target, we move i to make it bigger
        else
            j--;    //else if temp is bigger than target, we move j to make it smaller
    }
    if (temp==target){
        printf("%d + %d = %d\n",collection[i],collection[j],target);    //find the result,print
    }
    else{
        printf("None\n");   //didn't find,print None
    }
}

void solve_2(long* collection,long N,long V,long target){
    char *hash=(char *)calloc(V,sizeof(char));  //Set a hash chart,and initializing 0
    long i;    //just for traversing
    for (i=0;i<N;i++){
        if (!hash[collection[i]-1]){    //for repetitive item we skip
            hash[collection[i]-1]='1';    //if n appears in collection,hash[n-1] will turn from 0 to 1
            if (target>collection[i]&&(target-collection[i]<=V)){   //to avoid segmentation fault
                if (hash[target-collection[i]-1]){
                    //it means the number it needs to produce target has appeared,namely we find them
                    printf("%d + %d = %d\n",collection[i],target-collection[i],target);
                    return;     //leave this test
                }
            }
        }
    }
    printf("None\n");   //if we didn't find it,print None
    free(hash);
}
