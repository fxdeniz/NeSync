import FolderApi from "../rest_api/FolderApi.mjs";
import FileApi from "../rest_api/FileApi.mjs"

document.addEventListener("DOMContentLoaded", async (event) => {
    const fileApi = new FileApi("localhost", 1234);
    let fileInfo = await window.appState.get("currentFile");
    fileInfo = await fileApi.get(fileInfo.symbolFilePath);
    await window.appState.set("currentFile", fileInfo); // Update shared state with object containing `versionList`.

    const divFileName = document.getElementById("div-file-name");
    const inputCurrentPath = document.getElementById("input-current-path");
    divFileName.textContent = fileInfo.fileName;
    inputCurrentPath.value = fileInfo.symbolFilePath;

    const buttonBack = document.getElementById("button-back");
    const buttonPreview = document.getElementById("button-preview");
    const buttonSelectPath = document.getElementById("button-select-path");
    const buttonExtract = document.getElementById("button-extract");
    const extractModal = document.getElementById("extract-modal");

    buttonBack.addEventListener('click', async clickEvent => {
        window.router.routeToFileExplorer();
    });

    buttonPreview.addEventListener('click', onClickHandler_buttonPreview);
    buttonSelectPath.addEventListener('click', onClickHandler_buttonSelectPath);
    buttonExtract.addEventListener('click', onClickHandler_buttonExtract);
    extractModal.addEventListener("shown.bs.modal", onShownHandler_extractModal);

    const ulVersions = document.getElementById("ul-versions");
    const reverseList = JSON.parse(JSON.stringify(fileInfo.versionList));
    // TODO: Make this reversing on the server.
    reverseList.reverse(); // Make latest version appear at the top.
    reverseList.forEach(info => {
        ulVersions.appendChild(createListItem(info));
    });

    const firstItem = document.querySelector(".list-group .list-group-item");
    firstItem.click();
});

function createListItem(versionInfo) {
    const button = document.createElement("button");
    button.type = "button";
    button.className = "list-group-item list-group-item-action text-center";

    const span = document.createElement("span");
    span.className = "badge rounded-pill text-bg-primary";
    span.textContent = versionInfo.versionNumber;

    button.appendChild(span);
    button.addEventListener("click", async () => {
        const pDescription = document.getElementById("p-description");
        const hFileSize = document.getElementById("h-file-size");
        const divTimePassed = document.getElementById("div-time-passed");

        await window.appState.set("currentVersion", versionInfo);

        const items = document.querySelectorAll(".list-group .list-group-item");
        items.forEach(btn => btn.classList.remove("active"));
        button.classList.add("active");
    
        pDescription.innerHTML = versionInfo.description;
        hFileSize.innerText = formatFileSize(versionInfo.size);
        divTimePassed.innerText = timePassed(versionInfo.lastModifiedTimestamp);
    });

    return button;
}

async function onClickHandler_buttonPreview() {
    const folderApi = new FolderApi("localhost", 1234);
    const versionInfo = await window.appState.get("currentVersion");
    const fileInfo = await window.appState.get("currentFile");
    const symbolFilePath = fileInfo.symbolFilePath;
    const extension = symbolFilePath.split(".").pop();
    const storagePath = await folderApi.getStorageFolderPath();

    displayAlertDiv("Generating file preview, please wait...");
    disableUserControls();
    const result = await window.fsApi.previewFile(storagePath + versionInfo.internalFileName, extension);
    enableUserControls();
    closeAlertDiv();

    if(!result)
        alert("File preview couldn't generated.");
}

async function onClickHandler_buttonSelectPath() {
    const inputExtractPath = document.getElementById("input-extract-path");
    const selectedPath = await window.dialogApi.showFileSaveDialog();
    const fileInfo = await window.appState.get("currentFile");
    let extension = fileInfo.symbolFilePath;
    extension = extension.split('.');

    if(extension.length === 1)
        extension = null;
    else
        extension = extension.pop();

    if(selectedPath) {
        if(extension && !selectedPath.endsWith(extension)) {
            alert(`File name should end with \".${extension}\" extension.`);
            return;
        }

        inputExtractPath.value = selectedPath;
        const buttonExtract = document.getElementById("button-extract");
        buttonExtract.disabled = false;
        buttonExtract.focus();
    }
}

