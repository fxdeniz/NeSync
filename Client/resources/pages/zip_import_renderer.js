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


async function sendGetFolderRequest(symbolFolderPath) {
  return await fetchJSON(`http://localhost:1234/getFolderContent?symbolFolderPath=${symbolFolderPath}`);
}


async function sendGetFolderRequest() {
  return await fetchJSON(`http://localhost:1234/getFolderContent?symbolFolderPath=${symbolFolderPath}`);
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