document.addEventListener("DOMContentLoaded", async (event) => {
    let newAddedJson = await window.fmState.getNewAddedJson();
    let deletedJson = await window.fmState.getDeletedJson();
    let updatedJson = await window.fmState.getUpdatedJson();

    let buttonClose = document.getElementById('button-close');
    buttonClose.addEventListener('click', async clickEvent => window.router.routeToFileExplorer());
    disableButton(buttonClose);

    let textAreaLog = document.getElementById('text-area-log');

    appendLog(textAreaLog, "ℹ️ Deleting these folders including all child files & folders:");

    let reversedFolders = Array.from(deletedJson.folders);
    reversedFolders.reverse(); // TODO: improve outputs without using this
    // We are using reverse to delete leaf first
    // When deletion starts from roots, child items shown as error in log
    // This due to deleting roots also deletes children on the server.

    for (const currentFolder of reversedFolders) {
      const folderJson = await sendGetFolderByUserPathRequest(currentFolder);
      const response = await sendDeleteFolderRequest(folderJson.symbolFolderPath);

      const result = await response.json();
      appendLog(textAreaLog, `\t 👉 Deleting folder ${folderJson.symbolFolderPath} with contents...`);
      appendLog(textAreaLog, `\t\t Deleted Successfully: ${result.isDeleted ? '✅' : '❌'}`);
    }

    appendLog(textAreaLog, "👍 Finished deleting folders.")
    appendLog(textAreaLog, "");
    appendLog(textAreaLog, "ℹ️ Deleting these files:");

    for (const currentFolder in deletedJson.files) {
      if (!deletedJson.folders.includes(currentFolder)) {
        appendLog(textAreaLog, `\t Deleting selected files inside ${currentFolder}:`);

        for (const fileName of deletedJson.files[currentFolder]) {
            const fileJson = await sendGetFileByUserPathRequest(currentFolder + fileName);
            const response = await sendDeleteFileRequest(fileJson.symbolFilePath);
      
            const result = await response.json();
            appendLog(textAreaLog, `\t\t 👉 Deleting file ${fileName}`);
            appendLog(textAreaLog, `\t\t\t Deleted Successfully: ${result.isDeleted ? '✅' : '❌'}`);
        }
      }
    }
    
    appendLog(textAreaLog, "👍 Finished deleting files.")
    appendLog(textAreaLog, "");
    appendLog(textAreaLog, "ℹ️ Creating new added folders:");

    for (let index = 0; index < newAddedJson.rootFolders.length; index++) {
      const currentUserFolderPath = newAddedJson.rootFolders[index];
      const parentUserFolderPath = newAddedJson.rootOfRootFolder[currentUserFolderPath];

      const parentFolderJson = await sendGetFolderByUserPathRequest(parentUserFolderPath);
      let pathTokens = await window.pathApi.splitPath(currentUserFolderPath);
      pathTokens.pop(); // remove last element whcih is ''

      const parentSymbolFolderPath = parentFolderJson.symbolFolderPath + pathTokens.pop() + "/";

      appendLog(textAreaLog, `\t 👉 Creating new folder ${currentUserFolderPath}`);
      const result = await sendAddFolderRequest(parentSymbolFolderPath, currentUserFolderPath);
      appendLog(textAreaLog, `\t\t Created Successfully: ${result.isAdded ? '✅' : '❌'}:`);

      let childSuffixes = newAddedJson.childFolderSuffixes[currentUserFolderPath];

      if(childSuffixes) {
        appendLog(textAreaLog, `\t\t Creating child folders of: ${currentUserFolderPath}`);

        for(let childIndex = 0; childIndex < childSuffixes.length; childIndex++) {
          const childFolderUserPath = currentUserFolderPath + childSuffixes[childIndex]; // Suffix already ends with /.
          const childFolderSymbolPath = parentSymbolFolderPath + childSuffixes[childIndex]; // Suffix already ends with /.
  
          appendLog(textAreaLog, `\t\t\t 👉 Creating new child folder ${childSuffixes[childIndex]}`);
          const result = await sendAddFolderRequest(childFolderSymbolPath, childFolderUserPath);
          appendLog(textAreaLog, `\t\t\t\t Created Successfully: ${result.isAdded ? '✅' : '❌'}:`);
        }
      }
    }

    appendLog(textAreaLog, "👍 Finished creating new folders.")
    appendLog(textAreaLog, "");
    appendLog(textAreaLog, "ℹ️ Adding new files into previously created folders:");

    for (const currentFolder in newAddedJson.files) {
      appendLog(textAreaLog, `\t Adding new files of new folder: ${currentFolder}`);

      for (const fileName of newAddedJson.files[currentFolder]) {
        const folderJson = await sendGetFolderByUserPathRequest(currentFolder);
        const result = await sendAddFileRequest(folderJson.symbolFolderPath, currentFolder + fileName, "", false);
        appendLog(textAreaLog, `\t\t 👉 Adding new file  ${fileName}`);
        appendLog(textAreaLog, `\t\t\t Added  Successfully: ${result.isAdded ? '✅' : '❌'}:`);
      }
    }

    appendLog(textAreaLog, "👍 Finished adding new files.")
    appendLog(textAreaLog, "");
    appendLog(textAreaLog, "ℹ️ Adding updated files inside existing folders:");

    for (const currentFolder in updatedJson) {
      appendLog(textAreaLog, `\t Adding updated files inside folder: ${currentFolder}`);
      for (const fileName of updatedJson[currentFolder]) {
        const result = await sendAppendVersionRequest(currentFolder + fileName, await window.fmState.getCommitMessage());
        appendLog(textAreaLog, `\t\t 👉 Adding new version of ${fileName}`);
        appendLog(textAreaLog, `\t\t\t Added Successfully: ${result.isAppended ? '✅' : '❌'}:`);
      }
    }

    appendLog(textAreaLog, "👍 Finished adding updated files.")
    appendLog(textAreaLog, "");
    appendLog(textAreaLog, "💯 All file & folder operations finished.");
    enableButton(buttonClose);
});


