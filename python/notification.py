from win10toast import ToastNotifier
import socket
from time import sleep

toaster = ToastNotifier()
def testConection():
    try:
        socket.create_connection(("Google.com",80))
        return True
    except OSError:
        return False
    


previus_status:bool = True
curent_status:bool = testConection()
running:bool = True

def updateStatus():
    global previus_status
    global curent_status
    previus_status = curent_status
    curent_status = testConection()
while(running ):
    if(previus_status == False and curent_status == True):
        toaster.show_toast("reminder","if you have any video you want to download do it now the internet might cut off again or you might forget to later",icon_path= None,duration=5)
    updateStatus()
    sleep(2)