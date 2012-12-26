### Server

#### URLs

 * `/behav/rest` Let him rest.
 * `/behav/walk?motion=<id>` The parameter is optional. The 'motion' parameter &lt;id&gt; is a number 0~7, specifying how the robot walks, forward or backward, or turning around. **TODO** change the 'motion' parameter into string.
 * `/behav/stop-walk` Stop walking.
 * `/behav/action?id=<id>` Play an action on the robot. &lt;id&gt; is a number specifying the action page to be played.
 * `/walk/set-param?param=<name>&value=<v>` Set the parameter of `WalkingManager`. &lt;v&gt; must be a float number.
 * `/walk/get-param?param=<name>` Get the parameter of `WalkingManager`. The response will be a float number. &lt;param&gt; is a string in the following list:
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
 * `/walk/start?id=<id>` Let him walk according to the parameter set specified by &lt;id&gt;.
 * `/walk/stop` The same as `/behav/stop-walk`.
 * `/walk/load-parset?id=<id>` Load the parameter set &lt;id&gt; on the robot. If &lt;id&gt; is not given, it will reload the current parameter set. The parameter values will be responsed, in a JSON array.
 * `/walk/save-parset?id=<id>` Save current parameters to the parameter set &lt;id&gt;. If &lt;id&gt; is not given, they will be saved into the current set.
 * `/walk/save-new-parset` Save current parameters to a new parameter set. The new parameter set will be pushed at the end.
 * `/walk/del-parset?id=<id>` Delete the parameter set &lt;id&gt;. If &lt;id&gt; is not given, the current parameter set will be deleted.
 * `/walk/get-cur-parset` Get the values of all the parameters. The reponse will be a JSON array of floats.
 * `/walk/get-cur-parset-norm` Get the values and normalized values of all the parameters. The reponse will be a JSON array of two arrays, which are values and normalized values, respectively.
 * `/walk/get-parset-n` Get how many parameter sets there are. The response will be a integer.
 * `/walk/get-par-minmax` Get the min and max values of all the parameters. The reponse will be a JSON array of two arrays, which are min values and max values respectively.
