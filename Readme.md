# Writing the markdown content into a file

md_content = """
# File Sorting System Project

## 🚀 Overview

Welcome to the **File Sorting System Project**! This program is designed to help you organize your files based on their extensions and metadata. By using a custom hash table with binary search trees, it scans directories recursively, organizes files by extension, and allows you to move them into neatly categorized subdirectories.

### Key Features:
- **Recursive Directory Scanning**: Efficiently scans directories and subdirectories.
- **File Sorting by Extensions**: Uses hash tables and binary search trees to organize files by their extensions.
- **Sorting and Moving**: After organizing, you can sort and move files into a new folder on your desktop for better management.
- **User-Friendly Interface**: Provides prompts for interaction, making it simple to use.

---

## 🛠 Installation Instructions

### Prerequisites:
1. **C++17 or later** compatible compiler (e.g., GCC, Clang, MSVC).
   - **C++17's filesystem library** is used in the code, so a compiler supporting **C++17** or later is a must.
   
2. **C++17's filesystem library** should be supported by your compiler.

### Steps to Install:
1. **Clone or Download the Repository**:
   - Clone the repository or download the files to your local machine.

2. **Open in IDE/Editor**:
   - Open the project folder in your preferred IDE or text editor.

3. **Compile the Program**:
   - Open a terminal or use your IDE's build system.
   - Compile the program using the following command:
     ```bash
     g++ -std=c++17 -o file_sorter main.cpp
     ```
   - Ensure your compiler supports **C++17** or later for filesystem library support.

4. **Run the Program**:
   - After compiling, run the program by executing `file_sorter`.

---

## 🖥 Full Usage Guide

### 1. Running the Program:

After the program has been compiled, you can run the executable file. Upon running, the program will prompt you to enter the **path of the root directory** that you want to scan for files. This directory can contain other subdirectories as well, as the program will recursively scan all files in the directory tree.

Example:
```bash
Enter the path of the root directory to scan: /path/to/your/directory
