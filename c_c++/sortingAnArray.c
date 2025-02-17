#include <stdio.h>
#include <stdbool.h>


bool isSorted(int arr[], int n) {
    for (int i = 0; i <= n - 1; i++) {

        if (arr[i] > arr[i + 1]) {
            return false;
        }
    }
    return true;
}
void writeTheArray (int arr[],int length){
    printf("[");
    for (int i = 0; i <= length-1 ;i++){
        printf(" %d ",arr[i]);
    }
    printf("]");
}
void sort(int array[],int length){
    bool arrayIsSorted = false;
            int placeholder;
        for(int i =0 ;i<length-1;i++ ){
            for(int j = 0 ; j< length-i-1;j++)
            {
                if (array[j] >array[j+1])
                {
                    placeholder = array[j];
                    array[j] = array[j+1];
                    array[j+1] = placeholder;
                }
            }
        }
}
int main(){
    int rand[] = {4,15,6,23,63,29,69,34,50,78,67,13};
    int araylength = sizeof(rand)/sizeof(rand[0]);
    sort(rand,araylength);
    writeTheArray(rand,araylength);



    return 0 ;
};

