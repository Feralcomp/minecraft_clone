Possible Optimalizations:

Instancing Static Meshes (implemented)

Using Instanced Static Meshes, they are cheaper in terms of frametime cost, but a caveat is that you cannot interact with them as you can with actors or such
solution to that is : upon interaction with a "static mesh component instance" remove the instance and spawn an actor representing the desired look/behavior and interact with this actor

multithreading (partially implemented)

by default majority of the code would run within the GameThread
offloading calculations into another thread can greatly improve performance, 
unreal has a threaded for loop which can be used , but since this is outside of gamethread we cannot  modify/create objects 
also make sure we do not modify array elements outside of game thread
solution is using thread lock within unreal which enables us to ensure that only a single thread at a time can perform an action 
(such as adding new item to an array), this should ensure that the function is threadsafe

we can safely generate the world data in a separate thread and handle assigning that data to correct sub objects

Optimising use of Arrays (implemented, but can be improved)

Generally operations performed on arrays take exponentially longer the larger the array is
splitting data across multiple smaller arrays yields significant performance boost when dealing with large arrays