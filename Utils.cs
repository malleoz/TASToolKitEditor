using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Windows.Forms;

namespace TASToolKitEditor
{
    public partial class TASToolKitEditorForm : Form
    {
        /*******************
         *UTILITY FUNCTIONS*
         *******************/

        /// <summary>
        /// Based on stick data, does the stick value fall out-of-range of a particular centering?
        /// </summary>
        /// <returns>true if the centering was determined and changes the value of m_file7Centered if true</returns>
        private bool ableToDiscern7Centering(int iValue, out bool? file7Centered)
        {
            bool bCenter7 = iValue > 7;
            bool bCenter0 = iValue < 0;
            if (bCenter0 || bCenter7)
            {
                file7Centered = bCenter7 ? true : false;
                centered7Button.Checked = bCenter7 ? true : false;
                centered0Button.Checked = bCenter0 ? true : false;
                return true;
            }

            file7Centered = null;
            return false;
        }

        private void addActionToStack(CellEditAction action, InputFile info)
        {
            // Undos and Redos are handled in performUndoRedo
            if (m_curOpType != EOperationType.Normal)
            {
                m_curOpType = EOperationType.Normal;
                return;
            }

            if (info.m_redoStack.Count > 0)
                addActionToStackWithNonEmptyRedo(action, info);
            else
                info.m_undoStack.Push(action);
        }

        private void addActionToStackWithNonEmptyRedo(CellEditAction action, InputFile info)
        {
            // Since an action has been performed, we want to clear the redo stack unless the action matches the top of the redo stack
            CellEditAction redoTop = info.m_redoStack.Peek();
            redoTop.FlipValues();

            if (action.Equals(redoTop))
            {
                // User performed the action specified in the top of redo stack.
                // Basically this counts as a redo, so move the action from redo stack to undo stack
                action = info.m_redoStack.Pop();
                info.m_undoStack.Push(action);
            }
            else
            {
                // e.g. user changed cell (0,2) from 0 to 1, then un-did, then changed cell (0,2) from 0 to 2
                // We must delete the redo stack before pushing
                info.m_redoStack.Clear();
                info.m_undoStack.Push(action);
            }
        }

        private void addColumn(DataGridView gridView, string name, bool buttonCol)
        {
            DataGridViewColumn column;
            if (buttonCol)
            {
                DataGridViewCheckBoxColumn cbColumn = new DataGridViewCheckBoxColumn();
                cbColumn.TrueValue = 1;
                cbColumn.FalseValue = 0;
                cbColumn.Width = 25;
                cbColumn.ReadOnly = true;
                column = cbColumn;
            }
            else
            {
                column = new DataGridViewTextBoxColumn();
                column.Width = 40;
            }

            column.HeaderText = name;

            gridView.Columns.Add(column);
        }

        private void addColumnsAndHeaders(InputFile info)
        {
            // Add the column to display framecount for each row
            DataGridView gridView = info.m_dataGridView;
            addFrameCountColumn(gridView);

            addColumn(gridView, "A", true);
            addColumn(gridView, "B", true);
            addColumn(gridView, "L", true);
            addColumn(gridView, "L-R", false);
            addColumn(gridView, "U-D", false);
            addColumn(gridView, "UDLR", false);
        }

        /// <summary>
        /// Take data from m_curFileData and write to the inputGridView
        /// </summary>
        private void addDataFromCache(InputFile info)
        {
            int sourceFrameCount = info.m_fileData.Count;
            IEnumerable<int> frames = Enumerable.Range(0, sourceFrameCount);
            IEnumerable<int> columns = Enumerable.Range(0, NUM_INPUT_COLUMNS);

            for (int i = 0; i < sourceFrameCount; i++)
            {
                int frameCount = i + 1;
                // Write framecount to first column
                info.m_dataGridView.Rows[i].Cells[0].Value = frameCount;

                for (int j = 0; j < NUM_INPUT_COLUMNS; j++)
                {
                    info.m_dataGridView.Rows[i].Cells[j + ADJUST_FOR_FRAMECOUNT_COLUMN].Value = info.m_fileData[i][j];
                }
            }
        }

        private void addFrameCountColumn(DataGridView gridView)
        {
            DataGridViewTextBoxColumn column = new DataGridViewTextBoxColumn();
            column.HeaderText = "Frame";
            column.Width = 50;
            column.ReadOnly = true;
            column.DefaultCellStyle.BackColor = Color.LightGray;

            gridView.Columns.Add(column);
        }

