const { contextBridge, ipcRenderer } = require('electron/renderer');

contextBridge.exposeInMainWorld('router', {
  routeToFileExplorer: () => ipcRenderer.send('route:FileExplorer')
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
  }
});
