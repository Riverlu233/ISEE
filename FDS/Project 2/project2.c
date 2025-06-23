#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Declaration of the used structs
typedef struct Tree{ //Binary-Tree
    int state;  //indicate the type of the node:0-opperand,1-variant,2-constant
    int bracket_state; //to show whether it needs brackets when output
    struct Tree* left;
    struct Tree* right;
    char value[4]; //The space used to store values, whether they are operators, constants, or variables. 
}tree;

typedef struct Stack{
    tree** content; //the stack stores the pointer of tree node
    int top;
    int capacity; //the capacity of the stack
}stack;

typedef struct Set{ //an array to store all the variables
    char **variants;  //place to save the pointers of the variables
    int capacity;      
    int count;      //number of the variables in the set
}set;

//All the operations of stacks that needed
stack* init_stack(void);
void   push_stack(stack* target_stack,tree* new_node);
tree*  pop_stack(stack* target_stack);
int    empty_stack(stack* target_stack);
tree*  check_stack(stack* target_stack);

//functions that build the tree
tree* expression(char end); //"end" is the letter that indicates the end of input
tree* init_variant(char*);
tree* init_opperand(char);
void state_tree(tree* node);
int get_priority(char op);

//functions used to collect the variables
void collect_variant(tree* node, set* set_var);
void add_variable(set* set_var,char* var);
set* init_set();

//function uesd to get the derivative
tree* derivative(tree* root,char *target_variant);
tree* derivative_plus(tree* root,char *target_variant);
tree* derivative_minus(tree* root,char *target_variant);
tree* derivative_multi(tree* root,char *target_variant);
tree* derivative_div(tree* root,char *target_variant);
tree* derivative_pow(tree* root,char *target_variant);

//function used to simplify the tree
void simplify_tree(tree*node);
void simplify_const(tree* node);
void simplify_zero(tree* node);
void simplify_identity(tree* node);

//some auxiliary functions
int int_pow(int a,int b); //to avoid using pow in math.h
void convert_zero(tree* root);
void convert_one(tree* node);
void replace(tree* node1,tree* node2);
void test_print_post(tree* leaf);

//Functions used to output
void test_print_in(tree* leaf);
void update_bracket(tree* node);

int main(){
    int i;
    tree* result;
    tree* express=expression('\0'); //Init the expression tree
    state_tree(express); //update the type of every node
    set* set_variant=init_set(); //Init a new set to save the variables
    collect_variant(express,set_variant); //Collect the new variables
    for (i=0;i<set_variant->count;i++){
        result=derivative(express,set_variant->variants[i]); //find the derivative
        state_tree(result); //renew the type of every node
        simplify_tree(result); //simplify the expression
        update_bracket(result); //find the nodes that needs brackets
        printf("%s : ",set_variant->variants[i]);
        test_print_in(result); //print the in-order expression
        printf("\n");
    }
}

stack* init_stack(){ //to establish a new stack
    stack* new_stack=(stack*)malloc(sizeof(stack));
    new_stack->top=0;
    new_stack->capacity=4; //The initial capacity is set to 4
    new_stack->content=(tree**)calloc(new_stack->capacity,sizeof(tree*));
    return new_stack;
}

void push_stack(stack* target_stack,tree* new_node){ //push new_node into stack
    if (target_stack==NULL){
        printf("The to-push stack doesn't exist.\n");
        exit(-1);
    }
    else{
        if (target_stack->top==target_stack->capacity){ //to expand the capacity if it is full
            target_stack->capacity*=2;
            tree** tmp=(tree**)realloc(target_stack->content,sizeof(tree*)*target_stack->capacity);
            if (tmp){
                target_stack->content=tmp;
            }
            else{
                printf("Failed to realloc the space.\n");
                exit(-1);
            }
        }
        target_stack->content[target_stack->top]=new_node;
        target_stack->top++;
        return;
    }
}

tree* pop_stack(stack* target_stack){ //pop out the top node
    if (target_stack==NULL){
        printf("The to-pop stack  doesn't exist.\n");
        exit(-1);
    }
    else{
        if (target_stack->top==0){
            printf("The stack is empty.\n");
            exit(-1);
        }
        else{
            target_stack->top--;
            return target_stack->content[target_stack->top];
        }
    }
}

