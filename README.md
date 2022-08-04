# Asteroids  
Keybindings:  
-W = Thrust  
-A = Rotate Left  
-D = Rotate Right  
-Space = Shoot (Restart when Paused)  
-M = Hyperspace (Teleport)  
-Esc = Pause/Unpause (Exit Game upon Win/Lose)  

Tried to recreate an exact replica of the classic "Asteroids" arcade game in less than 7 days.  
  
Inaccuracies/Missing (in my knowledge):  
-Audio (wasn't interested in doing that)  
-Life counter is replaced with a Number rather than Ship Icons  
-UI Font isn't accurate  
-Added extra functionality such as Pause, Restart   
-Game is limited to 10 waves (can be easily changed if needed)  
  
Moreover, I split everything into "Engine" (Rendering) and "Game" (Gameplay) and tried to reduce as much dependencies as possible between the two.  
I used the HelperData.h as a centralized point with Data used by both "Engine" and "Game", due to the reason mentioned above.  
