/**
 * @file main_gui.cpp
 * @brief Qt5 Modern Desktop GUI for CGPA Calculator (Refactored Spacing, Scroll
 * View & Title Cleanup)
 * @details Premium dark-themed Qt application featuring full page scrolling,
 * compact student & subject buttons, automatic 4 CH Theory/Lab split, Target
 * CGPA simulator, and JSON/CSV profile persistence.
 */

#include <QApplication>
#include <QComboBox>
#include <QDir>
#include <QDoubleSpinBox>
#include <QFile>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMessageBox>
#include <QPushButton>
#include <QScreen>
#include <QScrollArea>
#include <QSpinBox>
#include <QStackedWidget>
#include <QTabWidget>
#include <QTableWidget>
#include <QTextStream>
#include <QVBoxLayout>
#include <QWidget>
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <vector>

// --- Core Data Structures & Models ---

struct Course {
  QString courseCode;
  double marks{-1.0};
  QString letterGrade;
  double gradePoints{0.0};
  double creditHours{0.0};
  QString remark;
  bool isLabSection{false};
};

struct Semester {
  int semesterNumber{1};
  std::vector<Course> courses;
  double totalCredits{0.0};
  double totalGradePoints{0.0};
  double semesterGPA{0.0};

  void calculateGPA() {
    totalCredits = 0.0;
    totalGradePoints = 0.0;
    for (const auto &c : courses) {
      totalCredits += c.creditHours;
      totalGradePoints += (c.gradePoints * c.creditHours);
    }
    semesterGPA =
        (totalCredits > 0.0) ? (totalGradePoints / totalCredits) : 0.0;
  }
};

class ComsatsGradeEngine {
public:
  static std::pair<QString, double> getGradeFromMarks(double marks) {
    if (marks >= 90.00)
      return {"A", 4.00};
    if (marks >= 85.00)
      return {"A-", 3.70};
    if (marks >= 80.00)
      return {"B+", 3.30};
    if (marks >= 75.00)
      return {"B", 3.00};
    if (marks >= 70.00)
      return {"B-", 2.70};
    if (marks >= 65.00)
      return {"C+", 2.30};
    if (marks >= 60.00)
      return {"C", 2.00};
    if (marks >= 55.00)
      return {"C-", 1.70};
    if (marks >= 50.00)
      return {"D", 1.00};
    return {"F", 0.00};
  }

  static std::pair<QString, double> getGradeFromLetter(const QString &letter) {
    QString u = letter.trimmed().toUpper();
    if (u == "A" || u == "A+")
      return {"A", 4.00};
    if (u == "A-")
      return {"A-", 3.70};
    if (u == "B+")
      return {"B+", 3.30};
    if (u == "B")
      return {"B", 3.00};
    if (u == "B-")
      return {"B-", 2.70};
    if (u == "C+")
      return {"C+", 2.30};
    if (u == "C")
      return {"C", 2.00};
    if (u == "C-")
      return {"C-", 1.70};
    if (u == "D" || u == "D+")
      return {"D", 1.00};
    return {"F", 0.00};
  }

  static QString getRemark(double gradePoints) {
    if (gradePoints >= 4.00)
      return "Superior/Excellent";
    if (gradePoints >= 3.70)
      return "Excellent";
    if (gradePoints >= 3.30)
      return "Very Good";
    if (gradePoints >= 3.00)
      return "Above Average/Good";
    if (gradePoints >= 2.70)
      return "Good";
    if (gradePoints >= 2.30)
      return "Above Average";
    if (gradePoints >= 2.00)
      return "Competent Average";
    if (gradePoints >= 1.70)
      return "Below Average";
    if (gradePoints >= 1.00)
      return "Minimum Passing";
    return "Fail";
  }

  static QString getAcademicStanding(double cgpa) {
    if (cgpa >= 3.75)
      return "Summa Cum Laude (Highest Distinction / Rector's List)";
    if (cgpa >= 3.50)
      return "Magna Cum Laude (High Distinction / Dean's List)";
    if (cgpa >= 3.00)
      return "Cum Laude (Distinction / Good Standing)";
    if (cgpa >= 2.00)
      return "Satisfactory / Pass";
    return "Academic Probation / Warning";
  }
};

// --- Main GUI Window ---

