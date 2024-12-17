document.addEventListener("DOMContentLoaded", async (event) => {

    let buttonClose = document.getElementById('button-close');
    buttonClose.addEventListener('click', async clickEvent => window.router.routeToFileExplorer());
    disableButton(buttonClose);

    let textAreaLog = document.getElementById('text-area-log');

    appendLog(textAreaLog, "ℹ️ Crearting zip file...");

    const responseFilePath = await sendGetZipFilePathRequest();
    const responseCreate = await sendPostCreateZipArchiveRequest(responseFilePath.filePath);

    appendLog(textAreaLog, `\t Created Successfully: ${responseCreate.isCreated ? '✅' : '❌'}`);

    if(!responseCreate.isCreated) {
      appendLog(textAreaLog, "⛔ Exporting aborted due to error, please try again.");
      enableButton(buttonClose);
      return;
    }

    appendLog(textAreaLog, "");
    appendLog(textAreaLog, "ℹ️ Adding folder tree metadata to zip file...");

    const responseAddFoldersJson = await sendAddFoldersJsonRequest();

    appendLog(textAreaLog, `\t Added Successfully: ${responseAddFoldersJson.isAdded ? '✅' : '❌'}`);

    if(!responseAddFoldersJson.isAdded) {
      appendLog(textAreaLog, "⛔ Exporting aborted due to error, please try again.");
      enableButton(buttonClose);
      return;
    }

    appendLog(textAreaLog, "");
    appendLog(textAreaLog, "ℹ️ Adding files' metadata to zip file...");

    const responseAddFilesJson = await sendAddFilesJsonRequest();

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
        const responseAddFileToZip = await sendAddFileToZipRequest(symbolFilePath, versionNumber);
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


async function sendGetZipFilePathRequest() {
  return await fetchJSON('http://localhost:1234/getZipFilePath');    
}


async function sendPostCreateZipArchiveRequest(filePath) {
  let requestBody = {"filePath": null};
  requestBody["filePath"] = filePath;

  return await postJSON('http://localhost:1234/postCreateZipArchive', requestBody);    
}


async function sendAddFilesJsonRequest() {
  let requestBody = {};
  return await postJSON('http://localhost:1234/postAddFileJson', requestBody);    
}


async function sendAddFoldersJsonRequest() {
  let requestBody = {};
  return await postJSON('http://localhost:1234/postAddFolderJson', requestBody);    
}


async function sendAddFileToZipRequest(symbolFilePath, versionNumber) {
  let requestBody = {"symbolFilePath": null, "versionNumber": null};
  requestBody["symbolFilePath"] = symbolFilePath;
  requestBody["versionNumber"] = versionNumber;

  return await postJSON('http://localhost:1234/postAddFileToZip', requestBody);    
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