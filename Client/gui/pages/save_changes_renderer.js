import FolderApi from "../rest_api/FolderApi.mjs";
import FileApi from "../rest_api/FileApi.mjs"
import MonitorApi from "../rest_api/MonitorApi.mjs";

document.addEventListener("DOMContentLoaded", async (event) => {
    const serverPort = await window.appState.get("serverPort");
    let folderApi = new FolderApi('localhost', serverPort);
    let fileApi = new FileApi('localhost', serverPort);
    let monitorApi = new MonitorApi('localhost', serverPort);

    let newAddedJson = await monitorApi.getNewAddedList();
    let deletedJson = await monitorApi.getDeletedList();
    let updatedJson = await monitorApi.getUpdatedFileList();  

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
      const folderJson = await folderApi.getByUserPath(currentFolder);
      appendLog(textAreaLog, `\t 👉 Deleting folder ${folderJson.userFolderPath} with contents...`);
      const response = await folderApi.delete(folderJson.symbolFolderPath);
      appendLog(textAreaLog, `\t\t Deleted Successfully: ${response.isDeleted ? '✅' : '❌'}`);
    }

    appendLog(textAreaLog, "👍 Finished deleting folders.")
    appendLog(textAreaLog, "");
    appendLog(textAreaLog, "ℹ️ Deleting these files:");

    for (const currentFolder in deletedJson.files) {
      if (!deletedJson.folders.includes(currentFolder)) {
        appendLog(textAreaLog, `\t Deleting selected files inside ${currentFolder}:`);

        for (const fileName of deletedJson.files[currentFolder]) {
            const fileJson = await fileApi.getByUserPath(currentFolder + fileName);
            appendLog(textAreaLog, `\t\t 👉 Deleting file ${fileName}`);
            const response = await fileApi.delete(fileJson.symbolFilePath);
            appendLog(textAreaLog, `\t\t\t Deleted Successfully: ${response.isDeleted ? '✅' : '❌'}`);
        }
      }
    }
    
    appendLog(textAreaLog, "👍 Finished deleting files.")
    appendLog(textAreaLog, "");
    appendLog(textAreaLog, "ℹ️ Creating new added folders:");

    for (let index = 0; index < newAddedJson.rootFolders.length; index++) {
      const currentUserFolderPath = newAddedJson.rootFolders[index];
      const parentUserFolderPath = newAddedJson.rootOfRootFolder[currentUserFolderPath];

      const parentFolderJson = await folderApi.getByUserPath(parentUserFolderPath);
      let pathTokens = await window.fsApi.splitPath(currentUserFolderPath);
      pathTokens.pop(); // remove last element whcih is ''

      const parentSymbolFolderPath = parentFolderJson.symbolFolderPath + pathTokens.pop() + "/";

      appendLog(textAreaLog, `\t 👉 Creating new folder ${currentUserFolderPath}`);
      const result = await folderApi.add(parentSymbolFolderPath, currentUserFolderPath);
      appendLog(textAreaLog, `\t\t Created Successfully: ${result.isAdded ? '✅' : '❌'}:`);

      let childSuffixes = newAddedJson.childFolderSuffixes[currentUserFolderPath];

      if(childSuffixes) {
        appendLog(textAreaLog, `\t\t Creating child folders of: ${currentUserFolderPath}`);

        for(let childIndex = 0; childIndex < childSuffixes.length; childIndex++) {
          const childFolderUserPath = currentUserFolderPath + childSuffixes[childIndex]; // Suffix already ends with /.
          const childFolderSymbolPath = parentSymbolFolderPath + childSuffixes[childIndex]; // Suffix already ends with /.
  
          appendLog(textAreaLog, `\t\t\t 👉 Creating new child folder ${childSuffixes[childIndex]}`);
          const result = await folderApi.add(childFolderSymbolPath, childFolderUserPath);
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
        const folderJson = await folderApi.getByUserPath(currentFolder);
        appendLog(textAreaLog, `\t\t 👉 Adding new file  ${fileName}`);
        const result = await fileApi.add(folderJson.symbolFolderPath,
                                         currentFolder + fileName,
                                         await window.appState.get("commitMessage"),
                                         false);
        appendLog(textAreaLog, `\t\t\t Added  Successfully: ${result.isAdded ? '✅' : '❌'}:`);
      }
    }

    appendLog(textAreaLog, "👍 Finished adding new files.")
    appendLog(textAreaLog, "");
    appendLog(textAreaLog, "ℹ️ Adding updated files inside existing folders:");

    for (const currentFolder in updatedJson) {
      appendLog(textAreaLog, `\t Adding updated files inside folder: ${currentFolder}`);
      for (const fileName of updatedJson[currentFolder]) {
        appendLog(textAreaLog, `\t\t 👉 Adding new version of ${fileName}`);
        const result = await fileApi.appendVersion(currentFolder + fileName, await window.appState.get("commitMessage"));
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