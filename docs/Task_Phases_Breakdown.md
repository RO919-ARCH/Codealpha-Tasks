# CodeAlpha C++ Tasks — 4-Phase Development Roadmap

This document divides each of the 4 CodeAlpha C++ internship tasks into **4 distinct execution phases**:
1. **Phase 1: Requirements Analysis & Design**
2. **Phase 2: Core Implementation & Algorithm Development**
3. **Phase 3: Data Persistence, Verification & Error Handling**
4. **Phase 4: Optimization, UI/UX Polish & Project Delivery**

---

## 📌 TASK 1: CGPA Calculator

### Phase 1: Requirements Analysis & Architecture Design
- Define data structures to represent individual courses (Course Name/Code, Grade, Grade Points, Credit Hours).
- Map letter grades (A, B, C, D, F) or percentage inputs to standard 4.0/5.0 grading scale numerical values.
- Design console UI layout for dynamic user inputs and outputs.

### Phase 2: Core Algorithm & Grade Computation
- Implement interactive dynamic input loop for `$N$` number of courses.
- Calculate Total Grade Points: $\sum (\text{Grade Points} \times \text{Credit Hours})$.
- Calculate Total Credits: $\sum (\text{Credit Hours})$.
- Compute Semester GPA: $\frac{\text{Total Grade Points}}{\text{Total Credits}}$.
- Implement multi-semester CGPA aggregation logic.

### Phase 3: Data Validation & Input Sanitization
- Validate numeric bounds for credits (non-negative, non-zero).
- Sanitize letter grade / numeric grade inputs to reject invalid entries.
- Guard against zero-division errors when total credit hours equal zero.

### Phase 4: Formatting, Reporting & Delivery
- Format tabular output display displaying Course, Credits, Letter Grade, and Earned Points.
- Display clean overall CGPA report with performance classification (Honors, Distinction, Pass).
- Prepare GitHub repository `CodeAlpha_CGPACalculator` with clean `README.md` and build script.

---

## 📌 TASK 2: Login and Registration System

### Phase 1: Security & File System Design
- Define credential model (Username, Hashing/Salting mechanism, User ID).
- Choose storage backend (Flat text file, CSV format, or binary structure).
- Design CLI state machine (Menu: 1. Register, 2. Login, 3. Exit).

### Phase 2: Core Authentication Logic
- **Registration Function**: Accept user credentials, check string length and complexity rules.
- **Duplicate Check**: Scan persistence storage to ensure unique usernames.
- **Login Function**: Read credentials from file and perform hash/plain lookup to verify identity.

### Phase 3: Input Validation & Edge Case Handling
- Handle missing database/flat file gracefully by auto-creating missing files.
- Reject spaces, special characters, or empty input strings where inappropriate.
- Display explicit error responses (e.g., "Username taken", "Invalid password", "User not found").

### Phase 4: Encryption, UI Enhancements & Documentation
- Implement basic password obfuscation or cryptographic hashing (SHA-256 / XOR mask).
- Hide password typing characters in CLI prompt (masked standard input).
- Setup repository `CodeAlpha_LoginSystem` with setup instructions.

---

## 📌 TASK 3: Sudoku Solver

### Phase 1: Matrix Representation & Rule Engine
- Represent $9 \times 9$ board using a 2D array (`int grid[9][9]`).
- Build constraint checking functions:
  - `isValidRow(grid, row, num)`
  - `isValidCol(grid, col, num)`
  - `isValidBox(grid, startRow, startCol, num)`
  - `isValidMove(grid, row, col, num)`

### Phase 2: Backtracking Solver Algorithm Implementation
- Write recursive function `solveSudoku(grid)`.
- Find empty cells marked with `0`.
- Iterate through candidate numbers $1..9$, checking validity before placement.
- Implement recursive step and backtrack (reset cell to `0`) upon hitting a dead-end.

### Phase 3: Matrix I/O & Custom Puzzle Input
- Implement custom puzzle loader from console or text file.
- Validate initial board setup to prevent pre-existing rule violations.
- Verify solution correctness upon solver completion.

### Phase 4: Visualization, GUI (Optional) & Benchmarking
- Build visually appealing ASCII matrix renderer with grid borders.
- Optional: Implement SFML, Raylib, or Qt GUI interface for animated solving process.
- Create `CodeAlpha_SudokuSolver` repository with sample test puzzles.

---

## 📌 TASK 4: Banking System

### Phase 1: Object-Oriented Architecture Design
- Define class models and relationships:
  - `Customer`: Name, Customer ID, Contact Info.
  - `Account`: Account Number, Balance, Customer Link.
  - `Transaction`: Transaction ID, Type (Deposit/Withdrawal/Transfer), Amount, Timestamp.
- Use encapsulation, getters/setters, and OOP best practices.

### Phase 2: Core Banking Operations
- Account creation and customer onboarding.
- `deposit(amount)`: Update balance and append transaction record.
- `withdraw(amount)`: Validate available funds and deduce balance.
- `transfer(targetAccount, amount)`: Atomic cross-account balance transfer.

### Phase 3: Transaction History & Persistence
- Store transaction logs in dynamic arrays/vectors per account.
- Persist system state (accounts, balances, transaction logs) to file system (`fstream`).
- Reload state accurately when re-launching application.

### Phase 4: UI/UX & Account Statements
- Build rich interactive CLI menu system.
- Format detailed passbook/account statements displaying timestamped transaction histories.
- Build `CodeAlpha_BankingSystem` repository with comprehensive user guides.
