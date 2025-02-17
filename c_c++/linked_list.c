#include <stdio.h>
#include <stdlib.h>
typedef struct nodeElement{
    int x;
    struct nodeElement *nextElement;
}nodeElement;
nodeElement makeNewNode(){
    int random = rand()%10; 
    return (nodeElement){1,NULL};
}
void insertNode(nodeElement* first,nodeElement* new){
    if(first->nextElement == NULL){
        first->nextElement = new;
    }else{
        insertNode(first->nextElement,new);
    }
}
void insertInBetween(nodeElement* first,nodeElement*new,int p){
    static int depth =1;
    if(depth == p){
        new->nextElement = first->nextElement;
        first->nextElement = new;
        new->x = p+1;
    }else if(depth <p){
        depth++;
        insertInBetween(first->nextElement,new,p);

    }else{
        printf("something went wrong");
    }
}
void printNodes(nodeElement* node){
    static int position = 1;
    printf("this is element number : %d with a value of :%d\n",position,node->x);
    if(node->nextElement == NULL){
        return;
    }else{
        position++;
        printNodes(node->nextElement);
    }
}
int main (){
    nodeElement first = makeNewNode();
    nodeElement second = makeNewNode();
    nodeElement third = makeNewNode();
    nodeElement fourth = makeNewNode();
    nodeElement inbetween = makeNewNode();
    insertNode(&first,&second);
    insertNode(&first,&third);
    insertNode(&first,&fourth);
    insertInBetween(&first,&inbetween,3);
    printNodes(&first);


   return 0;
}