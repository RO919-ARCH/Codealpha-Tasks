# CodeAlpha Task 1: CGPA Calculator (COMSATS University Rules)

![Language](https://img.shields.io/badge/Language-C%2B%2B17-blue.svg)
![Status](https://img.shields.io/badge/Status-Complete-brightgreen.svg)
![Domain](https://img.shields.io/badge/CodeAlpha-Task%201-orange.svg)

## 📌 Project Overview
The **CGPA Calculator** is a C++ application designed according to **COMSATS University Academic Rules** and **CodeAlpha Internship Requirements**. It evaluates student academic performance across multiple semesters, processes course grades, handles 4 CH theory/lab section splits, manages FYP phases, enforces credit hour limits, and generates formatted transcripts both on the console and exported to file.

---

## ✨ Key Features & Academic Rules

- 🖥️ **Modern Desktop GUI (Qt5)**: Sleek dark-mode desktop interface built using Qt5 Widgets with interactive tab navigation, real-time table rendering, and custom CSS styling (`CGPA-Calculator-GUI`).
- 📊 **COMSATS Grading Scale**: Automatic conversion from percentage marks (0-100%) or direct letter grades to GPA points & academic remarks.
- 🎯 **Target CGPA Simulator ("What-If" Analysis)**: Interactive tab simulating exact required future GPAs to hit a target graduating CGPA.
- 🎓 **Academic Standing Honor Roll**: Classifies student CGPA into honors (`Summa Cum Laude`, `Magna Cum Laude`, `Cum Laude`, `Pass`, `Probation`).
- ⏱️ **Semesters 1-8 Credit Hour Enforcement**: Enforces a strict minimum of **12.0 CH** and maximum of **21.0 CH** per semester (exempt for post 8th semesters).
- 🧪 **Automatic Course Section Split**: 4 Credit Hour subjects are split into 3 CH Theory + 1 CH Lab.
- 💾 **JSON & CSV Profile Persistence**: Saves and loads student profiles automatically (`student_record.json` & `student_record.csv`).
- 📄 **File Export Persistence**: Saves clean official transcript reports to `CGPA_Transcript.txt`.

---

## 📁 Directory Structure

```text
CGPA-Calculator_Task_1/
├── bin/                       # Compiled binaries (CGPA-Calculator, CGPA-Calculator-GUI)
├── build/                     # Temporary build artifacts (Qt MOC files)
├── data/                      # Profile persistence & exported transcripts (.json, .csv, .txt)
├── docs/                      # Technical documentation & task guidelines
│   └── README_CGPA_Calculator.md
├── src/                       # Source code directory
│   ├── cli/
│   │   └── CGPA-Calculator.cpp
│   └── gui/
│       └── main_gui.cpp
├── Makefile                   # Unified project Makefile
└── README.md                  # Project documentation
```

---

## 🛠️ Build & Run Instructions

### ⚡ Quick Build (Using Makefile)

```bash
# Build both CLI and GUI applications
make all

# Run CLI application
./bin/CGPA-Calculator

# Run GUI application
./bin/CGPA-Calculator-GUI

# Clean build artifacts
make clean
```

### 🔨 Manual Compilation Commands

#### 1. Compile Desktop GUI Application (Qt5)
```bash
moc src/gui/main_gui.cpp -o build/main_gui.moc
g++ -std=c++17 -Wall -Wextra -fPIC -Ibuild $(pkg-config --cflags Qt5Widgets Qt5Core Qt5Gui) src/gui/main_gui.cpp $(pkg-config --libs Qt5Widgets Qt5Core Qt5Gui) -o bin/CGPA-Calculator-GUI
./bin/CGPA-Calculator-GUI
```

#### 2. Compile CLI Console Application (C++17)
```bash
g++ -std=c++17 -Wall -Wextra src/cli/CGPA-Calculator.cpp -o bin/CGPA-Calculator
./bin/CGPA-Calculator
```

---

## 📄 Output Sample

```text
=========================================================================
                 COMSATS ACADEMIC TRANSCRIPT REPORT                      
=========================================================================
 Student Name      : Taha Tariq
 Registration ID   : FA21-BCS-088
 Academic Standing : Magna Cum Laude (High Distinction / Dean's List)
=========================================================================

[SEMESTER 1 DETAILS]
-------------------------------------------------------------------------
Course / Section Name             Credits   Grade     GPA Pts   Remark
-------------------------------------------------------------------------
English Composition               3.00      A-        3.70      Excellent
Calculus & Analytic Geometry      3.00      A         4.00      Superior/Excellent
Programming Fundamentals (Theory) 3.00      A-        3.70      Excellent
Programming Fundamentals (Lab)    1.00      B+        3.30      Very Good
-------------------------------------------------------------------------
 Semester Total Credits: 10.00 CH
 Semester GPA          : 3.73 / 4.00

=========================================================================
 OVERALL CUMULATIVE CREDITS: 16.00 CH
 OVERALL CUMULATIVE CGPA   : 3.52 / 4.00
 ACADEMIC STANDING         : Magna Cum Laude (High Distinction / Dean's List)
=========================================================================
```
