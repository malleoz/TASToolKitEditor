# TASToolKitEditor
A lightweight .csv visualizer/editor meant to assist in the development of Mario Kart Wii Tool-Assisted Speedruns. Developed in C++ using the Qt framework.

![image](https://user-images.githubusercontent.com/16770560/164950864-200bba6a-2d9b-44ee-8c14-9cf2349f2c36.png)

## TODO
- Left-click and drag for mass toggle
- Right-click and drag for mass write (starting cell value is written to all cells dragged over)
- Middle-click and drag a stick cell to change value? Is this useful?
- Branching (with GUI)
- ![image](https://user-images.githubusercontent.com/16770560/163035593-53df6bcf-7caa-4204-8d6f-649fc715de64.png)
- Bookmarks
- Adding/deleting rows

## Completed Features
- Inserting rows
- Handle File>Open operation when a file is already opened in the program
- Ghost and Player views
- FileSystemWatcher to detect file changes, rather than a hash
- Basic text editing functionality
- Auto file re-load on checksum change detected between cached and local file
- Toggle between 0 and 7 centering
- Toggle button cells even if you click outside of the actual checkbox but within the cell
- Undo/redo
- Shortcuts for File Open, Undo, Redo
- Allow DataGridView to resize along with window size