        /// <summary>
        /// Adds a row to the inputGridView for every frame in the cached data
        /// </summary>
        private void addRowsAndHeaders(InputFile info)
        {
            for (int i = 0; i < info.m_fileData.Count; i++)
            {
                DataGridViewRow row = new DataGridViewRow();
                row.HeaderCell.Value = i;

                info.m_dataGridView.Rows.Add(row);
            }
        }

        private void adjustGuiShow(InputFile info)
        {
            filesLoaded++;

            int x = (filesLoaded == 1) ? firstGridViewlocationX : secondGridViewlocationX;

            info.m_gridViewLabel.Location = new Point(x, GridViewLabelY);
            info.m_dataGridView.Location = new Point(x, gridViewLocationY);

            info.m_gridViewLabel.Visible = true;
            info.m_dataGridView.Visible = true;

            if (filesLoaded == 2)
                Width = secondGridViewlocationX + gridViewWidth + widthBetweenGridViews + DATAGRIDVIEW_PADDING;
        }

        private void applyGridViewFormatting(InputFile info)
        {
            // This must be performed *after* adding columns/rows otherwise effect is lost
            info.m_dataGridView.AllowUserToResizeColumns = false;
            info.m_dataGridView.AllowUserToResizeRows = false;
            info.m_dataGridView.RowHeadersVisible = false;
        }

        private void cellClick(DataGridViewCellMouseEventArgs e, InputFile info)
        {
            int colIdx = e.ColumnIndex;
            if (!BUTTON_COLUMNS.Contains(colIdx - ADJUST_FOR_FRAMECOUNT_COLUMN))
                return;

            int rowIdx = e.RowIndex;

            // Row index is -1 if clicking on column headers
            if (rowIdx < 0)
                return;

            int cellVal = int.Parse(info.m_dataGridView.Rows[rowIdx].Cells[colIdx].Value.ToString());
            // TODO: Crashes when clicking column header

            info.m_dataGridView.Rows[rowIdx].Cells[colIdx].Value = (cellVal == 0) ? 1 : 0;
            info.m_dataGridView.RefreshEdit();
        }

        private void centerInputs(bool centerOn7)
        {
            // Adjustment to make (either from 7-center to 0-center or vice versa)
            int centerOffset = centerOn7 ? 7 : -7;

            centerInputs(centerOffset, ghostFile);
            centerInputs(centerOffset, playerFile);

            m_files7Centered = centerOn7;
        }

        private void centerInputs(int centerOffset, InputFile info)
        {
            // It's easier (and probably more efficient) to iterate the cached file
            for (int i = 0; i < info.m_fileData.Count; i++)
            {
                for (int j = 3; j < 5; j++)
                {
                    int centeredInput = info.m_fileData[i][j] + centerOffset;
                    info.m_fileData[i][j] = centeredInput; // Write to cached file here to prevent a ridiculous amount of file I/O
                    info.m_dataGridView.Rows[i].Cells[j + ADJUST_FOR_FRAMECOUNT_COLUMN].Value = centeredInput;
                }
            }

            // Now re-save file to disk
            saveToFile(info);
        }

        public void clearAndReloadFile(object sender, FileSystemEventArgs e)
        {
            // Determine if one of the loaded files was modified
            bool bIsGhostFile = (e.FullPath == ghostFile.m_filePath);
            bool bIsPlayerFile = (e.FullPath == playerFile.m_filePath);
            if (!bIsGhostFile && !bIsPlayerFile)
                return;

            // Schedule on main thread
            Invoke(new Action(() =>
            {
                if (bIsGhostFile)
                    clearAndReloadFile(ghostFile);
                else
                    clearAndReloadFile(playerFile);
            }));
        }

        public void clearAndReloadFile(InputFile info)
        {
            info.m_fileData.Clear();
            info.m_dataGridView.Rows.Clear();
            info.m_gridViewLoaded = false;

            if (reOpenFile(info))
            {
                addRowsAndHeaders(info);
                addDataFromCache(info);
            }
            else
            {
                showError("Unable to re-open file when detecting change... Future behavior is undefined...");
            }

            info.m_gridViewLoaded = true;
        }

        private void clearData(InputFile info)
        {
            info.m_filePath = String.Empty;
            info.m_fileData.Clear();
        }

        private void clearInputFile(InputFile file)
        {
            file.m_gridViewLoaded = false;
            file.m_filePath = String.Empty;
            file.m_fileData.Clear();
            file.m_redoStack.Clear();
            file.m_undoStack.Clear();
        }