class CGPACalculatorWindow : public QMainWindow {
  Q_OBJECT

private:
  // UI Elements - Inputs
  QLineEdit *nameInput;
  QLineEdit *idInput;
  QSpinBox *semesterSpinBox;
  QComboBox *modeComboBox;

  // Fast Mode Group
  QGroupBox *fastModeGroup;
  QDoubleSpinBox *prevCGPASpinBox;
  QDoubleSpinBox *prevCreditsSpinBox;

  // Course Adding Elements
  QLineEdit *courseNameInput;
  QDoubleSpinBox *creditHoursSpinBox;
  QLineEdit *gradeMarksInput;
  QPushButton *addCourseBtn;

  // Table & Results
  QTableWidget *coursesTable;
  QLabel *cgpaLabel;
  QLabel *totalCreditsLabel;
  QLabel *standingLabel;

  // Target Simulator
  QDoubleSpinBox *targetCGPASpinBox;
  QSpinBox *remainingSemSpinBox;
  QDoubleSpinBox *estCreditsSpinBox;
  QLabel *simulatorResultLabel;

  // System Data State
  std::vector<Semester> semesters;
  double overallCGPA{0.0};
  double totalCredits{0.0};
  double totalPoints{0.0};

public:
  CGPACalculatorWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
    setWindowTitle("CGPA Calculator — CodeAlpha Task 1");

    QScreen *screen = QGuiApplication::primaryScreen();
    if (screen) {
      QRect screenGeometry = screen->geometry();
      int width =
          std::max(1050, static_cast<int>(screenGeometry.width() * 0.65));
      int height =
          std::max(750, static_cast<int>(screenGeometry.height() * 0.75));
      resize(width, height);
    } else {
      resize(1150, 800);
    }

    applyTheme();

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(14, 14, 14, 14);
    mainLayout->setSpacing(12);

    // Header Title Banner (Title simplified to "CGPA Calculator")
    QLabel *bannerLabel = new QLabel("🎓 CGPA Calculator", this);
    bannerLabel->setObjectName("BannerLabel");
    bannerLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(bannerLabel);

    QTabWidget *tabWidget = new QTabWidget(this);
    tabWidget->setObjectName("MainTabWidget");

    // Tab 1: Main Calculator & Profile
    QWidget *calcTab = new QWidget();
    setupCalculatorTab(calcTab);
    tabWidget->addTab(calcTab, "📊 CGPA Calculator & Course Entry");

    // Tab 2: Target CGPA Simulator
    QWidget *simTab = new QWidget();
    setupSimulatorTab(simTab);
    tabWidget->addTab(simTab, "🎯 Target CGPA Simulator (What-If)");

    mainLayout->addWidget(tabWidget);
  }

