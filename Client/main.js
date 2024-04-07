const { app, BrowserWindow, ipcMain, dialog } = require('electron');
const path = require('node:path');

function routeToFileExplorer (event) {
  const webContents = event.sender;
  const win = BrowserWindow.fromWebContents(webContents);
  win.loadFile(path.join(__dirname,'resources/tabs/file_explorer.html'));
}


async function showFolderSelectDialog () {
  const { canceled, filePaths } = await dialog.showOpenDialog({ properties: ['openDirectory'] });
  if (!canceled) {
    return filePaths[0];
  }
}


async function splitPath(givenPath) {
  return givenPath.split(path.sep);
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

app.whenReady().then(() => {
  ipcMain.on('route:FileExplorer', routeToFileExplorer);
  ipcMain.handle('dialog:OpenFolder', showFolderSelectDialog);

  ipcMain.handle('path:Split', async (event, input) => {
    const result = splitPath(input);
    return result;
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