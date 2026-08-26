/**
 * @file CGPA-Calculator.cpp
 * @brief CodeAlpha Task 1: CGPA Calculator (COMSATS University Rules - Fully
 * Featured with Target CGPA Simulator)
 * @details Feature-packed academic CGPA calculation system supporting Target
 * CGPA "What-If" Analysis, JSON/CSV profile persistence, auto grade/marks
 * detection, and previous CGPA shortcut mode.
 *
 * Features:
 * - Target CGPA "What-If" Simulator: Calculates exact required GPA per
 * remaining semester to reach a target graduating CGPA.
 * - JSON & CSV Persistence: Automatically saves & loads student profiles
 * (`student_record.json` & `student_record.csv`).
 * - Previous CGPA Shortcut Mode: Fast calculation using previous cumulative
 * CGPA + credits.
 * - Dual Grade/Marks Unified Input: Smart auto-detection of numeric percentage
 * (0-100) vs Capital Letter Grade (A, A-, B+, etc.).
 * - Automatic Subject Handling: Ask for Course Name & Credit Hours directly. 4
 * CH courses split into 3 CH Theory + 1 CH Lab.
 * - COMSATS Semesters 1-8 Rules: Enforces 12.0 to 21.0 Total CH per semester
 * (exempt for post 8th semesters).
 * - Export Academic Transcript: Saves detailed transcript report to
 * `CGPA_Transcript.txt`.
 */

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * @struct GradeInfo
 * @brief Stores details of a grade scale entry (Letter, Points, Remarks).
 */
struct GradeInfo {
  std::string letterGrade;
  double gradePoints{0.0};
  std::string remark;
};

/**
 * @struct Course
 * @brief Represents an academic course with COMSATS grading attributes.
 */
struct Course {
  std::string courseCode;
  double marks{-1.0};      // Percentage marks (-1.0 if entered by letter grade)
  std::string letterGrade; // COMSATS letter grade
  double gradePoints{0.0}; // Calculated GPA points (0.00 - 4.00)
  double creditHours{0.0}; // Course credit hours
  std::string remark;      // COMSATS academic remark
  bool isLabSection{false}; // True if entry represents a 1 CH Lab component
};

/**
 * @struct Semester
 * @brief Represents a single academic semester containing multiple courses.
 */
struct Semester {
  int semesterNumber{1};
  std::vector<Course> courses;
  double totalCredits{0.0};
  double totalGradePoints{0.0};
  double semesterGPA{0.0};

  void calculateGPA() {
    totalCredits = 0.0;
    totalGradePoints = 0.0;
    for (const auto &course : courses) {
      totalCredits += course.creditHours;
      totalGradePoints += (course.gradePoints * course.creditHours);
    }

    if (totalCredits > 0.0) {
      semesterGPA = totalGradePoints / totalCredits;
    } else {
      semesterGPA = 0.0;
    }
  }
};

/**
 * @class ComsatsGradeScaleManager
 * @brief Manages COMSATS University grading scale conversions.
 */
class ComsatsGradeScaleManager {
public:
  static GradeInfo getGradeFromMarks(double marks) {
    if (marks >= 90.00 && marks <= 100.00)
      return {"A", 4.00, "Superior/Excellent"};
    if (marks >= 85.00 && marks < 90.00)
      return {"A-", 3.70, "Excellent"};
    if (marks >= 80.00 && marks < 85.00)
      return {"B+", 3.30, "Very Good"};
    if (marks >= 75.00 && marks < 80.00)
      return {"B", 3.00, "Above Average/Good"};
    if (marks >= 70.00 && marks < 75.00)
      return {"B-", 2.70, "Good"};
    if (marks >= 65.00 && marks < 70.00)
      return {"C+", 2.30, "Above Average"};
    if (marks >= 60.00 && marks < 65.00)
      return {"C", 2.00, "Competent Average"};
    if (marks >= 55.00 && marks < 60.00)
      return {"C-", 1.70, "Below Average"};
    if (marks >= 50.00 && marks < 55.00)
      return {"D", 1.00, "Minimum Passing"};
    if (marks >= 0.00 && marks < 50.00)
      return {"F", 0.00, "Fail"};

    return {"INVALID", -1.00, "Invalid Marks"};
  }

