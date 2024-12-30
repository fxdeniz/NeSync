const { contextBridge, ipcRenderer } = require('electron/renderer');

contextBridge.exposeInMainWorld('router', {
  routeToFileExplorer: () => ipcRenderer.send('route:FileExplorer'),
  routeToFileMonitor: () => ipcRenderer.send('route:FileMonitor'),
  routeToAddFolder: () => ipcRenderer.send('route:AddFolder'),
  routeToSaveChanges: () => ipcRenderer.send('route:SaveChanges'),
  routeToZipExport: () => ipcRenderer.send('route:ZipExport'),
  routeToZipImport: () => ipcRenderer.send('route:ZipImport')
});

contextBridge.exposeInMainWorld('fileExplorerApi', {
  showFolderSelectDialog: () => ipcRenderer.invoke("dialog:OpenFolder"),
  showFileSelectDialog: () => ipcRenderer.invoke("dialog:OpenFile"),
  showFileSaveDialog: () => ipcRenderer.invoke("dialog:SaveFile")
});

contextBridge.exposeInMainWorld('pathApi', {
  splitPath: (input) => {
    return new Promise((resolve, reject) => {
      ipcRenderer.invoke('path:Split', input)
        .then(resolve)
        .catch(reject);
    });
  },

  fileNameWithExtension: (input) => {
    return new Promise((resolve, reject) => {
      ipcRenderer.invoke('path:FileName', input)
        .then(resolve)
        .catch(reject);
    });
  }
});

contextBridge.exposeInMainWorld('appState', {
  get: (key) => {
    return new Promise((resolve, reject) => {
      ipcRenderer.invoke('state:Get', key)
        .then(resolve)
        .catch(reject);
    });
  },

  set: (key, value) => {
    return new Promise((resolve, reject) => {
      ipcRenderer.invoke('state:Set', key, value)
        .then(resolve)
        .catch(reject);
    });
  }
});