private:
  void applyTheme() {
    QString style = R"(
            QMainWindow {
                background-color: #0b0f19;
            }
            #BannerLabel {
                background: linear-gradient(135deg, #1e293b, #0f172a);
                color: #38bdf8;
                font-size: 22px;
                font-weight: bold;
                padding: 12px;
                border-radius: 8px;
                border: 1px solid #1e293b;
            }
            QGroupBox {
                font-size: 14px;
                font-weight: bold;
                color: #f8fafc;
                border: 1px solid #334155;
                border-radius: 8px;
                margin-top: 10px;
                padding: 18px 14px 12px 14px;
                background-color: #0f172a;
            }
            QGroupBox::title {
                subcontrol-origin: margin;
                left: 14px;
                padding: 2px 8px;
                color: #38bdf8;
                background-color: #1e293b;
                border-radius: 4px;
                border: 1px solid #334155;
            }
            QLabel {
                color: #cbd5e1;
                font-size: 12px;
                font-weight: 500;
            }
            QLineEdit, QSpinBox, QDoubleSpinBox, QComboBox {
                background-color: #1e293b;
                color: #f8fafc;
                border: 1px solid #475569;
                border-radius: 5px;
                padding: 5px 8px;
                font-size: 12px;
                min-height: 18px;
            }
            QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus, QComboBox:focus {
                border: 1.5px solid #38bdf8;
                background-color: #1e293b;
            }
            QPushButton {
                background: linear-gradient(135deg, #0284c7, #0369a1);
                color: #ffffff;
                font-weight: 600;
                font-size: 11px;
                border-radius: 5px;
                padding: 4px 8px;
                max-height: 26px;
                border: 1px solid #38bdf8;
            }
            QPushButton:hover {
                background: linear-gradient(135deg, #0369a1, #075985);
                border: 1px solid #7dd3fc;
            }
            QPushButton:pressed {
                background: linear-gradient(135deg, #075985, #0c4a6e);
            }
            #AddSubjectBtn {
                background: linear-gradient(135deg, #10b981, #059669);
                border: 1px solid #34d399;
                max-width: 95px;
                max-height: 26px;
            }
            #AddSubjectBtn:hover {
                background: linear-gradient(135deg, #059669, #047857);
            }
            #ClearBtn {
                background: linear-gradient(135deg, #ef4444, #dc2626);
                border: 1px solid #f87171;
                max-width: 120px;
                max-height: 28px;
            }
            #ClearBtn:hover {
                background: linear-gradient(135deg, #dc2626, #b91c1c);
            }
            #CalcBtn {
                background: linear-gradient(135deg, #0284c7, #0369a1);
                border: 1px solid #38bdf8;
                max-width: 180px;
                max-height: 28px;
            }
            QTableWidget {
                background-color: #0f172a;
                color: #f8fafc;
                gridline-color: #334155;
                border: 1px solid #334155;
                border-radius: 6px;
                selection-background-color: #1e293b;
                selection-color: #38bdf8;
                min-height: 220px;
            }
            QHeaderView::section {
                background-color: #1e293b;
                color: #38bdf8;
                font-weight: bold;
                padding: 6px;
                border: 1px solid #334155;
            }
            #ResultBox {
                background-color: #0f172a;
                border: 1.5px solid #0284c7;
                border-radius: 8px;
                padding: 12px 16px;
            }
            #ResultHeader {
                color: #38bdf8;
                font-size: 15px;
                font-weight: bold;
            }
            #StandingText {
                color: #4ade80;
                font-size: 13px;
                font-weight: bold;
            }
            QTabWidget::pane {
                border: 1px solid #334155;
                border-radius: 8px;
                background-color: #121824;
            }
            QTabBar::tab {
                background-color: #1e293b;
                color: #94a3b8;
                padding: 10px 18px;
                border-top-left-radius: 6px;
                border-top-right-radius: 6px;
                font-weight: bold;
                font-size: 12px;
                margin-right: 4px;
            }
            QTabBar::tab:selected {
                background-color: #0284c7;
                color: #ffffff;
            }
            QScrollArea {
                border: none;
                background-color: transparent;
            }
        )";
    setStyleSheet(style);
  }

  void setupCalculatorTab(QWidget *tab) {
    QVBoxLayout *outerTabLayout = new QVBoxLayout(tab);
    outerTabLayout->setContentsMargins(0, 0, 0, 0);

    // Scroll Area wrapper for the whole calculator page
    QScrollArea *scrollArea = new QScrollArea(tab);
    scrollArea->setWidgetResizable(true);

    QWidget *scrollContent = new QWidget(scrollArea);
    QVBoxLayout *layout = new QVBoxLayout(scrollContent);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(12);

    // Student Info Group
    QGroupBox *infoGroup =
        new QGroupBox("👤 Student Profile & Entry Setup", scrollContent);
    QGridLayout *infoLayout = new QGridLayout(infoGroup);
    infoLayout->setHorizontalSpacing(14);
    infoLayout->setVerticalSpacing(10);

    infoLayout->addWidget(new QLabel("Student Name:"), 0, 0);
    nameInput = new QLineEdit(this);
    nameInput->setPlaceholderText("e.g. Ali Raza");
    infoLayout->addWidget(nameInput, 0, 1);

    infoLayout->addWidget(new QLabel("Registration ID:"), 0, 2);
    idInput = new QLineEdit(this);
    idInput->setPlaceholderText("e.g. SP21-BCS-045");
    infoLayout->addWidget(idInput, 0, 3);

    infoLayout->addWidget(new QLabel("Semester Number:"), 1, 0);
    semesterSpinBox = new QSpinBox(this);
    semesterSpinBox->setRange(1, 12);
    semesterSpinBox->setValue(1);
    infoLayout->addWidget(semesterSpinBox, 1, 1);

    infoLayout->addWidget(new QLabel("Entry Mode:"), 1, 2);
    modeComboBox = new QComboBox(this);
    modeComboBox->addItem("Standard Entry (Direct Course Input)");
    modeComboBox->addItem("Fast Entry (Previous CGPA + Latest Semester)");
    infoLayout->addWidget(modeComboBox, 1, 3);

    // Fast Mode Extra Group
    fastModeGroup =
        new QGroupBox("⚡ Previous Academic Record (Fast Mode)", scrollContent);
    QHBoxLayout *fastLayout = new QHBoxLayout(fastModeGroup);
    fastLayout->setSpacing(14);

    fastLayout->addWidget(new QLabel("Previous CGPA (0.0 - 4.0):"));
    prevCGPASpinBox = new QDoubleSpinBox(this);
    prevCGPASpinBox->setRange(0.0, 4.0);
    prevCGPASpinBox->setSingleStep(0.01);
    prevCGPASpinBox->setValue(3.20);
    fastLayout->addWidget(prevCGPASpinBox);

    fastLayout->addWidget(new QLabel("Total Previous Credits:"));
    prevCreditsSpinBox = new QDoubleSpinBox(this);
    prevCreditsSpinBox->setRange(1.0, 200.0);
    prevCreditsSpinBox->setValue(48.0);
    fastLayout->addWidget(prevCreditsSpinBox);

    fastModeGroup->setVisible(false);

    connect(modeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](int idx) { fastModeGroup->setVisible(idx == 1); });

    layout->addWidget(infoGroup);
    layout->addWidget(fastModeGroup);

    // Course Input Bar with Compact Sizing
    QGroupBox *courseGroup =
        new QGroupBox("📖 Add Course / Subject", scrollContent);
    QHBoxLayout *courseLayout = new QHBoxLayout(courseGroup);
    courseLayout->setSpacing(12);
    courseLayout->setContentsMargins(12, 18, 12, 12);

    QVBoxLayout *nameCol = new QVBoxLayout();
    nameCol->setSpacing(4);
    nameCol->addWidget(new QLabel("Course Name:"));
    courseNameInput = new QLineEdit(this);
    courseNameInput->setPlaceholderText("e.g., Programming Fundamentals");
    nameCol->addWidget(courseNameInput);
    courseLayout->addLayout(nameCol, 3);

    QVBoxLayout *chCol = new QVBoxLayout();
    chCol->setSpacing(4);
    chCol->addWidget(new QLabel("Credit Hours (1-4):"));
    creditHoursSpinBox = new QDoubleSpinBox(this);
    creditHoursSpinBox->setRange(1.0, 4.0);
    creditHoursSpinBox->setSingleStep(1.0);
    creditHoursSpinBox->setValue(3.0);
    chCol->addWidget(creditHoursSpinBox);
    courseLayout->addLayout(chCol, 2);

    QVBoxLayout *gradeCol = new QVBoxLayout();
    gradeCol->setSpacing(4);
    gradeCol->addWidget(new QLabel("Grade or Marks (0-100 or A/B/C):"));
    gradeMarksInput = new QLineEdit(this);
    gradeMarksInput->setPlaceholderText("e.g., 87.5 or A-");
    gradeCol->addWidget(gradeMarksInput);
    courseLayout->addLayout(gradeCol, 2);

    QVBoxLayout *btnCol = new QVBoxLayout();
    btnCol->setSpacing(4);
    btnCol->addWidget(new QLabel(""));
    addCourseBtn = new QPushButton("➕ Add Subject", this);
    addCourseBtn->setObjectName("AddSubjectBtn");
    addCourseBtn->setCursor(Qt::PointingHandCursor);
    addCourseBtn->setFixedHeight(26);
    btnCol->addWidget(addCourseBtn);
    courseLayout->addLayout(btnCol, 1);

    connect(addCourseBtn, &QPushButton::clicked, this,
            &CGPACalculatorWindow::addCourse);

    layout->addWidget(courseGroup);

    // Courses Table with Custom Column Resize Ratios
    coursesTable = new QTableWidget(this);
    coursesTable->setColumnCount(6);
    coursesTable->setHorizontalHeaderLabels({"Course / Section Name",
                                             "Credit Hours", "Grade", "GPA Pts",
                                             "Earned Points", "Remark"});

    QHeaderView *header = coursesTable->horizontalHeader();
    header->setSectionResizeMode(
        0, QHeaderView::Stretch); // Course Name gets space
    header->setSectionResizeMode(1,
                                 QHeaderView::ResizeToContents); // Credit Hours
    header->setSectionResizeMode(2, QHeaderView::ResizeToContents); // Grade
    header->setSectionResizeMode(3, QHeaderView::ResizeToContents); // GPA Pts
    header->setSectionResizeMode(
        4, QHeaderView::ResizeToContents);                 // Earned Points
    header->setSectionResizeMode(5, QHeaderView::Stretch); // Remark gets space
    coursesTable->setAlternatingRowColors(true);

    layout->addWidget(coursesTable, 1);

    // Action Buttons
    QHBoxLayout *bottomLayout = new QHBoxLayout();
    bottomLayout->setSpacing(14);
    bottomLayout->setContentsMargins(0, 4, 0, 4);

    QPushButton *calcBtn =
        new QPushButton("🧮 Calculate CGPA & Save Profile", this);
    calcBtn->setObjectName("CalcBtn");
    calcBtn->setCursor(Qt::PointingHandCursor);
    calcBtn->setFixedHeight(28);
    connect(calcBtn, &QPushButton::clicked, this,
            &CGPACalculatorWindow::calculateResults);
    bottomLayout->addWidget(calcBtn);

    QPushButton *loadBtn = new QPushButton("📂 Load Saved Profile", this);
    loadBtn->setCursor(Qt::PointingHandCursor);
    loadBtn->setFixedHeight(28);
    connect(loadBtn, &QPushButton::clicked, [this]() {
      loadProfileFromJSON();
      QMessageBox::information(
          this, "Profile Loaded",
          "Saved profile loaded from 'student_record.json'.");
    });
    bottomLayout->addWidget(loadBtn);

    QPushButton *clearBtn = new QPushButton("🧹 Clear Form", this);
    clearBtn->setObjectName("ClearBtn");
    clearBtn->setCursor(Qt::PointingHandCursor);
    clearBtn->setFixedHeight(28);
    connect(clearBtn, &QPushButton::clicked, [this]() {
      nameInput->clear();
      idInput->clear();
      courseNameInput->clear();
      gradeMarksInput->clear();
      coursesTable->setRowCount(0);
      semesters.clear();
      cgpaLabel->setText("Overall CGPA: 0.00 / 4.00");
      totalCreditsLabel->setText("Total Credits: 0.0 CH");
      standingLabel->setText("Academic Standing: N/A");
    });
    bottomLayout->addWidget(clearBtn);

    layout->addLayout(bottomLayout);

    // Summary Results Box
    QWidget *resultBox = new QWidget(this);
    resultBox->setObjectName("ResultBox");
    QHBoxLayout *resLayout = new QHBoxLayout(resultBox);
    resLayout->setSpacing(20);

    cgpaLabel = new QLabel("Overall CGPA: 0.00 / 4.00", this);
    cgpaLabel->setObjectName("ResultHeader");
    resLayout->addWidget(cgpaLabel);

    totalCreditsLabel = new QLabel("Total Credits: 0.0 CH", this);
    totalCreditsLabel->setObjectName("ResultHeader");
    resLayout->addWidget(totalCreditsLabel);

    standingLabel = new QLabel("Academic Standing: N/A", this);
    standingLabel->setObjectName("StandingText");
    resLayout->addWidget(standingLabel);

    layout->addWidget(resultBox);

    scrollArea->setWidget(scrollContent);
    outerTabLayout->addWidget(scrollArea);
  }

  void setupSimulatorTab(QWidget *tab) {
    QVBoxLayout *outerTabLayout = new QVBoxLayout(tab);
    outerTabLayout->setContentsMargins(0, 0, 0, 0);

    QScrollArea *scrollArea = new QScrollArea(tab);
    scrollArea->setWidgetResizable(true);

    QWidget *scrollContent = new QWidget(scrollArea);
    QVBoxLayout *layout = new QVBoxLayout(scrollContent);
    layout->setContentsMargins(14, 14, 14, 14);
    layout->setSpacing(16);

    QGroupBox *simGroup = new QGroupBox(
        "🎯 Target CGPA Goal & Degree Assumptions", scrollContent);
    QGridLayout *simLayout = new QGridLayout(simGroup);
    simLayout->setHorizontalSpacing(16);
    simLayout->setVerticalSpacing(12);

    simLayout->addWidget(
        new QLabel("Desired Target Graduating CGPA (0.00 - 4.00):"), 0, 0);
    targetCGPASpinBox = new QDoubleSpinBox(this);
    targetCGPASpinBox->setRange(0.0, 4.0);
    targetCGPASpinBox->setSingleStep(0.05);
    targetCGPASpinBox->setValue(3.75);
    simLayout->addWidget(targetCGPASpinBox, 0, 1);

    simLayout->addWidget(new QLabel("Remaining Semesters to Graduate (1 - 8):"),
                         1, 0);
    remainingSemSpinBox = new QSpinBox(this);
    remainingSemSpinBox->setRange(1, 8);
    remainingSemSpinBox->setValue(4);
    simLayout->addWidget(remainingSemSpinBox, 1, 1);

    simLayout->addWidget(
        new QLabel("Estimated Credit Hours per Remaining Semester:"), 2, 0);
    estCreditsSpinBox = new QDoubleSpinBox(this);
    estCreditsSpinBox->setRange(12.0, 21.0);
    estCreditsSpinBox->setValue(15.0);
    simLayout->addWidget(estCreditsSpinBox, 2, 1);

    QPushButton *runSimBtn = new QPushButton("🚀 Run What-If Simulation", this);
    runSimBtn->setCursor(Qt::PointingHandCursor);
    runSimBtn->setFixedHeight(32);
    connect(runSimBtn, &QPushButton::clicked, this,
            &CGPACalculatorWindow::runSimulation);
    simLayout->addWidget(runSimBtn, 3, 0, 1, 2);

    layout->addWidget(simGroup);

    // Simulation Results Display Box
    QGroupBox *resultGroup =
        new QGroupBox("📊 Simulation Analysis & Strategic Plan", scrollContent);
    QVBoxLayout *resLayout = new QVBoxLayout(resultGroup);
    resLayout->setContentsMargins(14, 20, 14, 14);

    simulatorResultLabel = new QLabel(
        "Run simulation to see required GPA and academic advice...", this);
    simulatorResultLabel->setStyleSheet(
        "font-size: 14px; color: #f8fafc; padding: 8px;");
    simulatorResultLabel->setWordWrap(true);
    resLayout->addWidget(simulatorResultLabel);

    layout->addWidget(resultGroup);
    layout->addStretch();

    scrollArea->setWidget(scrollContent);
    outerTabLayout->addWidget(scrollArea);
  }

