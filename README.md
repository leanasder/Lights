Consider an intersection. It has several traffic lights regulating vehicle movement (4 of them) and several regulating pedestrian movement at crosswalks (8 of them):

Pedestrian traffic lights have 2 states, vehicle traffic lights have 3.
Each traffic light has a built-in camera that records the number of vehicles/pedestrians in the queue for which the traffic light is installed.
This is the queue on the opposite side of the pedestrian crossing/intersection (see figure). 
When passing through the intersection, vehicles either go straight or turn right. People and vehicles cross or pass through the intersection one by one, reducing the size of the corresponding queue by 1.
Each traffic light has a unique ID. Traffic lights can communicate using events, sending events to each other by ID.
The events sent are containers with data (for example, they may contain the number of people/vehicles in the queue, the sender's ID, the current state of the traffic light).
A traffic light can set a timer that, after a specified time, sends a specified event to a specified ID. 
Sending an event means placing the container into the event queue for the traffic light; each traffic light has its own queue.
Traffic lights process events in parallel, independently of each other. At the same time, each traffic light processes its events sequentially, in the order they are placed in the queue.
A traffic light can obtain information about the current state of any other traffic light synchronously (not via an event).
Task: devise and describe an adaptive algorithm for the operation of traffic lights to optimize the overall throughput of the intersection depending on the situation at the intersection.


