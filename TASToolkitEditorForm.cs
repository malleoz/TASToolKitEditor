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
        public TASToolKitEditorForm()
        {
            InitializeComponent();
            adjustFormWidth();

            playerFile = new InputFile(playerInputGridView, playerMenu, playerUndoMenuItem, playerRedoMenuItem, fileClosePlayer, playerGridViewLabel);
            ghostFile = new InputFile(ghostInputGridView, ghostMenu, ghostUndoMenuItem, ghostRedoMenuItem, fileCloseGhost, ghostGridViewLabel);

            assignContextMenu(playerFile);
            assignContextMenu(ghostFile);

            m_files7Centered = null;
            m_curOpType = EOperationType.Normal;
            m_fileSystemWatcher = new FileSystemWatcher();
            m_scrollTogether = false;
        }

        #region Events
        private void on0CenterClick(object sender, EventArgs e) => onCenterClick(centered0Button, centered7Button, false);
        private void on7CenterClick(object sender, EventArgs e) => onCenterClick(centered7Button, centered0Button, true);

        private void onCellClickGhost(object sender, DataGridViewCellMouseEventArgs e) => cellClick(e, ghostFile);
        private void onCellClickPlayer(object sender, DataGridViewCellMouseEventArgs e) => cellClick(e, playerFile);

        private void onClickFileClosePlayer(object sender, EventArgs e) => closeFile(playerFile);
        private void onClickFileCloseGhost(object sender, EventArgs e) => closeFile(ghostFile);
        private void onClickFileOpenGhost(object sender, EventArgs e) => fileOpen(ghostFile);
        private void onClickFileOpenPlayer(object sender, EventArgs e) => fileOpen(playerFile);

        private void onClickRedoGhost(object sender, EventArgs e) => performUndoRedo(ghostFile, EOperationType.Redo);
        private void onClickRedoPlayer(object sender, EventArgs e) => performUndoRedo(playerFile, EOperationType.Redo);
        private void onClickUndoGhost(object sender, EventArgs e) => performUndoRedo(ghostFile, EOperationType.Undo);
        private void onClickUndoPlayer(object sender, EventArgs e) => performUndoRedo(playerFile, EOperationType.Undo);

        private void onClickScrollTogether(object sender, EventArgs e)
        {
            scrollTogetherMenuItem.Checked = !scrollTogetherMenuItem.Checked;

            if (m_scrollTogether)
            {
                m_scrollTogether = false;
                return;
            }

            // Snap views together (default to player?)
            // Get top-left visible cell of player and set that row as first visible cell of ghost
            int newRowIdx = playerInputGridView.FirstDisplayedScrollingRowIndex;

            if (ghostInputGridView.Rows.Count > newRowIdx)
                ghostInputGridView.FirstDisplayedScrollingRowIndex = newRowIdx;
            else
                ghostInputGridView.FirstDisplayedScrollingRowIndex = ghostInputGridView.Rows.Count - 1;

            m_scrollTogether = !m_scrollTogether;
        }

        private void onFileSwap(object sender, EventArgs e)
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

        private void onFormResize(object sender, EventArgs e)
        {
            ghostInputGridView.Height = this.Height - DATAGRIDVIEW_PADDING - ghostInputGridView.Location.Y;
            playerInputGridView.Height = this.Height - DATAGRIDVIEW_PADDING - playerInputGridView.Location.Y;
        }

        private void onInputChangedGhost(object sender, DataGridViewCellEventArgs e) => inputChanged(e, ghostFile);
        private void onInputChangedPlayer(object sender, DataGridViewCellEventArgs e) => inputChanged(e, playerFile);

        private void onScrollGhost(object sender, ScrollEventArgs e) => scrollTogether(e, playerInputGridView);
        private void onScrollPlayer(object sender, ScrollEventArgs e) => scrollTogether(e, ghostInputGridView);

        /// <summary>
        /// This is a more reliable method of the KeyDown event
        /// </summary>
        protected override bool ProcessCmdKey(ref Message msg, Keys keyData)
        {
            processHotkeys(keyData);

            return base.ProcessCmdKey(ref msg, keyData);
        }
        #endregion

        #region Members
        InputFile playerFile;
        InputFile ghostFile;
        bool? m_files7Centered;
        EOperationType m_curOpType;
        FileSystemWatcher m_fileSystemWatcher;
        public static int m_filesLoaded = 0;
        bool m_scrollTogether;

        // Constants
        private const int NUM_INPUT_COLUMNS = 6;
        private const char COMMA_SEPARATOR = ',';
        private static readonly int[] BUTTON_COLUMNS = new int[] { 0, 1, 2};
        private const int DPAD_COLUMN = 5;
        private const int ADJUST_FOR_FRAMECOUNT_COLUMN = 1;
        private const int DATAGRIDVIEW_PADDING = 50;
        private const int firstGridViewlocationX = 12;
        private const int gridViewRightPadding = 25;
        private const int secondGridViewlocationX = 283;
        private const int gridViewLocationY = 50;
        private const int GridViewLabelY = 32;
        private const int widthBetweenGridViews = 6;
        private const int formWidthWithSingleGridView = firstGridViewlocationX + gridViewWidth + gridViewRightPadding;
        private const int formWidthWithDoubleGridView = secondGridViewlocationX + gridViewWidth + gridViewRightPadding;
        private const int gridViewWidth = 265;
        #endregion
    }
}