  static GradeInfo getGradeFromLetter(const std::string &letterGrade) {
    std::string upperGrade = letterGrade;
    upperGrade.erase(0, upperGrade.find_first_not_of(" \t\n\r"));
    upperGrade.erase(upperGrade.find_last_not_of(" \t\n\r") + 1);
    std::transform(upperGrade.begin(), upperGrade.end(), upperGrade.begin(),
                   ::toupper);

    static const std::unordered_map<std::string, GradeInfo> letterMap = {
        {"A", {"A", 4.00, "Superior/Excellent"}},
        {"A-", {"A-", 3.70, "Excellent"}},
        {"B+", {"B+", 3.30, "Very Good"}},
        {"B", {"B", 3.00, "Above Average/Good"}},
        {"B-", {"B-", 2.70, "Good"}},
        {"C+", {"C+", 2.30, "Above Average"}},
        {"C", {"C", 2.00, "Competent Average"}},
        {"C-", {"C-", 1.70, "Below Average"}},
        {"D", {"D", 1.00, "Minimum Passing"}},
        {"F", {"F", 0.00, "Fail"}}};

    auto it = letterMap.find(upperGrade);
    if (it != letterMap.end()) {
      return it->second;
    }
    return {"INVALID", -1.00, "Invalid Grade Letter"};
  }
};

/**
 * @class InputValidator
 * @brief Helper Class: Handles streamlined user inputs & unified Grade/Marks
 * auto-detection.
 */
class InputValidator {
public:
  static std::string getNonEmptyString(const std::string &prompt) {
    std::string input;
    while (true) {
      std::cout << prompt;
      if (std::getline(std::cin >> std::ws, input)) {
        size_t start = input.find_first_not_of(" \t\n\r");
        if (start != std::string::npos) {
          size_t end = input.find_last_not_of(" \t\n\r");
          return input.substr(start, end - start + 1);
        }
      }
      if (std::cin.eof())
        return "";
      std::cin.clear();
      std::cout << "  [Error]: Input cannot be empty. Please try again.\n";
    }
  }

  static int getBoundedInt(const std::string &prompt, int minVal, int maxVal) {
    int val{0};
    while (true) {
      std::cout << prompt;
      if (std::cin >> val) {
        if (val >= minVal && val <= maxVal) {
          return val;
        }
        std::cout << "  [Error]: Value must be between " << minVal << " and "
                  << maxVal << ". Try again.\n";
      } else {
        if (std::cin.eof())
          return minVal;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout
            << "  [Error]: Invalid numeric input. Please enter an integer.\n";
      }
    }
  }

  static double getBoundedDouble(const std::string &prompt, double minVal,
                                 double maxVal) {
    double val{0.0};
    while (true) {
      std::cout << prompt;
      if (std::cin >> val) {
        if (val >= minVal && val <= maxVal) {
          return val;
        }
        std::cout << "  [Error]: Value must be between " << minVal << " and "
                  << maxVal << ". Try again.\n";
      } else {
        if (std::cin.eof())
          return minVal;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout
            << "  [Error]: Invalid numeric input. Please enter a number.\n";
      }
    }
  }

  static GradeInfo getGradeOrMarks(const std::string &prompt,
                                   double &outMarks) {
    while (true) {
      std::cout << prompt;
      std::string token;
      if (!(std::cin >> token)) {
        if (std::cin.eof())
          return {"F", 0.0, "Fail"};
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        continue;
      }

      token.erase(0, token.find_first_not_of(" \t\n\r"));
      token.erase(token.find_last_not_of(" \t\n\r") + 1);

      std::stringstream ss(token);
      double marksVal;
      if (ss >> marksVal && ss.eof()) {
        if (marksVal >= 0.0 && marksVal <= 100.0) {
          outMarks = marksVal;
          return ComsatsGradeScaleManager::getGradeFromMarks(marksVal);
        } else {
          std::cout << "  [Error]: Marks must be between 0.0 and 100.0. Please "
                       "try again.\n";
          continue;
        }
      }

      std::string upperToken = token;
      std::transform(upperToken.begin(), upperToken.end(), upperToken.begin(),
                     ::toupper);
      GradeInfo info = ComsatsGradeScaleManager::getGradeFromLetter(upperToken);
      if (info.gradePoints >= 0.0) {
        outMarks = -1.0;
        return info;
      }

      std::cout << "  [Error]: Invalid Grade/Marks entry ('" << token
                << "'). Enter percentage (0-100) or valid letter grade (A, A-, "
                   "B+, B, B-, C+, C, C-, D, F).\n";
    }
  }
};

