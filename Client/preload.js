const { contextBridge, ipcRenderer } = require('electron/renderer');

contextBridge.exposeInMainWorld('router', {
  routeToFileExplorer: () => ipcRenderer.send('route:FileExplorer'),
  routeToFileMonitor: () => ipcRenderer.send('route:FileMonitor'),
  routeToSaveChanges: () => ipcRenderer.send('route:SaveChanges'),
  routeToZipExport: () => ipcRenderer.send('route:ZipExport')
});

contextBridge.exposeInMainWorld('fileExplorerApi', {
  showFolderSelectDialog: () => ipcRenderer.invoke('dialog:OpenFolder'),
  showFileSaveDialog: () => ipcRenderer.invoke('dialog:SaveFile')
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

contextBridge.exposeInMainWorld('fmState', {
  setCommitMessage: (input) => {
    return new Promise((resolve, reject) => {
      ipcRenderer.invoke('fmState:setCommitMessage', input)
        .then(resolve)
        .catch(reject);
    });
  },

  getCommitMessage: () => {
    return new Promise((resolve, reject) => {
      ipcRenderer.invoke('fmState:getCommitMessage')
        .then(resolve)
        .catch(reject);
    });
  },

  setNewAddedJson: (input) => {
    return new Promise((resolve, reject) => {
      ipcRenderer.invoke('fmState:setNewAddedJson', input)
        .then(resolve)
        .catch(reject);
    });
  },

  getNewAddedJson: () => {
    return new Promise((resolve, reject) => {
      ipcRenderer.invoke('fmState:getNewAddedJson')
        .then(resolve)
        .catch(reject);
    });
  },

  setDeletedJson: (input) => {
    return new Promise((resolve, reject) => {
      ipcRenderer.invoke('fmState:setDeletedJson', input)
        .then(resolve)
        .catch(reject);
    });
  },

  getDeletedJson: () => {
    return new Promise((resolve, reject) => {
      ipcRenderer.invoke('fmState:getDeletedJson')
        .then(resolve)
        .catch(reject);
    });
  },

  setUpdatedJson: (input) => {
    return new Promise((resolve, reject) => {
      ipcRenderer.invoke('fmState:setUpdatedJson', input)
        .then(resolve)
        .catch(reject);
    });
  },

  getUpdatedJson: () => {
    return new Promise((resolve, reject) => {
      ipcRenderer.invoke('fmState:getUpdatedJson')
        .then(resolve)
        .catch(reject);
    });
  }
});

contextBridge.exposeInMainWorld('feState', {
  setZipFilePath: (input) => {
    return new Promise((resolve, reject) => {
      ipcRenderer.invoke('feState:setZipFilePath', input)
        .then(resolve)
        .catch(reject);
    });
  },

  getZipFilePath: () => {
    return new Promise((resolve, reject) => {
      ipcRenderer.invoke('feState:getZipFilePath')
        .then(resolve)
        .catch(reject);
    });
  }
});