document.addEventListener("DOMContentLoaded", async (event) => {
    let newAddedJson = await window.fmState.getNewAddedJson();
    let deletedJson = await window.fmState.getDeletedJson();
    let updatedJson = await window.fmState.getUpdatedJson();
  
    console.log(`newAdded: ${JSON.stringify(await window.fmState.getNewAddedJson(), null, 2)}`);
    console.log(`deleted: ${JSON.stringify(await window.fmState.getDeletedJson(), null, 2)}`);
    console.log(`updated: ${JSON.stringify(await window.fmState.getUpdatedJson(), null, 2)}`);

    let treeStatus = {};
    let folderPathSet = new Set();

    for(currentFolder of newAddedJson.folders) {
      folderPathSet.add(currentFolder);
      treeStatus[currentFolder] = "NEW";
    }

    for(currentFolder in newAddedJson.files) {
      folderPathSet.add(currentFolder);
      newAddedJson.files[currentFolder].forEach(fileName => {treeStatus[currentFolder + fileName] = "NEW"});
    }

    for(currentFolder of deletedJson.folders) {
      folderPathSet.add(currentFolder);
      treeStatus[currentFolder] = "DELETED";
    }

    for(currentFolder in deletedJson.files) {
      folderPathSet.add(currentFolder);
      deletedJson.files[currentFolder].forEach(fileName => {treeStatus[currentFolder + fileName] = "DELETED"});
    }

    Object.keys(updatedJson).forEach(key => {
      folderPathSet.add(key);
      updatedJson[key].forEach(fileName => {treeStatus[key + fileName] = "UPDATED"});
    });

    let folderPathList = Array.from(folderPathSet);
    folderPathList.sort((a, b) => a.length - b.length);

    let tree = {};

    for(path of folderPathList) {
      tree[path] = [];

      if(path in newAddedJson.files)
        tree[path].push(...newAddedJson.files[path]);

      if(path in deletedJson.files)
        tree[path].push(...deletedJson.files[path]);

      if(path in updatedJson)
        tree[path].push(...updatedJson[path]);

    }

    console.log(`tree: ${JSON.stringify(tree, null, 2)}`);
    console.log(`treeStatus: ${JSON.stringify(treeStatus, null, 2)}`);

    let textAreaLog = document.getElementById('text-area-log');

    for (const currentFolder of deletedJson.folders) {
      const folderJson = await fetchJSON(`http://localhost:1234/getFolderContentByUserPath?userFolderPath=${currentFolder}`);
      const response = await fetch(`http://localhost:1234/deleteFolder?symbolPath=${folderJson.symbolFolderPath}`, {
        method: 'DELETE',
      });

      const result = await response.json();
      console.log(`folder ${folderJson.symbolFolderPath} isDeleted = ${result.isDeleted}`);
      appendLog(textAreaLog, `(-) Deleting folder ${folderJson.symbolFolderPath} isDeleted = ${result.isDeleted}`);
    }

    for (const currentFolder in deletedJson.files) {
      for (const fileName of deletedJson.files[currentFolder]) {
        if (!deletedJson.folders.includes(currentFolder)) {
          const fileJson = await fetchJSON(`http://localhost:1234/getFileContentByUserPath?userFilePath=${currentFolder + fileName}`);
          const response = await fetch(`http://localhost:1234/deleteFile?symbolPath=${fileJson.symbolFilePath}`, {
            method: 'DELETE',
          });
    
          const result = await response.json();
          console.log(`file ${fileJson.symbolFilePath} isDeleted = ${result.isDeleted}`);
          appendLog(textAreaLog, `(-) Deleting file ${currentFolder + fileName} isDeleted = ${result.isDeleted}`);
        }
      }
    }
    
    for (let index = 0; index < newAddedJson.rootFolders.length; index++) {
      const currentUserFolderPath = newAddedJson.rootFolders[index];
      const parentUserFolderPath = newAddedJson.rootOfRootFolder[currentUserFolderPath];

      const parentFolderJson = await fetchJSON(`http://localhost:1234/getFolderContentByUserPath?userFolderPath=${parentUserFolderPath}`);
      let pathTokens = await window.pathApi.splitPath(currentUserFolderPath);
      pathTokens.pop(); // remove last element whcih is ''

      const parentSymbolFolderPath = parentFolderJson.symbolFolderPath + pathTokens.pop() + "/";

      await sendAddFolderRequest(parentSymbolFolderPath, currentUserFolderPath);
      appendLog(textAreaLog, `(+) Adding new folder ${currentUserFolderPath}:`);

      let childSuffixes = newAddedJson.childFolderSuffixes[currentUserFolderPath];

      for(let childIndex = 0; childIndex < childSuffixes.length; childIndex++) {
        const childFolderUserPath = currentUserFolderPath + childSuffixes[childIndex]; // Suffix already ends with /.
        const childFolderSymbolPath = parentSymbolFolderPath + childSuffixes[childIndex]; // Suffix already ends with /.

        await sendAddFolderRequest(childFolderSymbolPath, childFolderUserPath);
        appendLog(textAreaLog, `\t(+) Adding new child folder ${childFolderUserPath}...`);
      }
    }

    for (const currentFolder in newAddedJson.files) {
      textAreaLog.value += `(+) Adding new files of new folder:  ${currentFolder}:\n`;
      for (const fileName of newAddedJson.files[currentFolder]) {
        const folderJson = await fetchJSON(`http://localhost:1234/getFolderContentByUserPath?userFolderPath=${currentFolder}`);
        await sendAddFileRequest(folderJson.symbolFolderPath, currentFolder + fileName, "", false);
        appendLog(textAreaLog, `\t(+) Adding new file  ${currentFolder + fileName}...`);
      }
    }

    for (const currentFolder in updatedJson) {
      textAreaLog.value += `(+) Adding updated files inside folder:  ${currentFolder}:\n`;
      for (const fileName of updatedJson[currentFolder]) {
        const result = await sendAppendVersionRequest(currentFolder + fileName, await window.fmState.getCommitMessage());
        console.log(`new version of ${currentFolder + fileName} is added = ${result}`);
        appendLog(textAreaLog, `\t(+) Adding new version of ${currentFolder + fileName} is added = ${result}`);
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


async function sendAppendVersionRequest(pathToFile, description) {
  let requestBody = {};
  requestBody["pathToFile"] = pathToFile;
  requestBody["description"] = description;

  await postJSON('http://localhost:1234/appendVersion', requestBody);    
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