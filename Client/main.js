const { app, BrowserWindow, ipcMain, dialog } = require('electron');
const path = require('node:path');
const fs = require('node:fs');

function routeToFileExplorer (event) {
  const webContents = event.sender;
  const win = BrowserWindow.fromWebContents(webContents);
  win.loadFile(path.join(__dirname,'resources/pages/file_explorer.html'));
}


function routeToFileMonitor (event) {
  const webContents = event.sender;
  const win = BrowserWindow.fromWebContents(webContents);
  win.loadFile(path.join(__dirname,'resources/pages/file_monitor.html'));
}


function routeToSaveChanges (event) {
  const webContents = event.sender;
  const win = BrowserWindow.fromWebContents(webContents);
  win.loadFile(path.join(__dirname,'resources/pages/save_changes.html'));
}


function routeToZipExport (event) {
  const webContents = event.sender;
  const win = BrowserWindow.fromWebContents(webContents);
  win.loadFile(path.join(__dirname,'resources/pages/zip_export.html'));
}


async function showFolderSelectDialog () {
    const { canceled, filePaths } = await dialog.showOpenDialog({ properties: ['openDirectory'] });
    let result = null;

    if (!canceled) {
        let stack = [{
            folderPath: filePaths[0],
            childFiles: [],
            childFolders: []
        }];

        let result = stack[0];
    
        while (stack.length > 0) {
            const currentFolder = stack.pop();
    
            const files = fs.readdirSync(currentFolder.folderPath);
    
            files.forEach(file => {
                const fullPath = path.join(currentFolder.folderPath, file);
                const stats = fs.statSync(fullPath);
    
                if (stats.isDirectory()) {
                    const folderObj = {
                        folderPath: fullPath,
                        childFiles: [],
                        childFolders: []
                    };
                    currentFolder.childFolders.push(folderObj);
                    stack.push(folderObj);
                } else {
                    currentFolder.childFiles.push(fullPath);
                }
            });
        }

        return result;
    }
}


async function showFileSelectDialog () {
  const { canceled, filePaths } = await dialog.showOpenDialog({ properties: ["openFile"], defaultPath: app.getPath("desktop") });
  let result = null;

  if (!canceled)
    result = filePaths[0];

  return result;
}


async function showFileSaveDialog () {
  const { canceled, filePath } = await dialog.showSaveDialog({ defaultPath: app.getPath('desktop') });
  let result = null;

  if (!canceled)
    result = filePath;

  return result;
}


async function splitPath(givenPath) {
  return givenPath.split(path.sep);
}


async function fileNameWithExtension(givenPath) {
  return path.basename(givenPath);
}


const createWindow = () => {
  const mainWindow = new BrowserWindow({
    width: 800,
    height: 600,
    webPreferences: {
      preload: path.join(__dirname, 'preload.js')
    }
  });

  mainWindow.loadFile('index.html');

  // mainWindow.webContents.openDevTools();
};


let fmState_CommitMessage;
let fmState_NewAddedJson;
let fmState_DeletedJson;
let fmState_UpdatedJson;

app.whenReady().then(() => {
  ipcMain.on('route:FileExplorer', routeToFileExplorer);
  ipcMain.on('route:FileMonitor', routeToFileMonitor);
  ipcMain.on('route:SaveChanges', routeToSaveChanges);
  ipcMain.on('route:ZipExport', routeToZipExport);
  ipcMain.handle('dialog:OpenFolder', showFolderSelectDialog);
  ipcMain.handle('dialog:OpenFile', showFileSelectDialog);
  ipcMain.handle('dialog:SaveFile', showFileSaveDialog);

  ipcMain.handle('path:Split', async (event, input) => {
    const result = splitPath(input);
    return result;
  });

  ipcMain.handle('path:FileName', async (event, input) => {
    const result = fileNameWithExtension(input);
    return result;
  });

  ipcMain.handle('fmState:setCommitMessage', async (event, input) => {
    fmState_CommitMessage = input;
  });

  ipcMain.handle('fmState:getCommitMessage', async (event) => {
    return fmState_CommitMessage ? fmState_CommitMessage : "";
  });

  ipcMain.handle('fmState:setNewAddedJson', async (event, input) => {
    fmState_NewAddedJson = input;
  });

  ipcMain.handle('fmState:getNewAddedJson', async (event) => {
    return fmState_NewAddedJson ? fmState_NewAddedJson : null;
  });

  ipcMain.handle('fmState:setDeletedJson', async (event, input) => {
    fmState_DeletedJson = input;
  });

  ipcMain.handle('fmState:getDeletedJson', async (event) => {
    return fmState_DeletedJson ? fmState_DeletedJson : null;
  });

  ipcMain.handle('fmState:setUpdatedJson', async (event, input) => {
    fmState_UpdatedJson = input;
  });

  ipcMain.handle('fmState:getUpdatedJson', async (event) => {
    return fmState_UpdatedJson ? fmState_UpdatedJson : null;
  });
  
  createWindow();

  app.on('activate', () => {
    // On macOS it's common to re-create a window in the app when the
    // dock icon is clicked and there are no other windows open.
    if (BrowserWindow.getAllWindows().length === 0) createWindow();
  })
});

// Quit when all windows are closed, except on macOS. There, it's common
// for applications and their menu bar to stay active until the user quits
// explicitly with Cmd + Q.
app.on('window-all-closed', () => {
  if (process.platform !== 'darwin') app.quit()
});