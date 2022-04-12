# TASToolKitEditor
A lightweight .csv visualizer/editor meant to assist in the development of Mario Kart Wii Tool-Assisted Speedruns.

![image](https://user-images.githubusercontent.com/16770560/162370209-30066f00-5f80-4dfc-9055-110dd92bc101.png)

## TODO
- Allow DataGridView to resize along with window size
- Handle File>Open operation when a file is already opened in the program
- SaveToFile can fail if file is in use by other program. Determine what to do here. Should we warn user and just keep attempting to save on each input change?
- Left-click and drag for mass toggle
- Right-click and drag for mass write (starting cell value is written to all cells dragged over)
- Middle-click and drag a stick cell to change value? Is this useful?
- Branching
- Bookmarks
- Adding/deleting rows

## Completed Features
- Basic text editing functionality
- Auto file re-load on checksum change detected between cached and local file
- Toggle between 0 and 7 centering
- Toggle button cells even if you click outside of the actual checkbox but within the cell
- Undo/redo
- Shortcuts for File Open, Undo, Redo
