document.addEventListener("DOMContentLoaded", async (event) => {

    let inputCurrentPath = document.getElementById('input-current-path');
    let buttonPrev = document.getElementById('button-prev');
    let buttonNext = document.getElementById('button-next');
    const buttonAddNewFolder = document.getElementById('button-add-new-folder');
    const buttonFileMonitor = document.getElementById("button-file-monitor");
    const buttonSelectZipFileExportPath = document.getElementById("button-select-zip-export-path");
    const buttonExport = document.getElementById("button-export");
    const exportModal = document.getElementById("export-modal");

    buttonFileMonitor.addEventListener('click', async clickEvent => {
      window.router.routeToFileMonitor();
    });

    buttonPrev.disabled = true;
    buttonNext.disabled = true;
    inputCurrentPath.readOnly = true;

    inputCurrentPath.addEventListener('directoryNavigation', onDirectoryChangeHandler_inputCurrentPath);
    buttonAddNewFolder.addEventListener('click', onClickHandler_buttonAddNewFolder);
    buttonSelectZipFileExportPath.addEventListener("click", onClickHandler_buttonSelectZipFilePath);
    buttonExport.addEventListener("click", onClickHandler_buttonExport);
    exportModal.addEventListener("shown.bs.modal", onShownHandler_exportModal);

    let navigationEvnet = createDirectoryChangeEvent('/');
    inputCurrentPath.dispatchEvent(navigationEvnet);
});

async function onClickHandler_buttonAddNewFolder() {
  const selectedFolderTree = await window.fileExplorerApi.showFolderSelectDialog();

  if(selectedFolderTree) {
      let stack = [selectedFolderTree];
      let fileList = [];

      while (stack.length > 0) {
          let currentFolder = stack.pop();
          let pathTokens = await window.pathApi.splitPath(currentFolder.folderPath);
          let symbolFolderSuffix = pathTokens.pop() + "/";

          if(currentFolder.symbolFolderPath === undefined) // Check symbol folder path of the root.
            currentFolder.symbolFolderPath = "/" + symbolFolderSuffix;
          else
            currentFolder.symbolFolderPath += symbolFolderSuffix;

          await sendAddFolderRequest(currentFolder.symbolFolderPath, currentFolder.folderPath);

          for(filePath of currentFolder.childFiles) {
            let fileName = await window.pathApi.fileNameWithExtension(filePath);
            fileList.push({symbolFolderPath: currentFolder.symbolFolderPath,
                            pathToFile: filePath,
                            description: `First version of <b>${fileName}</b>.`,
                            isFrozen: false
            });
          }
  
          for (let index = (currentFolder.childFolders.length - 1); index >= 0; index--) {
              currentFolder.childFolders[index].symbolFolderPath = currentFolder.symbolFolderPath;
              stack.push(currentFolder.childFolders[index]);
          }
      }

      for(currentFile of fileList) {
        await sendAddFileRequest(currentFile.symbolFolderPath,
                                  currentFile.pathToFile,
                                  currentFile.description,
                                  currentFile.isFrozen);
      }
  }
}

async function onDirectoryChangeHandler_inputCurrentPath(event) {
  event.target.value = event.detail.targetPath; // Set inputCurrentPath's new value.

  let tableExplorerBody = document.querySelector('#table-explorer tbody');
  tableExplorerBody.innerHTML = "";  // Clean previous rows from table.

  let folderJson = await fetchJSON(`http://localhost:1234/getFolderContent?symbolPath=${event.detail.targetPath}`);

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
  
      tableExplorerBody.appendChild(row);
    });
  }
}

async function onClickHandler_buttonSelectZipFilePath() {
  const inputZipExportPath = document.getElementById("input-zip-export-path");
  const selectedPath = await window.fileExplorerApi.showFileSaveDialog();

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

async function onClickHandler_buttonExport() {
  const filePath = document.getElementById("input-zip-export-path").value;
  // Get rootSymbolFolderPath without the <b> tags
  const rootSymbolFolderPath = document.getElementById("p-export-source").textContent;
  await sendSetZipFilePathRequest(filePath);
  await sendSetRootSymbolFolderPath(rootSymbolFolderPath);
  window.router.routeToZipExport();
}

function onShownHandler_exportModal() {
  document.getElementById("input-zip-export-path").value = "";
  document.getElementById("button-export").disabled = true;
  document.getElementById("button-select-zip-export-path").focus();

  const currentPath = document.getElementById("input-current-path").value;
  const pExportSource = document.getElementById("p-export-source");
  pExportSource.innerHTML = `<b>${currentPath}</b>`;
}

function createDirectoryChangeEvent(targetPath) {
  return new CustomEvent('directoryNavigation', {bubbles: true, detail: {'targetPath': targetPath}});
}


async function sendSetZipFilePathRequest(filePath) {
  let requestBody = {"filePath": null};
  requestBody["filePath"] = filePath;

  return await postJSON('http://localhost:1234/postSetZipFilePath', requestBody);    
}


async function sendSetRootSymbolFolderPath(rootPath) {
  let requestBody = {"rootSymbolFolderPath": null};
  requestBody["rootSymbolFolderPath"] = rootPath;

  return await postJSON('http://localhost:1234/postSetRootSymbolFolderPath', requestBody);    
}


async function sendAddFolderRequest(symbolFolderPath, userFolderPath) {
  let requestBody = {"symbolFolderPath": null, "userFolderPath": null};
  requestBody["symbolFolderPath"] = symbolFolderPath;
  requestBody["userFolderPath"] = userFolderPath;

  return await postJSON('http://localhost:1234/addNewFolder', requestBody);    
}

async function sendAddFileRequest(symbolFolderPath, pathToFile, description, isFrozen) {
  let requestBody = {};
  requestBody["symbolFolderPath"] = symbolFolderPath;
  requestBody["pathToFile"] = pathToFile;
  requestBody["description"] = description;
  requestBody["isFrozen"] = isFrozen;

  return await postJSON('http://localhost:1234/addNewFile', requestBody);    
}

async function postJSON(targetUrl, requestBody) {
  try {
    const response = await fetch(targetUrl, {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify(requestBody),
    });

    const result = await response.json();
    return result;
  } catch (error) {
    console.error("Error:", error);
  }
}

async function fetchJSON(targetUrl, methodType = "GET") {
  try {
    const response = await fetch(targetUrl, {method: methodType});
    
    if (!response.ok) {
      throw new Error('Network response was not ok');
    }
    
    const result = await response.json();
    
    return result;

  } catch (error) {
    console.error('There was a problem with the fetch operation:', error);
  }
}