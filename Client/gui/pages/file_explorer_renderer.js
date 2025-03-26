import FolderApi from "../rest_api/FolderApi.mjs";
import ZipExportApi from "../rest_api/ZipExportApi.mjs";
import ZipImportApi from "../rest_api/ZipImportApi.mjs";

document.addEventListener("DOMContentLoaded", async (event) => {

    const inputCurrentPath = document.getElementById('input-current-path');
    const buttonBack = document.getElementById('button-back');
    const buttonAddNewFolder = document.getElementById('button-add-new-folder');
    const buttonFileMonitor = document.getElementById("button-file-monitor");
    const buttonSelectZipFileExportPath = document.getElementById("button-select-zip-export-path");
    const buttonSelectZipFileImportPath = document.getElementById("button-select-zip-import-path");
    const buttonExport = document.getElementById("button-export");
    const buttonImport = document.getElementById("button-import");
    const exportModal = document.getElementById("export-modal");
    const importModal = document.getElementById("import-modal");

    buttonFileMonitor.addEventListener('click', async clickEvent => {
      window.router.routeToFileMonitor();
    });

    inputCurrentPath.readOnly = true;
    inputCurrentPath.addEventListener('directoryNavigation', onDirectoryChangeHandler_inputCurrentPath);
    buttonBack.addEventListener('click', onClickHandler_buttonBack);
    buttonAddNewFolder.addEventListener('click', onClickHandler_buttonAddNewFolder);
    buttonSelectZipFileExportPath.addEventListener("click", onClickHandler_buttonSelectZipFileExportPath);
    buttonSelectZipFileImportPath.addEventListener("click", onClickHandler_buttonSelectZipFileImportPath);
    buttonExport.addEventListener("click", onClickHandler_buttonExport);
    buttonImport.addEventListener("click", onClickHandler_buttonImport);
    exportModal.addEventListener("shown.bs.modal", onShownHandler_exportModal);
    importModal.addEventListener("shown.bs.modal", onShownHandler_importModal);

    let target;
    const currentFolder = await window.appState.get("currentFolder");

    if(!currentFolder)
      target = '/';
    else {
      const folderApi = new FolderApi('localhost', 1234);
      const folder = await folderApi.get(currentFolder.symbolFolderPath);
  
      if(!folder.isExist) // Current folder not exists in the server.
        target = '/';
      else
        target = currentFolder.symbolFolderPath;
    }

    const navigationEvent = createDirectoryChangeEvent(target);
    inputCurrentPath.dispatchEvent(navigationEvent);
});

async function onClickHandler_buttonAddNewFolder() {
  const selectedFolderTree = await window.dialogApi.showFolderSelectDialog();

  if(selectedFolderTree) {
      let stack = [selectedFolderTree];
      await window.appState.set("selectedFolderStack", stack);
      window.router.routeToAddFolder();
  }
}

