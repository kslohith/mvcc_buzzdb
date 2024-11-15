
# BuzzDB with Multi-Version Concurrency Control (MVCC)

**BuzzDB** is a lightweight database built to support Multi-Version Concurrency Control (MVCC). It efficiently handles concurrent transactions and ensures data consistency through versioned tuple management.

## Supported Concurrency Control Protocols

### 1. MVOCC (Multi-Version Optimistic Concurrency Control)
- Assumes conflicts are rare and only checks for conflicts at commit time.
- If a conflict is detected during commit, the transaction is aborted and restarted.

### 2. MV2PL (Multi-Version Two-Phase Locking)
- Transactions acquire locks on all tuples they read or write, preventing changes by other transactions during the lock period.
- Ensures serializability by preventing conflicting operations from occurring concurrently.
- Eliminates conflict checks at commit time as locks safeguard against concurrent conflicts.

## Running BuzzDB

### Run with MV2PL Concurrency Protocol:
```bash
./buzzdb 2PL
```

### Run with MVOCC Concurrency Protocol:
```bash
./buzzdb OCC
```

## Key Features

### Tuple Metadata
Each tuple includes the following metadata:
- **Creation Timestamp**: Timestamp when the version was created.
- **Expiration Timestamp**: Timestamp when the version was superseded or expired.
- **Previous Version Metadata**: Metadata (page number and slot number) of the previous version, if applicable.

### Version Manager
The **Version Manager** is a centralized structure that:
- Maintains the latest committed version of each tuple.
- Enables traversal through the version chain for historical data.
- Facilitates scan operations to locate the correct tuple version based on transaction timestamps.

### Transactions
A **Transaction** in BuzzDB:
- Represents a set of isolated queries.
- Tracks tuples updated or written during execution.
- Includes a **Commit Method** that:
  - Safely commits changes by acquiring necessary locks.
  - Updates the **Transaction Manager** to reflect committed changes.
  - Makes changes visible to other transactions.

### Transaction Manager
The **Transaction Manager**:
- Tracks active and committed transactions.
- Maintains a mapping of committed transactions and their updates to ensure consistency.

### Locking Mechanism

#### Lock
- Locks prevent concurrent access to specific memory locations.
- In BuzzDB, locks are held on `(pageNumber + slotId)` combinations.

#### Lock Manager
- Manages acquisition and release of locks.
- Maps locks to memory locations for efficient transaction handling.

## Setup and Usage

1. Clone the repository:
   ```bash
   git clone https://github.com/your-username/mvcc_buzzdb.git
   cd mvcc_buzzdb
   ```

2. Compile the code:
   ```bash
   make
   ```

3. Run the database with the desired concurrency protocol:
   ```bash
   ./buzzdb <concurrency_protocol>
   ```

## Overview of Changes Made

### 1. Tuple Metadata Enhancements
- Added creation and expiration timestamps.
- Included metadata for previous tuple versions.

### 2. Version Manager Implementation
- Centralized management of tuple versions.
- Support for version traversal and scans.

### 3. Transaction Improvements
- Enhanced commit process with safe locking and visibility updates.
- Conflict detection for MVOCC at commit.

### 4. Locking Enhancements
- Locks applied to `(pageNumber + slotId)` for fine-grained control.
- Centralized management through **Lock Manager**.

---

