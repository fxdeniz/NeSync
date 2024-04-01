const { contextBridge, ipcRenderer } = require('electron/renderer');

contextBridge.exposeInMainWorld('router', {
  routeToFileExplorer: () => ipcRenderer.send('route:FileExplorer')
});

contextBridge.exposeInMainWorld('fileExplorerApi', {
  showFolderSelectDialog: () => ipcRenderer.invoke('dialog:OpenFolder')
});
