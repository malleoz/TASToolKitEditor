namespace TASToolKitEditor
{
    partial class TASToolKitEditorForm
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.playerInputGridView = new System.Windows.Forms.DataGridView();
            this.menuStrip = new System.Windows.Forms.MenuStrip();
            this.fileMenu = new System.Windows.Forms.ToolStripMenuItem();
            this.fileOpenPlayer = new System.Windows.Forms.ToolStripMenuItem();
            this.fileOpenGhost = new System.Windows.Forms.ToolStripMenuItem();
            this.inputCenteringToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.centered0Button = new System.Windows.Forms.ToolStripMenuItem();
            this.centered7Button = new System.Windows.Forms.ToolStripMenuItem();
            this.playerMenu = new System.Windows.Forms.ToolStripMenuItem();
            this.playerUndoMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.playerRedoMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.ghostMenu = new System.Windows.Forms.ToolStripMenuItem();
            this.ghostUndoMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.ghostRedoMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.ghostInputGridView = new System.Windows.Forms.DataGridView();
            this.ghostGridViewLabel = new System.Windows.Forms.Label();
            this.playerGridViewLabel = new System.Windows.Forms.Label();
            this.fileClosePlayer = new System.Windows.Forms.ToolStripMenuItem();
            this.fileCloseGhost = new System.Windows.Forms.ToolStripMenuItem();
            ((System.ComponentModel.ISupportInitialize)(this.playerInputGridView)).BeginInit();
            this.menuStrip.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.ghostInputGridView)).BeginInit();
            this.SuspendLayout();
            // 
            // playerInputGridView
            // 
            this.playerInputGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.playerInputGridView.Location = new System.Drawing.Point(283, 50);
            this.playerInputGridView.Name = "playerInputGridView";
            this.playerInputGridView.RowTemplate.Height = 25;
            this.playerInputGridView.Size = new System.Drawing.Size(265, 389);
            this.playerInputGridView.TabIndex = 0;
            this.playerInputGridView.CellMouseClick += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.onCellClickPlayer);
            this.playerInputGridView.CellValueChanged += new System.Windows.Forms.DataGridViewCellEventHandler(this.onInputChangedPlayer);
            // 
            // menuStrip
            // 
            this.menuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileMenu,
            this.playerMenu,
            this.ghostMenu});
            this.menuStrip.Location = new System.Drawing.Point(0, 0);
            this.menuStrip.Name = "menuStrip";
            this.menuStrip.Size = new System.Drawing.Size(562, 24);
            this.menuStrip.TabIndex = 1;
            this.menuStrip.Text = "menuStrip1";
            // 
            // fileMenu
            // 
            this.fileMenu.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileOpenPlayer,
            this.fileOpenGhost,
            this.fileClosePlayer,
            this.fileCloseGhost,
            this.inputCenteringToolStripMenuItem});
            this.fileMenu.Name = "fileMenu";
            this.fileMenu.Size = new System.Drawing.Size(37, 20);
            this.fileMenu.Text = "File";
            // 
            // fileOpenPlayer
            // 
            this.fileOpenPlayer.Name = "fileOpenPlayer";
            this.fileOpenPlayer.Size = new System.Drawing.Size(228, 22);
            this.fileOpenPlayer.Text = "Open Player (Ctrl + O)";
            this.fileOpenPlayer.Click += new System.EventHandler(this.onClickFileOpenPlayer);
            // 
            // fileOpenGhost
            // 
            this.fileOpenGhost.Name = "fileOpenGhost";
            this.fileOpenGhost.Size = new System.Drawing.Size(228, 22);
            this.fileOpenGhost.Text = "Open Ghost (Ctrl + Shift + O)";
            this.fileOpenGhost.Click += new System.EventHandler(this.onClickFileOpenGhost);
            // 
            // inputCenteringToolStripMenuItem
            // 
            this.inputCenteringToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.centered0Button,
            this.centered7Button});
            this.inputCenteringToolStripMenuItem.Name = "inputCenteringToolStripMenuItem";
            this.inputCenteringToolStripMenuItem.Size = new System.Drawing.Size(228, 22);
            this.inputCenteringToolStripMenuItem.Text = "Input Centering";
            // 
            // centered0Button
            // 
            this.centered0Button.Name = "centered0Button";
            this.centered0Button.Size = new System.Drawing.Size(180, 22);
            this.centered0Button.Text = "0 Centered";
            this.centered0Button.Click += new System.EventHandler(this.on0CenterClick);
            // 
            // centered7Button
            // 
            this.centered7Button.Name = "centered7Button";
            this.centered7Button.Size = new System.Drawing.Size(180, 22);
            this.centered7Button.Text = "7 Centered";
            this.centered7Button.Click += new System.EventHandler(this.on7CenterClick);
            // 
            // playerMenu
            // 
            this.playerMenu.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.playerUndoMenuItem,
            this.playerRedoMenuItem});
            this.playerMenu.Name = "playerMenu";
            this.playerMenu.Size = new System.Drawing.Size(51, 20);
            this.playerMenu.Text = "Player";
            this.playerMenu.Visible = false;
            // 
            // playerUndoMenuItem
            // 
            this.playerUndoMenuItem.Enabled = false;
            this.playerUndoMenuItem.Name = "playerUndoMenuItem";
            this.playerUndoMenuItem.Size = new System.Drawing.Size(180, 22);
            this.playerUndoMenuItem.Text = "Undo (Ctrl + Z)";
            this.playerUndoMenuItem.Click += new System.EventHandler(this.onClickUndoPlayer);
            // 
            // playerRedoMenuItem
            // 
            this.playerRedoMenuItem.Enabled = false;
            this.playerRedoMenuItem.Name = "playerRedoMenuItem";
            this.playerRedoMenuItem.Size = new System.Drawing.Size(180, 22);
            this.playerRedoMenuItem.Text = "Redo (Ctrl + Y)";
            this.playerRedoMenuItem.Click += new System.EventHandler(this.onClickRedoPlayer);
            // 
            // ghostMenu
            // 
            this.ghostMenu.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ghostUndoMenuItem,
            this.ghostRedoMenuItem});
            this.ghostMenu.Name = "ghostMenu";
            this.ghostMenu.Size = new System.Drawing.Size(50, 20);
            this.ghostMenu.Text = "Ghost";
            this.ghostMenu.Visible = false;
            // 
            // ghostUndoMenuItem
            // 
            this.ghostUndoMenuItem.Enabled = false;
            this.ghostUndoMenuItem.Name = "ghostUndoMenuItem";
            this.ghostUndoMenuItem.Size = new System.Drawing.Size(192, 22);
            this.ghostUndoMenuItem.Text = "Undo (Ctrl + Shift + Z)";
            this.ghostUndoMenuItem.Click += new System.EventHandler(this.onClickUndoGhost);
            // 
            // ghostRedoMenuItem
            // 
            this.ghostRedoMenuItem.Enabled = false;
            this.ghostRedoMenuItem.Name = "ghostRedoMenuItem";
            this.ghostRedoMenuItem.Size = new System.Drawing.Size(192, 22);
            this.ghostRedoMenuItem.Text = "Redo (Ctrl +Shift + Y)";
            this.ghostRedoMenuItem.Click += new System.EventHandler(this.onClickRedoGhost);
            // 
            // ghostInputGridView
            // 
            this.ghostInputGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.ghostInputGridView.Location = new System.Drawing.Point(12, 50);
            this.ghostInputGridView.Name = "ghostInputGridView";
            this.ghostInputGridView.RowTemplate.Height = 25;
            this.ghostInputGridView.Size = new System.Drawing.Size(265, 389);
            this.ghostInputGridView.TabIndex = 2;
            this.ghostInputGridView.CellMouseClick += new System.Windows.Forms.DataGridViewCellMouseEventHandler(this.onCellClickGhost);
            this.ghostInputGridView.CellValueChanged += new System.Windows.Forms.DataGridViewCellEventHandler(this.onInputChangedGhost);
            // 
            // ghostGridViewLabel
            // 
            this.ghostGridViewLabel.AutoSize = true;
            this.ghostGridViewLabel.Location = new System.Drawing.Point(12, 32);
            this.ghostGridViewLabel.Name = "ghostGridViewLabel";
            this.ghostGridViewLabel.Size = new System.Drawing.Size(74, 15);
            this.ghostGridViewLabel.TabIndex = 3;
            this.ghostGridViewLabel.Text = "Ghost Inputs";
            // 
            // playerGridViewLabel
            // 
            this.playerGridViewLabel.AutoSize = true;
            this.playerGridViewLabel.Location = new System.Drawing.Point(283, 32);
            this.playerGridViewLabel.Name = "playerGridViewLabel";
            this.playerGridViewLabel.Size = new System.Drawing.Size(75, 15);
            this.playerGridViewLabel.TabIndex = 4;
            this.playerGridViewLabel.Text = "Player Inputs";
            // 
            // fileClosePlayer
            // 
            this.fileClosePlayer.Enabled = false;
            this.fileClosePlayer.Name = "fileClosePlayer";
            this.fileClosePlayer.Size = new System.Drawing.Size(228, 22);
            this.fileClosePlayer.Text = "Close Player (Esc)";
            this.fileClosePlayer.Click += new System.EventHandler(this.onClickFileClosePlayer);
            // 
            // fileCloseGhost
            // 
            this.fileCloseGhost.Enabled = false;
            this.fileCloseGhost.Name = "fileCloseGhost";
            this.fileCloseGhost.Size = new System.Drawing.Size(228, 22);
            this.fileCloseGhost.Text = "Close Ghost (Shift + Esc)";
            this.fileCloseGhost.Click += new System.EventHandler(this.onClickFileCloseGhost);
            // 
            // TASToolKitEditorForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(562, 451);
            this.Controls.Add(this.playerGridViewLabel);
            this.Controls.Add(this.ghostGridViewLabel);
            this.Controls.Add(this.ghostInputGridView);
            this.Controls.Add(this.playerInputGridView);
            this.Controls.Add(this.menuStrip);
            this.MainMenuStrip = this.menuStrip;
            this.MaximizeBox = false;
            this.Name = "TASToolKitEditorForm";
            this.Text = "TTK Input Editor";
            this.Resize += new System.EventHandler(this.onFormResize);
            ((System.ComponentModel.ISupportInitialize)(this.playerInputGridView)).EndInit();
            this.menuStrip.ResumeLayout(false);
            this.menuStrip.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.ghostInputGridView)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.DataGridView playerInputGridView;
        private System.Windows.Forms.MenuStrip menuStrip;
        private System.Windows.Forms.ToolStripMenuItem fileMenu;
        private System.Windows.Forms.ToolStripMenuItem fileOpenPlayer;
        private System.Windows.Forms.ToolStripMenuItem inputCenteringToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem centered0Button;
        private System.Windows.Forms.ToolStripMenuItem centered7Button;
        private System.Windows.Forms.ToolStripMenuItem fileOpenGhost;
        private System.Windows.Forms.DataGridView ghostInputGridView;
        private System.Windows.Forms.ToolStripMenuItem ghostMenu;
        private System.Windows.Forms.ToolStripMenuItem ghostUndoMenuItem;
        private System.Windows.Forms.ToolStripMenuItem ghostRedoMenuItem;
        private System.Windows.Forms.ToolStripMenuItem playerMenu;
        private System.Windows.Forms.ToolStripMenuItem playerUndoMenuItem;
        private System.Windows.Forms.ToolStripMenuItem playerRedoMenuItem;
        private System.Windows.Forms.Label ghostGridViewLabel;
        private System.Windows.Forms.Label playerGridViewLabel;
        private System.Windows.Forms.ToolStripMenuItem fileClosePlayer;
        private System.Windows.Forms.ToolStripMenuItem fileCloseGhost;
    }
}
