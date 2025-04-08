import ZipExportApi from "../rest_api/ZipExportApi.mjs"

document.addEventListener("DOMContentLoaded", async (event) => {
    const serverPort = await window.appState.get("serverPort");
    let exportApi = new ZipExportApi('localhost', serverPort);

    let buttonClose = document.getElementById('button-close');
    buttonClose.addEventListener('click', async clickEvent => window.router.routeToFileExplorer());
    disableButton(buttonClose);

    let textAreaLog = document.getElementById('text-area-log');

    appendLog(textAreaLog, "ℹ️ Crearting zip file...");

    const responseFilePath = await exportApi.getFilePath();
    const responseCreate = await exportApi.createZip(responseFilePath.filePath);

    appendLog(textAreaLog, `\t Created Successfully: ${responseCreate.isCreated ? '✅' : '❌'}`);

    if(!responseCreate.isCreated) {
      appendLog(textAreaLog, "⛔ Exporting aborted due to error, please try again.");
      enableButton(buttonClose);
      return;
    }

    appendLog(textAreaLog, "");
    appendLog(textAreaLog, "ℹ️ Adding folder tree metadata to zip file...");

    const responseAddFoldersJson = await exportApi.addFoldersJson();

    appendLog(textAreaLog, `\t Added Successfully: ${responseAddFoldersJson.isAdded ? '✅' : '❌'}`);

    if(!responseAddFoldersJson.isAdded) {
      appendLog(textAreaLog, "⛔ Exporting aborted due to error, please try again.");
      enableButton(buttonClose);
      return;
    }

    appendLog(textAreaLog, "");
    appendLog(textAreaLog, "ℹ️ Adding files' metadata to zip file...");

    const responseAddFilesJson = await exportApi.addFilesJson();

    appendLog(textAreaLog, `\t Added Successfully: ${responseAddFilesJson.isAdded ? '✅' : '❌'}`);

    if(!responseAddFilesJson.isAdded) {
      appendLog(textAreaLog, "⛔ Exporting aborted due to error, please try again.");
      enableButton(buttonClose);
      return;
    }

    appendLog(textAreaLog, "");
    appendLog(textAreaLog, "ℹ️ Adding actual files into zip file...");

    const files = responseAddFilesJson.files;

    for (const symbolFilePath in files) {
      const fileObject = files[symbolFilePath];
      appendLog(textAreaLog, `\t 👉 Adding copies of file: ${fileObject.symbolFilePath}`);

      for (const versionObject of fileObject.versionList) {
        const versionNumber = versionObject.versionNumber;
        appendLog(textAreaLog, `\t\t Adding version: ${versionNumber}`);
        const responseAddFileToZip = await exportApi.addFile(symbolFilePath, versionNumber);
        appendLog(textAreaLog, `\t\t\t Added Successfully: ${responseAddFileToZip.isAdded ? '✅' : '❌'}`);

        if(!responseAddFileToZip.isAdded) {
          appendLog(textAreaLog, "⛔ Exporting aborted due to error, please try again.");
          enableButton(buttonClose);
          return;    
        }
      }
    }

    appendLog(textAreaLog, "");
    appendLog(textAreaLog, "💯 Exporting to a zip file completed successfully.");
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