function appendLog(elementTextArea, logText) {
  elementTextArea.value += logText + '\n';
  elementTextArea.scrollTop = elementTextArea.scrollHeight;
  elementTextArea.focus();
}


function disableButton(elementButton) {
  elementButton.disabled = true;
  elementButton.textContent = "In progress...";
}


function enableButton(elementButton) {
  elementButton.disabled = false;
  elementButton.textContent = "Close";
}


async function sendGetFolderByUserPathRequest(userFolderPath) {
  return await fetchJSON(`http://localhost:1234/getFolderContentByUserPath?userFolderPath=${userFolderPath}`);
}


async function sendGetFileByUserPathRequest(userFilePath) {
  return await fetchJSON(`http://localhost:1234/getFileContentByUserPath?userFilePath=${userFilePath}`);
}


async function sendDeleteFolderRequest(symbolFolderPath) {
  return await fetch(`http://localhost:1234/deleteFolder?symbolPath=${symbolFolderPath}`, {
    method: 'DELETE',
  });
}


async function sendAddFolderRequest(symbolFolderPath, userFolderPath) {
  let requestBody = {"symbolFolderPath": null, "userFolderPath": null};
  requestBody["symbolFolderPath"] = symbolFolderPath;
  requestBody["userFolderPath"] = userFolderPath;

  return await postJSON('http://localhost:1234/addNewFolder', requestBody);    
}


async function sendDeleteFileRequest(symbolFilePath) {
  return await fetch(`http://localhost:1234/deleteFile?symbolPath=${symbolFilePath}`, {
    method: 'DELETE',
  });
}


async function sendAddFileRequest(symbolFolderPath, pathToFile, description, isFrozen) {
  let requestBody = {};
  requestBody["symbolFolderPath"] = symbolFolderPath;
  requestBody["pathToFile"] = pathToFile;
  requestBody["description"] = description;
  requestBody["isFrozen"] = isFrozen;

  return await postJSON('http://localhost:1234/addNewFile', requestBody);    
}


async function sendAppendVersionRequest(pathToFile, description) {
  let requestBody = {};
  requestBody["pathToFile"] = pathToFile;
  requestBody["description"] = description;

  return await postJSON('http://localhost:1234/appendVersion', requestBody);    
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