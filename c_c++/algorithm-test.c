#include <stdio.h>

int linear_search(int list[],int listSize,int target)
{
    int targetPosition = -1;
    for (int i = 0;i < listSize;i++){
        if (list[i]==target){
            targetPosition = i;
        }
    }
    return targetPosition;
}

int main(){
    int numList[] = {14,35,26,74,96,89,6,43,2,46,57,86,76,9};
    int size = sizeof(numList)/sizeof(numList[0]);
    int target = 89;
    int resulte= linear_search(numList,size,target);
    if (resulte>=0){
        printf("the position of the number %d in this list is %d",target,resulte+1);
    }else{
        printf("the number %d does not exist in this list",target);
    }


}