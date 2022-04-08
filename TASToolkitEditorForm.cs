using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Windows.Forms;

// TODO:
// 1. What happens if you open a file while file is already open? We need to flush the table
// 2. SaveToFile might fail if file is in use by other process... We would need to fall back if so
// 3. Turn button cells to checkboxes
// 4. Click and drag along button cells to do mass toggle
// 5. Right click and drag to copy value from start of click to end of click

namespace TASToolKitEditor
{
    public partial class TASToolKitEditorForm : Form
    {
        public TASToolKitEditorForm()
        {
            InitializeComponent();

            m_curFilePath = string.Empty;
            m_curFileHash = Array.Empty<Byte>();
            m_curFileData = new List<List<int>>();
            m_gridViewLoaded = false;
            m_file7Centered = null;
        }

        /// <summary>
        ///  Event triggered when the user hits File>Open.
        ///  Tries to parse a selected .csv file and then loads the data to the DataGridView table.
        /// </summary>
        private void onClickFileOpen(object sender, EventArgs e)
        {
            if (openFile())
                loadDataToGridView();
        }

        /// <summary>
        /// Prompts user to select a .csv file and attempts to open the selected file for parsing.
        /// </summary>
        /// <returns>Returns true if the file was successfully opened, read, and parsed</returns>
        private bool openFile()
        {
            using (OpenFileDialog ofd = new OpenFileDialog())
            {
                ofd.Title = "Open TAS Toolkit File";
                ofd.Filter = ".csv files (*.csv)|*.csv";

                if (ofd.ShowDialog() != DialogResult.OK)
                    return false;

                m_curFilePath = ofd.FileName;

                if (!hashCachedFile())
                {
                    string errMsg = String.Format("Unable to open {0} as it may be opened by another program.", m_curFilePath);
                    showErrorAndClearData(errMsg);
                    return false;
                }

                Stream fileStream;

                try
                {
                    fileStream = ofd.OpenFile();
                }
                catch (IOException)
                {
                    string errMsg = String.Format("Unable to open {0} as it may be opened by another program.", m_curFilePath);
                    showErrorAndClearData(errMsg);
                    return false;
                }

                using (StreamReader reader = new StreamReader(fileStream))
                {
                    if (!parseFile(reader))
                        return false;
                }
            }

            return true;
        }

        private byte[] getHash()
        {
            MD5 md5 = MD5.Create();
            try
            {
                using (var stream = File.OpenRead(m_curFilePath))
                {
                    return md5.ComputeHash(stream);
                }
            }
            catch (IOException)
            {
            }

            return Array.Empty<Byte>();
        }

        private bool hashCachedFile()
        {
            byte[] hash = getHash();

            if (areHashesEqual(hash, Array.Empty<byte>()))
                return false;

            m_curFileHash = hash;
            return true;
        }

        private bool areHashesEqual(byte[] hash1, byte[] hash2)
        {
            if (hash1.Length != hash2.Length)
                return false;

            for (int i = 0; i < hash1.Length; i++)
            {
                if (hash1[i] != hash2[i])
                    return false;
            }
            return true;
        }

        /// <summary>
        /// Simple function to display an errMsg in a MessageBox, but also clear the cache for the current file.
        /// </summary>
        private void showErrorAndClearData(string errMsg)
        {
            showError(errMsg);
            clearData();
        }

