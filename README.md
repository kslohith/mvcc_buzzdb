# BuzzDB with Multi-Version Concurrency Control (MVCC)

**BuzzDB** is a lightweight database built to support Multi-Version Concurrency Control (MVCC), allowing for efficient handling of concurrent transactions and ensuring data consistency through versioned tuple management.

Currently 2 concurrency control protocols are supported, MVOCC AND MV2PL.

MVOCC: Optimistic concurrency control is implemented by assuming that conflicts will not occur during transaction updates. Conflicts are only checked at commit time; if any are detected, the transaction is aborted and restarted.

MV2PL: In MV2PL, a transaction acquires locks on all the tuples it reads or writes. This ensures that other transactions cannot make changes to these tuples while the locks are held. By acquiring locks, MV2PL ensures that transactions are serializable, meaning they can be ordered in a way that produces the same result as if they were executed one after the other. This eliminates the need for conflict checks at commit time because the locks prevent conflicting operations from occurring concurrently.

### For running BuzzDB using the MV2PL Concurrency Protocol:
```bash
   ./buzzdb 2PL
### For running BuzzDB using the MVOCC Concurrency Protocol
```bash
   ./buzzdb OCC

## Overview of Changes Made

### Tuple Metadata
In addition to the key and value, each tuple now contains:
- **Creation Timestamp**: When the version was created.
- **Expiration Timestamp**: When the version was expired (or superseded by a newer version).
- **Previous Version Metadata**: Information (page number and slot number) about the prior version of the tuple, if one exists.

### Version Manager
The **Version Manager** is a centralized data structure that:
- Maintains the latest committed version of each tuple, acting as the main access point for update and read queries.
- Stores metadata about each tuple's previous version, enabling traversal through the version chain.
- Supports scan operations that begin from the Version Manager, traversing through the tuple chain to find the version suited for the current transaction. This is done by checking if the transaction ID falls between the tuple’s creation and expiration timestamps.

### Transactions
A **Transaction** in BuzzDB:
- Represents a set of queries isolated from other concurrent transactions (a core MVCC feature).
- Maintains a list of tuples that the transaction has updated or written, allowing for conflict detection during the commit process.
- Includes a **Commit Method** which:
  - Acquires a lock for safe commit execution.
  - Once committed, makes all changes by the current transaction visible to other transactions.
  - Adds the commit timestamp and updates to the **Transaction Manager**.

### Transaction Manager
The **Transaction Manager**:
- Tracks active and committed transactions in the system.
- Maintains a map of committed transactions and their updates to prevent inconsistent writes or data corruption from concurrent transactions.

### Lock
A **Lock** in BuzzDB:
- Prevents concurrent access to a particular memory location. Since we deal with tuples in buzzdb, the lock is held on (pageNumber + slotId)

### Lock Manager
The **Lock Manager**:
- Manages the locks which are acquired and released for the database.
- Maps a Lock object to a memory location.

## How to Run
1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/mvcc_buzzdb.git
   cd mvcc_buzzdb
2. Compile the code:
    ```bash
    make
3. Run the Code
    ```bash
    ./buzzdb <concurrency_protocol>


