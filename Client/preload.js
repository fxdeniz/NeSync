const { contextBridge, ipcRenderer } = require('electron/renderer');

contextBridge.exposeInMainWorld('router', {
  routeToFileExplorer: () => ipcRenderer.send('route:FileExplorer'),
  routeToFileMonitor: () => ipcRenderer.send('route:FileMonitor'),
  routeToAddFolder: () => ipcRenderer.send('route:AddFolder'),
  routeToSaveChanges: () => ipcRenderer.send('route:SaveChanges'),
  routeToZipExport: () => ipcRenderer.send('route:ZipExport'),
  routeToZipImport: () => ipcRenderer.send('route:ZipImport'),
  routeToFileInfo: () => ipcRenderer.send('route:FileInfo'),
  routeToFolderInfo: () => ipcRenderer.send('route:FolderInfo')
});

contextBridge.exposeInMainWorld('dialogApi', {
  showFolderSelectDialog: () => ipcRenderer.invoke("dialog:OpenFolder"),
  showFileSelectDialog: () => ipcRenderer.invoke("dialog:OpenFile"),
  showFileSaveDialog: () => ipcRenderer.invoke("dialog:SaveFile")
});

contextBridge.exposeInMainWorld('fsApi', {
  splitPath: (input) => {
    return new Promise((resolve, reject) => {
      ipcRenderer.invoke('fs:SplitPath', input)
        .then(resolve)
        .catch(reject);
    });
  },
  normalizePath: (input) => {
    return new Promise((resolve, reject) => {
      ipcRenderer.invoke('fs:NormalizePath', input)
        .then(resolve)
        .catch(reject);
    });
  },
  fileNameWithExtension: (input) => {
    return new Promise((resolve, reject) => {
      ipcRenderer.invoke('fs:FileNameFromPath', input)
        .then(resolve)
        .catch(reject);
    });
  },
  isPathExists: (input) => {
    return new Promise((resolve, reject) => {
      ipcRenderer.invoke('fs:CheckPath', input)
        .then(resolve)
        .catch(reject);
    });
  },
  previewFile: (path, extension) => {
    return new Promise((resolve, reject) => {
      ipcRenderer.invoke('fs:Preview', path, extension)
        .then(resolve)
        .catch(reject);
    });
  },
  extractFile: (srcPath, destPath) => {
    return new Promise((resolve, reject) => {
      ipcRenderer.invoke('fs:Extract', srcPath, destPath)
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