        private void showError(string errMsg)
        {
            MessageBox.Show(errMsg, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        /// <summary>
        /// Clear cached data
        /// </summary>
        private void clearData()
        {
            m_curFilePath = String.Empty;
            Array.Clear(m_curFileHash, 0, m_curFileHash.Length);
            m_curFileData.Clear();
        }

        /// <summary>
        /// Attempts to parse the provided .csv file
        /// </summary>
        /// <returns>true if the file conforms to the formatting restraints</returns>
        private bool parseFile(StreamReader reader)
        {
            int frameCount = 1;
            for (; !reader.EndOfStream; frameCount++)
            {
                var line = reader.ReadLine();
                var values = line.Split(',');

                if (!valuesFormattedProperly(values))
                {
                    string errMsg = String.Format("There is an error on frame {0}\n\nFile: {1}", frameCount, m_curFilePath);
                    showErrorAndClearData(errMsg);
                    return false;
                }

                // Parsed successfully, add to cached data list
                List<int> frameData = new(Array.ConvertAll(values, v => int.Parse(v)));
                m_curFileData.Add(frameData);
            }
            return true;
        }

        /// <summary>
        /// Given a certain column of the .csv, determine the minimum value that is deemed correct.
        /// For buttons, this is a 0. For the stick, this varies depending on 0 or 7 centering.
        /// </summary>
        /// <returns>the smallest acceptable integer value for the given column</returns>
        private int getSmallestAcceptedValue(int colIndex, int iValue)
        {
            if (BUTTON_COLUMNS.Contains(colIndex))
                return 0;
            if (m_file7Centered == null && !ableToDiscern7Centering(iValue, out m_file7Centered))
                    return 0;
            return m_file7Centered.Value ? 0 : -7;
        }

        /// <summary>
        /// Given a certain column of the .csv, determine the maximum value that is deemed correct.
        /// For buttons, this is a 1. For the DPad this is a 4. For the stick, this varies depending on 0 or 7 centering.
        /// </summary>
        /// <returns>the largest acceptable integer value for the given column</returns>
        private int getLargestAcceptedValue(int colIndex, int iValue)
        {
            if (DPAD_COLUMN == colIndex)
                return 4;
            if (BUTTON_COLUMNS.Contains(colIndex))
                return 1;
            if (m_file7Centered == null && !ableToDiscern7Centering(iValue, out m_file7Centered))
                return 7;
            return m_file7Centered.Value ? 14 : 7;
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

        /// <summary>
        /// When user sets a cell value, perform a formatting check to see if input is acceptable
        /// </summary>
        /// <returns>true if formatting check passes, false if not</returns>
        private bool valueFormattedProperly(int value, DataGridViewCellEventArgs e)
        {
            int colIndex = e.ColumnIndex;
            int smallestAcceptedVal = getSmallestAcceptedValue(colIndex, value);
            int largestAcceptedVal = getLargestAcceptedValue(colIndex, value);

            return (value >= smallestAcceptedVal && value <= largestAcceptedVal);
        }

        private void loadDataToGridView()
        {
            addColumnsAndHeaders();
            addRowsAndHeaders();
            addDataFromCache();


            // This must be performed *after* adding columns/rows otherwise effect is lost
            inputGridView.AllowUserToResizeColumns = false;
            inputGridView.AllowUserToResizeRows = false;
            inputGridView.RowHeadersVisible = false;

            m_gridViewLoaded = true;
        }

        /// <summary>
        /// Take data from m_curFileData and write to the inputGridView
        /// </summary>
        private void addDataFromCache()
        {
            IEnumerable<int> frames = Enumerable.Range(0, m_curFileData.Count);
            IEnumerable<int> columns = Enumerable.Range(0, 6);

            for (int i = 0; i < m_curFileData.Count; i++)
            {
                // Write framecount to first column
                inputGridView.Rows[i].Cells[0].Value = i + 1;
                for (int j = 0; j < m_curFileData[i].Count; j++)
                {
                    inputGridView.Rows[i].Cells[j+1].Value = m_curFileData[i][j];
                }
            }
        }

        /// <summary>
        /// Standardized way to add columns, as they should all be uniform in size and behavior
        /// </summary>
        private void addColumn(string name)
        {
            DataGridViewTextBoxColumn column = new DataGridViewTextBoxColumn();
            column.HeaderText = name;
            column.Width = 30;
            inputGridView.Columns.Add(column);
        }

        private void addFrameCountColumn()
        {
            DataGridViewTextBoxColumn column = new DataGridViewTextBoxColumn();
            column.HeaderText = "Frame";
            column.Width = 50;
            column.ReadOnly = true;
            column.DefaultCellStyle.BackColor = Color.LightGray;
            inputGridView.Columns.Add(column);
        }

        private void addColumnsAndHeaders()
        {
            // Add the column to display framecount for each row
            addFrameCountColumn();

            addColumn("A");
            addColumn("B");
            addColumn("L");
            addColumn("L-R");
            addColumn("U-D");
            addColumn("Pad");
        }

        /// <summary>
        /// Adds a row to the inputGridView for every frame in the cached data
        /// </summary>
        private void addRowsAndHeaders()
        {
            for (int i = 0; i < m_curFileData.Count; i++)
            {
                DataGridViewRow row = new DataGridViewRow();
                row.HeaderCell.Value = i;
                inputGridView.Rows.Add(row);
            }
        }

        /// <summary>
        /// This event is fired whenever a cell is modified (not necessarily when the value has changed!)
        /// </summary>
        private void onInputChanged(object sender, DataGridViewCellEventArgs e)
        {
            if (inputChangeChecksPassed(e))
                saveToFile();
        }

        /// <summary>
        /// Perform various checks to make sure that we want to consider the file's inputs changed
        /// (e.g. table isn't loading up, user didn't just press enter on a cell, user typed a number)
        /// </summary>
        /// <returns>true if checks pass and input has changed in cache</returns>
        private bool inputChangeChecksPassed(DataGridViewCellEventArgs e)
        {
            // Happens when data is being loaded from file... We don't want to consider the cell data changed!
            if (!m_gridViewLoaded)
                return false;

            // To prevent unnecessary File I/O let's double-check that the value actually changed
            int inputNew;
            bool inputAccepted = int.TryParse(inputGridView.Rows[e.RowIndex].Cells[e.ColumnIndex].Value.ToString(), out inputNew);
            int inputCurFile = m_curFileData[e.RowIndex][e.ColumnIndex - ADJUST_FOR_FRAMECOUNT_COLUMN];

            // The value changed but doesn't conform with formatting rules... Reset to whatever it was before
            if (!inputAccepted || !valueFormattedProperly(inputNew, e))
            {
                inputGridView.Rows[e.RowIndex].Cells[e.ColumnIndex].Value = inputCurFile;
                return false;
            }

            // The value did not change
            if (inputNew == inputCurFile)
                return false;

            // The value changed, so change in the file data
            m_curFileData[e.RowIndex][e.ColumnIndex - ADJUST_FOR_FRAMECOUNT_COLUMN] = inputNew;
            return true;
        }

        // Takes the content in m_curFileData and writes to m_curFilePath
        private void saveToFile()
        {
            try
            {
                using (StreamWriter writer = new StreamWriter(m_curFilePath, false))
                {
                    // for each frame
                    foreach (List<int> curFileFrameData in m_curFileData)
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

            if (!hashCachedFile())
            {
                string errMsg = String.Format("Unable to hash file on save... Any further behavior is undefined." +
                                              "You should immediately back-up {0} and quit out of this program.", m_curFilePath);
                showError(errMsg);
            }
        }

        private void on7CenterClick(object sender, EventArgs e)
        {
            // If we're already 7-centered, do not do anything
            if (centered7Button.Checked) return;

            centered7Button.Enabled = true;
            centered0Button.Checked = false;

            // Iterate through all inputs and change to 7-centered
            centerInputs(true);
        }

        private void on0CenterClick(object sender, EventArgs e)
        {
            // If we're already 0-centered, do not do anything
            if (centered0Button.Checked) return;

            centered0Button.Checked = true;
            centered7Button.Checked = false;

            // Iterate through all inputs and change to 0-centered
            centerInputs(false);
        }

        private void centerInputs(bool centerOn7)
        {
            // Adjustment to make (either from 7-center to 0-center or vice versa)
            int centerOffset = centerOn7 ? 7 : -7;

            // Since we need to update both the grid and the m_curFileData, I think it's more efficient to iterate across the m_curFileData
            for (int i = 0; i < m_curFileData.Count; i++)
            {
                for (int j = 3; j < 5; j++)
                {
                    // Apply to cached file data
                    m_curFileData[i][j] += centerOffset;

                    // Apply to DataGridView
                    int iCellValue;
                    int.TryParse(inputGridView.Rows[i].Cells[j + ADJUST_FOR_FRAMECOUNT_COLUMN].Value.ToString(), out iCellValue);
                    inputGridView.Rows[i].Cells[j + ADJUST_FOR_FRAMECOUNT_COLUMN].Value = iCellValue + centerOffset;
                }
            }

            // Now re-save file to disk
            saveToFile();
        }

        /// <summary>
        /// When the form regains focus, check to see if the file has been modified by Dolphin (or other program).
        /// If so, we will want to re-load the file
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void onGainFocus(object sender, EventArgs e)
        {
            if (m_curFilePath == string.Empty)
                return;

            // Hash the local file and see if it matches the cached file
            // If not, then we know we want to re-load the file data
            byte[] newHash = getHash();
            if (areHashesEqual(newHash, m_curFileHash))
                return;

            // TODO: React to the checksum change
        }

        string m_curFilePath;
        byte[] m_curFileHash; 
        List<List<int>> m_curFileData;
        bool m_gridViewLoaded;
        bool? m_file7Centered;

        // Constants
        private const int NUM_INPUT_COLUMNS = 6;
        private const char COMMA_SEPARATOR = ',';
        private static readonly int[] BUTTON_COLUMNS = new int[] { 0, 1, 2, 5 };
        private const int DPAD_COLUMN = 5;
        private const int ADJUST_FOR_FRAMECOUNT_COLUMN = 1;
    }
}
