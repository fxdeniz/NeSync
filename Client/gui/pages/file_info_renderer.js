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
});