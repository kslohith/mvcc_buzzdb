# BuzzDB with Multi-Version Concurrency Control (MVCC)

**BuzzDB** is a lightweight database built to support Multi-Version Concurrency Control (MVCC), allowing for efficient handling of concurrent transactions and ensuring data consistency through versioned tuple management.

Optimistic concurrency control is implemented by assuming that conflicts will not occur during transaction updates. Conflicts are only checked at commit time; if any are detected, the transaction is aborted and restarted.

ToDo: MV2PL protocol for concurrency control

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
    ./buzzdb