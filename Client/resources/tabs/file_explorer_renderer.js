document.addEventListener("DOMContentLoaded", async (event) => {

    let inputCurrentPath = document.getElementById('input-current-path');
    let buttonPrev = document.getElementById('button-prev');
    let buttonNext = document.getElementById('button-next');
    const buttonAddNewFolder = document.getElementById('button-add-new-folder');
    const buttonFileMonitor = document.getElementById("button-file-monitor");
    const buttonSelectZipFilePath = document.getElementById("button-select-zip-path");

    buttonFileMonitor.addEventListener('click', async clickEvent => {
      window.router.routeToFileMonitor();
    });

    buttonSelectZipFilePath.addEventListener("click", async clickEvent => {
      console.log(await window.fileExplorerApi.showFileSaveDialog())
    });

    buttonPrev.disabled = true;
    buttonNext.disabled = true;
    inputCurrentPath.readOnly = true;

    inputCurrentPath.addEventListener('directoryNavigation', onDirectoryChangeHandler_inputCurrentPath);
    buttonAddNewFolder.addEventListener('click', onClickHandler_buttonAddNewFolder);

    let navigationEvnet = createDirectoryChangeEvent('/');
    inputCurrentPath.dispatchEvent(navigationEvnet);

    const exportModal = document.getElementById('export-modal');

    exportModal.addEventListener('shown.bs.modal', function () {
      const currentPath = document.getElementById("input-current-path").value;
      const pExportSource = document.getElementById("p-export-source");
      pExportSource.innerHTML = `<b>${currentPath}</b>`;
      });
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

function createDirectoryChangeEvent(targetPath) {
  return new CustomEvent('directoryNavigation', {bubbles: true, detail: {'targetPath': targetPath}});
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