        private void closeFile(InputFile file)
        {
            // Clear the grid, the InputFile class, and adjust menu items accordingly
            file.m_dataGridView.Rows.Clear();
            file.m_dataGridView.Columns.Clear();
            file.m_dataGridView.Refresh();
            file.m_dataGridView.Visible = false;
            file.m_gridViewLabel.Visible = false;
            filesLoaded--;
            scrollTogetherMenuItem.Enabled = false;
            scrollTogetherMenuItem.Checked = false;

            // If other file is still open, move to be to left-side of program window
            if (filesLoaded > 0)
            {
                InputFile sourceFile;
                if (file == ghostFile)
                    sourceFile = playerFile;
                else
                    sourceFile = ghostFile;

                sourceFile.m_dataGridView.Location = new Point(firstGridViewlocationX, gridViewLocationY);
                sourceFile.m_gridViewLabel.Location = new Point(firstGridViewlocationX, GridViewLabelY);
            }

            Width = firstGridViewlocationX + gridViewWidth + firstGridViewRightPadding;

            clearInputFile(file);

            file.m_undoMenuItem.Enabled = false;
            file.m_redoMenuItem.Enabled = false;
            file.m_rootMenuItem.Visible = false;
        }

        private void fileOpen(InputFile info)
        {
            if (openFile(info))
                loadDataToGridView(info);
        }

        /// <summary>
        /// Given a certain column of the .csv, determine the maximum value that is deemed correct.
        /// </summary>
        /// <returns>For buttons, this is 1. For the DPad this is a 4. For the stick, this varies depending on 0 or 7 centering.</returns>
        private int getLargestAcceptedValue(int colIndex, int iValue)
        {
            if (DPAD_COLUMN == colIndex)
                return 4;
            if (BUTTON_COLUMNS.Contains(colIndex))
                return 1;
            if (m_files7Centered == null && !ableToDiscern7Centering(iValue, out m_files7Centered))
                return 7;
            return m_files7Centered.Value ? 14 : 7;
        }

        /// <summary>
        /// Given a certain column of the .csv, determine the minimum value that is deemed correct.
        /// </summary>
        /// <returns>For buttons, this returns 0. For the stick, this varies depending on 0 or 7 centering.</returns>
        private int getSmallestAcceptedValue(int colIndex, int iValue)
        {
            if (BUTTON_COLUMNS.Contains(colIndex) || DPAD_COLUMN == colIndex)
                return 0;
            if (m_files7Centered == null && !ableToDiscern7Centering(iValue, out m_files7Centered))
                return 0;
            return m_files7Centered.Value ? 0 : -7;
        }

        private void inputChanged(DataGridViewCellEventArgs e, InputFile info)
        {
            if (inputChangeChecksPassed(e, info))
                saveToFile(info);
        }

        /// <summary>
        /// Perform various checks to make sure that we want to consider the file's inputs changed
        /// (e.g. table isn't loading up, user didn't just press enter on a cell, user typed a number)
        /// </summary>
        /// <returns>true if checks pass and input has changed in cache</returns>
        private bool inputChangeChecksPassed(DataGridViewCellEventArgs e, InputFile info)
        {
            // Happens when data is being loaded from file... We don't want to consider the cell data changed!
            if (!info.m_gridViewLoaded)
                return false;

            // Happens when file is being re-loaded and the frameCount column is written over
            if (e.ColumnIndex == 0)
                return false;

            // To prevent unnecessary File I/O let's double-check that the value actually changed
            int inputNew;
            bool inputAccepted = int.TryParse(info.m_dataGridView.Rows[e.RowIndex].Cells[e.ColumnIndex].Value.ToString(), out inputNew);
            int inputCurFile = info.m_fileData[e.RowIndex][e.ColumnIndex - ADJUST_FOR_FRAMECOUNT_COLUMN];

            // The value is non-numeric, or the value changed but doesn't conform with formatting rules... Reset to whatever it was before
            if (!inputAccepted || !valueFormattedProperly(inputNew, e))
            {
                info.m_dataGridView.Rows[e.RowIndex].Cells[e.ColumnIndex].Value = inputCurFile;
                return false;
            }

            // The value did not change
            if (inputNew == inputCurFile)
                return false;

            // Change the value in the cached file
            info.m_fileData[e.RowIndex][e.ColumnIndex - ADJUST_FOR_FRAMECOUNT_COLUMN] = inputNew;

            // Write to one of the stacks
            CellEditAction action = new CellEditAction(e.RowIndex, e.ColumnIndex, inputCurFile, inputNew);
            addActionToStack(action, info);

            // Adjust menu buttons
            info.m_undoMenuItem.Enabled = info.m_undoStack.Count > 0;
            info.m_redoMenuItem.Enabled = info.m_redoStack.Count > 0;

            return true;
        }