/**
 * @class CGPACalculatorSystem
 * @brief Core system class managing student course data, calculation
 * algorithms, JSON/CSV persistence, and Target CGPA simulation.
 */
class CGPACalculatorSystem {
private:
  std::string studentName;
  std::string studentID;
  std::vector<Semester> semesters;
  bool usedPreviousCGPAMode{false};
  double prevCGPA{0.0};
  double prevTotalCredits{0.0};
  double prevTotalPoints{0.0};
  double cumulativeCGPA{0.0};
  double totalCumulativeCredits{0.0};
  double totalCumulativeGradePoints{0.0};

  static std::string escapeJsonString(const std::string &input) {
    std::string output;
    for (char c : input) {
      if (c == '"' || c == '\\') {
        output += '\\';
      }
      output += c;
    }
    return output;
  }

public:
  CGPACalculatorSystem() = default;

  void calculateCGPA() {
    totalCumulativeCredits = 0.0;
    totalCumulativeGradePoints = 0.0;

    if (usedPreviousCGPAMode) {
      totalCumulativeCredits += prevTotalCredits;
      totalCumulativeGradePoints += prevTotalPoints;
    }

    for (auto &semester : semesters) {
      semester.calculateGPA();
      totalCumulativeCredits += semester.totalCredits;
      totalCumulativeGradePoints += semester.totalGradePoints;
    }

    if (totalCumulativeCredits > 0.0) {
      cumulativeCGPA = totalCumulativeGradePoints / totalCumulativeCredits;
    } else {
      cumulativeCGPA = 0.0;
    }
  }

  std::string getAcademicStanding() const {
    if (cumulativeCGPA >= 3.75)
      return "Summa Cum Laude (Highest Distinction / Rector's List)";
    if (cumulativeCGPA >= 3.50)
      return "Magna Cum Laude (High Distinction / Dean's List)";
    if (cumulativeCGPA >= 3.00)
      return "Cum Laude (Distinction / Good Standing)";
    if (cumulativeCGPA >= 2.00)
      return "Satisfactory / Pass";
    return "Academic Probation / Warning";
  }

