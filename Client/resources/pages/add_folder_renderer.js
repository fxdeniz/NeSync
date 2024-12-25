
document.addEventListener("DOMContentLoaded", async (event) => {

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