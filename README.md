# YsIIScenaTool
Basic tool to edit the ys2_libre.ys2 file from Ys II Chronicles.

I'm not 100% sure it works at the moment. Using empty dialogs softlocks the game but it seems to run fine if there is a text to display.
It's possible there are pointers within the script code (outside the pointers sections), I'll try to add them whenever I find them (extremely simple).  
Also no idea how the font works in that game so a custom encoding might need to be added.

To use it just drag and drop the ys2_libre.ys2 file onto the exe to produce a .csv. Fill out the 3rd column with your translation, then run the following command:  
  
YsIIScenaTool.exe \<path to the csv containing the translations\> \<path to the original ys2_libre.ys2 file\>
 
Thanks to Darkmet for the decryption function that I shamelessly stole.