  /**
   * @brief Feature #2: Target CGPA Simulator ("What-If" Analysis).
   * Calculates the exact average GPA required per remaining semester to achieve
   * a target graduating CGPA.
   */
  void runTargetCGPASimulator() const {
    std::cout << "\n==========================================================="
                 "==============\n";
    std::cout << "  🎯 TARGET CGPA SIMULATOR — WHAT-IF ACADEMIC ANALYSIS       "
                 "            \n";
    std::cout << "============================================================="
                 "============\n";
    std::cout << " Current Cumulative CGPA : " << std::fixed
              << std::setprecision(2) << cumulativeCGPA << " / 4.00\n";
    std::cout << " Current Total Credits   : " << totalCumulativeCredits
              << " CH\n";
    std::cout << "-------------------------------------------------------------"
                 "------------\n";

    double targetCGPA = InputValidator::getBoundedDouble(
        "Enter Desired Target CGPA upon Graduation (0.00 - 4.00): ", 0.0, 4.0);
    int remainingSemesters = InputValidator::getBoundedInt(
        "Enter Remaining Semesters to Complete Degree (1 - 8): ", 1, 8);
    double estCreditsPerSem = InputValidator::getBoundedDouble(
        "Enter Estimated Average Credit Hours per Remaining Semester (12.0 - "
        "21.0): ",
        12.0, 21.0);

    double totalFutureCredits = remainingSemesters * estCreditsPerSem;
    double finalTotalCredits = totalCumulativeCredits + totalFutureCredits;
    double requiredTotalPoints = targetCGPA * finalTotalCredits;
    double requiredFuturePoints =
        requiredTotalPoints - totalCumulativeGradePoints;
    double requiredGPA = requiredFuturePoints / totalFutureCredits;

    std::cout << "\n==========================================================="
                 "==============\n";
    std::cout << "                      WHAT-IF SIMULATION RESULTS             "
                 "            \n";
    std::cout << "============================================================="
                 "============\n";
    std::cout << " Current Earned Points   : " << totalCumulativeGradePoints
              << "\n";
    ssize_t reqPts = requiredTotalPoints;
    (void)reqPts;
    std::cout << " Required Total Points   : " << requiredTotalPoints << "\n";
    std::cout << " Required Future Points  : "
              << (requiredFuturePoints > 0.0 ? requiredFuturePoints : 0.0)
              << "\n";
    std::cout << " Total Degree Credits    : " << finalTotalCredits << " CH\n";
    std::cout << "-------------------------------------------------------------"
                 "------------\n";

    if (requiredGPA <= 0.0) {
      std::cout << "  🎉 GREAT NEWS! You have already achieved or surpassed "
                   "your target CGPA!\n";
      std::cout << "  Even with minimum passing grades (2.00 GPA), your target "
                   "is guaranteed.\n";
    } else if (requiredGPA > 4.00) {
      std::cout << "  ⚠️ MATHEMATICALLY IMPOSSIBLE:\n";
      std::cout << "  To reach a " << targetCGPA
                << " CGPA, you would need an average GPA of " << requiredGPA
                << " per semester.\n";
      std::cout << "  The maximum possible GPA in COMSATS is 4.00.\n";
      double maxPossibleCGPA =
          (totalCumulativeGradePoints + (4.00 * totalFutureCredits)) /
          finalTotalCredits;
      std::cout << "  💡 Maximum achievable CGPA if you get a perfect 4.00 in "
                   "all remaining semesters: "
                << maxPossibleCGPA << " / 4.00\n";
    } else {
      std::cout << "  📌 REQUIRED AVERAGE GPA: " << requiredGPA
                << " / 4.00 per remaining semester.\n";
      std::cout << "  Target Academic Standing: ";
      if (targetCGPA >= 3.75)
        std::cout << "Summa Cum Laude\n";
      else if (targetCGPA >= 3.50)
        std::cout << "Magna Cum Laude\n";
      else if (targetCGPA >= 3.00)
        std::cout << "Cum Laude\n";
      else
        std::cout << "Passing Standing\n";

      if (requiredGPA >= 3.70) {
        std::cout << "  🔥 STRATEGY: You need mostly 'A' and 'A-' grades in "
                     "all remaining courses.\n";
      } else if (requiredGPA >= 3.30) {
        std::cout << "  👍 STRATEGY: You need mostly 'B+' and 'A-' grades in "
                     "remaining courses.\n";
      } else {
        std::cout << "  ✅ STRATEGY: Maintain steady 'B' / 'C+' performance to "
                     "comfortably hit your target.\n";
      }
    }
    std::cout << "============================================================="
                 "============\n";
  }

