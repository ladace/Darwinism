### Server

#### URLs

 * `/behav/start` Start the robot, and the robot can do other things. 
 * `/behav/rest` Let him rest.
 * `/behav/walk?motion=<id>` The parameter is optional. The 'motion' parameter &lt;id&gt; is a number 0~7, specifying how the robot walks, forward or backward, or turning around. TODO change the 'motion' parameter into string.
 * `/behav/walkstop` Stop walking.
 * `/behav/action?id=<id>` Play an action on the robot. &lt;id&gt; is a number specifying the action page to be played.

 * `/walk/set-param?param=<name>&value=<v>` Set the parameter of `WalkingManager`. &lt;v&gt; must be a float number.
 * `/walk/get-param?param=<name>` Get the parameter of `WalkingManager`. The response will be a float number.
   &lt;param&gt; is a string in the following list.
     - `X-OFFSET`
     - `Y-OFFSET`
     - `Z-OFFSET`
     - `ROLL-OFFSET`
     - `PITCH-OFFSET`
     - `YAW-OFFSET`
     - `HIP-PITCH-OFFSET`
     - `PERIOD-TIME`
     - `DSP-RATIO`
     - `STEP-FORWARDBACK-RATIO`
     - `STEP-FORWARDBACK`
     - `STEP-RIGHTLEFT`
     - `STEP-DIRECTION`
     - `FOOT-HEIGHT`
     - `SWING-RIGHTLEFT`
     - `SWING-TOPDOWN`
     - `PELVIS-OFFSET`
     - `ARM-SWING-GAIN`
     - `PARAMETER-NUM`