async function onClickHandler_buttonExtract() {
    const folderApi = new FolderApi("localhost", 1234);
    const version = await window.appState.get("currentVersion");
    let src = await folderApi.getStorageFolderPath();
    src += version.internalFileName;
    const dest = document.getElementById("input-extract-path").value;

    displayAlertDiv("Extracting file, please wait...");
    disableUserControls();
    const result = await window.fsApi.extractFile(src, dest);
    enableUserControls();
    closeAlertDiv();

    if(!result)
        alert("File couldn't extracted.");
}

async function onShownHandler_extractModal() {
    document.getElementById("input-extract-path").value = "";
    document.getElementById("button-extract").disabled = true;
    document.getElementById("button-select-path").focus();

    const pVersion = document.getElementById("p-extract-file-version");
    const pName = document.getElementById("p-extract-file-name");

    const file = await window.appState.get("currentFile");
    const version = await window.appState.get("currentVersion");

    pVersion.innerHTML = `Extracting version <strong>${version.versionNumber}</strong> of:`;
    pName.innerHTML = `<strong>${file.fileName}</strong>`;
}

function disableUserControls() {
    const buttonPreview = document.getElementById('button-preview');
    const buttonOpenExtractModal = document.getElementById('button-open-extract-modal');
    const versionButtons = document.querySelectorAll("#ul-versions *");

    buttonPreview.disabled = true;
    buttonOpenExtractModal.disabled = true;
    versionButtons.forEach(element => {element.disabled = true;});
}

function enableUserControls() {
    const buttonPreview = document.getElementById('button-preview');
    const buttonOpenExtractModal = document.getElementById('button-open-extract-modal');
    const versionButtons = document.querySelectorAll("#ul-versions *");

    buttonPreview.disabled = false;
    buttonOpenExtractModal.disabled = false;
    versionButtons.forEach(element => {element.disabled = false;});
}

function displayAlertDiv(alertMessage) {
    let divAlert = document.createElement("div");
    divAlert.id = "div-alert";
    divAlert.className = "alert alert-warning text-center mt-3";
    divAlert.setAttribute("role", "alert");
    divAlert.textContent = alertMessage;

    const container = document.getElementById("content-container");
    const targetRow = container.querySelector(".row.mt-3");
    const parentElement = targetRow.parentNode;

    parentElement.insertBefore(divAlert, targetRow);
}
  
function closeAlertDiv() {
    let divAlert = document.getElementById("div-alert");
    divAlert.remove();
}

function formatFileSize(bytes) {
    if (bytes === 0) return '0 Bytes';
    
    const sizes = ['Bytes', 'KB', 'MB', 'GB', 'TB', 'PB', 'EB', 'ZB', 'YB'];
    const index = Math.floor(Math.log(bytes) / Math.log(1024));
    const formattedSize = (bytes / Math.pow(1024, index)).toFixed(2);
    
    return `${formattedSize} ${sizes[index]}`;
}

function timePassed(date) {
    const now = new Date();
    const past = new Date(date);

    if (isNaN(past)) return "Invalid date";

    const diffInSeconds = Math.floor((now - past) / 1000);
    const seconds = diffInSeconds % 60;
    const minutes = Math.floor(diffInSeconds / 60) % 60;
    const hours = Math.floor(diffInSeconds / 3600) % 24;
    const days = Math.floor(diffInSeconds / (3600 * 24));
    const weeks = Math.floor(days / 7);
    const months = Math.floor(days / 30); // Approximate
    const years = Math.floor(days / 365); // Approximate

    // Format the original date
    const monthNames = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"];
    const originalDate = `${past.getFullYear()} ` +
                         `${monthNames[past.getMonth()]} ` +
                         `${String(past.getDate()).padStart(2, '0')} ` +
                         `${String(past.getHours()).padStart(2, '0')}:` +
                         `${String(past.getMinutes()).padStart(2, '0')}`;

    // If it's today, return in hours and minutes
    if (days === 0) {
        if (hours > 0) return `${hours} hours ${minutes} minutes ago - ${originalDate}`;
        if (minutes > 0) return `${minutes} minutes ago - ${originalDate}`;
        return `${seconds} seconds ago - ${originalDate}`;
    }

    // Otherwise, format as years, months, weeks, and days
    let result = "";
    if (years > 0) result += `${years} years `;
    if (months % 12 > 0) result += `${months % 12} months `;
    if (weeks % 4 > 0) result += `${weeks % 4} weeks `;
    if (days % 7 > 0) result += `${days % 7} days `;

    return `${result.trim()} ago - ${originalDate}`;
}