int empty_stack(stack* target_stack){ //to check whether the stack is empty
    if (target_stack==NULL){
        printf("The to-judge-empty stack doesn't exist.\n");
        exit(-1);
    }
    else{
        return target_stack->top==0 ? 1 : 0 ;
    }
}

tree* check_stack(stack* target_stack){ //to check the element that at the top of stack
    if (target_stack==NULL){
        printf("The to-check stack doesn't exist.\n");
        exit(-1);
    }
    else{
        if (target_stack->top){
            return target_stack->content[target_stack->top-1];
        }
        else{
            printf("The to-check stack is empty.\n");
            exit(-1);
        }
    }
}

void state_tree(tree* node){
    int i=0;
    if (node){
        node->bracket_state=0; //First initialize all to 0,we will update it by "update_bracket"
        if (node->state){
            node->state=2; //First assume they are all constant
            while (node->value[i]!='\0'&&i<4){
                if (node->value[i]>='a'&&node->value[i]<='z'||node->value[i]>='A'&&node->value[i]<='Z'){
                    //if one of the constant is a letter,it should be a variable,eg'6a4'is variable
                    node->state=1;
                    break;
                }
                i++;
            }
        }
        state_tree(node->left); //update the type of the tree nodes recursively
        state_tree(node->right);
    }
    return;
}

void simplify_tree(tree* node){ 
    if (node){
        simplify_tree(node->left); //simplify the children node first
        simplify_tree(node->right); //so that we can avoid repetitive work
        
        simplify_const(node); //simplify rule 1
        simplify_zero(node);    //simplify rule 2
        simplify_identity(node);    //simplify rule 3
    }
    else{
        return;
    }
}

void simplify_const(tree* node){ //rule 1:if all children are constant, merge by operators.
    if (node->state==0){
        int a,b,result=0;
        char op;
        if (node->left->state+node->right->state==4) { //to judge all children are constant
            a=atoi(node->left->value); //change left->children->value to int number
            b=atoi(node->right->value); //likewise
            char op=node->value[0]; //get the operators
            switch(op) {
                case '+': result=a+b; break; //process according to op
                case '-': result=a-b; break;
                case '*': result=a*b; break;
                case '/': result=a/b; break;
                case '^': result=int_pow(a, b); break;
            }
            node->state = 2; //now the new node is a constant
            sprintf(node->value, "%d", result); //change the int result to char* and copy it to value 
            node->left=NULL; //now a constant don't have children node
            node->right=NULL;
        }
    }
    return;
}

void simplify_zero(tree* node){ //rule 2:operate with zero element returns zero element
    if (node->state==0){
        char* left_value=node->left->value; //get the left&right children's value.
        char* right_value=node->right->value;
        char op=node->value[0]; //get the operator
        switch(op) {
            case '-':
                if (strcmp(left_value,right_value)==0&&node->left->state!=0&&node->right->state!=0) 
                    convert_zero(node); //special case: a-a=0
                    break;
            case '*':
                if (strcmp(left_value,"0")==0||strcmp(right_value,"0")==0)
                    convert_zero(node); //zero element of * is 0
                break;
            case '/':
                if (strcmp(left_value,"0")==0) 
                    convert_zero(node); //left-zero-element of / is 0
                else if(strcmp(left_value,right_value)==0&&node->left->state!=0&&node->right->state!=0)
                    convert_one(node); //special case:a/a=1
                break;    
            case '^':
                if (strcmp(left_value,"0")==0)
                    convert_zero(node); // special case:0^a=0
                else if (strcmp(right_value,"0")==0||strcmp(left_value,"1")==0)
                    convert_one(node); //zero-element of ^ is 1 and special case:1^a=1
                break;
        }
    }
    return;
}

