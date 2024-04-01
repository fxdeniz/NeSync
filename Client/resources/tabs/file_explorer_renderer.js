document.addEventListener("DOMContentLoaded", (event) => {

    const buttonAddNewFolder = document.getElementById('button-add-new-folder');
    buttonAddNewFolder.addEventListener('click', async () => {
        const folderPath = await window.fileExplorerApi.showFolderSelectDialog();
        alert(`selected = ${folderPath}`);
    });
  });
  