import math
import random

flips = []
should_close : bool = False

def coinFlip():
    if(random.randint(0,1) == 0):
            return "head"
    else:
        return "tail"

def probability(arr):
    probability : float= 0
    head_count :int  = 0 
    n :int = len(arr)
    tails_count :int
    for index in range(n):
        if arr[index] == "head" :
            head_count += 1
    tails_count = n-head_count
    probability = ((math.factorial(n)/(math.factorial(head_count)*math.factorial(tails_count)))/math.pow(2,n))*100
    print(f"the chance of getting {head_count } heads and {tails_count} tails is %{"%.2f"%probability}")
 



while(not should_close):
    buffer :str = input("do you want to flip a coin [Y\\n]")
    if (buffer=="Y" or buffer == "y") :
        result :str = coinFlip()
        flips.append(result)
        print(f"you got {result}")
    elif buffer == "N" or buffer == "n":
        should_close = not should_close


probability(flips)

