//
//
// Code for Archive Viewer in C#

using System;
using System.IO;
using System.IO.Compression;
using Gtk;

public class ArchiveViewer : Window{
    private string archivePath;
    private TreeView tree;
    private ListStore listStore;

    public ArchiveViewer() : base("Archive Viewer"){
        SetDefaultSize(600, 400);
        SetPosition(WindowPosition.Center);

        VBox vbox=new VBox();
        HBox hbox=new HBox();

        Button openButton=new Button("Open Archive");
        Button extractAllButton=new Button("Extract All");
        Button aboutButton=new Button("About");

        openButton.Clicked += OnOpenArchive;
        extractAllButton.Clicked += OnExtractAll;
        aboutButton.Clicked += OnAbout;

        hbox.PackStart(openButton, false, false, 5);
        hbox.PackStart(extractAllButton, false, false, 5);
        hbox.PackStart(aboutButton, false, false, 5);

        tree=new TreeView();
        listStore=new ListStore(typeof(string), typeof(string));
        tree.Model=listStore;

        TreeViewColumn column=new TreeViewColumn { Title="File/Folder" };
        CellRendererText cell=new CellRendererText();
        column.PackStart(cell, true);
        column.AddAttribute(cell, "text", 0);
        tree.AppendColumn(column);

        tree.RowActivated += OnRowActivated;

        vbox.PackStart(hbox, false, false, 5);
        vbox.PackStart(tree, true, true, 5);
        Add(vbox);

        DeleteEvent += delegate { Application.Quit(); };
        ShowAll();
    }

    private void OnOpenArchive(object sender, EventArgs args){
        FileChooserDialog fileChooser=new FileChooserDialog(
            "Choose a zip archive", this, FileChooserAction.Open,
            "Cancel", ResponseType.Cancel, "Open", ResponseType.Accept);

        if (fileChooser.Run() == (int)ResponseType.Accept)
        {
            archivePath=fileChooser.Filename;
            LoadArchiveContents(archivePath);
        }
        fileChooser.Destroy();
    }

    private void LoadArchiveContents(string path){
        listStore.Clear();
        try{
            using ZipArchive archive=ZipFile.OpenRead(path);
            foreach (ZipArchiveEntry entry in archive.Entries)
            {
                listStore.AppendValues(entry.FullName);
            }
        } catch (Exception ex){
            ShowError($"Failed to open archive: {ex.Message}");
        }
    }

    private void OnRowActivated(object sender, RowActivatedArgs args){
        if (archivePath == null) return;

        TreeIter iter;
        if (listStore.GetIter(out iter, args.Path)){
            string selectedFile=(string)listStore.GetValue(iter, 0);
            ExtractSingleFile(selectedFile);
        }
    }

    private void ExtractSingleFile(string entryName){
        string extractFolder=Path.Combine(Path.GetDirectoryName(archivePath), Path.GetFileNameWithoutExtension(archivePath));
        Directory.CreateDirectory(extractFolder);

        using ZipArchive archive=ZipFile.OpenRead(archivePath);
        ZipArchiveEntry entry=archive.GetEntry(entryName);
        if (entry != null)
        {
            string destinationPath=Path.Combine(extractFolder, entry.FullName);
            Directory.CreateDirectory(Path.GetDirectoryName(destinationPath));
            entry.ExtractToFile(destinationPath, overwrite: true);

            ShowInfo($"Extracted '{entry.FullName}' to '{destinationPath}'");
        }
    }

    private void OnExtractAll(object sender, EventArgs args){
        if (archivePath == null) return;

        string extractFolder=Path.Combine(Path.GetDirectoryName(archivePath), Path.GetFileNameWithoutExtension(archivePath));
        Directory.CreateDirectory(extractFolder);

        using ZipArchive archive=ZipFile.OpenRead(archivePath);
        archive.ExtractToDirectory(extractFolder, overwriteFiles: true);

        ShowInfo($"All files extracted to '{extractFolder}'");
    }

    private void OnAbout(object sender, EventArgs args){
        MessageDialog aboutDialog=new MessageDialog(this, DialogFlags.Modal, MessageType.Info, ButtonsType.Ok,
            "Archive Viewer\nAuthor: Jay Mee");
        aboutDialog.Run();
        aboutDialog.Destroy();
    }

    private void ShowError(string message){
        MessageDialog errorDialog=new MessageDialog(this, DialogFlags.Modal, MessageType.Error, ButtonsType.Ok, message);
        errorDialog.Run();
        errorDialog.Destroy();
    }

    private void ShowInfo(string message){
        MessageDialog infoDialog=new MessageDialog(this, DialogFlags.Modal, MessageType.Info, ButtonsType.Ok, message);
        infoDialog.Run();
        infoDialog.Destroy();
    }

    public static void Main(){
        Application.Init();
        new ArchiveViewer();
        Application.Run();
    }
}



Explanation of Key Sections
Open Archive: The OnOpenArchive method opens a dialog to select a zip file. It calls LoadArchiveContents to load and display the file contents.
Display Archive Contents: LoadArchiveContents lists each file in the archive.
Single File Extraction: Double-clicking a file name calls ExtractSingleFile, which extracts the selected file into a folder named after the archive.
Extract All: OnExtractAll extracts all files to a similar folder structure.
About Button: Shows the author's name, "Jay Mee," in a simple dialog.
Compilation Instructions
Save the code to a file named ArchiveViewer.cs.
Run the following commands in the terminal to compile and run the application:

mcs -pkg:gtk-sharp-3.0 ArchiveViewer.cs -out:ArchiveViewer.exe
mono ArchiveViewer.exe
Dependencies
Make sure you have GtkSharp installed on your Linux system to run this application:


sudo apt install gtk-sharp2