  void saveToJSONAndCSV() const {
    std::ofstream jsonFile("student_record.json");
    if (jsonFile.is_open()) {
      jsonFile << "{\n";
      jsonFile << "  \"studentName\": \"" << escapeJsonString(studentName)
               << "\",\n";
      jsonFile << "  \"studentID\": \"" << escapeJsonString(studentID)
               << "\",\n";
      jsonFile << "  \"cumulativeCGPA\": " << std::fixed << std::setprecision(2)
               << cumulativeCGPA << ",\n";
      jsonFile << "  \"totalCumulativeCredits\": " << totalCumulativeCredits
               << ",\n";
      jsonFile << "  \"academicStanding\": \""
               << escapeJsonString(getAcademicStanding()) << "\",\n";
      jsonFile << "  \"usedPreviousCGPAMode\": "
               << (usedPreviousCGPAMode ? "true" : "false") << ",\n";

      if (usedPreviousCGPAMode) {
        jsonFile << "  \"prevCGPA\": " << prevCGPA << ",\n";
        jsonFile << "  \"prevTotalCredits\": " << prevTotalCredits << ",\n";
      }

      jsonFile << "  \"semesters\": [\n";
      for (size_t i = 0; i < semesters.size(); ++i) {
        const auto &sem = semesters[i];
        jsonFile << "    {\n";
        jsonFile << "      \"semesterNumber\": " << sem.semesterNumber << ",\n";
        jsonFile << "      \"semesterGPA\": " << sem.semesterGPA << ",\n";
        jsonFile << "      \"totalCredits\": " << sem.totalCredits << ",\n";
        jsonFile << "      \"courses\": [\n";
        for (size_t j = 0; j < sem.courses.size(); ++j) {
          const auto &c = sem.courses[j];
          jsonFile << "        {\n";
          jsonFile << "          \"courseCode\": \""
                   << escapeJsonString(c.courseCode) << "\",\n";
          jsonFile << "          \"creditHours\": " << c.creditHours << ",\n";
          jsonFile << "          \"letterGrade\": \""
                   << escapeJsonString(c.letterGrade) << "\",\n";
          jsonFile << "          \"gradePoints\": " << c.gradePoints << ",\n";
          jsonFile << "          \"marks\": " << c.marks << ",\n";
          jsonFile << "          \"remark\": \"" << escapeJsonString(c.remark)
                   << "\"\n";
          jsonFile << "        }" << (j + 1 < sem.courses.size() ? "," : "")
                   << "\n";
        }
        jsonFile << "      ]\n";
        jsonFile << "    }" << (i + 1 < semesters.size() ? "," : "") << "\n";
      }
      jsonFile << "  ]\n";
      jsonFile << "}\n";
      jsonFile.close();
      std::cout
          << "  [JSON Saved]: Profile persisted to 'student_record.json'.\n";
    }

    std::ofstream csvFile("student_record.csv");
    if (csvFile.is_open()) {
      csvFile
          << "Student Name,Student ID,Semester,Course Code,Credit Hours,Letter "
             "Grade,Grade Points,Marks,Remark,Semester GPA,Overall CGPA\n";
      for (const auto &sem : semesters) {
        for (const auto &c : sem.courses) {
          csvFile << "\"" << studentName << "\","
                  << "\"" << studentID << "\"," << sem.semesterNumber << ","
                  << "\"" << c.courseCode << "\"," << c.creditHours << ","
                  << "\"" << c.letterGrade << "\"," << c.gradePoints << ","
                  << c.marks << ","
                  << "\"" << c.remark << "\"," << sem.semesterGPA << ","
                  << cumulativeCGPA << "\n";
        }
      }
      csvFile.close();
      std::cout << "  [CSV Saved]: Spreadsheet record persisted to "
                   "'student_record.csv'.\n";
    }
  }

  static std::string extractJsonValue(const std::string &line,
                                      const std::string &key) {
    size_t keyPos = line.find("\"" + key + "\"");
    if (keyPos == std::string::npos)
      return "";
    size_t colonPos = line.find(":", keyPos);
    if (colonPos == std::string::npos)
      return "";
    std::string valStr = line.substr(colonPos + 1);

    size_t commaPos = valStr.rfind(",");
    if (commaPos != std::string::npos &&
        commaPos > valStr.find_last_not_of(" \t\r\n,")) {
      valStr = valStr.substr(0, commaPos);
    }

    valStr.erase(0, valStr.find_first_not_of(" \t\r\n"));
    valStr.erase(valStr.find_last_not_of(" \t\r\n,") + 1);

    if (valStr.size() >= 2 && valStr.front() == '"' && valStr.back() == '"') {
      return valStr.substr(1, valStr.size() - 2);
    }
    return valStr;
  }