        private void loadDataToGridView(InputFile info)
        {
            addColumnsAndHeaders(info);
            addRowsAndHeaders(info);
            addDataFromCache(info);
            applyGridViewFormatting(info);

            info.m_gridViewLoaded = true;
            info.m_rootMenuItem.Visible = true;
            info.m_closeMenuItem.Enabled = true;

            adjustGuiShow(info);
            scrollTogetherMenuItem.Enabled = true;
        }

        private void onCenterClick(ToolStripMenuItem centerTo, ToolStripMenuItem centerFrom, bool bCenterOn7)
        {
            if (centerTo.Checked)
                return;

            centerTo.Checked = true;
            centerFrom.Checked = false;

            centerInputs(bCenterOn7);
        }

        private bool openFile(InputFile info)
        {
            using (OpenFileDialog ofd = new OpenFileDialog())
            {
                ofd.Title = "Open TAS Toolkit File";
                ofd.Filter = ".csv files (*.csv)|*.csv";

                if (ofd.ShowDialog() != DialogResult.OK)
                    return false;

                if (info.m_filePath != string.Empty)
                    closeFile(info);

                info.m_filePath = ofd.FileName;

                Stream fileStream;

                try
                {
                    fileStream = ofd.OpenFile();
                }
                catch (IOException)
                {
                    string errMsg = String.Format("Unable to open {0} as it may be opened by another program.", info.m_filePath);
                    showErrorAndClearData(errMsg, info);
                    return false;
                }

                using (StreamReader reader = new StreamReader(fileStream))
                {
                    if (!parseFile(reader, info))
                        return false;
                }
            }

            if (m_fileSystemWatcher.Path == String.Empty)
                watchFileSystem(info);

            return true;
        }

        private bool parseFile(StreamReader reader, InputFile info)
        {
            int frameCount = 1;
            for (; !reader.EndOfStream; frameCount++)
            {
                var line = reader.ReadLine();
                var values = line.Split(',');

                if (!valuesFormattedProperly(values))
                {
                    // It's possible there is a centering mismatch between the ghost and player files
                    string errMsg = String.Format("There is an error on frame {0}\n\nFile: {1}", frameCount, info.m_filePath);
                    errMsg += "\n\nIf you're opening a second input file, it's possible that there is a 0/7 centering mismatch.";

                    showErrorAndClearData(errMsg, info);
                    return false;
                }

                // Parsed successfully, add to cached data list
                List<int> frameData = new(Array.ConvertAll(values, v => int.Parse(v)));

                info.m_fileData.Add(frameData);
            }
            return true;
        }

        private void performUndoRedo(InputFile info, EOperationType opType)
        {
            if ((opType == EOperationType.Undo && info.m_undoStack.Count == 0) ||
                (opType == EOperationType.Redo && info.m_redoStack.Count == 0))
            {
                return;
            }

            m_curOpType = opType;

            CellEditAction action = (m_curOpType == EOperationType.Undo) ? info.m_undoStack.Pop() : info.m_redoStack.Pop();

            action.FlipValues();

            if (m_curOpType == EOperationType.Undo)
                info.m_redoStack.Push(action);
            else
                info.m_undoStack.Push(action);

            int rowIdx = action.m_cellRowIdx;
            int colIdx = action.m_cellColIdx;
            info.m_dataGridView.Rows[rowIdx].Cells[colIdx].Value = action.m_cellCurVal;
            info.m_dataGridView.RefreshEdit();

            // Adjust menu items accordingly
            info.m_redoMenuItem.Enabled = info.m_redoStack.Count > 0;
            info.m_undoMenuItem.Enabled = info.m_undoStack.Count > 0;

            // Change scroll to show the row that was just edited
            info.m_dataGridView.FirstDisplayedScrollingRowIndex = rowIdx;

            saveToFile(info);
        }

