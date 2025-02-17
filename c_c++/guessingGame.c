#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int check(int answer,int tries){
    int a = answer ;
    int t = tries;
    int numberOfGuesses;
    int g;
    printf("\nEnter your guess please:");
    scanf("%d",&g);
    if (t == 0){
        printf("You lost :( You ran out of tires");
        printf("\nthe correct guess was :%d",a);
        return 0 ;
    }
    if(g<a){
        printf("\nYour guess is too low! try again");
        numberOfGuesses++;
        t--;
        check(a,t);
    }else if (g>a) {
        printf("\nYour guess is too high!try again");
        numberOfGuesses++;
        t--;
        check(a,t);
    }else{
        numberOfGuesses++;
        printf("\nCongrasulation!!! you won");
        printf("\nYou got it in :%d guesses",numberOfGuesses);
        return 0 ;
    }
}

int main (){
    srand(time(0));
    int tries = 5;
    int answer = (rand()%100) + 1; 
    printf("Wellcome to todays game");
    printf("\nYour task is to guess a number in the range between 1 - 100");
    printf("\nYou will have 5 tries to try and guess the correct answer");
    printf("\nYou would be informed if your guess is higher or lower then the correct answer acordingly");
    printf("\nlets start the game ");
    check(answer,tries);
    return 0;
}

