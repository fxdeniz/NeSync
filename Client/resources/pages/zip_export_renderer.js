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
      enableButton(buttonClose);
      return;
    }

    appendLog(textAreaLog, "");
    appendLog(textAreaLog, "ℹ️ Adding folder tree metadata to zip file...");

    const responseAddFoldersMeta = await sendAddFoldersJsonRequest();

    appendLog(textAreaLog, `\t Added Successfully: ${responseAddFoldersMeta.isAdded ? '✅' : '❌'}`);

    if(!responseAddFoldersMeta.isAdded) {
      enableButton(buttonClose);
      return;
    }

    appendLog(textAreaLog, "");
    appendLog(textAreaLog, "ℹ️ Adding files' metadata to zip file...");

    const responseAddFilesMeta = await sendAddFilesJsonRequest();

    appendLog(textAreaLog, `\t Added Successfully: ${responseAddFilesMeta.isAdded ? '✅' : '❌'}`);

    if(!responseAddFilesMeta.isAdded) {
      enableButton(buttonClose);
      return;
    }

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


async function sendGetFolderByUserPathRequest(userFolderPath) {
  return await fetchJSON(`http://localhost:1234/getFolderContentByUserPath?userFolderPath=${userFolderPath}`);
}


async function sendGetFileByUserPathRequest(userFilePath) {
  return await fetchJSON(`http://localhost:1234/getFileContentByUserPath?userFilePath=${userFilePath}`);
}


async function sendDeleteFolderRequest(symbolFolderPath) {
  return await fetchJSON(`http://localhost:1234/deleteFolder?symbolPath=${symbolFolderPath}`, "DELETE");
}


async function sendAddFolderRequest(symbolFolderPath, userFolderPath) {
  let requestBody = {"symbolFolderPath": null, "userFolderPath": null};
  requestBody["symbolFolderPath"] = symbolFolderPath;
  requestBody["userFolderPath"] = userFolderPath;

  return await postJSON('http://localhost:1234/addNewFolder', requestBody);    
}


async function sendDeleteFileRequest(symbolFilePath) {
  return await fetchJSON(`http://localhost:1234/deleteFile?symbolPath=${symbolFilePath}`, "DELETE");
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