        private void processHotkeys(Keys keyData)
        {
            bool undoGhost = keyData.HasFlag(Keys.Z) && keyData.HasFlag(Keys.Control) && keyData.HasFlag(Keys.Shift);
            bool undoPlayer = keyData.HasFlag(Keys.Z) && keyData.HasFlag(Keys.Control);
            bool redoGhost = keyData.HasFlag(Keys.Y) && keyData.HasFlag(Keys.Control) && keyData.HasFlag(Keys.Shift);
            bool redoPlayer = keyData.HasFlag(Keys.Y) && keyData.HasFlag(Keys.Control);
            bool openGhost = keyData.HasFlag(Keys.O) && keyData.HasFlag(Keys.Control) && keyData.HasFlag(Keys.Shift);
            bool openPlayer = keyData.HasFlag(Keys.O) && keyData.HasFlag(Keys.Control);
            bool closeGhost = keyData.HasFlag(Keys.Escape) && keyData.HasFlag(Keys.Shift);
            bool closePlayer = keyData.HasFlag(Keys.Escape);

            if (undoGhost)
                performUndoRedo(ghostFile, EOperationType.Undo);
            else if (undoPlayer)
                performUndoRedo(playerFile, EOperationType.Undo);
            else if (redoGhost)
                performUndoRedo(ghostFile, EOperationType.Redo);
            else if (redoPlayer)
                performUndoRedo(playerFile, EOperationType.Redo);
            else if (openGhost)
                openFile(ghostFile);
            else if (openPlayer)
                openFile(playerFile);
            else if (closeGhost)
                closeFile(ghostFile);
            else if (closePlayer)
                closeFile(playerFile);
        }

        private bool reOpenFile(InputFile info)
        {
            using (StreamReader reader = new StreamReader(info.m_filePath))
            {
                if (!parseFile(reader, info))
                    return false;
            }
            return true;
        }

        private void saveToFile(InputFile file)
        {
            m_fileSystemWatcher.EnableRaisingEvents = false;
            try
            {
                using (StreamWriter writer = new StreamWriter(file.m_filePath, false))
                {
                    // for each frame
                    foreach (List<int> curFileFrameData in file.m_fileData)
                    {
                        string sFrameData = string.Empty;

                        foreach (int frameData in curFileFrameData)
                        {
                            sFrameData += frameData.ToString() + COMMA_SEPARATOR;
                        }

                        // Last column does not need to be followed by comma
                        // Check out this sick ass short-hand way to substring without the last char
                        sFrameData = sFrameData[0..^1];

                        writer.WriteLine(sFrameData);
                    }
                }
            }
            catch (Exception e)
            {
                System.Diagnostics.Debug.WriteLine(e.ToString());
            }

            m_fileSystemWatcher.EnableRaisingEvents = true;
        }

        private void scrollTogether(ScrollEventArgs e, DataGridView playerInputGridView)
        {
            if (!m_scrollTogether)
                return;

            playerInputGridView.FirstDisplayedScrollingRowIndex = e.NewValue;
        }

