import FolderApi from "../rest_api/FolderApi.mjs";

document.addEventListener("DOMContentLoaded", async (event) => {
    const folderApi = new FolderApi("localhost", 1234);
    let currentFolder = await window.appState.get("currentFolder");
    currentFolder = await folderApi.get(currentFolder.symbolFolderPath);
    const parentFolder = await folderApi.get(currentFolder.parentFolderPath);
    await window.appState.set("currentFolder", currentFolder); // Refresh

    const divFolderName = document.getElementById("div-folder-name");
    const inputCurrentPath = document.getElementById("input-current-path");
    divFolderName.textContent = currentFolder.suffixPath.slice(0, -1); // Remove last '/'.
    inputCurrentPath.value = currentFolder.symbolFolderPath;

    const pStatus = document.getElementById("p-status");
    const hUserPath = document.getElementById("h-user-path");

    if(currentFolder.isFrozen) {
        pStatus.textContent = "This folder is archived and not in sync.";
        hUserPath.textContent = "";
    }
    else {
        pStatus.textContent = "This folder is in sync and source is:";
        hUserPath.textContent = currentFolder.userFolderPath;
    }

    const buttonBack = document.getElementById("button-back");
    const buttonRename = document.getElementById("button-rename");
    const buttonFreeze = document.getElementById("button-freeze");
    const buttonDelete = document.getElementById("button-delete");
    const buttonSelectPath = document.getElementById("button-select-path");
    const buttonRelocate = document.getElementById("button-relocate");
    const renameModal = document.getElementById("rename-modal");
    const relocateModal = document.getElementById("relocate-modal");

    currentFolder.isFrozen ? buttonFreeze.textContent = '▶️' : buttonFreeze.textContent = '⏸️';
    parentFolder.isFrozen ? buttonFreeze.disabled = true : buttonFreeze.disabled = false;

    buttonBack.addEventListener('click', async () => {
        await window.appState.set("currentFolder", parentFolder);
        window.router.routeToFileExplorer();
    });

    buttonDelete.addEventListener('click', onClickHandler_buttonDelete);
    buttonFreeze.addEventListener('click', onClickHandler_buttonFreeze);
    buttonRename.addEventListener('click', onClickHandler_buttonRename);
    buttonSelectPath.addEventListener('click', onClickHandler_buttonSelectPath);
    buttonRelocate.addEventListener('click', onClickHandler_buttonRelocate);
    renameModal.addEventListener("shown.bs.modal", onShownHandler_renameModal);
    relocateModal.addEventListener("shown.bs.modal", onShwownHandler_relocateModal);
    document.getElementById("input-foldername").addEventListener("input", inputHandler_inputFolderName);
});

async function onClickHandler_buttonDelete() {
    let folder = await window.appState.get("currentFolder");

    const message = `Are you sure you want to delete this folder?\n
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

async function onClickHandler_buttonFreeze() {
    const currentFolder = await window.appState.get("currentFolder");
    const folderApi = new FolderApi('localhost', 1234);
    const parentFolder = await folderApi.get(currentFolder.parentFolderPath);

    if(currentFolder.isFrozen) {
        if(currentFolder.parentFolderPath === '/') { // If relocating root.
            const relocateModal = document.getElementById('relocate-modal');
            const modal = new bootstrap.Modal(relocateModal);
            modal.show();
        }
        else if(!parentFolder.isFrozen && parentFolder.userFolderPath) { // If relocating children of root.
            const destination = parentFolder.userFolderPath + currentFolder.suffixPath;
            const result = await folderApi.relocate(currentFolder.symbolFolderPath, destination);

            if(!result.isRelocated)
                alert("Couldn't relocate the folder, please try again later.");
            else{
                alert("Folder is activated successfully, and ready for sync.");
                window.location.reload();
            }
        }
    } else {
        const result = await folderApi.freeze(currentFolder.symbolFolderPath);

        if(!result.isFrozen)
            alert("Couldn't freeze the folder, please try again later.");
        else {
            alert("Folder is frozen successfully.");
            window.location.reload();
        }
    }
}

async function onClickHandler_buttonRename() {
    const folderName = document.getElementById("input-foldername").value + '/';
    let folder = await window.appState.get("currentFolder");
    const folderApi = new FolderApi('localhost', 1234);

    const result = await folderApi.rename(folder.symbolFolderPath, folderName);

    if(!result.isRenamed)
        alert("Couldn't rename the folder, please try again later.");
    else {
        folder = await folderApi.get(result.newSymbolFolderPath);
        await window.appState.set("currentFolder", folder);
        window.location.reload();
    }
}

async function onClickHandler_buttonSelectPath() {
    const currentFolder = await window.appState.get("currentFolder");
    const selectedFolderTree = await window.dialogApi.showFolderSelectDialog();

    if(selectedFolderTree) {
        let destination = await window.fsApi.normalizePath(selectedFolderTree.folderPath);
        document.getElementById("input-user-path").value = destination;

        destination += currentFolder.suffixPath;
        destination = await window.fsApi.normalizePath(destination);

        document.getElementById("button-relocate").disabled = false;
        await window.appState.set("relocationDestination", destination);
    }
}

async function onClickHandler_buttonRelocate() {
    const currentFolder = await window.appState.get("currentFolder");
    const destination = await window.appState.get("relocationDestination");
    const folderApi = new FolderApi("localhost", 1234);

    const isExists = await window.fsApi.isPathExists(destination);

    if(isExists) {
        const accepted = confirm(`Another folder with the same name already exists.
                                  It will be overwritten with your last saved data. And sync will continue from that.
                                  Would you like to continue ?`);

        if(!accepted)
            return;
    }

    const result = await folderApi.relocate(currentFolder.symbolFolderPath, destination);

    if(!result.isRelocated)
        alert("Couldn't relocate the folder, please try again later.");
    else {
        alert("Folder is activated successfully, and ready for sync.");
        window.location.reload();
    }}

async function onShownHandler_renameModal() {
    const inputFoldername = document.getElementById("input-foldername");
    const folder = await window.appState.get("currentFolder");
    inputFoldername.value = folder.suffixPath.slice(0, -1);
    inputFoldername.select();
}

async function onShwownHandler_relocateModal() {
    document.getElementById("input-user-path").value = "";
    document.getElementById("button-relocate").disabled = true;
}

function inputHandler_inputFolderName(event) {
    const inputText = event.target.value;
    const buttonRename = document.getElementById("button-rename");

    if(inputText.length === 0)
        buttonRename.disabled = true;
    else
        buttonRename.disabled = false;
}