  bool loadFromJSON() {
    std::ifstream inFile("student_record.json");
    if (!inFile.is_open())
      return false;

    semesters.clear();
    std::string line;
    Semester currentSem;
    Course currentCourse;

    while (std::getline(inFile, line)) {
      if (line.find("\"studentName\"") != std::string::npos) {
        studentName = extractJsonValue(line, "studentName");
      } else if (line.find("\"studentID\"") != std::string::npos) {
        studentID = extractJsonValue(line, "studentID");
      } else if (line.find("\"usedPreviousCGPAMode\"") != std::string::npos) {
        usedPreviousCGPAMode =
            (extractJsonValue(line, "usedPreviousCGPAMode") == "true");
      } else if (line.find("\"prevCGPA\"") != std::string::npos) {
        try {
          prevCGPA = std::stod(extractJsonValue(line, "prevCGPA"));
        } catch (...) {
        }
      } else if (line.find("\"prevTotalCredits\"") != std::string::npos) {
        try {
          prevTotalCredits =
              std::stod(extractJsonValue(line, "prevTotalCredits"));
        } catch (...) {
        }
      } else if (line.find("\"semesterNumber\"") != std::string::npos) {
        try {
          currentSem.semesterNumber =
              std::stoi(extractJsonValue(line, "semesterNumber"));
        } catch (...) {
        }
      } else if (line.find("\"courseCode\"") != std::string::npos) {
        currentCourse.courseCode = extractJsonValue(line, "courseCode");
      } else if (line.find("\"creditHours\"") != std::string::npos &&
                 line.find("\"totalCredits\"") == std::string::npos) {
        try {
          currentCourse.creditHours =
              std::stod(extractJsonValue(line, "creditHours"));
        } catch (...) {
        }
      } else if (line.find("\"letterGrade\"") != std::string::npos) {
        currentCourse.letterGrade = extractJsonValue(line, "letterGrade");
      } else if (line.find("\"gradePoints\"") != std::string::npos) {
        try {
          currentCourse.gradePoints =
              std::stod(extractJsonValue(line, "gradePoints"));
        } catch (...) {
        }
      } else if (line.find("\"marks\"") != std::string::npos) {
        try {
          currentCourse.marks = std::stod(extractJsonValue(line, "marks"));
        } catch (...) {
        }
      } else if (line.find("\"remark\"") != std::string::npos) {
        currentCourse.remark = extractJsonValue(line, "remark");
        currentSem.courses.push_back(currentCourse);
        currentCourse = Course();
      } else if (line.find("]") != std::string::npos &&
                 !currentSem.courses.empty()) {
        semesters.push_back(currentSem);
        currentSem = Semester();
      }
    }
    inFile.close();

    if (usedPreviousCGPAMode) {
      prevTotalPoints = prevCGPA * prevTotalCredits;
    }

    calculateCGPA();
    return !studentName.empty();
  }

  Semester inputSingleSemester(int semesterNum) {
    Semester semester;
    semester.semesterNumber = semesterNum;

    while (true) {
      semester.courses.clear();
      std::cout << "\n---------------------------------------------------------"
                   "----------------\n";
      std::cout << "--- Semester " << semesterNum << " Entry ---\n";
      if (semesterNum <= 8) {
        std::cout << "  [COMSATS Policy]: Semesters 1-8 require 12.0 to 21.0 "
                     "Credit Hours.\n";
      } else {
        std::cout << "  [COMSATS Policy]: Semester " << semesterNum
                  << " (Post 8th Semester) — Credit Hour limit REMOVED "
                     "(Flexible CH).\n";
      }
      std::cout << "-----------------------------------------------------------"
                   "--------------\n";

      int numSubjects = InputValidator::getBoundedInt(
          "Enter number of subjects taken in Semester " +
              std::to_string(semesterNum) + " (1 - 7): ",
          1, 7);

      for (int j = 1; j <= numSubjects; ++j) {
        std::cout << "\n  [Subject " << j << " Details]\n";
        std::string courseName =
            InputValidator::getNonEmptyString("  Subject Name/Code: ");
        double creditCH = InputValidator::getBoundedDouble(
            "  Credit Hours (1.0 - 4.0): ", 1.0, 4.0);

        if (creditCH == 4.0) {
          std::cout << "  --> 4 CH Subject detected! Entering Theory (3 CH) & "
                       "Lab (1 CH) separately.\n";

          Course theoryCourse;
          theoryCourse.courseCode = courseName + " (Theory)";
          theoryCourse.creditHours = 3.0;
          GradeInfo theoryInfo = InputValidator::getGradeOrMarks(
              "  Enter Theory Grade or Marks (0-100 or A/B/C...): ",
              theoryCourse.marks);
          theoryCourse.letterGrade = theoryInfo.letterGrade;
          theoryCourse.gradePoints = theoryInfo.gradePoints;
          theoryCourse.remark = theoryInfo.remark;
          semester.courses.push_back(theoryCourse);

          Course labCourse;
          labCourse.courseCode = courseName + " (Lab)";
          labCourse.creditHours = 1.0;
          labCourse.isLabSection = true;
          GradeInfo labInfo = InputValidator::getGradeOrMarks(
              "  Enter Lab Grade or Marks (0-100 or A/B/C...): ",
              labCourse.marks);
          labCourse.letterGrade = labInfo.letterGrade;
          labCourse.gradePoints = labInfo.gradePoints;
          labCourse.remark = labInfo.remark;
          semester.courses.push_back(labCourse);
        } else {
          Course course;
          course.courseCode = courseName;
          course.creditHours = creditCH;
          GradeInfo info = InputValidator::getGradeOrMarks(
              "  Enter Grade or Marks (0-100 or A/B/C...): ", course.marks);
          course.letterGrade = info.letterGrade;
          course.gradePoints = info.gradePoints;
          course.remark = info.remark;
          semester.courses.push_back(course);
        }
      }

      double currentSemesterCH = 0.0;
      for (const auto &c : semester.courses) {
        currentSemesterCH += c.creditHours;
      }

      if (semesterNum <= 8) {
        if (currentSemesterCH < 12.0 || currentSemesterCH > 21.0) {
          std::cout << "\n  [Rule Violation Error]: Semester " << semesterNum
                    << " total credit hours = " << currentSemesterCH
                    << " CH.\n";
          std::cout << "  COMSATS Rule Enforced: Semesters 1-8 MUST have "
                       "between 12.0 and 21.0 Credit Hours!\n";
          std::cout << "  Please re-enter subjects for Semester " << semesterNum
                    << ".\n";
          continue;
        }
      }

      break;
    }

    return semester;
  }

