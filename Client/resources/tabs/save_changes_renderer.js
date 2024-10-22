document.addEventListener("DOMContentLoaded", async (event) => {
    let newAddedJson = await window.fmState.getNewAddedJson();
    let deletedJson = await window.fmState.getDeletedJson();
    let updatedJson = await window.fmState.getUpdatedJson();

    let textAreaLog = document.getElementById('text-area-log');

    appendLog(textAreaLog, "Deleting these folders including all child files & folders:");

    for (const currentFolder of deletedJson.folders) {
      const folderJson = await fetchJSON(`http://localhost:1234/getFolderContentByUserPath?userFolderPath=${currentFolder}`);
      const response = await fetch(`http://localhost:1234/deleteFolder?symbolPath=${folderJson.symbolFolderPath}`, {
        method: 'DELETE',
      });

      const result = await response.json();
      appendLog(textAreaLog, `\t - Deleting folder ${folderJson.symbolFolderPath} with contents...`);
      appendLog(textAreaLog, `\t\t Deleted Successfully: ${result.isDeleted ? '✅' : '❌'}`);
    }

    appendLog(textAreaLog, "");
    appendLog(textAreaLog, "Deleting these files:");

    for (const currentFolder in deletedJson.files) {
      appendLog(textAreaLog, `\t - Deleting selected files inside ${currentFolder}:`);

      for (const fileName of deletedJson.files[currentFolder]) {
        if (!deletedJson.folders.includes(currentFolder)) {
          const fileJson = await fetchJSON(`http://localhost:1234/getFileContentByUserPath?userFilePath=${currentFolder + fileName}`);
          const response = await fetch(`http://localhost:1234/deleteFile?symbolPath=${fileJson.symbolFilePath}`, {
            method: 'DELETE',
          });
    
          const result = await response.json();
          appendLog(textAreaLog, `\t\t - Deleting file ${fileName}...`);
          appendLog(textAreaLog, `\t\t\t Deleted Successfully: ${result.isDeleted ? '✅' : '❌'}`);
        }
      }
    }
    
    appendLog(textAreaLog, "");
    appendLog(textAreaLog, "Creating new added folders:");

    for (let index = 0; index < newAddedJson.rootFolders.length; index++) {
      const currentUserFolderPath = newAddedJson.rootFolders[index];
      const parentUserFolderPath = newAddedJson.rootOfRootFolder[currentUserFolderPath];

      const parentFolderJson = await fetchJSON(`http://localhost:1234/getFolderContentByUserPath?userFolderPath=${parentUserFolderPath}`);
      let pathTokens = await window.pathApi.splitPath(currentUserFolderPath);
      pathTokens.pop(); // remove last element whcih is ''

      const parentSymbolFolderPath = parentFolderJson.symbolFolderPath + pathTokens.pop() + "/";

      appendLog(textAreaLog, `\t - Creating new folder ${currentUserFolderPath}...`);
      const result = await sendAddFolderRequest(parentSymbolFolderPath, currentUserFolderPath);
      appendLog(textAreaLog, `\t\t Created Successfully: ${result.isAdded ? '✅' : '❌'}:`);

      let childSuffixes = newAddedJson.childFolderSuffixes[currentUserFolderPath];

      if(childSuffixes) {
        appendLog(textAreaLog, `\t\t Creating child folders of: ${currentUserFolderPath}...`);

        for(let childIndex = 0; childIndex < childSuffixes.length; childIndex++) {
          const childFolderUserPath = currentUserFolderPath + childSuffixes[childIndex]; // Suffix already ends with /.
          const childFolderSymbolPath = parentSymbolFolderPath + childSuffixes[childIndex]; // Suffix already ends with /.
  
          appendLog(textAreaLog, `\t\t\t - Creating new child folder ${childSuffixes[childIndex]}...`);
          const result = await sendAddFolderRequest(childFolderSymbolPath, childFolderUserPath);
          appendLog(textAreaLog, `\t\t\t\t Created Successfully: ${result.isAdded ? '✅' : '❌'}:`);
        }
      }
    }

    appendLog(textAreaLog, "");
    appendLog(textAreaLog, "Adding new files into previously created folders:");

    for (const currentFolder in newAddedJson.files) {
      appendLog(textAreaLog, `\t Adding new files of new folder:  ${currentFolder}...`);

      for (const fileName of newAddedJson.files[currentFolder]) {
        const folderJson = await fetchJSON(`http://localhost:1234/getFolderContentByUserPath?userFolderPath=${currentFolder}`);
        const result = await sendAddFileRequest(folderJson.symbolFolderPath, currentFolder + fileName, "", false);
        appendLog(textAreaLog, `\t\t Adding new file  ${fileName}...`);
        appendLog(textAreaLog, `\t\t\t Added  Successfully: ${result.isAdded ? '✅' : '❌'}:`);
      }
    }

    appendLog(textAreaLog, "");
    appendLog(textAreaLog, "Adding updated files inside existing folders:");

    for (const currentFolder in updatedJson) {
      appendLog(textAreaLog, `\t - Adding updated files inside folder: ${currentFolder}...`);
      for (const fileName of updatedJson[currentFolder]) {
        const result = await sendAppendVersionRequest(currentFolder + fileName, await window.fmState.getCommitMessage());
        appendLog(textAreaLog, `\t\t - Adding new version of ${fileName}...`);
        appendLog(textAreaLog, `\t\t\t Added Successfully: ${result.isAppended ? '✅' : '❌'}:`);
      }
    }
});


function appendLog(elementTextArea, logText) {
  elementTextArea.value += logText + '\n';
  elementTextArea.scrollTop = elementTextArea.scrollHeight;
  elementTextArea.focus();
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