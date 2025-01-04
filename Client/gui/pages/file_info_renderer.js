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

    const olVersions = document.getElementById("ol-versions");
    info.versionList.forEach(item => olVersions.appendChild(createListItem(item.versionNumber, item.description)));
});

function createListItem(versionNumber, description) {
    const li = document.createElement("li");
    li.className = "list-group-item d-flex justify-content-between align-items-start clickable";
    li.setAttribute("role", "button");
  
    const textDiv = document.createElement("div");
    textDiv.className = "ms-2 me-auto";

    const subheadingDiv = document.createElement('div');
    subheadingDiv.className = "fw-bold";
    subheadingDiv.textContent = "Subheading";

    textDiv.appendChild(subheadingDiv);
    textDiv.appendChild(document.createTextNode(description));

    const badge = document.createElement("span");
    badge.className = "badge text-bg-primary rounded-pill";
    badge.textContent = versionNumber;

    li.appendChild(textDiv);
    li.appendChild(badge);

    return li;
}
