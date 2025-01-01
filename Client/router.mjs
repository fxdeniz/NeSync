import { BrowserWindow } from "electron";
import { fileURLToPath } from "url";
import path from "node:path";

// https://iamwebwiz.medium.com/how-to-fix-dirname-is-not-defined-in-es-module-scope-34d94a86694d
// https://byby.dev/node-dirname-not-defined
// https://nodejs.org/api/esm.html#importmeta
// https://gist.github.com/sindresorhus/a39789f98801d908bbc7ff3ecc99d99c
const __filename = fileURLToPath(import.meta.url); // get the resolved path to the file
const __dirname = path.dirname(__filename); // get the name of the directory

function routeToFileExplorer (event) {
  const webContents = event.sender;
  const win = BrowserWindow.fromWebContents(webContents);
  win.loadFile(path.join(__dirname,'gui/pages/file_explorer.html'));
}

function routeToFileMonitor (event) {
  const webContents = event.sender;
  const win = BrowserWindow.fromWebContents(webContents);
  win.loadFile(path.join(__dirname,'gui/pages/file_monitor.html'));
}

function routeToAddFolder (event) {
  const webContents = event.sender;
  const win = BrowserWindow.fromWebContents(webContents);
  win.loadFile(path.join(__dirname,'gui/pages/add_folder.html'));
}

function routeToSaveChanges (event) {
  const webContents = event.sender;
  const win = BrowserWindow.fromWebContents(webContents);
  win.loadFile(path.join(__dirname,'gui/pages/save_changes.html'));
}

function routeToZipExport (event) {
  const webContents = event.sender;
  const win = BrowserWindow.fromWebContents(webContents);
  win.loadFile(path.join(__dirname,'gui/pages/zip_export.html'));
}

function routeToZipImport (event) {
  const webContents = event.sender;
  const win = BrowserWindow.fromWebContents(webContents);
  win.loadFile(path.join(__dirname,'gui/pages/zip_import.html'));
}

export {routeToFileExplorer, routeToFileMonitor, routeToAddFolder, routeToSaveChanges, routeToZipExport, routeToZipImport};