private slots:
  void addCourse() {
    QString name = courseNameInput->text().trimmed();
    if (name.isEmpty()) {
      QMessageBox::warning(this, "Input Error",
                           "Please enter a valid course name.");
      return;
    }

    double ch = creditHoursSpinBox->value();
    QString valToken = gradeMarksInput->text().trimmed();
    if (valToken.isEmpty()) {
      QMessageBox::warning(
          this, "Input Error",
          "Please enter percentage marks (0-100) or letter grade.");
      return;
    }

    if (ch == 4.0) {
      bool ok;
      double marksVal = valToken.toDouble(&ok);
      std::pair<QString, double> gInfo;
      if (ok && marksVal >= 0.0 && marksVal <= 100.0) {
        gInfo = ComsatsGradeEngine::getGradeFromMarks(marksVal);
      } else {
        gInfo = ComsatsGradeEngine::getGradeFromLetter(valToken);
      }

      insertTableRow(name + " (Theory)", 3.0, gInfo.first, gInfo.second,
                     (ok ? marksVal : -1.0));
      insertTableRow(name + " (Lab)", 1.0, gInfo.first, gInfo.second,
                     (ok ? marksVal : -1.0));
    } else {
      bool ok;
      double marksVal = valToken.toDouble(&ok);
      std::pair<QString, double> gInfo;
      if (ok && marksVal >= 0.0 && marksVal <= 100.0) {
        gInfo = ComsatsGradeEngine::getGradeFromMarks(marksVal);
      } else {
        gInfo = ComsatsGradeEngine::getGradeFromLetter(valToken);
      }

      insertTableRow(name, ch, gInfo.first, gInfo.second,
                     (ok ? marksVal : -1.0));
    }

    courseNameInput->clear();
    gradeMarksInput->clear();
  }

  void insertTableRow(const QString &name, double ch, const QString &letter,
                      double pts, double marks) {
    int row = coursesTable->rowCount();
    coursesTable->insertRow(row);

    coursesTable->setItem(row, 0, new QTableWidgetItem(name));
    coursesTable->setItem(row, 1,
                          new QTableWidgetItem(QString::number(ch, 'f', 1)));
    coursesTable->setItem(row, 2, new QTableWidgetItem(letter));
    coursesTable->setItem(row, 3,
                          new QTableWidgetItem(QString::number(pts, 'f', 2)));
    coursesTable->setItem(
        row, 4, new QTableWidgetItem(QString::number(pts * ch, 'f', 2)));
    coursesTable->setItem(
        row, 5, new QTableWidgetItem(ComsatsGradeEngine::getRemark(pts)));

    coursesTable->item(row, 0)->setData(Qt::UserRole, marks);
  }

  void calculateResults() {
    int rows = coursesTable->rowCount();
    if (rows == 0 && modeComboBox->currentIndex() == 0) {
      QMessageBox::warning(this, "Calculation Error",
                           "Please add at least one course to calculate.");
      return;
    }

    double semPts = 0.0;
    double semCH = 0.0;

    for (int i = 0; i < rows; ++i) {
      double ch = coursesTable->item(i, 1)->text().toDouble();
      double pts = coursesTable->item(i, 3)->text().toDouble();
      semCH += ch;
      semPts += (pts * ch);
    }

    int currentSemNum = semesterSpinBox->value();
    if (currentSemNum <= 8 && modeComboBox->currentIndex() == 0) {
      if (semCH < 12.0 || semCH > 21.0) {
        QMessageBox::warning(
            this, "COMSATS Policy Error",
            QString("Semester %1 total credit hours (%2 CH) violates COMSATS "
                    "policy.\nSemesters 1-8 MUST have between 12.0 and 21.0 "
                    "Credit Hours!")
                .arg(currentSemNum)
                .arg(semCH));
        return;
      }
    }

    if (modeComboBox->currentIndex() == 1) {
      double prevCGPA = prevCGPASpinBox->value();
      double prevCH = prevCreditsSpinBox->value();
      double prevPts = prevCGPA * prevCH;

      totalCredits = prevCH + semCH;
      totalPoints = prevPts + semPts;
    } else {
      totalCredits = semCH;
      totalPoints = semPts;
    }

    overallCGPA = (totalCredits > 0.0) ? (totalPoints / totalCredits) : 0.0;
    QString standing = ComsatsGradeEngine::getAcademicStanding(overallCGPA);

    cgpaLabel->setText(QString("Overall CGPA: %1 / 4.00")
                           .arg(QString::number(overallCGPA, 'f', 2)));
    totalCreditsLabel->setText(QString("Total Credits: %1 CH")
                                   .arg(QString::number(totalCredits, 'f', 1)));
    standingLabel->setText(QString("Academic Standing: %1").arg(standing));

    saveProfileToJSON();
    QMessageBox::information(this, "Success",
                             "CGPA calculated successfully! Profile persisted "
                             "to 'student_record.json'.");
  }

  void runSimulation() {
    if (totalCredits <= 0.0) {
      calculateResults();
    }

    double targetCGPA = targetCGPASpinBox->value();
    int remSem = remainingSemSpinBox->value();
    double estCH = estCreditsSpinBox->value();

    double futureCH = remSem * estCH;
    double finalCH = totalCredits + futureCH;
    double reqTotalPts = targetCGPA * finalCH;
    double reqFuturePts = reqTotalPts - totalPoints;
    double reqGPA = reqFuturePts / futureCH;

    QString html =
        QString("<b>Current Cumulative CGPA:</b> %1 / 4.00 (%2 CH)<br>")
            .arg(QString::number(overallCGPA, 'f', 2))
            .arg(QString::number(totalCredits, 'f', 1));
    html += QString("<b>Target CGPA Goal:</b> %1 / 4.00<br><hr>")
                .arg(QString::number(targetCGPA, 'f', 2));

    if (reqGPA <= 0.0) {
      html += "<span style='color:#4ade80; font-size:16px;'>🎉 GREAT NEWS! You "
              "have already achieved or surpassed your target CGPA!</span>";
    } else if (reqGPA > 4.00) {
      double maxCGPA = (totalPoints + (4.00 * futureCH)) / finalCH;
      html += QString("<span style='color:#ef4444; font-size:16px;'>⚠️ "
                      "MATHEMATICALLY IMPOSSIBLE:</span><br>") +
              QString("To reach a %1 CGPA, you would need an average GPA of "
                      "<b>%2</b> per semester.<br>")
                  .arg(targetCGPA)
                  .arg(QString::number(reqGPA, 'f', 2)) +
              QString("💡 Maximum achievable CGPA with perfect 4.00 in all "
                      "remaining semesters: <b>%1 / 4.00</b>")
                  .arg(QString::number(maxCGPA, 'f', 2));
    } else {
      html += QString("<span style='color:#38bdf8; font-size:17px;'>📌 "
                      "REQUIRED AVERAGE GPA: <b>%1 / 4.00</b> per remaining "
                      "semester.</span><br><br>")
                  .arg(QString::number(reqGPA, 'f', 2));

      if (reqGPA >= 3.70) {
        html += "🔥 <b>STRATEGY:</b> You need mostly 'A' and 'A-' grades in "
                "all remaining courses.";
      } else if (reqGPA >= 3.30) {
        html += "👍 <b>STRATEGY:</b> You need mostly 'B+' and 'A-' grades in "
                "remaining courses.";
      } else {
        html += "✅ <b>STRATEGY:</b> Maintain steady 'B' / 'C+' performance to "
                "comfortably hit your target.";
      }
    }

    simulatorResultLabel->setText(html);
  }

  void saveProfileToJSON() {
    QJsonObject root;
    root["studentName"] = nameInput->text().trimmed();
    root["studentID"] = idInput->text().trimmed();
    root["cumulativeCGPA"] = overallCGPA;
    root["totalCumulativeCredits"] = totalCredits;
    root["academicStanding"] =
        ComsatsGradeEngine::getAcademicStanding(overallCGPA);

    QJsonArray coursesArr;
    for (int i = 0; i < coursesTable->rowCount(); ++i) {
      QJsonObject cObj;
      cObj["courseCode"] = coursesTable->item(i, 0)->text();
      cObj["creditHours"] = coursesTable->item(i, 1)->text().toDouble();
      cObj["letterGrade"] = coursesTable->item(i, 2)->text();
      cObj["gradePoints"] = coursesTable->item(i, 3)->text().toDouble();
      cObj["remark"] = coursesTable->item(i, 5)->text();
      coursesArr.append(cObj);
    }

    QJsonObject semObj;
    semObj["semesterNumber"] = 1;
    semObj["semesterGPA"] = overallCGPA;
    semObj["totalCredits"] = totalCredits;
    semObj["courses"] = coursesArr;

    QJsonArray semArr;
    semArr.append(semObj);

    root["semesters"] = semArr;
    root["courses"] = coursesArr;

    QJsonDocument doc(root);
    QString jsonPath =
        QFile::exists("data/student_record.json") || QDir("data").exists()
            ? "data/student_record.json"
            : "student_record.json";
    QFile file(jsonPath);
    if (file.open(QIODevice::WriteOnly)) {
      file.write(doc.toJson());
      file.close();
    }
  }

  void loadProfileFromJSON() {
    QString jsonPath = QFile::exists("data/student_record.json")
                           ? "data/student_record.json"
                           : "student_record.json";
    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly))
      return;

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject())
      return;

    QJsonObject root = doc.object();
    nameInput->setText(root["studentName"].toString());
    idInput->setText(root["studentID"].toString());

    coursesTable->setRowCount(0);
    if (root.contains("semesters") && root["semesters"].isArray()) {
      QJsonArray semArr = root["semesters"].toArray();
      for (const auto &semVal : semArr) {
        QJsonObject semObj = semVal.toObject();
        if (semObj.contains("courses") && semObj["courses"].isArray()) {
          QJsonArray cArr = semObj["courses"].toArray();
          for (const auto &val : cArr) {
            QJsonObject cObj = val.toObject();
            insertTableRow(
                cObj["courseCode"].toString(), cObj["creditHours"].toDouble(),
                cObj["letterGrade"].toString(), cObj["gradePoints"].toDouble(),
                cObj.contains("marks") ? cObj["marks"].toDouble() : -1.0);
          }
        }
      }
    } else if (root.contains("courses") && root["courses"].isArray()) {
      QJsonArray arr = root["courses"].toArray();
      for (const auto &val : arr) {
        QJsonObject cObj = val.toObject();
        insertTableRow(
            cObj["courseCode"].toString(), cObj["creditHours"].toDouble(),
            cObj["letterGrade"].toString(), cObj["gradePoints"].toDouble(),
            cObj.contains("marks") ? cObj["marks"].toDouble() : -1.0);
      }
    }
  }
};

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  CGPACalculatorWindow window;
  window.show();

  return app.exec();
}

#include "main_gui.moc"
