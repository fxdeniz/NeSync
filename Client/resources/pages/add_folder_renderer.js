import FolderApi from "../rest_api/FolderApi.mjs";
import FileApi from "../rest_api/FileApi.mjs";

document.addEventListener("DOMContentLoaded", async (event) => {

  let stack = JSON.parse(sessionStorage.getItem('selectedFolderStack'));

  let folderApi = new FolderApi('localhost', 1234);
  let fileApi = new FileApi('localhost', 1234);
  let fileList = [];

  while (stack.length > 0) {
      let currentFolder = stack.pop();
      let pathTokens = await window.pathApi.splitPath(currentFolder.folderPath);
      let symbolFolderSuffix = pathTokens.pop() + "/";

      if(currentFolder.symbolFolderPath === undefined) // Check symbol folder path of the root.
        currentFolder.symbolFolderPath = "/" + symbolFolderSuffix;
      else
        currentFolder.symbolFolderPath += symbolFolderSuffix;

      // TODO: check return result
      await folderApi.add(currentFolder.symbolFolderPath, currentFolder.folderPath);

      for(const filePath of currentFolder.childFiles) {
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

  for(const currentFile of fileList) {
    // TODO: Check return result
    await fileApi.add(currentFile.symbolFolderPath,
                      currentFile.pathToFile,
                      currentFile.description,
                      currentFile.isFrozen);
  }


  // start of new code
  let buttonClose = document.getElementById('button-close');
  let textAreaLog = document.getElementById('text-area-log');
  
  buttonClose.addEventListener('click', async clickEvent => window.router.routeToFileExplorer());
  appendLog(textAreaLog, "💯 Adding files & folders finished.");
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