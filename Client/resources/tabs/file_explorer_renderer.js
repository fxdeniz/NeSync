async function postJSON(targetUrl, requestBody) {
    try {
      const response = await fetch(targetUrl, {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify(requestBody),
      });
  
      const result = await response.text();
      console.log("Success:", result);
    } catch (error) {
      console.error("Error:", error);
    }
  }

  async function fetchJSON(targetUrl) {
    try {
      const response = await fetch(targetUrl);
      
      if (!response.ok) {
        throw new Error('Network response was not ok');
      }
      
      const result = await response.json();
      
      return result;

    } catch (error) {
      console.error('There was a problem with the fetch operation:', error);
    }
  }

document.addEventListener("DOMContentLoaded", async (event) => {

    let inputCurrentPath = document.getElementById('input-current-path');
    let buttonPrev = document.getElementById('button-prev');
    let buttonNext = document.getElementById('button-next');
    const buttonAddNewFolder = document.getElementById('button-add-new-folder');

    buttonPrev.disabled = true;
    buttonNext.disabled = true;
    inputCurrentPath.value = '/';
    inputCurrentPath.readOnly = true;

    buttonAddNewFolder.addEventListener('click', async () => {

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

            // When files added inside the while loop, they cause some folders not to be created.
            // So first create folders, then add the files.
            for(currentFile of fileList) {
              await sendAddFileRequest(currentFile.symbolFolderPath,
                                       currentFile.pathToFile,
                                       currentFile.description,
                                       currentFile.isFrozen);
            }
        }
    });

    let folderJson = await fetchJSON(`http://localhost:1234/getFolderContent?symbolPath=${inputCurrentPath.value}`);

    folderJson['childFolders'].forEach(currentFolder => {
      let tableExplorerBody = document.querySelector('#table-explorer tbody');

      let row = document.createElement('tr');
      let colName = document.createElement('td');
      let colLocation = document.createElement('td');
      
      colName.innerText = currentFolder['suffixPath'];
      colLocation.innerText = currentFolder['userFolderPath'];

      row.appendChild(colName);
      row.appendChild(colLocation);

      tableExplorerBody.appendChild(row);
    });

  });
  

async function sendAddFolderRequest(symbolFolderPath, userFolderPath) {
  let requestBody = {"symbolFolderPath": null, "userFolderPath": null};
  requestBody["symbolFolderPath"] = symbolFolderPath;
  requestBody["userFolderPath"] = userFolderPath;

  await postJSON('http://localhost:1234/addNewFolder', requestBody);    
}

async function sendAddFileRequest(symbolFolderPath, pathToFile, description, isFrozen) {
  let requestBody = {};
  requestBody["symbolFolderPath"] = symbolFolderPath;
  requestBody["pathToFile"] = pathToFile;
  requestBody["description"] = description;
  requestBody["isFrozen"] = isFrozen;

  await postJSON('http://localhost:1234/addNewFile', requestBody);    
}