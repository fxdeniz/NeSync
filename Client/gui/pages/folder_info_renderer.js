import FolderApi from "../rest_api/FolderApi.mjs";
import FileApi from "../rest_api/FileApi.mjs"
import VersionApi from "../rest_api/VersionApi.mjs"

document.addEventListener("DOMContentLoaded", async (event) => {
    const folderApi = new FolderApi("localhost", 1234);
    const currentFolder = await window.appState.get("currentFolder");
    const parentFolder = await folderApi.get(currentFolder.parentFolderPath);

    const divFolderName = document.getElementById("div-folder-name");
    const inputCurrentPath = document.getElementById("input-current-path");
    divFolderName.textContent = currentFolder.suffixPath.slice(0, -1); // Remove last '/'.
    inputCurrentPath.value = currentFolder.symbolFolderPath;

    const buttonBack = document.getElementById("button-back");
    const buttonDelete = document.getElementById("button-delete");


    buttonBack.addEventListener('click', async () => {
        await window.appState.set("currentFolder", parentFolder);
        window.router.routeToFileExplorer();
    });

    buttonDelete.addEventListener('click', onClickHandler_buttonDelete);
});

async function onClickHandler_buttonDelete() {
    let folder = await window.appState.get("currentFolder");

    let message = `Are you sure you want to delete this folder?\n
                   This will delete your working folder with everything inside.`;

    const userConfirmed = confirm(message);
    if (!userConfirmed)
        return;

    const folderApi = new FolderApi("localhost", 1234);
    const result = await folderApi.delete(folder.symbolFolderPath);

    if(result.isDeleted) {
        alert("Folder deleted succesfully.");
        window.router.routeToFileExplorer();
    }
    else {
        alert("Folder couldn't deleted, please try again.");
        window.location.reload();
    }
}