        private static void showError(string errMsg)
        {
            MessageBox.Show(errMsg, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        private void showErrorAndClearData(string errMsg, InputFile info)
        {
            showError(errMsg);
            clearData(info);
        }

        private void swapPlayerAndGhost()
        {
            // No point in swapping if both files aren't open
            if (playerFile.m_filePath == string.Empty || ghostFile.m_filePath == string.Empty)
                return;

            // All we need to do is take player data and swap with ghost data
            List<List<int>> tempData = playerFile.m_fileData;
            playerFile.m_fileData = ghostFile.m_fileData;
            ghostFile.m_fileData = tempData;

            saveToFile(playerFile);
            saveToFile(ghostFile);

            clearAndReloadFile(playerFile);
            clearAndReloadFile(ghostFile);
        }

        /// <summary>
        /// When user sets a cell value, perform a formatting check to see if input is acceptable
        /// </summary>
        /// <returns>true if formatting check passes, false if not</returns>
        private bool valueFormattedProperly(int value, DataGridViewCellEventArgs e)
        {
            int colIndex = e.ColumnIndex;
            int smallestAcceptedVal = getSmallestAcceptedValue(colIndex - ADJUST_FOR_FRAMECOUNT_COLUMN, value);
            int largestAcceptedVal = getLargestAcceptedValue(colIndex - ADJUST_FOR_FRAMECOUNT_COLUMN, value);

            return value >= smallestAcceptedVal && value <= largestAcceptedVal;
        }

        /// <summary>
        /// Checks formatting rules for all values of a given frame during parsing
        /// </summary>
        /// <returns>true if all formatting checks pass for the given frame's data</returns>
        private bool valuesFormattedProperly(string[] values)
        {
            // There should be 6 comma-separated values per line
            if (values.Length != NUM_INPUT_COLUMNS)
                return false;

            // Certain columns have restricted values
            if (!valueRestrictionsAreMet(values))
                return false;

            // Place other error checks here

            return true;
        }

        /// <summary>
        /// Certain columns have limited acceptable values (e.g. buttons can only be 0/1)
        /// </summary>
        /// <returns>true if restrictions are satisfied, false if not satisfied</returns>
        private bool valueRestrictionsAreMet(string[] values)
        {
            for (int i = 0; i < values.Length; i++)
            {
                int iValue;
                bool parseSuccessful = int.TryParse(values[i], out iValue);

                if (!parseSuccessful)
                    return false;

                int smallestAcceptedVal = getSmallestAcceptedValue(i, iValue);
                int largestAcceptedVal = getLargestAcceptedValue(i, iValue);

                if (iValue > largestAcceptedVal || iValue < smallestAcceptedVal)
                    return false;
            }

            return true;
        }

        private void watchFileSystem(InputFile info)
        {
            m_fileSystemWatcher.Path = Path.GetDirectoryName(info.m_filePath);
            m_fileSystemWatcher.NotifyFilter = NotifyFilters.LastWrite;
            m_fileSystemWatcher.Filter = "*.csv";
            m_fileSystemWatcher.Changed += new FileSystemEventHandler(clearAndReloadFile);
            m_fileSystemWatcher.EnableRaisingEvents = true;

        }
    }

    #region Support Classes
    /// <summary>
    /// This class contains all distinct information for a given source input file.
    /// This includes the path, hash, associated menu buttons, and DataGridView.
    /// </summary>
    public class InputFile
    {
        public InputFile(DataGridView dataGridView, ToolStripMenuItem rootMenu, ToolStripMenuItem undoMenu, ToolStripMenuItem redoMenu, ToolStripMenuItem closeMenu, Label label)
        {
            m_filePath = String.Empty;
            m_fileData = new List<List<int>>();
            m_gridViewLoaded = false;
            m_undoStack = new Stack<CellEditAction>();
            m_redoStack = new Stack<CellEditAction>();
            m_dataGridView = dataGridView;
            m_rootMenuItem = rootMenu;
            m_undoMenuItem = undoMenu;
            m_redoMenuItem = redoMenu;
            m_closeMenuItem = closeMenu;
            m_gridViewLabel = label;
        }

        public string m_filePath;
        public List<List<int>> m_fileData; // TODO: Vectorized implementation for better iteration on centerInputs()?
        public bool m_gridViewLoaded;
        public Stack<CellEditAction> m_undoStack;
        public Stack<CellEditAction> m_redoStack;
        public DataGridView m_dataGridView;
        public ToolStripMenuItem m_rootMenuItem;
        public ToolStripMenuItem m_undoMenuItem;
        public ToolStripMenuItem m_redoMenuItem;
        public ToolStripMenuItem m_closeMenuItem;
        public Label m_gridViewLabel;
    }

    /// <summary>
    /// A class to represent a cell modification action. This is used to undo/redo actions concisely
    /// </summary>
    public class CellEditAction
    {
        public CellEditAction(int row, int col, int prevVal, int nextVal)
        {
            m_cellRowIdx = row;
            m_cellColIdx = col;
            m_cellPrevVal = prevVal;
            m_cellCurVal = nextVal;
        }

        public int m_cellRowIdx;
        public int m_cellColIdx;
        public int m_cellPrevVal;
        public int m_cellCurVal;

        internal void FlipValues()
        {
            // Swap prev and cur
            int temp = m_cellCurVal;
            m_cellCurVal = m_cellPrevVal;
            m_cellPrevVal = temp;
        }

        internal bool Equals(CellEditAction rhs)
        {
            bool bRowsEqual = m_cellRowIdx == rhs.m_cellRowIdx;
            bool bColsEqual = m_cellColIdx == rhs.m_cellColIdx;
            bool bValuesEqual = m_cellCurVal == rhs.m_cellCurVal;

            return bRowsEqual && bColsEqual && bValuesEqual;
        }
    }
    #endregion // Support Classes

    #region Enums
    public enum EOperationType
    {
        Normal,
        Undo,
        Redo,
    };
    #endregion
}