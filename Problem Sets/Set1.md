Problem Set 1
=============
**1: Suppose we have a round-robin scheduler in the OS kernel of a preemptively time sliced system, and three programs in its ready queue: P1, P2, and P3. P1 takes 10 seconds to complete, P2 takes 8.5 seconds, and P3 takes 3 seconds. Suppose the time slice is 2 seconds, and the overhead for OS context switching is 0.2 seconds. If P1 starts executing first at time 0, when do each of the programs finish executing? Draw a timeline of execution. What is the percentage overhead due to context switching? Assume that a process that finishes early transfers control back to the OS scheduler.**

---
Execution Timeline:
```
P1        | P1        | P1    | P1    | P1
  | P2        | P2        | P2    | P2    | P2
      | P3        | P3
```
* Total Execution Time: 23.7 seconds
* P3 Finishes: 12.0 seconds
* P1 Finishes: 23.0 seconds
* P2 Finishes: 23.7 seconds
* Context Switches: 11 times
* Total Time Spent on Context Switches: 2.2 seconds
* Context Switch Overhead (Percent): 10%

---
**2: Repeat above for a batch mode multiprogrammed OS. Which system, preemptively multitasked or batch mode multiprogrammed finished execution of all three programs the fastest? Under what conditions might the reverse be true?**

---
Execution Timeline:
```
P1
  | P2
      | P3
```
* Total Execution Time: 
* P1 Finishes: 10 seconds
* P2 Finishes: 18.5 seconds
* P3 Finishes: 21.5 seconds

Batch Mode Multiprogrammed finished first in this instance. The reverse would be true if a program failed to finish executing (i.e either maliciously or if an error occured [bad fault isolation]) or if the CPU is blocked because of an I/O bound job (i.e reading/writing to disk) in which case other programs would be starved of CPU time and unable to make any progress.

---
**3: Explain the differences between the four different kinds of exceptions found in an OS exception table. What is a software interrupt, and how does that differ from a hardware interrupt?**

---
Exceptions:

|Type|Differences|
|----|-----------|
|Trap|Intentional exception used as a "software interrupt" for system calls. Passes control from the application to the kernel.|
|Fault|Exception used for an error that might potentially be recoverable (i.e. page faults, stack overflow, segmentation faults)|
|Interrupt|Exception used as a form of signaling that an I/O task has been completed.|
|Abort|Exception used for serious problems such as hardware failure. There is no recovery here.|

A software interrupt or trap is a way to process system calls in an application. It causes the current application to halt execution, sets the mode bit to kernel bit, and gives control to the kernel to execute the desired system call. Once the system call completes, the mode bit is set back, and control is given back to the application.

A hardware interrupt is a way to let the application know that an I/O task has completed (e.g. read/write from/to disk). It allows the current application to not wait on the operating system to return (i.e. completing a system call) for a potentially long running task (such as an I/O operation) and allows the program to be notified of when that task has been completed in the future (asynchronously). Once a hardware interrupt is received, the CPU interrupts normal execution and invokes the operating system's interrupt handler. Once the handler finishes execution, normal execution resumes.

---
**4: What role does the jump table play in executing a system call?**

---
The trap table contains the memory location (pointer) to the trap handler for the system call in question. The trap handler is then executed and control is given back to the application that called the system call.

---
**5: Explain in what way overlapping I/O with CPU processing is advantageous. Explain two ways that I/O can be overlapped with CPU execution and how they are each an improvement over not overlapping I/O with the CPU.**

---
Overlapping I/O with CPU processing is very advantageous because it allows the CPU to do something else useful rather than idling or polling the I/O device to check if the I/O task has completed, saving a lot of CPU cycles. The two ways to overlap I/O with CPU execution are direct I/O with interrupts and DMA (Direct Memory Access) with interrupts. The use of direct I/O with interrupts allows the transfer of data to commence without the CPU continually polling to check if the I/O has completed, the CPU receives notification of completion through an interrupt. The use of DMA with interrupts allow the transferring of data from an I/O device to memory without interrupting the CPU multiple times. Instead, the CPU is notified once the data transfer fully completes. This substantially improves the performance of large I/O transfers because the CPU is not constantly being interrupted.

---
**6: Describe each step of a write() operation to disk, from the application first calling a write() through the OS processing the write() to the final return from the write() call upon completion of the disk operation. Assume interrupt driven I/O operation. You may draw and label a figure if you'd like. Your answer should include components such as the device controller, interrupt handler, device handler, device driver, and any other OS components you deem appropriate to add.**

---
System Call Steps:

1. Application calls the system call write()
2. Trap/"software interrupt" immediately follows
3. Mode bit is set to kernel bit.
4. Kernel begins to execute the system write driver
5. The write driver obtains the status from the device controller
6. Add to request queue
7. Give control back to application

If device is not busy:

1. Device handler is executed
2. Pull data from device controller
3. Return data to application.

If device is busy:

1. If device is busy, interrupt occurs when it is available
2. Interrupt handler is executed, find out which device raised the interrupt flag.
3. Device handler is executed
4. Pull data from device controller
5. Return data to application.
