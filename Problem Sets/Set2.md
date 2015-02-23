Problem Set 2
=============
**1. There is a one-lane east-west bridge in Hawaii such that when a car is on the bridge going eastbound, no westbound cars are allowed until the eastbound car has left the bridge.  Similarly when a westbound car is on the bridge, no eastbound cars are allowed until the westbound car has left the bridge.  To make matters more complicated, if an eastbound car arrives and sees another eastbound car already on the bridge, then that eastbound car will also proceed onto the bridge.  This is true even if there is a westbound car already waiting to enter the bridge.  Similarly, a westbound car can tailgate behind another westbound car already on the bridge even if an eastbound car was waiting.   Deign a synchronization solution using only locks, semaphores and integer variables that achieves the following: allows all cars bound in a certain direction to continue crossing as long as there is at least one car still on the bridge that is bound in that direction, then toggles to allow all cars in the opposite direction to proceed in a similar manner.  The solution need not be starvation-free.**

---

---
**2. Suppose task T1 has code C1 that must execute before task T2's code C2.  Design a solution that enforces this ordering using only condition variables, locks, and integer variables.  Semaphores and monitors are not allowed.**

---
```
// this is pseudocode
lock mutex_lock;
condition c;
int t1_complete = 0;

// Task T1
Acquire(mutex_lock);
// execute code c1
t1_complete = 1;
Release(mutex_lock);
c.signal();
// End Task T1

// Task T2
Acquire(mutex_lock);
while (!t1_complete) {
	Release(mutex_lock);
	c.wait();
	Acquire(mutex_lock);
}
// execute code c2
Release(mutex_lock);
// End Task T2

```

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
No, the swap() function is not thread-safe for the following reason. Suppose there were two threads (A and B) that are calling the swap() function. If A enters swap() and halts execution after "*y = *z" (either by interruption or a context switch), the consequence is now that half of a swap has occured and the reference to y is now lost. If a context switch occured in the middle of this swap, the value of temp would be overwritten and the value of y in thread A would be lost (it was previously stored in temp), if A were to resume execution (context switched back) it would essentially use the wrong value of temp.

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
Yes, swap() is reentrant since there is no use of a semaphore, monitor, or lock whatsoever. Therefore calling this method would pose no risk of deadlock and would execute normally.

---
**5. Suppose you are asked to design a server application consisting of two processes P1 and P2, such that (1) P2 is to sleep until woken up by P1, whereupon (2) P2 would take a 10 MB file from P1 and compress it. What forms of IPC would be best suited to implement these types of information sharing? Describe your solution.**

---
Memory Sharing IPC would be ideal for this situation since you have a fairly large file (10 MB) that you wish to pass from one process P1 to another process P2. Process P1 could first read the file into main memory, wake process P2 while passing the pointer to memory where the file is stored and wait asynchronously for P2 to complete. Once process P2 wakes, it will synchronize access to that memory and begin to compress the data in that memory location. Once the compression finishes, P2 will release the lock to memory, send an interrupt to P1 to alert that the file has finished compression, and resume sleeping.

---
**6. Suppose processes P0 and P1 share variable V1, and processes P1 and P2 share variable V2, while processes P0, P1 and P2 share V3.  Operations on V1 are limited to increment() and decrement().  Operations on V2 are limited to square() and squareroot().  Operations on V3 are limited to sin() and cos().  Design a monitor-based solution that synchronizes access to and manipulation of these variables between the three processes so that race conditions are eliminated.**

---
```
monitor SomeMonitor {
	condition c1, c2, c3;
	private int v1, v2, v3;

	public function increment() {
		c1.wait();
		v1++;
		c1.signal();
	}

	public function decrement() {
		c1.wait();
		v1--;
		c1.signal();
	}

	public function square() {
		c2.wait();
		v2 *= v2;
		c2.signal();
	}

	public function squareRoot() {
		c2.wait();
		v2 = sqrt(v2);
		c2.signal();
	}

	public function sin() {
		c3.wait();
		v3 = sin(v3);
		c3.signal();
	}

	public function cos() {
		c3.wait();
		v3 = cos(v3);
		c3.signal();
	}
}
```
