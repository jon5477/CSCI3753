1. You are asked to allocate a file according to either a File
Allocation Table (FAT) or multi-level indexed allocation (UNIX
inode - triply indirect). Assume that the file is 2000 disk
blocks long, there are 4 KB per disk block, each pointer in the
FAT occupies 4 bytes, the first index block contains 15 entries
(of which 12 are direct, and one each is singly indirect, doubly
indirect, and triply indirect - see slides or textbook), every
other index block contains 15 entries (may be indirect depending
on the nesting level), each index block entry takes 4 bytes, and
unused index blocks don't count in the total memory cost, though
unusued entries in partially filled index blocks do count.

How many bytes are used to lay out the file when using

a. a FAT file system?
b. a UNIX-style file system?

Now suppose that you wish to read the 1099'th block of the file.
Assume each of the following counts as one search operation:
moving from one element to the next in a linked list; indexing
into an index block; moving from index block to the next. How
many searches are needed to read block 1099 when using

c. the same FAT file system? 
d. the same UNIX-style file system?

2. Suppose you are given a flash memory consisting of 4 KB pages, and
there are 1024 pages. How many bytes of memory would the OS
need to keep track of free space in the worst case (all of flash
memory is free) if

a. a bitmap was used?

1024 bits since all 1024 pages/blocks are free

b. a linked list was used? Assume 2 bytes/pointer.

2048 bytes since each pointer uses 2 bytes and all 1024 pages/blocks are free

Under what conditions would a linked list be more memory-efficient
than a bitmap?

The linked list would be more memory efficient when a lot of disk space has been
allocated (not free) since the linked list allocation does not keep track of allocated blocks.

3. The read/write head of a disk is at track 97, moving towards track
199 (the highest-numbered track on the disk), and the disk request
queue contains read/write requests for sectors on tracks 84, 155,
103, 96, and 197, respectively. Starting from the current head
position, what is the total distance in tracks that the disk arm
moves to satisfy all the pending requests for:

a. FCFS scheduling
97 -> 84 -> 155 -> 103 -> 96 -> 197
13 + 71 + 52 + 7 + 101 = 244 total tracks

b. SCAN scheduling
97 -> 103 -> 155 -> 197 -> 199 -> 96 -> 84
6 + 52 + 42 + 2 + 103 + 12 = 217 total tracks

c. LOOK scheduling
97 -> 103 -> 155 -> 197 -> 96 -> 84
6 + 52 + 42 + 101 + 12 = 213 total tracks

4. Describe the SSH protocol in detail, i.e. describe the initial 
handshaking phase as well as the subsequent data messaging phase.  When a user 
supplies their login password, is it encrypted by a public key or a symmetric key, 
and why?  Explain why or why not SSH is resilient to eavesdropping attacks, 
man-in-the-middle attacks, and/or replay attacks.

