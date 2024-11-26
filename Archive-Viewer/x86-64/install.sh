#!/bin/bash
# (c) J~Net 2024
#

# Compile the program
gcc -o archive-viewer archive-viewer.c $(pkg-config --cflags --libs gtk+-3.0) -larchive

# Copy executable to system bin
sudo cp archive-viewer /usr/local/bin/
sudo chmod +x /usr/local/bin/archive-viewer

# Create desktop entry
cat > archive-viewer.desktop << EOF
[Desktop Entry]
Name=Archive Viewer
Comment=Simple Archive Viewer
Exec=archive-viewer %f
Icon=system-file-manager
Terminal=false
Type=Application
Categories=Utility;Archiving;
MimeType=application/zip;application/x-zip-compressed;
EOF

# Install desktop entry
sudo cp archive-viewer.desktop /usr/share/applications/

# Create MIME type associations
cat > archive-viewer-mimetypes.xml << EOF
<?xml version="1.0" encoding="UTF-8"?>
<mime-info xmlns="http://www.freedesktop.org/standards/shared-mime-info">
  <mime-type type="application/zip">
    <comment>ZIP archive</comment>
    <glob pattern="*.zip"/>
  </mime-type>
</mime-info>
EOF

# Install MIME associations
sudo cp archive-viewer-mimetypes.xml /usr/share/mime/packages/
sudo update-mime-database /usr/share/mime

# Add to file manager's context menu
xdg-mime default archive-viewer.desktop application/zip
xdg-mime default archive-viewer.desktop application/x-zip-compressed