void simplify_identity(tree* node){ //rule 3:operate with unit-element is identtity
    if (node->state==0){
        char* left_value=node->left->value; //get the left&right children's value.
        char* right_value=node->right->value;
        char op=node->value[0]; //get the operator
        switch(op) {
            case '+':
                if (strcmp(left_value,"0")==0)
                    replace(node,node->right); //unit-element of + is 0
                else if (strcmp(right_value,"0")==0)
                    replace(node,node->left);
                break;
            case '-':
                if (strcmp(right_value,"0")==0) //right-unit-element of - is 0 
                    replace(node,node->left);
                break;
            case '*':
                if (strcmp(left_value,"1")==0)
                    replace(node, node->right); //unit element of * is 1
                else if (strcmp(right_value,"1")==0)
                    replace(node, node->left);
                break;
            case '/':
                if (strcmp(right_value,"1")==0) //right-unit-element of / is 1
                    replace(node, node->left);
                break;
            case '^':
                if (strcmp(right_value,"1")==0) //right-unit-element of ^ is 1
                    replace(node, node->left);
                break;
        }
    }
    return;
}

void convert_zero(tree* root){ //change a node to constant 0
    root->left=NULL;
    root->right=NULL;
    strcpy(root->value,"0");
    root->state=2;
}

void convert_one(tree* node){ //change a node to constant 1
    node->left=NULL;
    node->right=NULL;
    node->state=2;
    strcpy(node->value,"1");
}

void replace(tree* node1,tree* node2){ //replace node 1 by node 2
    node1->left=node2->left;
    node1->right=node2->right;
    node1->state=node2->state;
    strcpy(node1->value,node2->value);
}

int int_pow(int a,int b){ //a function to return int a^b
    int i,mul=1;        //avoid the floating calculate of pow() in <math.h>
    for (i=0;i<b;i++){
        mul*=a;
    }
    return mul;
}

void test_print_in(tree* leaf){ //output the expression
    if (leaf){ //when meet function "ln()",we process specially
        if (leaf->state==0&&leaf->value[0]=='l'){
            printf("ln(%s)",leaf->right->value);
        }
        else{
            if (leaf->bracket_state){ //if it need a bracket,before printing its left side,print a (
                printf("(");
            }
            test_print_in(leaf->left); //as the operator are binoncular,we print left element first
            printf("%s",leaf->value); //i.e. print in-order traversal
            test_print_in(leaf->right);
            if (leaf->bracket_state){ //if it need a bracket,before printing its left side,print a )
                printf(")");
            }
        }
    }
    return;
}
void test_print_post(tree* leaf){ //a function just for test, using to print post-order traversal
    if (leaf){
        test_print_post(leaf->left);
        test_print_post(leaf->right);
        printf("%s %d,",leaf->value,leaf->bracket_state);
    }
    return;
}

void update_bracket(tree* node){ //to judge whether a operator needs bracket
    if (!node) return;
    if (node->state==0){
        if (node->left->state==0||node->right->state==0){ //if one of the children is operator
            if (node->left->state==0&&(get_priority(node->left->value[0])<get_priority(node->value[0]))){
                //if priority of thr left operator is lower ,then it needs brackets.
                node->left->bracket_state=1;
            }
            if (node->right->state==0&&(get_priority(node->right->value[0])<get_priority(node->value[0]))){
                //if priority of thr right operator is lower ,then it needs brackets.
                node->right->bracket_state=1;
            }
        }
        update_bracket(node->left); //update the state recursively
        update_bracket(node->right);
    }
    return;
}

int get_priority(char op) { //return the priority of the operator
    switch(op) {
        case '+':
        case '-': return 1; //prior 1:+ & -
        case '*':
        case '/': return 2; //prior 2:* & /
        case '^': return 3; //prior 3:^
        default:  return 0; //not an operator
    }
}

