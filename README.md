# Traffic Stop Simulator
Four-way traffic stop automation, built for four Idea7 Tecbot 7.0s

Robots would either make a left turn, right turn, or go straight. These decisions would be made beforehand through button inputs on the LCD. Then, the robots would wait a random amount of time before driving up to the intersection. A line sensor was used to detect when the robot passed the black line. Each of the 3 robots, called the clientbots, would send a signal (when they arrived to the line) to the server, which was hosted by the 4th robot, called the serverbot. Based on which robots arrived first, the serverbot would send a signal and execute the command to cross the intersection one by one. We even added a distance sensor to stop the robot when there is an obstacle in the way and resume motion afterwards. When all three robots cross the intersection, the simulation is finished.

https://user-images.githubusercontent.com/43790515/188505877-14a6787d-9ee1-4a9b-98dd-9bdd2b9a5b6e.mp4

