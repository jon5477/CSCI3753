Problem Set 2
=============
**1. There is a one-lane east-west bridge in Hawaii such that when a car is on the bridge going eastbound, no westbound cars are allowed until the eastbound car has left the bridge.  Similarly when a westbound car is on the bridge, no eastbound cars are allowed until the westbound car has left the bridge.  To make matters more complicated, if an eastbound car arrives and sees another eastbound car already on the bridge, then that eastbound car will also proceed onto the bridge.  This is true even if there is a westbound car already waiting to enter the bridge.  Similarly, a westbound car can tailgate behind another westbound car already on the bridge even if an eastbound car was waiting.   Deign a synchronization solution using only locks, semaphores and integer variables that achieves the following: allows all cars bound in a certain direction to continue crossing as long as there is at least one car still on the bridge that is bound in that direction, then toggles to allow all cars in the opposite direction to proceed in a similar manner.  The solution need not be starvation-free.**

---

---
**2. Suppose task T1 has code C1 that must execute before task T2's code C2.  Design a solution that enforces this ordering using only condition variables, locks, and integer variables.  Semaphores and monitors are not allowed.**

---

---
**3. Explain why the solution to the 3rd Readers/Writers problem is starvation-free.**

---

---
**4. a. Is the swap() function below thread-safe or not?  Explain your reasoning.**

```
int temp;
 
void swap(int *y, int *z)
{
    int local;
 
    local = temp;
    temp = *y;
    *y = *z;
    *z = temp;
    temp = local;

}
```

---

---
**b. Suppose the swap() function above is called by the interrupt service routine (ISR) below.  Assume that if swap() is interrupted during normal execution, the ISR below is called and calls swap() again, reentering swap() and executing in the context of the interrupted thread.  Is swap() reentrant?  Explain your reasoning.**

```
void interrupt_service_routine()
{
    int a = 1, b = 2;
    swap(&a, &b);
}
```

---

---
**5. Suppose you are asked to design a server application consisting of two processes P1 and P2, such that (1) P2 is to sleep until woken up by P1, whereupon (2) P2 would take a 10 MB file from P1 and compress it. What forms of IPC would be best suited to implement these types of information sharing? Describe your solution.**

---

---
**6. Suppose processes P0 and P1 share variable V1, and processes P1 and P2 share variable V2, while processes P0, P1 and P2 share V3.  Operations on V1 are limited to increment() and decrement().  Operations on V2 are limited to square() and squareroot().  Operations on V3 are limited to sin() and cos().  Design a monitor-based solution that synchronizes access to and manipulation of these variables between the three processes so that race conditions are eliminated.**

---