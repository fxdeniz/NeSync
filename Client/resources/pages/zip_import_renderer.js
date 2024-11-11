document.addEventListener("DOMContentLoaded", async (event) => {

    let buttonClose = document.getElementById('button-close');
    buttonClose.addEventListener('click', async clickEvent => window.router.routeToFileExplorer());
    disableButton(buttonClose);

    let textAreaLog = document.getElementById('text-area-log');

    const responseOpenZip = await sendOpenImportZipRequest();

    appendLog(textAreaLog, "ℹ️ Trying to open zip file...");
    appendLog(textAreaLog, `\t File Opened Successfully: ${responseOpenZip.isOpened ? '✅' : '❌'}`);

    if(!responseOpenZip.isOpened) {
      appendLog(textAreaLog, "⛔ File could not opened, please try again.");
      enableButton(buttonClose);
      return;
    }

    const foldersJson = await sendReadFoldersJsonRequest();

    appendLog(textAreaLog, "");
    appendLog(textAreaLog, "ℹ️ Reading folders.json");
    appendLog(textAreaLog, `\t Read Successfully: ${foldersJson.lenth !== 0 ? '✅' : '❌'}`);

    if(foldersJson.lenth === 0) {
      appendLog(textAreaLog, "⛔ folder.json could not opened, please try again.");
      enableButton(buttonClose);
      return;
    }

    appendLog(textAreaLog, "");
    appendLog(textAreaLog, "ℹ️ Creating folders...");

    for (const symbolFolderPath of foldersJson) {
      const folder = await sendGetFolderRequest(symbolFolderPath);

      appendLog(textAreaLog, `\t 👉 Checking folder: ${symbolFolderPath}`);

      if(folder.isExist)
        appendLog(textAreaLog, "\t\t Folder exists, no need to create.");
      else {
        appendLog(textAreaLog, "\t\t Folder not exists, creating it...");
        const responseCreateFolder = await sendAddFolderRequest(symbolFolderPath, null);
        appendLog(textAreaLog, `\t\t\t Created Successfully: ${responseCreateFolder.isAdded ? '✅' : '❌'}`);

        if(!responseCreateFolder.isAdded) {
          appendLog(textAreaLog, "⛔ Could not create a folder, please try again.");
          enableButton(buttonClose);
          return;
        }
      }
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


async function sendOpenImportZipRequest(symbolFolderPath) {
  return await fetchJSON("http://localhost:1234/zip/import/OpenFile");
}


async function sendReadFoldersJsonRequest(symbolFolderPath) {
  return await fetchJSON("http://localhost:1234/zip/import/ReadFoldersJson");
}


async function sendGetFolderRequest(symbolFolderPath) {
  return await fetchJSON(`http://localhost:1234/getFolderContent?symbolPath=${symbolFolderPath}`);
}


async function sendAddFolderRequest(symbolFolderPath, userFolderPath) {
  let requestBody = {"symbolFolderPath": null, "userFolderPath": null};
  requestBody["symbolFolderPath"] = symbolFolderPath;
  requestBody["userFolderPath"] = userFolderPath;

  return await postJSON('http://localhost:1234/addNewFolder', requestBody);    
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