  void inputStudentData() {
    std::cout << "\n==========================================================="
                 "==============\n";
    std::cout << "  🎓 COMSATS CGPA CALCULATOR - ACADEMIC SYSTEM (WITH FILE "
                 "PERSISTENCE)   \n";
    std::cout << "============================================================="
                 "============\n";

    std::ifstream checkFile("student_record.json");
    if (checkFile.is_open()) {
      checkFile.close();
      std::cout << "  [Saved Profile Found]: A saved student profile "
                   "('student_record.json') was detected.\n";
      std::cout << "    1: Load Existing Saved Profile\n";
      std::cout << "    2: Create New Profile (Overwrite)\n";
      int loadChoice =
          InputValidator::getBoundedInt("  Select Option (1 or 2): ", 1, 2);

      if (loadChoice == 1) {
        if (loadFromJSON()) {
          std::cout << "\n  [Success]: Loaded profile for " << studentName
                    << " (" << studentID << "). CGPA: " << cumulativeCGPA
                    << "\n";
          return;
        } else {
          std::cout << "  [Error]: Corrupted profile file. Falling back to "
                       "manual input.\n";
        }
      }
    }

    studentName = InputValidator::getNonEmptyString("Enter Student Name: ");
    studentID = InputValidator::getNonEmptyString(
        "Enter Student Registration/ID Number: ");

    int numSemesters = InputValidator::getBoundedInt(
        "Enter Total Number of Semesters Completed/Enrolled (1 - 12): ", 1, 12);

    semesters.clear();
    usedPreviousCGPAMode = false;

    if (numSemesters > 1) {
      std::cout << "\n---------------------------------------------------------"
                   "----------------\n";
      std::cout << "  Calculation Mode Option:\n";
      std::cout << "    1: Fast Entry (Enter Previous CGPA + Previous Credits, "
                   "then Most Recent Semester)\n";
      std::cout << "    2: Full Entry (Enter all " << numSemesters
                << " Semesters step-by-step)\n";
      std::cout << "-----------------------------------------------------------"
                   "--------------\n";
      int modeChoice =
          InputValidator::getBoundedInt("Select Mode (1 or 2): ", 1, 2);

      if (modeChoice == 1) {
        usedPreviousCGPAMode = true;
        std::cout << "\n--- PREVIOUS ACADEMIC RECORD ---\n";
        prevCGPA = InputValidator::getBoundedDouble(
            "Enter Previous CGPA (0.00 - 4.00): ", 0.0, 4.0);
        prevTotalCredits = InputValidator::getBoundedDouble(
            "Enter Total Previous Completed Credit Hours: ", 1.0, 200.0);
        prevTotalPoints = prevCGPA * prevTotalCredits;

        std::cout << "\n--- MOST RECENT SEMESTER (SEMESTER " << numSemesters
                  << ") ---\n";
        Semester recentSem = inputSingleSemester(numSemesters);
        semesters.push_back(recentSem);

        calculateCGPA();
        saveToJSONAndCSV();
        return;
      }
    }

    for (int i = 1; i <= numSemesters; ++i) {
      Semester sem = inputSingleSemester(i);
      semesters.push_back(sem);
    }

    calculateCGPA();
    saveToJSONAndCSV();
  }

