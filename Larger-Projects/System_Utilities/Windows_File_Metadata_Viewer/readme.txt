Windows File Metadata Viewer - Usage Guide
==========================================

Overview
--------
The Windows File Metadata Viewer is a command-line tool for displaying detailed information about files and directories on your system. It provides metadata, size analysis, timestamps, attributes, type identification, and (optionally) security information.

Usage
-----
To run the program, use the following syntax:

    file_metadata_viewer.exe [options] <filepath>

Where <filepath> is the path to the file or directory you want to analyze.

Options
-------

  -h, --help
      Show this help message and exit. When this option is used, no file analysis is performed.

  -v, --verbose
      Enable verbose mode. In verbose mode, the program will display additional security information about the file, such as owner, group, and access permissions.

  -i, --interactive
      Start the program in interactive mode. You will be prompted to enter file paths one at a time, and can analyze multiple files in a single session. Type 'exit' or 'quit' to leave interactive mode.

Examples
--------

  1. Analyze a specific file:
         file_metadata_viewer.exe "C:\Windows\System32\notepad.exe"

  2. Analyze a file with verbose (security) information:
         file_metadata_viewer.exe -v "C:\Users\Alice\Documents\report.pdf"
         file_metadata_viewer.exe --verbose "C:\Users\Alice\Documents\report.pdf"

  3. Show help:
         file_metadata_viewer.exe -h
         file_metadata_viewer.exe --help

  4. Use interactive mode:
         file_metadata_viewer.exe -i
         file_metadata_viewer.exe --interactive

  5. Analyze a directory:
         file_metadata_viewer.exe "C:\Users\Alice\Documents"

Notes
-----
- If you do not provide a file path, the program will display an error and show the help message.
- You can combine options, e.g., `file_metadata_viewer.exe -v -i`
- File paths with spaces should be enclosed in quotes.
- The program supports both files and directories, but some features (like size analysis) may not apply to directories.

Features
--------
- Comprehensive file information display
- Detailed size analysis with multiple units
- Complete timestamp information
- File attribute detection
- File type identification
- Security information (owner, group, permissions) in verbose mode
- Support for both files and directories
- Interactive mode for analyzing multiple files in one session
