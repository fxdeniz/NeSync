const { contextBridge, ipcRenderer } = require('electron/renderer');

contextBridge.exposeInMainWorld('router', {
  routeToFileExplorer: () => ipcRenderer.send('route:FileExplorer'),
  routeToFileMonitor: () => ipcRenderer.send('route:FileMonitor'),
  routeToSaveChanges: () => ipcRenderer.send('route:SaveChanges')
});

contextBridge.exposeInMainWorld('fileExplorerApi', {
  showFolderSelectDialog: () => ipcRenderer.invoke('dialog:OpenFolder')
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
});