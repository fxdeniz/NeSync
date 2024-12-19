import FolderApi from "../rest_api/FolderApi.mjs";
import FileApi from "../rest_api/FileApi.mjs";
import ZipImportApi from "../rest_api/ZipImportApi.mjs";

document.addEventListener("DOMContentLoaded", async (event) => {

    let folderApi = new FolderApi('localhost', 1234);
    let fileApi = new FileApi('localhost', 1234);
    let importApi = new ZipImportApi('localhost', 1234);

    let buttonClose = document.getElementById('button-close');
    buttonClose.addEventListener('click', async clickEvent => window.router.routeToFileExplorer());
    disableButton(buttonClose);

    let textAreaLog = document.getElementById('text-area-log');

    const responseOpenZip = await importApi.openImportZip();

    appendLog(textAreaLog, "ℹ️ Trying to open zip file...");
    appendLog(textAreaLog, `\t File Opened Successfully: ${responseOpenZip.isOpened ? '✅' : '❌'}`);

    if(!responseOpenZip.isOpened) {
      appendLog(textAreaLog, "⛔ File could not opened, please try again.");
      enableButton(buttonClose);
      return;
    }

    const foldersJson = await importApi.readFoldersJson();

    appendLog(textAreaLog, "");
    appendLog(textAreaLog, "ℹ️ Reading folders.json");
    appendLog(textAreaLog, `\t Read Successfully: ${foldersJson.lenth !== 0 ? '✅' : '❌'}`);

    if(foldersJson.lenth === 0) {
      appendLog(textAreaLog, "⛔ folder.json could not opened, please try again.");
      enableButton(buttonClose);
      return;
    }

    const filesJson = await importApi.readFilesJson();

    appendLog(textAreaLog, "");
    appendLog(textAreaLog, "ℹ️ Reading files.json");
    appendLog(textAreaLog, `\t Read Successfully: ${Object.keys(filesJson).length !== 0 ? '✅' : '❌'}`);

    if(Object.keys(filesJson).length === 0) {
      appendLog(textAreaLog, "⛔ files.json could not opened, please try again.");
      enableButton(buttonClose);
      return;
    }

    appendLog(textAreaLog, "");
    appendLog(textAreaLog, "ℹ️ Creating folders...");

    for (const symbolFolderPath of foldersJson) {
      appendLog(textAreaLog, `\t 👉 Checking folder: ${symbolFolderPath}`);
      const folder = await folderApi.get(symbolFolderPath);

      if(folder.isExist)
        appendLog(textAreaLog, "\t\t Folder exists, no need to create.");
      else {
        appendLog(textAreaLog, "\t\t Folder not exists, creating it...");
        const responseCreateFolder = await folderApi.add(symbolFolderPath, null);
        appendLog(textAreaLog, `\t\t\t Created Successfully: ${responseCreateFolder.isAdded ? '✅' : '❌'}`);

        if(!responseCreateFolder.isAdded) {
          appendLog(textAreaLog, "⛔ Could not create a folder, please try again.");
          enableButton(buttonClose);
          return;
        }
      }
    }

    appendLog(textAreaLog, "👍 Finished creating folders.")
    appendLog(textAreaLog, "");
    appendLog(textAreaLog, "ℹ️ Importing files...");

    for (const symbolFilePath in filesJson) {
      appendLog(textAreaLog, `\t 👉 Checking file: ${symbolFilePath}`);
      const file = await fileApi.get(symbolFilePath);

      if(file.isExist)
        appendLog(textAreaLog, "\t\t File exists, no need to add.");
      else {
        appendLog(textAreaLog, "\t\t File not exists, importing it...");
        const newFile = filesJson[symbolFilePath];

        for(const version of newFile.versionList) {
          const versionNumber = version.versionNumber;
          appendLog(textAreaLog, `\t\t\t Importing version ${versionNumber}:`);
          const importVersion = await importApi.importFile(symbolFilePath, versionNumber);
          appendLog(textAreaLog, `\t\t\t\t Imported Successfully ${importVersion.isImported ? '✅' : '❌'}`);

          if(!importVersion.isImported) {
            appendLog(textAreaLog, "⛔ Could not import a file, please try again.");
            enableButton(buttonClose);
            return;
          }
        }
      }
    }

    appendLog(textAreaLog, "👍 Finished importing files.")
    appendLog(textAreaLog, "");
    appendLog(textAreaLog, "💯 Import completed.");
    
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