tree* expression(char end){ //end is the letter that indicate the end of input
    int i=0; //index for temp
    char ch='0'; //to store the letter that read in
    char temp[5]; //a temporary space for those long variables.
    tree* temp_tree; //store a tree* variable temporarily
    stack* stack_variant=init_stack(); //init a stack to store the variables
    stack* stack_opperand=init_stack(); //init a stack to store the operators
    while ((ch=getchar())!='\n'&&ch!=end){ //if the input didn't end
        if (get_priority(ch)){ //if the readin letter is an operator
            if (i!=0){ 
                //Sometimes when we readin an operator,the former variable maybe didn't exist.
                //eg a*(b-1)/c,when / is read in,there's no former varaible
                //to avoid a empty temp is turned to node ,we judge whether i==0
                temp[i]='\0';i=0; //when temp is not empty,we can turn it to a new node
                tree* new_variant=init_variant(temp); 
                //right now we don't now the type,just know it is an operator
                push_stack(stack_variant,new_variant);
                //push the new node into the stack
            }
            tree* new_opperand=init_opperand(ch); //turn the new operator into a operator type node
            //as you can see,all the operators will be update their type here,so "state_tree" will skip them
            if (empty_stack(stack_opperand)){ //if the operator stack is empty,just push it in
                push_stack(stack_opperand,new_opperand);
            }
            else{
                //if the stack is not empty
                if (get_priority(ch)<=get_priority(check_stack(stack_opperand)->value[0])){
                    /*When the current operator has a lower priority than the top-of-stack-op operator or both have
                    equal priority, the top-of-stack-op operator is popped until the top-of-stack-op operator has a 
                    lower priority than the current operator or the stack-op is empty.*/
                    while (!empty_stack(stack_opperand)&&get_priority(ch)<=get_priority(check_stack(stack_opperand)->value[0])){
                        temp_tree=pop_stack(stack_opperand);
                        temp_tree->right=pop_stack(stack_variant);
                        temp_tree->left=pop_stack(stack_variant);
                        push_stack(stack_variant,temp_tree);
                    }
                }
                //righr now push the new operator into stack
                push_stack(stack_opperand,new_opperand);
            }
        }
        else if (ch=='('){
            /*When left bracket is encountered, do expression tree construction for the contents up to
            the right bracket and push it to the stack-var.*/
            push_stack(stack_variant,expression(')')); //to control it stop at the )
        }
        else{
            //nothing happened,it is a longer variable
            temp[i++]=ch;
        }
    }
    //when the read-in finished,all things should be popped out
    if (i!=0){
        //samely,if temp isn't empty,then it stores the last variable,we should push it into the stack 
        temp[i]='\0';
        tree* new_variant=init_variant(temp);
        push_stack(stack_variant,new_variant);
    }
    while (!empty_stack(stack_opperand)){ //now we should empty all the stacks
        /*When an operator goes off the stack-op, the variable stack-var pop out one element as its
        right subtree and one more element as its left subtree, and then push the new tree into the
        stack-var.*/
        temp_tree=pop_stack(stack_opperand);
        temp_tree->right=pop_stack(stack_variant);
        temp_tree->left=pop_stack(stack_variant);
        push_stack(stack_variant,temp_tree);
    }
    return pop_stack(stack_variant);
    //finally only the pointer of the root will remain in the stack_variable
}

tree* init_variant(char *variant){ //init a new node with variant/constant,here we don't distinguish them
    tree* leaf=(tree*)malloc(sizeof(tree));
    leaf->state=1; //just assume it is a variable
    leaf->left=NULL;
    leaf->right=NULL;
    strcpy(leaf->value,variant);
    return leaf;
}

tree* init_opperand(char op){ //init a new node with operator
    tree* leaf=(tree*)malloc(sizeof(tree));
    leaf->state=0; //it must be an operator
    leaf->left=NULL;
    leaf->right=NULL;
    leaf->value[0]=op;
    leaf->value[1]='\0';
    return leaf;
}

set* init_set(){ //init the set to save the variables
    set* set_var=malloc(sizeof(set));
    set_var->capacity=8; //initial capacity is 8
    set_var->count=0; //init it to be empty
    set_var->variants=malloc(sizeof(char*)*set_var->capacity); //init the char* pointers array
    return set_var;
}

void add_variable(set* set_var,char* var){ //to add new variables into the set
    int i;
    for (i=0;i<set_var->count;i++) { //if the variable has appeared,no need to continue
        if (strcmp(set_var->variants[i],var)==0) {
            return;
        }
    }
    //if the variable hasn't appeared before
    if (set_var->count>=set_var->capacity) { //if it is full,expand
        set_var->capacity*=2;
        set_var->variants=realloc(set_var->variants,sizeof(char*)*set_var->capacity);
    }
    set_var->variants[set_var->count]=(char*)malloc(sizeof(char)*4); //allocate a space to store the new
    strcpy(set_var->variants[set_var->count],var);//copy the new variable into it
    set_var->count++;
}

