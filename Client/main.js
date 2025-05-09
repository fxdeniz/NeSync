import { app, BrowserWindow, ipcMain, dialog } from 'electron';
import { fileURLToPath } from 'url';
import path from 'node:path';
import * as ChildProcess from "node:child_process";
import * as router from './router.mjs';
import * as DialogApi from './DialogApi.mjs'
import { splitPath, normalizePath, fileNameWithExtension, isPathExists, previewFile, extractFile } from './FileSystemApi.mjs'

// https://iamwebwiz.medium.com/how-to-fix-dirname-is-not-defined-in-es-module-scope-34d94a86694d
// https://byby.dev/node-dirname-not-defined
// https://nodejs.org/api/esm.html#importmeta
// https://gist.github.com/sindresorhus/a39789f98801d908bbc7ff3ecc99d99c
const __filename = fileURLToPath(import.meta.url); // get the resolved path to the file
const __dirname = path.dirname(__filename); // get the name of the directory

// https://github.com/electron/windows-installer
// this should be placed at top of main.js to handle setup events quickly
if (process.platform === 'win32' && handleSquirrelEvent()) {
  // squirrel event handled and app will exit in 1000ms, so don't do anything else
  app.quit();
}

let appState = new Map();
let serverProcess;

const createWindow = () => {
  const mainWindow = new BrowserWindow({
    width: 800,
    height: 600,
    webPreferences: {
      preload: path.join(__dirname, 'preload.js'),
      icon: "resources/icon/app_icon.png"
    }
  });

  mainWindow.maximize();
  mainWindow.loadFile("./gui/pages/server_startup.html");
  mainWindow.focus();
  // mainWindow.webContents.openDevTools();
};

app.whenReady().then(() => {
  
  ipcMain.on('serverProcess:Run', () => {
    // Also works on windows without .exe extension.
    const serverPath = path.join(process.resourcesPath, "cli", "nesync");

    const min = 10000, max = 65535;
    const portNumber = Math.floor(Math.random() * (max - min + 1)) + min;

    serverProcess = ChildProcess.spawn(serverPath, ['--port', portNumber], {
      stdio: "ignore"
    });

    appState.set("serverPid", serverProcess.pid);
    appState.set("serverPort", portNumber);

    console.log(`server started on ${portNumber} with pid ${serverProcess.pid}`);
    // Below line can't use router.routeToServerStartup() 
    // because this function gets the browser window from the event sender.
    // In here, event is sent by the child process.
    serverProcess.on('exit', (code, signal) => {
      const allWindows = BrowserWindow.getAllWindows();
      
      if(allWindows && allWindows[0]) // Prevent re-starting the server after client exited.
        allWindows[0].loadFile("./gui/pages/server_startup.html")
    });
  });

  app.on('before-quit', () => {
    console.log(`shutting server down.`);
    if (serverProcess && !serverProcess.killed)
      serverProcess.kill(); // TODO: Does not properly kills the process, just sends the kill signal.
  });

  ipcMain.on('route:ServerStartup', router.routeToServerStartup);
  ipcMain.on('route:FileExplorer', router.routeToFileExplorer);
  ipcMain.on('route:FileMonitor', router.routeToFileMonitor);
  ipcMain.on('route:AddFolder', router.routeToAddFolder);
  ipcMain.on('route:SaveChanges', router.routeToSaveChanges);
  ipcMain.on('route:ZipExport', router.routeToZipExport);
  ipcMain.on('route:ZipImport', router.routeToZipImport);
  ipcMain.on('route:FileInfo', router.routeToFileInfo);
  ipcMain.on('route:FolderInfo', router.routeToFolderInfo);
  ipcMain.handle('dialog:OpenFolder', DialogApi.showFolderSelectDialog);
  ipcMain.handle('dialog:OpenFile', DialogApi.showFileSelectDialog);
  ipcMain.handle('dialog:SaveFile', DialogApi.showFileSaveDialog);

  ipcMain.handle('fs:SplitPath', async (event, input) => {
    return splitPath(input);
  });

  ipcMain.handle('fs:NormalizePath', async (event, input) => {
    return normalizePath(input);
  });

  ipcMain.handle('fs:FileNameFromPath', async (event, input) => {
    return fileNameWithExtension(input);
  });

  ipcMain.handle('fs:CheckPath', async (event, input) => {
    return isPathExists(input);
  });

  ipcMain.handle('fs:Preview', async (event, path, extension) => {
    return await previewFile(path, extension);
  });

  ipcMain.handle('fs:Extract', async (event, srcPath, destPath) => {
    return await extractFile(srcPath, destPath);
  });

  ipcMain.handle('state:Get', async (event, key) => {
    return appState.get(key);
  });

  ipcMain.handle('state:Set', async (event, key, value) => {
    appState.set(key, value);
  });

  ipcMain.handle('app:IsPacked', () => app.isPackaged);
  
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

// https://github.com/electron/windows-installer
function handleSquirrelEvent() {
  if (process.argv.length === 1) {
    return false;
  }

  const appFolder = path.resolve(process.execPath, '..');
  const rootAtomFolder = path.resolve(appFolder, '..');
  const updateDotExe = path.resolve(path.join(rootAtomFolder, 'Update.exe'));
  const exeName = path.basename(process.execPath);

  const spawn = function(command, args) {
    let spawnedProcess, error;

    try {
      spawnedProcess = ChildProcess.spawn(command, args, {detached: true});
    } catch (error) {}

    return spawnedProcess;
  };

  const spawnUpdate = function(args) {
    return spawn(updateDotExe, args);
  };

  const squirrelEvent = process.argv[1];
  switch (squirrelEvent) {
    case '--squirrel-firstrun':
      return true;
      
    case '--squirrel-install':
    case '--squirrel-updated':
      // Optionally do things such as:
      // - Add your .exe to the PATH
      // - Write to the registry for things like file associations and
      //   explorer context menus

      // Install desktop and start menu shortcuts
      spawnUpdate(['--createShortcut', exeName]);

      setTimeout(app.quit, 1000);
      return true;

    case '--squirrel-uninstall':
      // Undo anything you did in the --squirrel-install and
      // --squirrel-updated handlers

      // Remove desktop and start menu shortcuts
      spawnUpdate(['--removeShortcut', exeName]);

      setTimeout(app.quit, 1000);
      return true;

    case '--squirrel-obsolete':
      // This is called on the outgoing version of your app before
      // we update to the new version - it's the opposite of
      // --squirrel-updated

      app.quit();
      return true;
  }
}