import FolderApi from "../rest_api/FolderApi.mjs";
import FileApi from "../rest_api/FileApi.mjs"

document.addEventListener("DOMContentLoaded", async (event) => {
    const symbolPath = await window.appState.get("currentFile");

    const fileApi = new FileApi("localhost", 1234);
    const info = await fileApi.get(symbolPath)

    const divFileName = document.getElementById("div-file-name");
    const inputCurrentPath = document.getElementById("input-current-path");
    divFileName.textContent = info.fileName;
    inputCurrentPath.value = info.symbolFilePath;

    const ulVersions = document.getElementById("ul-versions");
    info.versionList.forEach(item => ulVersions.appendChild(createListItem(item.versionNumber)));
});

function createListItem(versionNumber) {
    const button = document.createElement("button");
    button.type = "button";
    button.className = "list-group-item list-group-item-action text-center";

    const span = document.createElement("span");
    span.className = "badge rounded-pill text-bg-primary";
    span.textContent = versionNumber;

    button.appendChild(span);

    return button;
}