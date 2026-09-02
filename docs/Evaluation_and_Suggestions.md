# 🧪 CGPA Calculator — Test Scenarios, Quality Evaluation & Feature Recommendations

This document provides a comprehensive evaluation of **Task 1: CGPA Calculator (`CGPA-Calculator.cpp`)**, including test scenarios executed, program quality grading, and architectural suggestions to make the project stand out for CodeAlpha evaluation.

---

## 🔬 1. Test Scenarios & Runtime Verification Results

### Scenario 1: Standard 1-Semester Calculation
- **Input Data**: 1 Semester, 4 Courses (3 CH each: Discrete Structures `A`, Linear Algebra `B+`, Calculus `A-`, English `B`).
- **Expected Total CH**: 12.0 CH
- **Expected GPA**: $3.50 / 4.00$
- **Actual Output**: `12.00 CH`, `3.50 GPA`, `Magna Cum Laude`
- **Result**: ✅ **PASSED**

---

### Scenario 2: Previous CGPA Fast Entry Mode
- **Input Data**: 5 Semesters total (Mode 1), Previous CGPA `3.20` @ `64 CH`, Semester 5 with 4 subjects (including two 4 CH split courses: Software Arch & Web Eng).
- **Expected Semester CH**: 14.0 CH @ $3.61$ GPA
- **Expected Overall Cumulative CH**: $64 + 14 = 78.0$ CH
- **Expected Cumulative CGPA**: $\frac{204.80 + 50.60}{78.0} = \mathbf{3.27}$
- **Actual Output**: `78.00 CH`, `3.27 CGPA`, `Cum Laude`
- **Result**: ✅ **PASSED**

---

### Scenario 3: COMSATS 12.0 - 21.0 CH Rule Violation Guard
- **Input Data**: User attempts to enter only 1 course of 3 CH in Semester 1 (Total CH = 3.0).
- **Expected Behavior**: Reject semester, trigger COMSATS Rule Violation warning (`Semesters 1-8 require 12.0 to 21.0 CH`), and prompt re-entry.
- **Actual Output**: Displayed warning message and safely allowed re-entry without crashing.
- **Result**: ✅ **PASSED**

---

### Scenario 4: Smart Unified Grade/Marks Auto-Detection
- **Input Data**: User inputs numeric percentage `87.5`, lowercase letter `a-`, uppercase `B+`, and invalid text `XYZ`.
- **Expected Behavior**: Automatically converts `87.5` to `A-`, converts `a-` to `A-` (3.70 GPA), accepts `B+`, and rejects `XYZ` with a clean error prompt.
- **Actual Output**: Handled string streams smoothly, sanitized case, and rejected invalid entries cleanly.
- **Result**: ✅ **PASSED**

---

### Scenario 5: COMSATS Post 8th Semester Credit Limit Removal Policy
- **Input Data**: Semester 9 (Post 8th Semester) with only 2 subjects totaling `7.0 CH` (below the 12.0 CH threshold).
- **Expected Behavior**: Enforces 12-21 CH limits only on Semesters 1-8. For Semester 9+, removes credit limit restrictions entirely, enabling flexible repeat/extra semester course registration.
- **Actual Output**: Displayed `[COMSATS Policy]: Semester 9 (Post 8th Semester) — Credit Hour limit REMOVED (Flexible CH).` and successfully calculated final CGPA (`3.12`) without triggering any rule error.
- **Result**: ✅ **PASSED**

---

### Scenario 6: Full Entry vs. Fast Entry Comparative Consistency Test (8 Semesters)
- **Objective**: Verify if entering 8 semesters step-by-step (Full Entry) produces the **exact same final CGPA and cumulative credit hours** as taking the first 7 semesters' cumulative CGPA and entering Semester 8 (Fast Entry).
- **Test Execution**:
  1. **Full Entry (8 Semesters)**:
     - Semesters 1–7 Total Credits: `93.00 CH`, Cumulative Quality Points: `335.70` ($\implies \text{CGPA} = 3.61$).
     - Semester 8 (13.0 CH @ 3.77 GPA): `49.00 Quality Points`.
     - **Full Entry Final CGPA**: $\frac{335.70 + 49.00}{93.00 + 13.00} = \frac{384.70}{106.00} = \mathbf{3.63\text{ / 4.00}}$ (`Magna Cum Laude`).
  2. **Fast Entry (Previous CGPA + Semester 8)**:
     - Previous CGPA: `3.61` @ `93.00 CH` ($\implies \text{Previous Points} = 335.73$).
     - Semester 8 (13.0 CH @ 3.77 GPA): `49.00 Quality Points`.
     - **Fast Entry Final CGPA**: $\frac{335.73 + 49.00}{106.00} = \frac{384.73}{106.00} = \mathbf{3.63\text{ / 4.00}}$ (`Magna Cum Laude`).
- **Comparative Result**: Both modes produced **106.00 Total Cumulative Credits**, **3.63 CGPA**, and identical `Magna Cum Laude` Academic Standing!
- **Result**: ✅ **PASSED (100% Mathematical Consistency)**

---

## 🏆 2. Program Quality Grading Scorecard

| Evaluation Criterion | Score | Assessment Notes |
|---|---|---|
| **Functional Correctness** | **10 / 10** | Accurate GPA/CGPA calculations, zero-division protection, COMSATS scale precision. |
| **User Experience (UX/UI)** | **9.5 / 10** | Smart auto-detection (marks vs letter grades), automatic 4 CH Theory/Lab split, fast CGPA mode. |
| **Robustness & Validation** | **10 / 10** | Complete stream sanitization (`std::cin` buffer clearing), numeric bound checks, non-empty text enforcement. |
| **Code Architecture & OOP** | **9.5 / 10** | Modular C++17 design (`Course`, `Semester`, `GradeScaleManager`, `InputValidator`, `CGPACalculatorSystem`). |
| **Persistence & Reporting** | **10 / 10** | Tabular ASCII formatting, Academic Standing honors roll, automatic `CGPA_Transcript.txt` file export. |
| **OVERALL GRADE** | **A+ (98 / 100)** | **Exceptional Quality & Industry-Ready Implementation** |

---

## 🚀 3. Recommendations & Stand-Out Feature Suggestions

To make this project stand out even further for the CodeAlpha internship submission and potential placement support, consider adding the following features:

### 1. 💾 JSON / CSV File Persistence & Reload
- **Feature**: Allow saving student profiles to `student_data.json` or `.csv` and loading them back on application start so users don't have to re-enter past semesters.

### 2. 🎯 Target CGPA Simulator ("What-If" Analysis)
- **Feature**: Add a feature asking: *"What CGPA do you want to achieve by graduation?"*
- **Logic**: Calculates the exact required GPA per remaining semester to reach the target CGPA.

### 3. 🎨 GUI / Web Dashboard Extension
- **Feature**: Build a lightweight GUI front-end using **SFML**, **Raylib**, or **Qt** (or a web wrapper using WebAssembly / HTML5-CSS) to display interactive grade distribution charts and GPA progress bars.

### 4. 📄 PDF Transcript Export
- **Feature**: Integrate a basic PDF report generator or HTML-formatted transcript template printable directly from a web browser.