void collect_variant(tree* node, set* set_var) { //collect the variables in the expression
    if (!node){ 
        return;
    }
    else if (node->state == 1) { //if it is a variable,check whether it need to be add
        add_variable(set_var, node->value);
    }
    collect_variant(node->left, set_var); //collect the variabls recursively
    collect_variant(node->right, set_var);
}


tree* derivative(tree* root,char *target_variant){ //derivative the expression
    tree* new=NULL;
    if (!root->state){ //if it is a operator
        char op = root->value[0];
        switch(op){
            //case the op to derivative them
            case '+': new=derivative_plus(root,target_variant);break;
            case '-': new=derivative_minus(root,target_variant);break;
            case '*': new=derivative_multi(root,target_variant);break;
            case '/': new=derivative_div(root,target_variant);break;
            case '^': new=derivative_pow(root,target_variant);break;
            default: break;
        }
    }
    else{
        if (strcmp(root->value,target_variant)==0){
            //the derivative of target_variant=1
            new=init_variant("1");
            new->state=2;
        }
        else{
            //the derivative of non-target_variant or constant is 0
            new=init_variant("0");
            new->state=2;
        }
    }
    return new;
}

tree* derivative_plus(tree* root,char *target_variant){
    //+,-: Derive both children
    tree* new=init_opperand('+');
    new->left=derivative(root->left,target_variant);
    new->right=derivative(root->right,target_variant);
    return new;
}

tree* derivative_minus(tree* root,char *target_variant){
    //+,-: Derive both children
    tree* new=init_opperand('-');
    new->left=derivative(root->left,target_variant);
    new->right=derivative(root->right,target_variant);
    return new;
}

tree* derivative_multi(tree*root,char *target_variant){
    //*: Apply product rule (f'g + fg')
    tree* new=init_opperand('+');
    //Initialize left side
    new->left=init_opperand('*');
        new->left->left=derivative(root->left,target_variant);
        new->left->right=root->right;
    //Initialize right side
    new->right=init_opperand('*');
        new->right->left=root->left;
        new->right->right=derivative(root->right,target_variant);
    return new;
}

tree* derivative_div(tree *root,char *target_variant){
    //Apply quotient rule [(f'g - fg')/g²]
    tree* new=init_opperand('-');
    //Initialize left side
    new->left=init_opperand('/');
        //Init left->left,namely deri(divident)*divisor
        new->left->left=init_opperand('*');
            new->left->left->left=derivative(root->left,target_variant);
            new->left->left->right=root->right;
        //Init left->right,namely divisor^2
        new->left->right=init_opperand('^');
            new->left->right->left=root->right;
            new->left->right->right=init_variant("2");
    //Initialize right side
    new->right=init_opperand('/');
        //Init right->left,namely divident*deri(divisor)
        new->right->left=init_opperand('*');
            new->right->left->left=root->left;
            new->right->left->right=derivative(root->right,target_variant);
        //Init right->right,same as left->left
        new->right->right=init_opperand('^');
            new->right->right->left=root->right;
            new->right->right->right=init_variant("2");
    return new;
}

tree* derivative_pow(tree* root,char *target_variant){
    //Apply power rule [f^g*(g'lnf + gf'/f)]
    tree* new=init_opperand('*');
    new->left=root;
    new->right=init_opperand('+');
        new->right->left=init_opperand('*');
        new->right->right=init_opperand('*');
            new->right->left->left=derivative(root->right,target_variant);
            new->right->left->right=init_opperand('l');//to "l"
            new->right->left->right->state=0;
                new->right->left->right->left=init_variant("e");
                new->right->left->right->right=root->left;
            new->right->right->left=root->right;
            new->right->right->right=init_opperand('/');
                new->right->right->right->left=derivative(root->left,target_variant);
                new->right->right->right->right=root->left;
    return new;
}