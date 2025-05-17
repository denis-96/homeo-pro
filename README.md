# HomeoPro - Homeopathy Practice Management System

![Qt](https://img.shields.io/badge/Qt-6.5.1-green.svg)
![C++](https://img.shields.io/badge/C++-17-blue.svg)

HomeoPro is a professional desktop application designed for homeopathic doctors to manage patient care and treatment analysis through advanced repertorization tools and comprehensive patient records management.

## Features

### 1. Repertorization Management
- Create and edit repertorization cases
- Add/modify rubrics (symptom categories) with corresponding homeopathic drugs
- Intelligent sorting algorithm that prioritizes drugs based on rubric matches
- Multi-symptom analysis with dynamic drug effectiveness evaluation
- Save/Load repertorization cases for future reference

### 2. Patient Card System
- Digital patient records management
- Complete patient profiles including:
  - Personal information
  - Medical history (Anamnesis)
  - Treatment plans and progress tracking
  - Prescription history
- Chronological treatment documentation

## Installation

### Prerequisites
- Qt Framework (6.5.1 or newer)
- C++17 compatible compiler
- CMake (3.21+ recommended)

### Building from Source
1. Clone repository:

   git clone https://github.com/yourusername/homeopro.git
   cd homeopro

2. Create build directory:

   mkdir build && cd build

3. Configure with CMake:

   cmake -DCMAKE_PREFIX_PATH=/path/to/qt ..

4. Build project:

   cmake --build .

5. Run application:

   ./HomeoPro


## Usage

### Repertorization Workflow
1. Create New Repertorization Case
2. Add relevant rubrics from library
3. Input specific symptoms and modalities
4. Review automatically sorted drug suggestions
5. Save case or export results to patient record

### Patient Management
1. Create New Patient Card
2. Fill in personal and medical history
3. Attach relevant repertorization cases
4. Track treatment progress through timeline
5. Generate periodic health reports

## Dependencies
- Qt Core
- Qt Widgets
- STL


## Support & Contact
For technical issues or feature requests:
- Email: denn.bargan@gmail.com

