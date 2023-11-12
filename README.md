# Asteroids  
Keybindings:  
-W = Thrust  
-A = Rotate Left  
-D = Rotate Right  
-Space = Shoot (Restart when Paused)  
-M = Hyperspace (Teleport)  
-Esc = Pause/Unpause (Exit Game upon Win/Lose)  

Tried to recreate an exact replica of the classic "Asteroids" arcade game in 6 days.  

Inaccuracies/Missing (to my knowledge):  
-Audio (wasn't interested in doing that during that timeframe)  
-Life counter is replaced with a number rather than ship icons  
-UI font isn't accurate  
-Added extra functionality such as pause, restart   
-Game is limited to 10 waves (can be easily changed if needed)  
  
Moreover, I split everything into "Engine" (Rendering) and "Game" (Gameplay) and tried to reduce as much dependencies as possible between the two.  
I used the HelperData.h as a centralized point with Data used by both "Engine" and "Game", due to the reason mentioned above.

Various online sources were used to create the "Engine" as this was my first time experimenting with rendering and the "Game" was done entirely by me.

Disclaimer: 
This project should be considered as a quick and dirty game jam rather then a reflection of my programming skills as it was produced in a very limited timeframe with little to no time for code structure and optimizations. 
