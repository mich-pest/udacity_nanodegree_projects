## Motion Planning Project

![img](https://user-images.githubusercontent.com/74416077/200648479-b6daf092-980f-45a9-b875-5d8483a928d6.png)


The aim of this project was to define the FSM states and transitions between them required to handle static objects and intersections that are required to STOP for 5 seconds. The implemented behavior is able to handle the following transitions:

* from “lane following” to “deceleration to stop”,
* from “deceleration to stop” to “stay stopped”,
* from “stay stopped” back to “lane following” (when is safe to do so)

At lower level what happens is that the logic picks the optimal trajectory choosing among all the proposed cubic spirals (for which generation code is already provided).
However, depending on the desired behavior, the corresponding goal position is set, along with a settable number of subgoals.
To avoid colliding with obstacles, including other vehicles, a circle-based collision checking algorithm is implemented.

![img](https://user-images.githubusercontent.com/74416077/200633588-87802969-a612-4e06-a08c-8dd0875851ec.png)
