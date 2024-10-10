# mvcc_buzzdb
BuzzDB with Multi Version Concurrency Control

Two phase locking with mvcc
Have a lock for each tuple inserted into the table and only release the lock from a transaction when it is not in the growth phase ( shrink phase )
Maintain a lock manager which allocates and deallocates the locks to the tuples. 
Key: ( page_number + slot_id ), Status of Lock