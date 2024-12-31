import { app, BrowserWindow, ipcMain, dialog } from 'electron';
import { fileURLToPath } from 'url';
import path from 'node:path';
import * as router from './router.mjs';
import * as DialogApi from './DialogApi.mjs'

// https://iamwebwiz.medium.com/how-to-fix-dirname-is-not-defined-in-es-module-scope-34d94a86694d
// https://byby.dev/node-dirname-not-defined
// https://nodejs.org/api/esm.html#importmeta
// https://gist.github.com/sindresorhus/a39789f98801d908bbc7ff3ecc99d99c
const __filename = fileURLToPath(import.meta.url); // get the resolved path to the file
const __dirname = path.dirname(__filename); // get the name of the directory

let appState = new Map();

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

app.whenReady().then(() => {
  ipcMain.on('route:FileExplorer', router.routeToFileExplorer);
  ipcMain.on('route:FileMonitor', router.routeToFileMonitor);
  ipcMain.on('route:AddFolder', router.routeToAddFolder);
  ipcMain.on('route:SaveChanges', router.routeToSaveChanges);
  ipcMain.on('route:ZipExport', router.routeToZipExport);
  ipcMain.on('route:ZipImport', router.routeToZipImport);
  ipcMain.handle('dialog:OpenFolder', DialogApi.showFolderSelectDialog);
  ipcMain.handle('dialog:OpenFile', DialogApi.showFileSelectDialog);
  ipcMain.handle('dialog:SaveFile', DialogApi.showFileSaveDialog);

  ipcMain.handle('path:Split', async (event, input) => {
    const result = splitPath(input);
    return result;
  });

  ipcMain.handle('path:FileName', async (event, input) => {
    const result = fileNameWithExtension(input);
    return result;
  });

  ipcMain.handle('state:Get', async (event, key) => {
    return appState.get(key);
  });

  ipcMain.handle('state:Set', async (event, key, value) => {
    appState.set(key, value);
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