  std::string generateTranscriptString() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2);

    ss << "===================================================================="
          "=====\n";
    ss << "                 COMSATS ACADEMIC TRANSCRIPT REPORT                 "
          "     \n";
    ss << "===================================================================="
          "=====\n";
    ss << " Student Name      : " << studentName << "\n";
    ss << " Registration ID   : " << studentID << "\n";
    ss << " Academic Standing : " << getAcademicStanding() << "\n";
    ss << "===================================================================="
          "=====\n";

    if (usedPreviousCGPAMode) {
      ss << "\n[PREVIOUS ACADEMIC RECORD SUMMARY]\n";
      ss << "------------------------------------------------------------------"
            "-------\n";
      ss << " Total Previous Credits  : " << prevTotalCredits << " CH\n";
      ss << " Previous Cumulative CGPA: " << prevCGPA << " / 4.00\n";
      ss << " Total Previous Points   : " << prevTotalPoints << "\n";
      ss << "------------------------------------------------------------------"
            "-------\n";
    }

    for (const auto &semester : semesters) {
      ss << "\n[SEMESTER " << semester.semesterNumber
         << " DETAILS (MOST RECENT)]\n";
      ss << "------------------------------------------------------------------"
            "-------\n";
      ss << std::left << std::setw(34) << "Course / Section Name"
         << std::setw(10) << "Credits" << std::setw(10) << "Grade"
         << std::setw(10) << "GPA Pts"
         << "Remark\n";
      ss << "------------------------------------------------------------------"
            "-------\n";

      for (const auto &course : semester.courses) {
        ss << std::left << std::setw(34) << course.courseCode << std::setw(10)
           << course.creditHours << std::setw(10) << course.letterGrade
           << std::setw(10) << course.gradePoints << course.remark << "\n";
      }
      ss << "------------------------------------------------------------------"
            "-------\n";
      ss << " Semester Total Credits: " << semester.totalCredits << " CH\n";
      ss << " Semester GPA          : " << semester.semesterGPA << " / 4.00\n";
    }

    ss << "\n=================================================================="
          "=======\n";
    ss << " OVERALL CUMULATIVE CREDITS: " << totalCumulativeCredits << " CH\n";
    ss << " OVERALL CUMULATIVE CGPA   : " << cumulativeCGPA << " / 4.00\n";
    ss << " ACADEMIC STANDING         : " << getAcademicStanding() << "\n";
    ss << "===================================================================="
          "=====\n";

    return ss.str();
  }

  void displayAndExportReport() const {
    std::string transcriptText = generateTranscriptString();

    std::cout << "\n" << transcriptText;

    std::ofstream outFile("CGPA_Transcript.txt");
    if (outFile.is_open()) {
      outFile << transcriptText;
      outFile.close();
      std::cout << "\n  [Academic Report Export]: Transcript saved to "
                   "'CGPA_Transcript.txt'.\n";
    } else {
      std::cout
          << "\n  [Warning]: Could not create 'CGPA_Transcript.txt' file.\n";
    }
  }
};

int main() {
  CGPACalculatorSystem system;

  system.inputStudentData();
  system.displayAndExportReport();

  // Feature #2: Ask user if they want to run the Target CGPA Simulator
  // ("What-If" Analysis)
  std::cout << "\nDo you want to run the Target CGPA Simulator ('What-If' "
               "Analysis)? (1: Yes, 2: No): ";
  int simChoice = InputValidator::getBoundedInt("", 1, 2);
  if (simChoice == 1) {
    system.runTargetCGPASimulator();
  }

  return 0;
}
