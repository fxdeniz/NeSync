import FolderApi from "../rest_api/FolderApi.mjs";
import FileApi from "../rest_api/FileApi.mjs"
import VersionApi from "../rest_api/VersionApi.mjs"

document.addEventListener("DOMContentLoaded", async (event) => {
    const folderApi = new FolderApi("localhost", 1234);
    const currentFolder = await window.appState.get("currentFolder");
    const parentFolder = await folderApi.get(currentFolder.parentFolderPath);

    const divFolderName = document.getElementById("div-folder-name");
    const inputCurrentPath = document.getElementById("input-current-path");
    const buttonBack = document.getElementById("button-back");

    divFolderName.textContent = currentFolder.suffixPath.slice(0, -1); // Remove last '/'.
    inputCurrentPath.value = currentFolder.symbolFolderPath;

    buttonBack.addEventListener('click', async clickEvent => {
        await window.appState.set("currentFolder", parentFolder);
        window.router.routeToFileExplorer();
    });
});