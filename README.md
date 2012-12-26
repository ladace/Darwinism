### Server

#### URLs

 * */behav/start* Start the robot, and the robot can do other things. 
 * */behav/rest* Let him rest.
 * */behav/walk?motion=<id>* The parameter is optional. The 'motion' parameter <id> is a number 0~7, specifying how the robot walks, forward or backward, or turning around. TODO change the 'motion' parameter into string.
 * */behav/walkstop* Stop walking.
 * */behav/action?id=<id>* Play an action on the robot. <id> is a number specifying the action page to be played.

 * */walk/set-param?param=<name>&value=<v>* Set the parameter of `WalkingManager`. <v> must be a float number.
 * */walk/get-param?param=<name>* Get the parameter of `WalkingManager`. The response will be a float number.
