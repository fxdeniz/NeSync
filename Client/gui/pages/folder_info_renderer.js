import FolderApi from "../rest_api/FolderApi.mjs";
import FileApi from "../rest_api/FileApi.mjs"
import VersionApi from "../rest_api/VersionApi.mjs"

document.addEventListener("DOMContentLoaded", async (event) => {
    const folderInfo = await window.appState.get("currentFolder");

    const divFolderName = document.getElementById("div-folder-name");
    const inputCurrentPath = document.getElementById("input-current-path");
    divFolderName.textContent = folderInfo.suffixPath.slice(0, -1); // Remove last '/'.
    inputCurrentPath.value = folderInfo.symbolFolderPath;
});