async function onDirectoryChangeHandler_inputCurrentPath(event) {
  event.target.value = event.detail.targetPath; // Set inputCurrentPath's new value.

  const buttonBack = document.getElementById('button-back');
  const tableExplorerBody = document.querySelector('#table-explorer tbody');
  tableExplorerBody.innerHTML = "";  // Clean previous rows from table.

  const folderApi = new FolderApi('localhost', 1234);
  const folderJson = await folderApi.get(event.detail.targetPath);
  await window.appState.set("currentFolder", folderJson);

  if(folderJson.parentFolderPath === '')
    buttonBack.disabled = true;
  else
    buttonBack.disabled = false;

  if(folderJson.childFolders) {
    folderJson.childFolders.forEach(currentFolder => {
        let row = document.createElement('tr');
        let colName = document.createElement('td');
        let colLocation = document.createElement('td');
        
        colName.innerText = currentFolder.suffixPath;
        colLocation.innerText = currentFolder.userFolderPath;
    
        row.appendChild(colName);
        row.appendChild(colLocation);
        row.dataset.type = "folder";
        row.dataset.symbolFolderPath = currentFolder.symbolFolderPath;
    
        row.addEventListener('click', () => {
          if(row.dataset.type === "folder") {
            let navigationEvnet = createDirectoryChangeEvent(row.dataset.symbolFolderPath);
            event.target.dispatchEvent(navigationEvnet);
          }
        });

        row.addEventListener('contextmenu', () => {
          if(row.dataset.type === "folder")
            window.router.routeToFolderInfo();
        });
    
        tableExplorerBody.appendChild(row);
    });
  }

  if(folderJson.childFiles) {
    folderJson.childFiles.forEach(currentFile => {
      let row = document.createElement('tr');
      let colName = document.createElement('td');
      let colLocation = document.createElement('td');
      
      colName.innerText = currentFile.fileName;
      colLocation.innerText = currentFile.userFilePath;
  
      row.appendChild(colName);
      row.appendChild(colLocation);
      row.dataset.type = "file";
      row.dataset.symbolFolderPath = currentFile.symbolFolderPath;
      row.dataset.symbolFilePath = currentFile.symbolFilePath;

      row.addEventListener('click', async () => {
        if(row.dataset.type === "file") {
          await window.appState.set("currentFile", currentFile);
          await window.appState.set("selectedVersionNumber", null);
          window.router.routeToFileInfo();
        }
      });
  
      tableExplorerBody.appendChild(row);
    });
  }
}

async function onClickHandler_buttonBack() {
  const inputCurrentPath = document.getElementById('input-current-path');
  const folder = await window.appState.get("currentFolder");

  const navigationEvent = createDirectoryChangeEvent(folder.parentFolderPath);
  inputCurrentPath.dispatchEvent(navigationEvent);
}

async function onClickHandler_buttonSelectZipFileExportPath() {
  const inputZipExportPath = document.getElementById("input-zip-export-path");
  const selectedPath = await window.dialogApi.showFileSaveDialog();

  if(selectedPath) {
    if(!selectedPath.endsWith(".zip")) {
      alert("File name should end with \".zip\" extension.");
      return;
    }

    inputZipExportPath.value = selectedPath;
    const buttonExport = document.getElementById("button-export");
    buttonExport.disabled = false;
    buttonExport.focus();
  }
}

async function onClickHandler_buttonSelectZipFileImportPath() {
  const inputZipImportPath = document.getElementById("input-zip-import-path");
  const selectedPath = await window.dialogApi.showFileSelectDialog();

  if(selectedPath) {
    if(!selectedPath.endsWith(".zip")) {
      alert("File name should end with \".zip\" extension.");
      return;
    }

    inputZipImportPath.value = selectedPath;
    const buttonImport = document.getElementById("button-import");
    buttonImport.disabled = false;
    buttonImport.focus();
  }
}

async function onClickHandler_buttonExport() {
  let exportApi = new ZipExportApi('localhost', 1234);
  const filePath = document.getElementById("input-zip-export-path").value;
  // Get rootSymbolFolderPath without the <b> tags
  const rootSymbolFolderPath = document.getElementById("p-export-source").textContent;
  await exportApi.setFilePath(filePath);
  await exportApi.setRootFolder(rootSymbolFolderPath);
  window.router.routeToZipExport();
}

async function onClickHandler_buttonImport() {
  let importApi = new ZipImportApi('localhost', 1234);
  const filePath = document.getElementById("input-zip-import-path").value;
  await importApi.setFilePath(filePath);
  window.router.routeToZipImport();
}

function onShownHandler_exportModal() {
  document.getElementById("input-zip-export-path").value = "";
  document.getElementById("button-export").disabled = true;
  document.getElementById("button-select-zip-export-path").focus();

  const currentPath = document.getElementById("input-current-path").value;
  const pExportSource = document.getElementById("p-export-source");
  pExportSource.innerHTML = `<b>${currentPath}</b>`;
}

function onShownHandler_importModal() {
  document.getElementById("input-zip-import-path").value = "";
  document.getElementById("button-import").disabled = true;
  document.getElementById("button-select-zip-import-path").focus();
}

function createDirectoryChangeEvent(targetPath) {
  return new CustomEvent('directoryNavigation', {bubbles: true, detail: {'targetPath': targetPath}});
}